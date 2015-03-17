// FILE: src/reply_job.cpp  Date Modified: March 17, 2015
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



#include "reply_job.hpp"

ReplyJob* ReplyJob::New(ReplyPacketHandler *reply_handler,
                        RequestPacket* request_packet,
                        ReplyPacket* reply_packet,
                        string type)
{
    ReplyJob * new_job;
    reply_packet->set_rqst_pckt_id(request_packet->get_id());
    reply_packet->set_src(request_packet->get_dest());
    reply_packet->set_dest(request_packet->get_src());
    reply_packet->set_node_group(request_packet->get_grp());

    if (type == "Dram")
    {
        new_job = new ReplyJobDram(reply_handler, reply_packet);
    }
    else
    {
        new_job = new ReplyJob(reply_handler,
                               reply_packet,"Default");
    }
    return new_job;
}


ReplyJobDram::ReplyJobDram(ReplyPacketHandler *reply_handler,
                           ReplyPacket* reply_packet)
  : ReplyJob(reply_handler, reply_packet,"Dram")
{
    m_status = IN_NETWORK;
    m_bank_count = 0;
    m_access_count = 0;
}

size_t ReplyJobDram::inc_count()
{
    ++m_wait_count;

    if (m_status == WAITING_BANK)
    {
        ++m_bank_count;
    }
    else if (m_status == WAITING_ACCESS)
    {
        ++m_access_count;
    }

    return m_wait_count;
}

