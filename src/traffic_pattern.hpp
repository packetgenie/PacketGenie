// FILE: src/traffic_pattern.hpp  Date Modified: March 17, 2015
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
