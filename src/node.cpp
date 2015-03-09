#include "node.hpp"
#include "node_group.hpp"
#include "node_configs.hpp"
#include "packet_handler.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <ostream>


// Constructor for Node Class
Node::Node(size_t idx, size_t network_size)
  : m_idx(idx), m_ntwk_size(network_size), m_clk_ratio(1)
{
    m_status = NODE_STATUS_ACTIVE;
    m_clk = 0;
    stat_pckt_sent = 0;
    stat_pckt_recv = 0;

    m_config.buffer = false;
    buffer_list = NULL;
    buffer_map  = NULL;

    m_config.dram = false;
    m_dram = NULL;

    send_queue = queue<Packet*>();
    recv_queue = queue<Packet*>();

    m_config.port = false;
#if _BOOKSIM
    m_port_in  = NULL;
    m_port_out = NULL;
#endif
}

Node::~Node()
{
    if (m_config.buffer)
    {
        delete buffer_list;
        delete buffer_map;
    }

    if (m_config.dram)
        delete m_dram;

#if _BOOKSIM
    if (m_config.port)
    {
        delete m_port_in;
        delete m_port_out;
    }
#endif

}

size_t Node::update_clock()
{
    if (g_clock % m_clk_ratio != 0)
    {
        m_status = NODE_STATUS_INACTIVE;
        return m_clk;
    }

    m_status = NODE_STATUS_ACTIVE;
    ++m_clk;

    if (m_config.buffer)
        update_buffers();
    return m_clk;
}


int Node::wake_node(NodeGroup * group)
{
#if _DEBUG
    cout << "Woke node " << m_idx << endl;
#endif

    if (m_status == NODE_STATUS_INACTIVE)
        return 0;

    return test_for_packet(group);
}

int Node::test_for_packet(NodeGroup * group)
{

    double rand_num, inj_rate, traf_prob, thresh;

    // Get a random number to see if this node will create a packet.
    rand_num = get_rand_decimal();

    // Get per-cycle injection rate for current cycle. (per-node)
    inj_rate = group->get_inj_rate(m_clk);

    traf_prob = group->get_traf_pattern(m_idx);

    // Get the probability of packet creation for this node, normalized 
    // by simulation step size.
    //   P(packet) = ( weight of this node ) 
    //                    * ( packets per cycle of node)) 
    //                          * (cycles per step )
    thresh = traf_prob * inj_rate;

#if _DEBUG
    // DEBUGGING OUTPUT
    

    cout << "thresh=  " << thresh  << "\t | "
         << "inj_rate= " << inj_rate << "\t | "
         << "traffic_prob= " << traf_prob << "\t | "
         << "rand_num= " << rand_num << endl;
#endif

    if (rand_num < thresh)
    {
        return generate_packet(group);
    }
    else
        return SUCCESS;
}

int Node::generate_packet(NodeGroup* group)
{
        // Find the destination node
        Node * dest_node = group->pick_dest(m_idx);

        if (dest_node == NULL)
              return ERROR;

        // Determine the type of packet generated
        // Default & Request packets get created here, deleted at traffic_report_handler.
        Packet* new_packet = (group->pick_packet())->clone();
        if (new_packet == NULL)
              return ERROR;

        new_packet->set_node_group(group);
        new_packet->set_src(this);
        new_packet->set_dest(dest_node);

#if _DEBUG        
    cout << "NodeGroup= " << group->get_id() << "\t | "
         << "SrcIdx= " << m_idx << "\t | "
         << "DestIdx= " << dest_node->get_idx() << "\t | "
         << "Packet= " << new_packet->id << endl;
#endif

#if _BOOKSIM
        // Check for buffer availability here...
        // This could cause some problems for packets generated on the
        // same cycle...
        if (m_config.port)
        {
            bool avail = m_port_out->port_available(new_packet);
            if (!avail) // If port is full, don't send.
                return SUCCESS;
        }
#endif

        // If the node has buffers, send the packet to the buffer.
        if (m_config.buffer)
        {
            PacketBuffer* buffer = find_matching_buffer(new_packet->get_id()); 
            // It is possible for the given packet to not be one of the
            // "buffered" packets.
            if (buffer != NULL)
            {
                // Check buffer to see if there is space.
                if (buffer->check())
                {
                    buffer->push(new_packet);
                }
                else
                {
#if _DEBUG
                    cout << "BUFFER FULL: Packet " << new_packet->get_id() 
                         << " dropped." << endl;
#endif
                    return SUCCESS; // Buffer full.
                }
            }
        }

        push_packet_to_queue(new_packet);
        return 1;
}

void Node::push_packet_to_queue(Packet *packet)
{
    send_queue.push(packet);
}

int Node::process_send_queue()
{
    // Initializing variables
    size_t pckt_count = 0;
    Packet* curr_pckt;

    while(!send_queue.empty())
    {
        curr_pckt = send_queue.front();
        
        // Push new packet to report
        create_traffic_incident(curr_pckt);
        send_queue.pop();
        ++pckt_count;
    }

#if _BOOKSIM
    if (m_config.port)
    {
        m_port_out->process_all_ports();
    }
#endif

    return pckt_count;
}

void Node::create_traffic_incident(Packet* new_packet)
{
    // Update internal node statistic
    ++stat_pckt_sent;
    (new_packet->get_grp())->pckt_generated();
    // Traffic output!
    g_traffic_report->add_incident(new_packet);

    if (!m_config.port)
    {
        (new_packet->get_dest())->receive_pckt(new_packet);
    }
#if _BOOKSIM
    else
    {
        m_port_out->push_packet(new_packet);     
    }
#endif
}


