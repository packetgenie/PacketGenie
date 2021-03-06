// FILE: src/port.hpp  Date Modified: March 17, 2015
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



#ifndef _NODE_PORT_HPP_
#define _NODE_PORT_HPP_

#include "globals.hpp"
#include <queue>
#include <map>
#include <stack>
#include "packet.hpp"


namespace tf_gen
{

  typedef queue<Packet*> packet_queue;

  struct channel_t
  {
      // For PortOut, credit keeps track of number of outstanding credits.
      // For PortIn, credit keeps track of the total number of credit.
      //              (size of buffer)
      packet_queue* packet_buffer;

  };

  class Port
  {
      protected:
        int m_node_idx;
        int default_queue_idx;
        int num_channels;
        vector<channel_t> m_channels;
        map<string, int> channel_map;

        static Packet* null_pckt;

      public:
        Port();
        Port(int idx);
        virtual ~Port();

        // Default channel. Any packet can go through.
        virtual int setup_default_channel(); 
        // Default channel. Any packet can go through.
        virtual int setup_channel(); 
        // Custom channel specific to certain packets.
        virtual int setup_channel(vector<string> *packet_ids); 


        // Functions for Node.
        // Find appropriate channel index for given packet.
        virtual int find_channel(Packet* packet);

        virtual size_t get_num_channels();
        virtual size_t get_num_waiting(int channel_idx);
  };

  // PortOut sends packet from Node to Channel.
  class PortOut: public Port
  {
      public:
        PortOut(int idx)
          : Port(idx) { };
      
        // Check the available credits on a channel
        bool port_available(Packet* packet);
        // Push given packet to Port.
        void push_packet(Packet* packet);
        void push_packet(int channel_idx, Packet* packet);

        // Try sending *a* packet for a given channel.
        // If 0, no packets were sent.
        // If packets were sent, return # packets sent.
        int process_port(int channel_idx);
        // Run process_port on each port.
        int process_all_ports();

        //void receive_credit(int channel_idx);

      private:
        //void send_credit(int channel_idx);:w

        void send_packet(int channel_idx);
  };

  class PortIn: public Port
  {
      public:
        PortIn(int idx)
          : Port(idx) { };

        // Check if there is a packet to be taken.
        int num_packet_available(int channel_idx);
        int num_packet_available();

        // Get the received packet.
        Packet* pop_packet(int channel_idx);
        Packet* pop_packet();

        int process_all_ports();

      private:
        //void return_credit(int channel_idx);
        //void receive_credits(int channel_idx);
        void receive_packets();

  };

}

#endif
