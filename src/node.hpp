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
#include "traffic_report_handler.hpp"

#if _BOOKSIM
#include "port.hpp"
#endif

//class tf_gen::Node;
//class tf_gen::NodeGroup;
class ReplyPacketHandler;
class PacketBuffer;
class Dram;

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
      int config_buffer(PacketBuffer &new_buffer);

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
      list<PacketBuffer> *buffer_list;
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
