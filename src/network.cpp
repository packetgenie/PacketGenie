// FILE: src/network.cpp  Date Modified: March 17, 2015
// PacketGenie
// Copyright 2014 The Regents of the University of Michigan
// Dong-Hyeon Park, Ritesh Parikh, Valeria Bertacco
// 
// PacketGenie is licensed under the Apache License, Version 2.0
// (the "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//     
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.



#include "network.hpp"
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <boost/lexical_cast.hpp>

#if _BOOKSIM
#include "icnt_wrapper.hpp"
#endif

Network::Network(NetworkType ntwk_type, 
                 size_t dim_x, size_t dim_y)
  : m_ntwk_type(ntwk_type)
{
    m_status = NETWORK_STATUS_PRE_INIT;
    m_ntwk_size = dim_x * dim_y;

    node_list = new vector<Node>();

    group_list = new list<NodeGroup *>();

    sender_map       = new map<string, senders_t>();
    receiver_map     = new map<string, receivers_t>();
    inj_proc_map     = new map<string, InjectionProcess *>();
    traf_pattern_map = new map<string, TrafficPattern *>();    

    dram_map = new map<string,Dram>();

    packet_map = new map<string, Packet*>(); 
    reply_handler_map = new map<string, ReplyPacketHandler *>();

    global_packet_pool.weight_total = 0;
    global_packet_pool.packet_list = list<Packet*>();

    // Create unitialized nodes.
    size_t idx;
    for (idx = 0; idx < m_ntwk_size; ++idx)
    {
        node_list->push_back(Node(idx, m_ntwk_size));
    }
    
}

Network::~Network()
{
    delete node_list;

    // Delete all instances of injection process and traffic pattern
    // in each group.
    for (list<NodeGroup*>::iterator grp_it = group_list->begin();
         grp_it != group_list->end(); ++grp_it)
    {
        delete *grp_it;
    }

    delete group_list;
    delete sender_map;
    delete receiver_map;
    delete dram_map;

    for (map<string,InjectionProcess *>::iterator inj_it = inj_proc_map->begin();
         inj_it != inj_proc_map->end(); ++inj_it)
        delete inj_it->second;

    for (map<string,TrafficPattern *>::iterator tp_it = traf_pattern_map->begin();
         tp_it != traf_pattern_map->end(); ++tp_it)
        delete tp_it->second;
    
    for (map<string,Packet *>::iterator pckt_it = packet_map->begin();
         pckt_it != packet_map->end(); ++pckt_it)
        delete pckt_it->second;

    for (map<string,ReplyPacketHandler *>::iterator rh_it = reply_handler_map->begin();
         rh_it != reply_handler_map->end(); ++rh_it)
        delete rh_it->second;

    delete inj_proc_map;
    delete traf_pattern_map;

    delete packet_map;
    delete reply_handler_map;

#if _BOOKSIM
    if (gBookSim)
        icnt_delete();
#endif

}

