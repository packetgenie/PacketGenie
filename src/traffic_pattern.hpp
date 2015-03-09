#ifndef _TRAFFIC_PATTERN_HPP_
#define _TRAFFIC_PATTERN_HPP_


#include <vector>
#include <string>
#include "globals.hpp"
#include <iostream>
#include "misc.cpp"

// Traffic pattern class
class TrafficPattern
{
    protected:
       string m_id, m_type;
       size_t m_grp_size;

    public:
       // Default constructor. 
       TrafficPattern(string id, string type)
         : m_id(id), m_type(type) { };

       virtual ~TrafficPattern() { };
       virtual void init(size_t group_size)
          { m_grp_size = group_size; }
       
       string get_type() { return m_type; }
       string get_id() { return m_id; }

       virtual double get_traf_pattern(size_t grp_idx)
          { return 0; }

       virtual int parse_params(vector<string> *str_params)
          { return SUCCESS; }

       virtual TrafficPattern* clone() const
          { return (new TrafficPattern(*this)); }

};

class TrafficPatternUniform: public TrafficPattern
{
    public:
      TrafficPatternUniform(string id, string type)
        : TrafficPattern(id, type) { };
   
      double get_traf_pattern(size_t grp_idx)
        { return 1.0; }
      int parse_params(vector<string> *str_params)
        { return SUCCESS; }

      TrafficPattern* clone() const
        { return (new TrafficPatternUniform(*this)); }

};


#endif
