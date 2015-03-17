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

#ifndef _NETWORK_HPP_
#define _NETWORK_HPP_


#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <list>

#include "node.hpp"
#include "node_configs.hpp"
#include "node_group.hpp"
#include "globals.hpp"
#include "misc.cpp"
#include "injection_process.hpp"
#include "traffic_pattern.hpp"
#include "packet_handler.hpp"

#include "packet.hpp"
#include "reply_job.hpp"


namespace tf_gen
{

  enum NetworkType
  {
      NETWORK_MESH = 0,
      //NETWORK_CROSSBAR,
  };


  enum NetworkStatus
  {
       NETWORK_STATUS_READY = 0,
       NETWORK_STATUS_DONE,
       NETWORK_STATUS_ACTIVE,
       NETWORK_STATUS_WRAPPING_UP,
       NETWORK_STATUS_PRE_INIT,
       NETWORK_STATUS_ERROR,
  };

  class Network
  {
    public:
      Network(NetworkType ntwk_type, 
              size_t dim_x, size_t dim_y);

      virtual ~Network();

      int set_inj_process(vector<string> *str_params);
      int set_sender_nodes(vector<string> *idx_list);
      int set_receiver_nodes(vector<string> *idx_list);  
      int set_traffic_pattern(vector<string> *str_params);

      int set_packet_type_reply(vector<string> *str_params);
      int set_packet_type(vector<string> *str_params); 

      int set_node_group(vector<string> *str_params);
      int config_nodes(vector<string> *str_params);
    
#if _BOOKSIM
      int config_BookSim(const char* const config_file);
#endif

      int init_sim(size_t sim_end);

      int step();
      int get_traffic_records(string & printout);
      int print_traffic_records();

      NetworkStatus get_status();

      int set_dram_bank_config(vector<string> *str_params);
      int set_dram_timing_config(vector<string> *str_params);

      bool no_outstanding_reply();

    private:

      int process_all_packet_queues();
      void update_all_clocks();

      int config_nodes_buffer(vector<string> *str_params,
                              list<Node *> &nodes);
      int config_nodes_dram(vector<string> *str_params,
                            list<Node*> &nodes);
      int config_nodes_clock(vector<string> *str_params,
                             list<Node*> &nodes);

#if _BOOKSIM
      int config_nodes_default_port();
//      int config_nodes_port(vector<string> *str_params,
//                            list<Node*> &nodes);
#endif

      vector<Node> *node_list;
      size_t m_ntwk_size;

      NetworkType m_ntwk_type;
      NetworkStatus m_status;

      double base_inj_per_step; // Base injection rate, normalized
                                // by step size
      size_t sim_end;

      list<NodeGroup*> *group_list; 

      map<string, senders_t> *sender_map;
      map<string, receivers_t> *receiver_map;
      map<string, InjectionProcess *> *inj_proc_map;
      map<string, TrafficPattern *> *traf_pattern_map;
      
      map<string, Packet*> *packet_map;
      map<string, ReplyPacketHandler *> *reply_handler_map;

      map<string, Dram> *dram_map;
      
      // Packet pool of all existing packets.
      packet_pool_t global_packet_pool;    
  };
}
#endif
