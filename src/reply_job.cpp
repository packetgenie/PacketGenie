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

