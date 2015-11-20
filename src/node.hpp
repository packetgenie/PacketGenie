// FILE: src/node.hpp  Date Modified: March 17, 2015
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



#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <list>
#include "globals.hpp"
#include "misc.cpp"
#include "injection_process.hpp"
#include "traffic_pattern.hpp"
#include "packet.hpp"
#include "reply_job.hpp"
#include "node_configs.hpp"
#include "traffic_report_handler.hpp"

#if _BOOKSIM
#include "port.hpp"
#endif

//class tf_gen::Node;
//class tf_gen::NodeGroup;
class ReplyPacketHandler;
//class PacketBuffer;
//class Dram;

namespace tf_gen
{

  struct senders_t
  {
      string id;
      list<Node *> nodes;
  };

  struct receivers_t
  {
      string id;
      vector<Node *> nodes;
  };

  struct node_config
  {
      bool buffer;
      bool dram;
      bool port;
  };

  enum NodeStatus
  {
      NODE_STATUS_ACTIVE = 0,
      NODE_STATUS_INACTIVE,
  };

  class Node
  {
    public:
      Node(size_t idx, size_t network_size);
      virtual ~Node();

      void change_clock_ratio(size_t clock_ratio)
      {
          m_clk_ratio = clock_ratio;
      }

      size_t update_clock();

      size_t get_idx()
      {
          return m_idx;
      }

      size_t get_size()
      {
          return m_ntwk_size;
      }

      // Send a reply packet
      // Output:  0   - No packet generated
      //          1   - Generated a packet
      //         -1   - error
      int send_reply_packet(ReplyJob * reply_job, NodeGroup *group);

      // Wake a node and test if it will generate a packet
      // Output:  0   - No packet generated
      //          1   - Generates a packet
      //         -1   - error
      int wake_node(NodeGroup * group);

      // for Node Config - Buffer
      int config_buffer(PacketBuffer* new_buffer);

      // for Node Config - Dram
      bool is_dram()
      { return m_config.dram; }
      int config_dram(Dram &new_dram);
      bool bank_ready(size_t bank_time);
      size_t generate_access_time();

      int process_packet_queues();

      int process_send_queue();
      int process_recv_queue();
      void receive_pckt(Packet *packet); // May end up doing more.
      string get_stat();

#if _BOOKSIM
      int config_port(bool set_default);
      int config_port_channel(vector<string> *packet_ids);
#endif

    private:
      Node * get_dest(receivers_t *receivers);

      int test_for_packet(NodeGroup * group);
      Packet* pick_packet(packet_pool_t * pool);
      void push_packet_to_queue(Packet *packet);
      int generate_packet(NodeGroup* group);
      void create_traffic_incident(Packet *new_packet);
      void process_incoming_packet(Packet *packet);

      // for Node Config - Buffer
      void update_buffers();
      PacketBuffer * find_matching_buffer(string pckt_id);

      // Variables
      size_t m_idx;
      size_t m_ntwk_size;
      size_t m_clk;
      size_t m_clk_ratio; // global_clock/node_clock

      NodeStatus m_status;
      node_config m_config;

      size_t stat_pckt_sent;
      size_t stat_pckt_recv;

      // Variables for Node Config - Buffer
      list<PacketBuffer*> *buffer_list;
      map<string,PacketBuffer*> *buffer_map;

      // Variables for Node Config - Dram
      Dram* m_dram;

      // Packet Queues
      queue<Packet*> send_queue;
      queue<Packet*> recv_queue;

#if _BOOKSIM
      PortIn*     m_port_in;
      PortOut*    m_port_out;
#endif
     
  };
}

#endif
