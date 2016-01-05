// FILE: src/network.hpp  Date Modified: March 17, 2015
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

      long next_event_time;
      list<NodeGroup*> *group_list; 
      list<NodeGroup*> *active_groups; 
      list<NodeGroup*> *waiting_groups; 
      list<NodeGroup*> *finished_groups; 

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