int Node::send_reply_packet(ReplyJob *reply_job, NodeGroup *group)
{
    // This ensures that send_reply_packet (and by extension, ready_to_reply)
    // gets invoked based on node's internal clock
    if (m_status == NODE_STATUS_INACTIVE)
        return 0;

    ReplyPacketHandler *curr_handler = reply_job->get_reply_handler();

    if (curr_handler->ready_to_reply(m_clk,
                                     reply_job, group->get_receivers()))
    {
        Packet* reply_packet = (Packet*) reply_job->get_reply_packet();
        // Traffic output!
        push_packet_to_queue(reply_packet);
        return 1;
    }
    
    return 0;
}


void Node::receive_pckt(Packet *packet)
{
    recv_queue.push(packet);
}

int Node::process_recv_queue()
{
    // Initializing variables
    size_t pckt_count = 0;
    Packet* curr_pckt;

    if (!m_config.port) 
    {
        while(!recv_queue.empty())
        {
            curr_pckt = recv_queue.front();   
            // Push new packet to report
            process_incoming_packet(curr_pckt);
            recv_queue.pop();
            ++pckt_count;
        }
    }
#if _BOOKSIM
    else
    {
        bool done = false;
        m_port_in->process_all_ports();

        while(!done)
        {
            curr_pckt = m_port_in->pop_packet();
            if (curr_pckt == NULL)
            {
                done = true;
            }
            else
            {
                process_incoming_packet(curr_pckt);
                ++pckt_count;
            }
        } 
    }
#endif
    return pckt_count;
}

void Node::process_incoming_packet(Packet *packet)
{
    ReplyJob* new_job;
    ReplyPacketHandler *reply_handler;
    if (m_config.buffer)
    {
        // Check if I am receiving a reply packet.
        if (packet->get_type() == REPLY)
        {
            ReplyPacket* reply_packet = (ReplyPacket*) packet;
            PacketBuffer* buffer = find_matching_buffer(reply_packet->get_rqst_pckt_id());
            if (buffer != NULL)
               buffer->reply_received(reply_packet);
            
        }
    }
    
    // Check if this packet is a request packet.
    if (packet->get_type() == REQUEST)
    {
        RequestPacket* request_packet = (RequestPacket*) packet;
        reply_handler = request_packet->get_reply_handler();
        new_job = reply_handler->create_reply_job(request_packet);
        (packet->get_grp())->push_reply_job(new_job);
    }

    ++stat_pckt_recv;
}

int Node::process_packet_queues()
{
    process_send_queue();

    process_recv_queue();

    return SUCCESS;
}


int Node::config_buffer(PacketBuffer &new_buffer)
{
    PacketBuffer *buffer_ptr;
    vector<string> pckt_ids;

    // If this is the first time creating a buffer for this node.
    if (!m_config.buffer)
    {
        m_config.buffer = true;
        buffer_list = new list<PacketBuffer>();
        buffer_map  = new map<string, PacketBuffer*>();
    }

    // Add new buffer to buffer list.
    buffer_list->push_back(new_buffer);
    buffer_ptr = &(buffer_list->back());

    pckt_ids = buffer_ptr->get_packet_ids();

    // Now register the new buffer in buffer_map
    pair<map<string,PacketBuffer*>::iterator, bool> ret;
    for (vector<string>::iterator it = pckt_ids.begin();
         it != pckt_ids.end(); ++it)
    {
        ret = buffer_map->insert(
                          pair<string,PacketBuffer*>(*it, buffer_ptr));
        if (!ret.second) // If the given packet already has a buffer.
            return ERROR;
    }
    
    return SUCCESS;
}

void Node::update_buffers()
{
    for(list<PacketBuffer>::iterator it = buffer_list->begin();
        it != buffer_list->end(); ++it)
    {
        it->update();
    }
}

string Node::get_stat()
{
    ostringstream out;
    out << "Node[" << m_idx << "] : " 
        << "Sent: " << stat_pckt_sent
        << "\tReceived: " << stat_pckt_recv << endl;

    return out.str();

}

PacketBuffer * Node::find_matching_buffer(string pckt_id)
{
    map<string,PacketBuffer*>::iterator buf_it;
    buf_it = buffer_map->find(pckt_id);

    // Make sure a buffer was found
    if (buf_it == buffer_map->end())
    {
#if _DEBUG
        cout << pckt_id << " NOT FOUND" << endl;
#endif
        return NULL;
    }

    return buf_it->second;
}


int Node::config_dram(Dram &new_dram)
{
    m_config.dram = true;
    m_dram = new Dram(new_dram);
    m_clk_ratio = new_dram.get_clk_ratio();
    return SUCCESS;
}

bool Node::bank_ready(size_t bank_time)
{
    assert(m_config.dram);
        
    return m_dram->bank_ready(bank_time);
}

size_t Node::generate_access_time()
{
    assert(m_config.dram);
    return m_dram->generate_access_time();
}

#if _BOOKSIM
int Node::config_port(bool set_default)
{
    m_port_in  = new PortIn(m_idx);
    m_port_out = new PortOut(m_idx);

    if (set_default)
    {
        m_port_in->setup_default_channel();
        m_port_out->setup_default_channel();
    }
    m_config.port = true;

    return SUCCESS;
}

int Node::config_port_channel(vector<string> *packet_ids)
{
    // In Port and Out Port are mirrors of each other for now.
    m_port_in->setup_channel(packet_ids);
    m_port_out->setup_channel(packet_ids);

    return SUCCESS;
}
#endif