int Network::set_traffic_pattern(vector<string> *str_params)
{
    string id = str_params->at(0);
    string tp_type = str_params->at(1);
    int error;

#if _DEBUG
    cout << "Traffic Pattern: id=" << id << ", "
         << "tp_type=" << tp_type << endl;
#endif

    TrafficPattern *traf_pattern;

    if (tp_type == "uniform")
        traf_pattern = new TrafficPatternUniform(id, tp_type);
    else
        return ERROR;

    error = traf_pattern->parse_params(str_params);

    if (error)
        return error;

    // Add the new injection process to the map
    traf_pattern_map->insert(pair<string,TrafficPattern*>(id, traf_pattern));

    if (gPrint)
    {
      cout << "Traffic Pattern: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::set_sender_nodes(vector<string> *str_params)
{
    string id = str_params->front();
    senders_t temp;
    list<int> temp_list;
  
    temp.id = id;

    // Get the list of node indexes.
    int error = process_strvec(str_params, &temp_list, m_ntwk_size);
    if (error)
        return error;

    int node_idx;
    for (list<int>::iterator it = temp_list.begin();
         it != temp_list.end(); ++it)
    {
        node_idx = *it;
        // Append the address of the destination nodes
        (temp.nodes).push_back(&(node_list->at(node_idx)));
    }

    sender_map->insert(pair<string,senders_t>(id, temp));

    if (gPrint)
    {
      cout << "Senders: " << *str_params << endl;

    }

    return SUCCESS;
}

int Network::set_receiver_nodes(vector<string> *str_params)
{
    string id = str_params->front();
    receivers_t temp;
    list<int> temp_list;
  
    temp.id = id;


    // Get the list of node indexes.
    int error = process_strvec(str_params, &temp_list, m_ntwk_size);
    if (error)
        return error;

    int node_idx;
    for (list<int>::iterator it = temp_list.begin();
         it != temp_list.end(); ++it)
    {
        node_idx = *it;
        // Append the address of the destination nodes
        (temp.nodes).push_back(&(node_list->at(node_idx)));
    }

    receiver_map->insert(pair<string,receivers_t>(id, temp));

    if (gPrint)
    {
      cout << "Receivers: " << *str_params << endl;

    }

    return SUCCESS;

}

int Network::set_inj_process(vector<string> *str_params)
{

    // Injection Process ID is the first element
    string id = str_params->at(0);

    // Process type is the second element
    string process_type = str_params->at(1);

    InjectionProcess *inj_proc = InjectionProcess::New(str_params);

    if (inj_proc == NULL)
        return ERROR;

    // Add the new injection process to the map
    inj_proc_map->insert(pair<string,InjectionProcess*>(id, inj_proc));

    if (gPrint)
    {
      cout << "Injection Process: " << *str_params << endl;
    }


    return SUCCESS;


}

int Network::set_packet_type_reply(vector<string> *str_params)
{

    string new_id, new_type;
    Packet* new_pckt;
    
    new_id = str_params->at(0);
    new_type = str_params->at(1);

    // Create reply packet handler
    ReplyPacketHandler *new_handler = ReplyPacketHandler::New(str_params);
    new_pckt = (Packet*) new_handler->get_packet();

    // Put the reply Packet in map
    reply_handler_map->insert(pair<string, ReplyPacketHandler*>
                                  (new_id, new_handler));
    packet_map->insert(pair<string,Packet*>(new_id, new_pckt));


    // Place reply packets at the back. 
    (global_packet_pool.packet_list).push_back(new_pckt);

    if (gPrint)
    {
      cout << "Reply Packet: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::set_packet_type(vector<string> *str_params)
{

    Packet* new_pckt;

    string new_id = str_params->at(0);
    size_t new_size = boost::lexical_cast<size_t>(str_params->at(1));
    double new_weight = boost::lexical_cast<double>(str_params->at(2));

    string need_rply = str_params->at(3);

    // Get the corresponding reply handler if it is a request packet
    map<string, ReplyPacketHandler*>::iterator handler_it;
    if (need_rply == "1" || need_rply == "Y" || need_rply == "y")
    {
         handler_it = reply_handler_map->find(str_params->at(4));
         if(handler_it == reply_handler_map->end())
              return ERROR;
         new_pckt = Packet::New(new_id, new_size, new_weight,
                                NULL, NULL, NULL,
                                handler_it->second, "N/A",
                                REQUEST);
    }
    else
    {
         new_pckt = Packet::New(new_id, new_size, new_weight,
                                NULL, NULL, NULL,
                                NULL, "N/A",
                                DEFAULT);
    }

    packet_map->insert(pair<string,Packet*>(new_pckt->get_id(), new_pckt));

    global_packet_pool.weight_total += new_pckt->get_weight();
    // Place new packet in front of reply packets. 
    (global_packet_pool.packet_list).push_front(new_pckt);

    if (gPrint)
    {
      cout << "Packet: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::set_dram_bank_config(vector<string> *str_params)
{
    string dram_id;
    size_t n_banks;
    double serv_time, arrv_rate;

    dram_id = str_params->at(0);
    n_banks = boost::lexical_cast<size_t>(str_params->at(1));
    serv_time = boost::lexical_cast<double>(str_params->at(2));
    arrv_rate = boost::lexical_cast<double>(str_params->at(3));

    Dram new_dram(dram_id);

    new_dram.bank_config(n_banks, serv_time, arrv_rate);
    dram_map->insert(pair<string,Dram>(dram_id, new_dram));
    
    if (gPrint)
    {
      cout << "Dram Bank: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::set_dram_timing_config(vector<string> *str_params)
{
    string dram_id;
    size_t dram_clk;
    size_t t_rcd, t_cas, t_rp, t_ras;
    double p_rc, p_co, p_cc;

    dram_id = str_params->at(0);
    dram_clk = boost::lexical_cast<size_t>(str_params->at(1));
    t_rcd = boost::lexical_cast<double>(str_params->at(2));
    t_cas = boost::lexical_cast<double>(str_params->at(3));
    t_rp  = boost::lexical_cast<double>(str_params->at(4));
    t_ras = boost::lexical_cast<double>(str_params->at(5));
    p_rc  = boost::lexical_cast<double>(str_params->at(6));
    p_co  = boost::lexical_cast<double>(str_params->at(7));
    p_cc  = boost::lexical_cast<double>(str_params->at(8));

    map<string,Dram>::iterator dram_it;
    dram_it = dram_map->find(dram_id);
    if (dram_it == dram_map->end())
        return ERROR;

    (dram_it->second).timing_config(dram_clk,
                                    t_rcd, t_cas, t_rp, t_ras,
                                    p_rc, p_co, p_cc);

    if (gPrint)
    {
      cout << "Dram Timing: " << *str_params << endl;
    }
    return SUCCESS;
}


int Network::set_node_group(vector<string> *str_params)
{
    int error;

    string grp_id = str_params->front();
    string inj_id = str_params->at(1);
    string tp_id = str_params->at(2);
    string sen_id = str_params->at(3);
    string rec_id = str_params->at(4);

    map<string,InjectionProcess*>::iterator inj_it;
    map<string,TrafficPattern*>::iterator tp_it;
    map<string,senders_t>::iterator sen_it;
    map<string,receivers_t>::iterator rec_it;

    inj_it = inj_proc_map->find(inj_id);
    tp_it = traf_pattern_map->find(tp_id);
    sen_it = sender_map->find(sen_id);
    rec_it = receiver_map->find(rec_id);

    // Check if the ids were found.
    if ( inj_it == inj_proc_map->end()     ||
         tp_it  == traf_pattern_map->end() ||
         sen_it == sender_map->end()       ||
         rec_it == receiver_map->end()     )
        return ERROR;

    // Now get the packets
    packet_pool_t packets;
    vector<string> packet_ids;
    map<string, Packet*>::iterator handler_it;

    // Extract packets specified in < >
    error = extract_str(str_params, packet_ids);
    if (error)
       return error;

    packets.weight_total = 0;
    for ( vector<string>::iterator packet_it = packet_ids.begin();
          packet_it != packet_ids.end(); ++packet_it)
    {
        handler_it = packet_map->find(*packet_it);
        if(handler_it == packet_map->end())
            return ERROR;
        
        (packets.packet_list).push_back(handler_it->second);
        packets.weight_total += (handler_it->second)->get_weight();
    }

    // Assign parameters to the new group
    NodeGroup * new_group = new NodeGroup(grp_id,
                                          inj_it->second, tp_it->second, 
                                          &(sen_it->second),
                                          &(rec_it->second), 
                                          packets);

    group_list->push_back(new_group);
  
    if (gPrint)
    {
      cout << "Node Group: " << *str_params << endl;
    }


    return SUCCESS;
}

int Network::config_nodes(vector<string> *str_params)
{
    int error;
    string config_id   = str_params->front();
    string config_type = str_params->at(1);
    list<int> temp_list;
    list<Node*> nodes;

    // Get the list of node indexes.
    error = process_strvec(str_params, &temp_list, m_ntwk_size);
    if (error)
        return error;

    // Get the list of nodes
    for (list<int>::iterator it = temp_list.begin();
         it != temp_list.end(); ++it)
    {
        nodes.push_back(&(node_list->at(*it)));
    }

    if (config_type == "buffer")
    {
        error = config_nodes_buffer(str_params, nodes);
        if (error)
        {
#if _DEBUG
            cout << "ERROR: Error configuring buffer " << config_id
                 << endl;
#endif
            return error;
        }
    }
    else if (config_type == "dram")
    {
        error = config_nodes_dram(str_params, nodes);
    }
    else if (config_type == "clock")
    {
        error = config_nodes_clock(str_params, nodes);
    }
    else
        return ERROR;


    return error;

}

int Network::config_nodes_buffer(vector<string> *str_params, list<Node*> &nodes)
{
    int error;
    // [name] [type] [buffer_size] [delay_for_non-reply_packets] < [packets] >
    size_t buffer_size = boost::lexical_cast<size_t>(str_params->at(2));
    size_t delay  = boost::lexical_cast<double>(str_params->at(3));
    
    // Now get the packets
    vector<string> packet_ids;
    map<string, Packet*>::iterator handler_it;

    // Extract packets specified in < >
    error = extract_str(str_params, packet_ids);
    if (error)
       return error;

    // Make sure these ids have been declared.
    for ( vector<string>::iterator packet_it = packet_ids.begin();
          packet_it != packet_ids.end(); ++packet_it)
    {
        handler_it = packet_map->find(*packet_it);
        if(handler_it == packet_map->end())
        {
#if _DEBUG
            cout << "ERROR: PACKET " << *packet_it << " NOT FOUND" << endl;
#endif
            return ERROR;
        }
    }

    PacketBuffer new_buffer (buffer_size, delay, packet_ids);
    // Go through each node and config.
    for (list<Node*>::iterator node_it = nodes.begin();
         node_it != nodes.end(); ++node_it)
    {
        error = (*node_it)->config_buffer(new_buffer);
        if (error)
            return error;
    }
   
    if (gPrint)
    {
        cout << "Buffer Config: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::config_nodes_dram(vector<string> *str_params, list<Node*> &nodes)
{
    int error;
    // [name] [type] [dram_name]
    string dram_id  = str_params->at(2);
    
    map<string, Dram>::iterator dram_it;

    dram_it = dram_map->find(dram_id);
    if(dram_it == dram_map->end())
    {
        cerr << "ERROR: DRAM " << dram_id << " NOT FOUND" << endl;
        return ERROR;
    }
    
    // Go through each node and config.
    for (list<Node*>::iterator node_it = nodes.begin();
         node_it != nodes.end(); ++node_it)
    {
        error = (*node_it)->config_dram(dram_it->second);
        if (error)
            return error;
    }
   

    if (gPrint)
    {
        cout << "Dram Config: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::config_nodes_clock(vector<string> *str_params, list<Node*> &nodes)
{
    // [name] [type] [node_clk]
    size_t node_clk  = boost::lexical_cast<size_t>(str_params->at(2));
    
    // Go through each node and config.
    for (list<Node*>::iterator node_it = nodes.begin();
         node_it != nodes.end(); ++node_it)
    {
         (*node_it)->change_clock_ratio(node_clk);
    }

    if (gPrint)
    {
        cout << "Clock Config: " << *str_params << endl;
    }

    return SUCCESS;
}

int Network::init_sim(size_t cycle_end)
{
    if (m_status != NETWORK_STATUS_PRE_INIT)
          return ERROR; //ERROR;

    // INITIALIZE GLOBAL CLOCK
    g_clock = 0;
    sim_end = cycle_end;

    g_traffic_report = new TrafficReportHandler(m_ntwk_size, sim_end);

    // Normalize injection rate to per-step, per-node.
    //double inj_norm = inj_rate * 
    //                  ((double) step_size) / ((double) m_ntwk_size);

#if _BOOKSIM
    if (gBookSim)
    {
        if (gPrint)
        {
            cout << endl
                 << "=== Initializing Booksim Interconnect ===" << endl;
        }
        icnt_create(m_ntwk_size);
        icnt_init();
    }
#endif

    if (gPrint)
    {
        cout << endl
             << "=== Initializing Simulator ===" << endl
             << "Simclk end: " << sim_end << endl;
    }


    // Initialize the injection processes in each node group.
    for (list<NodeGroup*>::iterator grp_it = group_list->begin();
         grp_it != group_list->end(); ++grp_it)
    {
         (*grp_it)->init();
    }

    m_status = NETWORK_STATUS_READY;

    if (gPrint)
    {
        cout << "=== Initialization Complete ===" << endl << endl;
    }

    return SUCCESS;
}
                                              
          
int Network::step()
{
#if _DEBUG
        cout << "<<<<< CYCLE " << g_clock << " START <<<<" << endl;
#endif

    // Check network status. 
    if (m_status == NETWORK_STATUS_READY)
    {
        m_status = NETWORK_STATUS_ACTIVE;
    }
    else if (m_status == NETWORK_STATUS_DONE)
    {
        return SUCCESS;
    }
    else if (m_status != NETWORK_STATUS_ACTIVE 
              && m_status != NETWORK_STATUS_WRAPPING_UP)
    {
       return ERROR; 
    }

    // Go through each node in the list and test.

    int error;
    NodeGroup * curr_grp;

    // Steps through the nodes by looking through each group list
    // and testing for packet creation. Some nodes will be tested
    // multiple times if they are in more than one group.

    // Loop through the group list.
    for (list<NodeGroup*>::iterator grp_it = group_list->begin();
         grp_it != group_list->end(); ++grp_it)
    {
        curr_grp = *grp_it;      
#if _DEBUG
        cout << "At Node Group: " << curr_grp->get_id() << endl;
#endif
        if (m_status == NETWORK_STATUS_ACTIVE || !gWaitForReply)
        {
            error = curr_grp->wake_each_node();
            if (error)
                return error;
        }

        error = curr_grp->process_reply_queue();
        if (error)
            return error;
    }

    error = process_all_packet_queues();
    if (error)
    {
        if (gPrint)
            cout << "ERROR: Error processing packet queues" << endl;
        return error;
    }


#if _DEBUG
        cout << ">>>> CYCLE " << g_clock << " DONE >>>>" << endl;
#endif


    // Check and increment clock
    if (g_clock >= sim_end)
    {
        m_status = NETWORK_STATUS_DONE;

        if (gWaitForReply)
        {
            for (list<NodeGroup*>::iterator grp_it = group_list->begin();
                 grp_it != group_list->end(); ++grp_it)
            {
                curr_grp = *grp_it;      
                if (!curr_grp->reply_queue_empty())
                {
                    m_status = NETWORK_STATUS_WRAPPING_UP;
                    break;
                }
            }
            
            if (gBookSim)
            {
                if (icnt_busy() || (!no_outstanding_reply()) ) 
                    m_status = NETWORK_STATUS_WRAPPING_UP;
            }
        }
    }

    ++g_clock;
    update_all_clocks();

    return SUCCESS;
}

bool Network::no_outstanding_reply()
{
    NodeGroup * curr_grp;
    for (list<NodeGroup*>::iterator grp_it = group_list->begin();
         grp_it != group_list->end(); ++grp_it)
    {
        curr_grp = *grp_it;
        if (!curr_grp->reply_queue_empty())
        {
            return false;
        }
    }
    return true;
}

int Network::process_all_packet_queues()
{
    int error;
    vector<Node>::iterator node_it;

    for (node_it = node_list->begin();
         node_it != node_list->end(); ++node_it)
    {
        error = node_it->process_send_queue();
        if (error<0)
        {
            if (gPrint)
            {
                cout << "ERROR: Error processing send queue for node "
                      << node_it->get_idx();
            }
            return error;
        }
    }

#if _BOOKSIM
    if (gBookSim)
    {
        // Advance Booksim Network 
        icnt_transfer();
    }
#endif

    for (node_it = node_list->begin();
         node_it != node_list->end(); ++node_it)
    {
        error = node_it->process_recv_queue();
        if (error<0)
        {
            if (gPrint)
            {
                cout << "ERROR: Error processing receive queue for node "
                      << node_it->get_idx();
            }
            return error;
        }
    }
    return SUCCESS;
}

void Network::update_all_clocks()
{
    for (vector<Node>::iterator node_it = node_list->begin();
         node_it != node_list->end(); ++node_it)
    {
        node_it->update_clock();
    }
}

NetworkStatus Network::get_status()
{
    return m_status;
}

int Network::print_traffic_records()
{
    cout << "<< << << Simulation Statistics >> >> >>" << endl
         << "Network Size: " << m_ntwk_size << endl
         << "Cycles Simulated: " << sim_end << endl
         << "Packets Generated: " << g_traffic_report->get_total_num_packets() << endl
         << "Avg. Network Injection Rate: " << g_traffic_report->get_avg_inj_rate() << endl;

    cout << "=== Group Statistics ===" << endl;
    for (list<NodeGroup*>::iterator grp_it = group_list->begin();
         grp_it != group_list->end(); ++grp_it)
    {
        (*grp_it)->print_stat();
        cout << endl;
    }
    cout << "=== End Group Statistics ===" << endl << endl;

    cout << "=== Node Statistics ===" << endl;
    for (vector<Node>::iterator node_it = node_list->begin();
         node_it != node_list->end(); ++node_it)
    {
        cout << node_it->get_stat();
    }
    cout << "=== End Node Statistics ===" << endl << endl;
    cout << "<< << << End Simulation Statistics >> >> >>" << endl << endl;
    
    cout << g_traffic_report->generate_str_report() << endl;


    return SUCCESS;

}


int Network::get_traffic_records(string &printout)
{

   g_traffic_report->get_all_records(printout);

   return SUCCESS;

}


#if _BOOKSIM

int Network::config_BookSim(const char* const config_file)
{
    gBookSim = true;
    icnt_wrapper_init(config_file);
    return config_nodes_default_port();
}

int Network::config_nodes_default_port()
{
    int error=0;

    for (vector<Node>::iterator node_it = node_list->begin();
         node_it != node_list->end(); ++node_it)
    {
        error = node_it->config_port(true);

        if (error<0)
            return error;
    }

    if (gPrint)
    {
        cout << "Default ports configured for all nodes" << endl;
    }
    return SUCCESS;
}
#endif

