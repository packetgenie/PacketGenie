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

#include "node_configs.hpp"

PacketBuffer::PacketBuffer(size_t size, size_t delay,
                           vector<string> packet_ids)
  : m_size(size), m_delay(delay), m_packet_ids(packet_ids)
{
    m_num_to_clear = 0;
    m_timer = 0;
    m_pckt_num = 0;
    m_rqst_pending = 0;
}

PacketBuffer::~PacketBuffer()
{
    // Nothing
}

bool PacketBuffer::check()
{
    // Check if the buffer is full
    if (m_size == m_pckt_num)
        return false;
    else
        return true;
}

size_t PacketBuffer::push(Packet *packet)
{

    // Check if the packet coming in is a request packet
    if(packet->get_type() == REQUEST)
        ++m_rqst_pending;

    ++m_pckt_num;
    return m_pckt_num;

}

size_t PacketBuffer::reply_received(ReplyPacket *packet)
{
    --m_pckt_num;
    --m_rqst_pending;
    return m_pckt_num;
}

size_t PacketBuffer::update()
{

    ++m_timer;
    if (m_timer >= m_delay)
    {
        m_pckt_num -= m_num_to_clear;
        m_num_to_clear = m_pckt_num - m_rqst_pending;
        m_timer = 0;
    }

    if (m_num_to_clear == 0)
        m_timer = m_delay;

    return m_pckt_num;
}


/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////
size_t Dram::bank_config(size_t n_banks,
                         double service_time, double arrival_rate)
{
    num_banks = n_banks;
    serv_time = service_time;
    arrv_rate = arrival_rate;
    bank_erlangC = erlang_delay_constant(n_banks, arrv_rate, serv_time);

    return SUCCESS;
}

size_t Dram::timing_config(size_t dram_clk_ratio,
                           size_t RCD, size_t CAS,
                           size_t RP, size_t RAS,
                           double p_rc, double p_co, double p_cc)
{ 
    // Ratio respect to system clock
    clk_ratio = dram_clk_ratio;
    t_rcd = RCD;
    t_cas = CAS;
    t_rp = RP;
    t_ras = RAS;
    prob_row_closed = p_rc;
    prob_conf_open = p_co;
    prob_conf_closed = p_cc;

    return SUCCESS;
}

bool Dram::bank_ready(size_t bank_time)
{
    size_t dclk_bank_time = bank_time/clk_ratio;

    double rand_val = get_rand_decimal();
    double thresh = erlang_prob_wait(dclk_bank_time,
                                     num_banks,
                                     arrv_rate, serv_time, 
                                     bank_erlangC);

    return !(rand_val <= thresh);
}

size_t Dram::generate_access_time()
{
    double rand_num;
    size_t delay_val;

    delay_val = t_cas;
    rand_num = get_rand_decimal();
    if (rand_num <= prob_row_closed)
    {
        delay_val += t_rcd;

        rand_num = get_rand_decimal();
        if (rand_num <= prob_conf_open)
        {
            delay_val += t_rp;
            rand_num = get_rand_decimal();
            if (rand_num <= prob_conf_closed)
                delay_val += t_ras;
        }
    }

    // In system time
    return delay_val; 
}


