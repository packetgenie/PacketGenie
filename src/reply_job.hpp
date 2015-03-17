// FILE: src/reply_job.hpp  Date Modified: March 17, 2015
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



#ifndef _REPLY_JOB_HPP_
#define _REPLY_JOB_HPP_

#include <string>
#include <map>
#include <vector>
#include "globals.hpp"
#include "packet.hpp"

//class tf_gen::Node;
class ReplyPacketHandler;

class ReplyJob
{

 protected:
    ReplyPacketHandler *m_reply_handler;
    ReplyPacket* m_reply_packet;
    size_t m_wait_count;

    string m_type;

  public: 

    // Default constructor
    ReplyJob()
      : m_reply_handler(NULL),
        m_wait_count(0), m_type("UNDEFINED") { };

    ReplyJob(ReplyPacketHandler *reply_handler,
             ReplyPacket* reply_packet,
             string type)
      : m_reply_handler(reply_handler),
        m_reply_packet(reply_packet),
        m_wait_count(0), m_type(type) { };


    static ReplyJob* New(ReplyPacketHandler *reply_handler,
                         RequestPacket* request_packet, ReplyPacket* reply_packet,
                         string type);

    virtual ~ReplyJob() { };

    Packet* get_reply_packet()
    {
        return m_reply_packet;
    }

    Node * get_replier()
    {   return m_reply_packet->get_src(); }

    Node * get_requester()
    {   return m_reply_packet->get_dest(); }

    ReplyPacketHandler * get_reply_handler()
    {   return m_reply_handler; }

    size_t get_wait_count()
    {   return m_wait_count;  }

    virtual size_t inc_count()
    {   return ++m_wait_count;  }

    string get_type()
    {   return m_type;  }
 
};

enum DramJobStatus
{
    IN_NETWORK = 0,
    WAITING_BANK,
    WAITING_ACCESS,
    DONE_ACCESS,
};

class ReplyJobDram: public ReplyJob
{
    public:
      ReplyJobDram(ReplyPacketHandler *reply_handler,
                   ReplyPacket* reply_packet);

      DramJobStatus get_status()
      {   return m_status;  }

      DramJobStatus advance_status()
      {
          if (m_status == IN_NETWORK)
              m_status = WAITING_BANK;
          else if (m_status == WAITING_BANK)
              m_status = WAITING_ACCESS;
          else if (m_status == WAITING_ACCESS)
              m_status = DONE_ACCESS;
          else
              m_status = DONE_ACCESS;

          return m_status;
      }

      size_t inc_count();

      size_t get_bank_count()
      {   return m_bank_count;  }
    
      size_t get_access_count()
      {   return m_access_count;   }

      void set_access_time(size_t set_time)
      {   m_access_time = set_time;   }

      size_t get_access_time()
      {   return m_access_time;   }

      bool check_access_status()
      {   return (m_access_time <= m_access_count);   }
              

    private:
     DramJobStatus m_status;
     size_t m_bank_count;
     size_t m_access_count;

     size_t m_access_time;

};


#endif

