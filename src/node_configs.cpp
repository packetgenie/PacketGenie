// FILE: src/node_configs.cpp  Date Modified: March 17, 2015
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


