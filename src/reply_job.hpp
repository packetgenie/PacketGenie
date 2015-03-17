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

