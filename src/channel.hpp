#ifndef _CHANNEL_HPP_
#define _CHANNEL_HPP_

#include "globals.hpp"
#include <queue>
#include "packet.hpp"
#include "credit.hpp"

class Channel
{
    // when a Node generates a packet
    //    - consumes a credit.
    //    - send credit to network.
  
    protected:
      size_t num_credits;
      stack<Credit *> credit_stack;
      queue<packet_t *> packet_queue;

    public:
      Channel();
      virtual ~Channel();
  
      static Channel * New();
      void push_packet(packet_t packet);
};

// Upstream Channel
class Channel: public ChannelUp
{
    public:
      void push_packet(packet_t packet);
      send_packet_up();

    private:
};

class Channel: public ChannelDown
{
    public:
      send_packet_down();

    private:
};



#endif
