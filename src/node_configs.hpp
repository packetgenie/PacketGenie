// FILE: src/node_configs.hpp  Date Modified: March 17, 2015
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



#ifndef _NODE_CONFIGS_HPP_
#define _NODE_CONFIGS_HPP_

#include <string>
#include <vector>
#include <queue>
#include "packet.hpp"
#include "globals.hpp"
#include "misc.cpp"

namespace tf_gen
{

enum BufferType
{
	TYPE_BUFFER = 0,
	TYPE_QUEUE	
};

class PacketBuffer
{
  public:
      PacketBuffer(size_t size, size_t delay, 
                   vector<string> packet_ids);
      virtual ~PacketBuffer();

      bool check();
      size_t push(Packet *packet);
      size_t reply_received(ReplyPacket *packet);

      size_t update();

      size_t get_num_packets()
      {
          return m_pckt_num;
      }

      size_t get_num_requests()
      {
          return m_rqst_pending;
      }

      vector<string> get_packet_ids() 
      { 
          return m_packet_ids; 
      }

      BufferType get_type()
      {	  return TYPE_BUFFER; }
 

  protected:
      size_t m_size; // Size of the buffer
      size_t m_delay;

      size_t m_num_to_clear; // Packets waiting on the timer
      size_t m_timer;
      size_t m_pckt_num; // Number of packets in the buffer
      size_t m_rqst_pending;
  private:
      vector<string> m_packet_ids;
};


class PacketQueue: public PacketBuffer
{
  public:
      PacketQueue(size_t size, size_t delay, 
                   vector<string> packet_ids);
      bool check();
      size_t push(Packet *packet);
      size_t reply_received(ReplyPacket *packet);

      size_t update();

      Packet* front();
      BufferType get_type()
      {	  return TYPE_QUEUE; }

  private:
      queue<Packet *> m_packet_queue;
      bool m_front_active;

};


      // keep an actualy list of packets as a buffer if we want to be 
      // super accurate
      // Or at least a list with a counter.

class Dram
{
  public:
      Dram(string id)
        : m_id(id) { };
      virtual ~Dram() { };

      size_t get_clk_ratio()
      { return clk_ratio; }
      size_t bank_config(size_t n_banks, 
                         double service_time, double arrival_rate);
      size_t timing_config(size_t dram_clk_ratio,
                           size_t RCD, size_t CAS,
                           size_t RP, size_t RAS,
                           double p_rc, double p_co, double p_cc);

      bool bank_ready(size_t bank_time);
      size_t generate_access_time();


  private:
      string m_id;
      size_t m_cycle;
      
      size_t clk_ratio;
      size_t num_banks;
      double serv_time, arrv_rate;
      size_t t_rcd, t_cas, t_rp, t_ras;
      double prob_row_closed, prob_conf_open, prob_conf_closed;

      double bank_erlangC;   
    
};


}




#endif
