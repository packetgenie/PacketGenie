// FILE: src/node_group.hpp  Date Modified: March 17, 2015
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



#ifndef _NODE_GROUP_HPP_
#define _NODE_GROUP_HPP_

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <list>
#include "globals.hpp"
#include "misc.cpp"
#include "injection_process.hpp"
#include "traffic_pattern.hpp"
#include "node.hpp"

#include "packet.hpp"
#include "reply_job.hpp"
//class tf_gen::Node;

namespace tf_gen
{
  class NodeGroup
  {
    public:
      NodeGroup(string id,
                InjectionProcess * inj_proc,
                TrafficPattern * traf_pattern,
                senders_t * src_list,
                receivers_t * dest_list,
                packet_pool_t packet_pool);

      virtual ~NodeGroup();

      string get_id()
      { return m_id; }

      size_t get_size()
      { return m_nodes->size(); }

      double get_inj_rate(size_t simclk)
      { return m_inj_proc->get_inj_rate(simclk); }

      double get_traf_pattern(size_t m_idx)
      { return m_tp->get_traf_pattern(m_idx); }

      packet_pool_t * get_packet_pool()
      { return &m_packet_pool; }

      senders_t * get_senders()
      { return m_senders; }

      receivers_t * get_receivers()
      { return m_receivers; }

      void pckt_generated()
      { ++stat_tot_pckt_gen; }

      bool reply_queue_empty()
      { return m_rply_q->empty(); }

      long get_start_cyc()
      { return m_inj_proc->get_start(); }

      long get_end_cyc()
      { return m_inj_proc->get_end(); }

      bool is_active()
      { 
         if (m_inj_proc->get_start() > g_clock)
           return false;
         if (m_inj_proc->get_end() < g_clock)
           return false;
         else
           return true;
      }

      long next_event()
      {
         if (m_inj_proc->get_start() > g_clock)
           return m_inj_proc->get_start();
	 else if (m_inj_proc->get_end() > g_clock)
           return m_inj_proc->get_end();
	 else
	   return g_clock_end+1;
      }

      int init();

      int process_reply_queue();

      int wake_each_node();

      Packet* pick_packet();
      Node* pick_dest(size_t rqst_idx);

      void push_reply_job(ReplyJob *new_job);

      void print_stat();

    private:
      
      string m_id;
      list<Node *>*       m_nodes;
      InjectionProcess*   m_inj_proc;
      TrafficPattern*     m_tp;
      senders_t*          m_senders;
      receivers_t*        m_receivers;
      packet_pool_t       m_packet_pool;
      queue<ReplyJob*>* m_rply_q; // Queue of reply jobs that need to 
                                   // be processed.

      size_t stat_tot_pckt_gen;
      double stat_avg_inj_rate;
     
  };

}

#endif
