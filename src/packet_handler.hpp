// FILE: src/packet_handler.hpp  Date Modified: March 17, 2015
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
