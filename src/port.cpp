#include "port.hpp"
#include "node.hpp"

#if _BOOKSIM
#include "icnt_wrapper.hpp"
#endif

Port::Port()
{
    m_node_idx = -1;
    default_queue_idx = -1;
    num_channels = 0;
}

Port::Port(int idx)
{
    m_node_idx = idx;
    default_queue_idx = -1;
    num_channels = 0;
}
Port::~Port()
{
    vector<channel_t>::iterator it;
    for(it = m_channels.begin(); 
        it != m_channels.end(); ++it)
    {
        delete it->packet_buffer;
//        delete it->credit_stack;
    }
        
}

// Return the index of the new channel.
int Port::setup_channel()
{
    // Initialize the new channel
    channel_t new_channel;
//    new_channel.credit = credit_size;
//    new_channel.credit_stack = new stack<Credit*>();
    new_channel.packet_buffer = new packet_queue();

    m_channels.push_back(new_channel);

    return num_channels++;
}

int Port::setup_channel(vector<string> *packet_ids)
{
    int channel_idx = setup_channel();

    for(vector<string>::iterator pckt_it = packet_ids->begin();
        pckt_it != packet_ids->end(); ++pckt_it)
    {
        channel_map.insert(pair<string,int>(*pckt_it, channel_idx));
    }

    return channel_idx;
}

// Return the index of the new channel.
int Port::setup_default_channel()
{
    default_queue_idx = setup_channel();
    return default_queue_idx;
}

int Port::find_channel(Packet* packet)
{
    map<string,int>::iterator it = channel_map.find(packet->get_id());
    if (it != channel_map.end())
        return it->second;
    
    if (default_queue_idx != -1)
        return default_queue_idx;
    else
        return ERROR;
}

size_t Port::get_num_waiting(int channel_idx)
{
    return (m_channels.at(channel_idx)).packet_buffer->size();
}

size_t Port::get_num_channels()
{
    return num_channels;
}

bool PortOut::port_available(Packet* packet)
{
    bool ret = false;
#if _BOOKSIM
    ret = icnt_has_buffer(packet->get_src()->get_idx(), packet->get_size());
#endif
    return ret;
}

void PortOut::push_packet(Packet* packet)
{
    int idx = find_channel(packet);
    assert(idx != ERROR);
    push_packet(idx, packet);    
}

void PortOut::push_packet(int channel_idx, Packet* packet)
{
    packet_queue* buffer = (m_channels.at(channel_idx)).packet_buffer;
    buffer->push(packet);
}

int PortOut::process_port(int channel_idx)
{
  //  if (!credit_available(channel_idx))
  //      return 0;

    if (get_num_waiting(channel_idx) == 0)
        return 0;

//    send_credit(channel_idx);
    send_packet(channel_idx);

    return 1;
}

int PortOut::process_all_ports()
{
    int pckts_sent = 0;
    for (int i=0; i<num_channels; ++i)
    {
        pckts_sent += process_port(i);
    }

    return pckts_sent;
}


void PortOut::send_packet(int channel_idx)
{
    bool avail = true;
    packet_queue* buffer = m_channels[channel_idx].packet_buffer;
    
    Packet* pckt_sending = buffer->front();
    
#if _BOOKSIM    
    avail = port_available(pckt_sending);
    if (avail)
    {
      icnt_push(pckt_sending->get_src()->get_idx(), 
                pckt_sending->get_dest()->get_idx(),
                (int) pckt_sending->get_type(),
                pckt_sending, pckt_sending->get_size());
    }
#endif

    if (avail)
      buffer->pop(); 
}


int PortIn::num_packet_available(int channel_idx)
{
    return (m_channels.at(channel_idx)).packet_buffer->size();
}

int PortIn::num_packet_available()
{
    int pckts_available = 0;
    for (int i=0; i<num_channels; ++i)
    {
        pckts_available += num_packet_available(i);
    }

    return pckts_available;
}

Packet*  PortIn::pop_packet(int channel_idx)
{
    packet_queue* buffer = m_channels[channel_idx].packet_buffer;
    assert(!buffer->empty());

    Packet* pckt_received = buffer->front();
    buffer->pop();
    //return_credit(channel_idx);
    return pckt_received;
}

Packet*  PortIn::pop_packet()
{
    packet_queue* buffer;
    for (vector<channel_t>::iterator it = m_channels.begin();
         it != m_channels.end(); ++it)
    {
        buffer = it->packet_buffer;
        if (!buffer->empty())
        {
            Packet* pckt_received = buffer->front();
            buffer->pop();
            //return_credit(channel_idx);
            return pckt_received;
        }
    }
    return NULL;
}

int PortIn::process_all_ports()
{
    receive_packets();
    return SUCCESS;
}

void PortIn::receive_packets()
{
    Packet* pckt_received = NULL;
    packet_queue* buffer;
    int channel_idx;

    do
    {
#if _BOOKSIM
      pckt_received = (Packet*) icnt_pop(m_node_idx);
#endif
      if (pckt_received != NULL)
      {
          channel_idx = find_channel(pckt_received);
          buffer = m_channels[channel_idx].packet_buffer;
          buffer->push(pckt_received);
      }
    }
    while(pckt_received != NULL);
}


