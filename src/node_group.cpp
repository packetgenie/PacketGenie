// Copyright (c) 2015, Dong-hyeon Park, Ritesh Parikh Valeria Bertacco
// University of Michigan
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// Redistributions in binary form must reproduce the above copyright notice, this
// list of conditions and the following disclaimer in the documentation and/or
// other materials provided with the distribution.
// Neither the name of The University of British Columbia nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "node_group.hpp"
#include <cstdlib>
#include <sstream>
#include <boost/lexical_cast.hpp>


NodeGroup::NodeGroup(string id,
                     InjectionProcess *inj_proc,
                     TrafficPattern * traf_pattern,
                     senders_t *src_list,
                     receivers_t *dest_list,
                     packet_pool_t packet_pool)
  : m_id(id), m_senders(src_list), m_receivers(dest_list), m_packet_pool(packet_pool)
{
    // Create unique instances of inj_proc and traffic pattern
    m_nodes = new list<Node *>(src_list->nodes);

    // Need to clone so we keep the inheritance.
    m_inj_proc = inj_proc->clone();
    m_tp = traf_pattern->clone();
    m_rply_q = new queue<ReplyJob*>();

    stat_tot_pckt_gen = 0;
    stat_avg_inj_rate = 0;
    
 }

NodeGroup::~NodeGroup()
{
    delete m_inj_proc;
    delete m_tp;
    delete m_nodes;   
    
    // Empty the job queue if it is not empty.
    while(!m_rply_q->empty())
    { 
      delete m_rply_q->front();
      m_rply_q->pop(); 
    }

    delete m_rply_q;
}


int NodeGroup::init()
{
    size_t grp_size = m_nodes->size();
    // Initialize InjectionProcess
    m_inj_proc->init(grp_size);
    // Initialize TrafficPattern
    m_tp->init(grp_size);

  /*
    // Need to set them after they are cloned, node groups only have the pointers.
    for(list<Packet*>::iterator packet_it = (m_packet_pool.packet_list).begin();
        packet_it != (m_packet_pool.packet_list).end(); ++packet_it)
    {
        packet_it->set_node_group(this);
    }
  */

    if (gPrint)
    {
         cout << "Initializing node group: " << m_id << endl
              << "Injection Process: " << m_inj_proc->get_id()
              << "\tType: " << m_inj_proc->get_type() << endl
              << "Traffic Pattern: " << m_tp->get_id()
              << "\tType: " << m_tp->get_type() << endl
              << "Senders: " << m_senders->id
              << "\tReceivers: " << m_receivers->id << endl
              << "Group Size: " << grp_size << endl << endl;
    }

    return SUCCESS;
}
                                              
          
int NodeGroup::process_reply_queue()
{

    int node_status;
    traffic_incident_t traffic_buffer;

    ReplyJob *curr_job;

    size_t q_size = m_rply_q->size();
    for (size_t i=0; i < q_size; ++i)
    {
         curr_job = m_rply_q->front();
         // Invoke node to send a reply packet
         node_status = 
            (curr_job->get_replier())->send_reply_packet(curr_job,
                                                         this);
         m_rply_q->pop();
         // If packet was generated successfully
         if (node_status == 1)
         {
             // Delete the job
             delete curr_job;
         }
         // If packet was not generated, put it back in the queue
         else if (node_status == 0)
             m_rply_q->push(curr_job);
         else
         {
             // ERROR. Delete the job.
             delete curr_job;
             return node_status;
         }
    }
    return SUCCESS;
}
      
 
int NodeGroup::wake_each_node()
{
    int status;
    traffic_incident_t traffic_buffer;

    // Loop through the nodes in the group.
    for (list<Node *>::iterator it = m_nodes->begin();
         it != m_nodes->end(); ++it)
    {
         // Test the node.
         status = (*it)->wake_node(this);

         if (status < 0) // ERROR occured.
             return status;
    }

    return SUCCESS;
}

void NodeGroup::push_reply_job(ReplyJob *new_job)
{
    // The reply_job was created and allocated by the node
    // requesting the job
    m_rply_q->push(new_job);
}

Packet* NodeGroup::pick_packet()
{
    double w_total = m_packet_pool.weight_total;
    list<Packet*> *packet_list = &(m_packet_pool.packet_list);

    // Get a random value from 0~w_total.
    double rand_val = w_total * get_rand_decimal();
    double w_bound = 0;

    // Find out which packet was chosen by going through the weights 
    // of each packet.
    for (list<Packet*>::iterator pckt_it = packet_list->begin();
         pckt_it != packet_list->end(); ++pckt_it)
    {
        w_bound += (*pckt_it)->get_weight();

        if (rand_val < w_bound)
            return *pckt_it;
    }

    return NULL;
}

Node* NodeGroup::pick_dest(size_t rqst_idx)
{
    vector<Node*> * pool = &(m_receivers->nodes);
    size_t pool_size = pool->size();
    Node * ret;
     
    // destinations in this list.
    size_t rand_idx;
    do
    {
        rand_idx = rand() % pool_size;
        ret = pool->at(rand_idx);

    } while(ret->get_idx() == rqst_idx);

    // DEBUGGING OUTPUT
#if _Debug
        cout << "Destination Chosen: " << ret->get_idx() << endl;
#endif

    return ret;    
}

void NodeGroup::print_stat()
{
    cout << "Node Group: " << m_id << endl
         << "Group Size: " << get_size() << endl
         << "Total Packets Generated: " << stat_tot_pckt_gen << endl
         << "Avg. Group Injection Rate: " << stat_avg_inj_rate/g_clock << endl;
    // Loop through the nodes in the group.
    for (list<Node *>::iterator it = m_nodes->begin();
         it != m_nodes->end(); ++it)
    {
         cout << (*it)->get_stat();
    }
}

