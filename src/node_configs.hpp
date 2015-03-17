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

#ifndef _NODE_CONFIGS_HPP_
#define _NODE_CONFIGS_HPP_

#include <string>
#include <vector>
#include "packet.hpp"
#include "globals.hpp"
#include "misc.cpp"

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

  private:
      size_t m_size; // Size of the buffer
      size_t m_delay;

      size_t m_num_to_clear; // Packets waiting on the timer
      size_t m_timer;
      size_t m_pckt_num; // Number of packets in the buffer
      size_t m_rqst_pending;

      vector<string> m_packet_ids;
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







#endif
