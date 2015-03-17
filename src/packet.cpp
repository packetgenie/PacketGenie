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

#include "packet.hpp"

Packet::Packet()
  : m_id(""), m_size(0), m_weight(0),
    m_src(NULL), m_dest(NULL), m_grp(NULL),
    m_type(DEFAULT)
{
    // Nothing  
}

Packet::Packet(string id, size_t size, double weight,
               Node* src, Node* dest, NodeGroup* group,
               PacketType type)
  : m_id(id), m_size(size), m_weight(weight),
    m_src(src), m_dest(dest), m_grp(group),
    m_type(type)
{
    // Nothing  
}

Packet::~Packet()
{
    // Nothing
}

Packet* Packet::New(string id, size_t size, double weight,
                    Node* src, Node* dest,
                    NodeGroup* group,
                    ReplyPacketHandler* reply_handler,
                    string rqst_pckt_id,
                    PacketType type)
{
    Packet* ret;

    if (type == DEFAULT)
        ret = new Packet(id, size, weight, src, dest, group, type);
    else if (type == REQUEST)
        ret = new RequestPacket(id, size, weight, src, dest, group,
                                reply_handler);
    else if (type == REPLY)
        ret = new ReplyPacket(id, size, weight, src, dest, group,
                              rqst_pckt_id);
    else
        ret = NULL;

    return ret;
}

RequestPacket::RequestPacket(string id, size_t size, double weight,
                             Node* src, Node* dest,
                             NodeGroup* group,
                             ReplyPacketHandler* reply_handler)
  : Packet(id, size, weight, src, dest, group, REQUEST),
    m_reply_handler(reply_handler)
{
    // Nothing
}

RequestPacket::~RequestPacket()
{
    // Nothing
}

ReplyPacket::ReplyPacket(string id, size_t size, double weight,
                         Node* src, Node* dest,
                         NodeGroup* group,
                         string rqst_pckt_id)
  : Packet(id, size, weight, src, dest, group, REPLY),
    m_rqst_pckt_id(rqst_pckt_id)
{
    // Nothing
}


ReplyPacket::~ReplyPacket()
{
    // Nothing
}
