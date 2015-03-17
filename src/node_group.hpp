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
