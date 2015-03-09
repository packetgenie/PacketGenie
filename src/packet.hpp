#ifndef _PACKET_HPP_
#define _PACKET_HPP_

#include "globals.hpp"
#include <string>
#include <list>

//class tf_gen::Node;
//class tf_gen::Packet;
//class tf_gen::NodeGroup;
class ReplyPacketHandler;

namespace tf_gen
{

  enum PacketType
  {
      DEFAULT = 0,
      REQUEST = 1,
      REPLY   = 2,
  };
  /*
  struct packet_t
  {
      string id;
      size_t size;
      double weight;
      ReplyPacketHandler *reply_handler;
      string rqst_pckt_id;

      Node* src;
      Node* dest;
      NodeGroup* node_group;
  };


  struct packet_pool_t
  {
      double weight_total;
      list<packet_t> packet_list;
  };
  */

  struct packet_pool_t
  {
      double weight_total;
      list<Packet*> packet_list;
  };

  class Packet
  {
    protected:
      string      m_id;
      size_t      m_size;
      double      m_weight;
      Node*       m_src;
      Node*       m_dest;
      NodeGroup*  m_grp;
      PacketType  m_type;

    public:
      Packet();
      Packet(string id, size_t size, double weight,
             Node* src, Node* dest,
             NodeGroup* group,
             PacketType type);
      virtual ~Packet();

      static Packet* New(string id, size_t size, double weight,
                         Node* src, Node* dest,
                         NodeGroup* group,
                         ReplyPacketHandler* reply_handler,
                         string rqst_pckt_id,
                         PacketType type);

      virtual Packet* clone() const
      { return (new Packet(*this)); }

      string get_id() 
      { return m_id; }

      size_t get_size()
      { return m_size; }

      double get_weight()
      { return m_weight; }

      Node* get_src()
      { return m_src; }

      Node* get_dest()
      { return m_dest; }

      NodeGroup* get_grp()
      { return m_grp; }

      virtual void set_node_group(NodeGroup* group)
      { m_grp = group; }

      virtual void set_src(Node* src)
      { m_src = src; }

      virtual void set_dest(Node* dest)
      { m_dest = dest; }

      virtual PacketType get_type()
      { return m_type; }

  };

  class RequestPacket: public Packet 
  {
    public:
      RequestPacket(string id, size_t size, double weight,
                    Node* src, Node* dest,
                    NodeGroup* group,
                    ReplyPacketHandler* reply_handler);
      virtual ~RequestPacket();

      ReplyPacketHandler* get_reply_handler()
      { return m_reply_handler; }

      virtual RequestPacket* clone() const
      { return (new RequestPacket(*this)); }

    private:
      ReplyPacketHandler* m_reply_handler;             
  };

  class ReplyPacket: public Packet 
  {
    public:
      ReplyPacket(string id, size_t size, double weight,
                  Node* src, Node* dest,
                  NodeGroup* group,
                  string rqst_pckt_id);
      virtual ~ReplyPacket();

      virtual ReplyPacket* clone() const
      { return (new ReplyPacket(*this)); }

      string get_rqst_pckt_id()
      { return m_rqst_pckt_id; }

      virtual void set_rqst_pckt_id(string id)
      { m_rqst_pckt_id = id; }

    private:
      string m_rqst_pckt_id;
  };
}

#endif
