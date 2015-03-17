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

#ifndef _PACKET_HANDLER_HPP_
#define _PACKET_HANDLER_HPP_

#include <string>
#include <map>
#include <vector>
#include <queue>
#include <fstream>
#include <list>
#include <cmath>
#include <ostream>
#include "globals.hpp"
#include "misc.cpp"
#include "node.hpp"

#include "packet.hpp"
#include "reply_job.hpp"
#include "node_configs.hpp"
// Each reply packet type has its own unique handler
class ReplyPacketHandler
{
    protected:
      string m_type;
      ReplyPacket* m_packet;
      size_t m_path_delay;      

    public:
        ReplyPacketHandler(string type, ReplyPacket *pckt, size_t path_delay)
          : m_type(type), m_packet(pckt),
            m_path_delay(path_delay) { };

        static ReplyPacketHandler* New(vector<string> *str_params);

        virtual ~ReplyPacketHandler() 
        { delete m_packet; };
        virtual int init(vector<string> *str_params);

        // Ready to reply determination is based on local clock of the 
        // node, not the global clock (hope this is okay)
        // Return: 1 - job is consumed. 
        //             reply packet is ready to be generated
        //         0 - job is not consumed. 
        //             reply packet is not ready.
        virtual bool ready_to_reply(size_t node_clock,
                                    ReplyJob *test_job,
                                    receivers_t *replier_pool);

        string get_id() { return m_packet->get_id(); }
        size_t get_packet_size() { return m_packet->get_size(); }
        Packet* get_packet() { return m_packet->clone(); }

        virtual ReplyPacketHandler* clone() const
          { return (new ReplyPacketHandler(*this)); }

        virtual ReplyJob* create_reply_job(RequestPacket* request_packet);

    private:
      size_t m_process_delay;

};

class ReplyPacketHandlerErlang: public ReplyPacketHandler
{
    public:
      ReplyPacketHandlerErlang(ReplyPacket* pckt, size_t path_delay)
        : ReplyPacketHandler("erlang", pckt, path_delay) { };

      int init(vector<string> *str_params);

      bool ready_to_reply(size_t node_clock,
                          ReplyJob *test_job,
                          receivers_t *replier_pool);

      ReplyPacketHandler* clone() const
        { return (new ReplyPacketHandlerErlang(*this)); }

      
    private:
      double m_arrival_rate;
      double m_service_time;
      double m_erlangC;
};

// ADD DRAM CLASS
// ready_to_reply, 
//    - If wait_count >= m_path_delay
//        - Go to the replier and get its MC.
//        - Check if the received MC is valid.
//        - Check the MC if it is ready

// PROBABLY WANT TO MAKE THE REPLY_JOB an OBJECT! <- DONE!
class ReplyPacketHandlerDram: public ReplyPacketHandler
{
    public:
      ReplyPacketHandlerDram(ReplyPacket* pckt, size_t path_delay)
        : ReplyPacketHandler("dram", pckt, path_delay) { };
      
      int init(vector<string> *str_params) { return SUCCESS; };

      bool ready_to_reply(size_t node_clock,
                          ReplyJob *test_job,
                          receivers_t *replier_pool);

      // Need to create ReplyJobDram
      ReplyJob* create_reply_job(RequestPacket* request_packet);

      ReplyPacketHandler* clone() const
        { return (new ReplyPacketHandlerDram(*this)); }


    private:


};

#endif
