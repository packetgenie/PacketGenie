// FILE: src/packet.cpp  Date Modified: March 17, 2015
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
