// FILE: src/injection_process.hpp  Date Modified: March 17, 2015
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



#ifndef _INJECTION_PROCESS_HPP_
#define _INJECTION_PROCESS_HPP_

#include <string>
#include <vector>
#include "globals.hpp"
#include <iostream>
#include "misc.cpp"

class InjectionProcess;
class InjectionProcessUniform;
class InjectionProcessBurst;
class InjectionProcessGaussian;
class InjectionProcessOnOff;
class InjectionProcessMultiTime;

// Traffic pattern class
// Only uniform pattern for now.
// Want to make it a template that can change to different classes later
class InjectionProcess
{
    protected:
        string m_id, m_type;
        bool m_ready;
        long m_start, m_end;
        int m_seed, m_grp_size;

    public:
       // Constructor for uniform 
       InjectionProcess(string id, string type)
        : m_id(id), m_type(type), m_ready(false), m_start(0), m_end(0) { }; 
       InjectionProcess(string id, string type, int seed)
        : m_id(id), m_type(type), m_ready(false), m_start(0), m_end(0), m_seed(seed) { }; 

       static InjectionProcess * New(vector<string> *str_params);

       // This is for UNIFORM
       virtual ~InjectionProcess();
       virtual void init(int group_size);

       string get_type() { return m_type; }
       string get_id() { return m_id; }

       virtual double get_inj_rate(long cycle);
       virtual int parse_params(vector<string> *str_params);

       virtual InjectionProcess* clone() const
          { return (new InjectionProcess(*this)); }
       
};

class InjectionProcessUniform: public InjectionProcess
{
    public:
      InjectionProcessUniform(string id, string type)
        : InjectionProcess(id, type) { }; 
      InjectionProcessUniform(string id, string type, int seed)
        : InjectionProcess(id, type, seed) { }; 

      double get_inj_rate(long cycle);
      int parse_params(vector<string> *str_params);

      InjectionProcess* clone() const
          { return (new InjectionProcessUniform(*this)); }

    private:
       double m_rate;
};

class InjectionProcessBurst: public InjectionProcess
{
    public:
      InjectionProcessBurst(string id, string type)
        : InjectionProcess(id, type) { }; 
      InjectionProcessBurst(string id, string type, int seed)
        : InjectionProcess(id, type, seed) { }; 

      double get_inj_rate(long cycle);
      int parse_params(vector<string> *str_params);

      InjectionProcess* clone() const
          { return (new InjectionProcessBurst(*this)); }

    private:
       long m_period;
       long m_cycle_high;
       bool m_start_low;
       double m_rate_base;
       double m_rate_high;
};

// Injection process where injection rate exhibits a periodic peak,
// with the peaks shaped in a guassian curve
class InjectionProcessGaussian: public InjectionProcess
{
    public:
      InjectionProcessGaussian(string id, string type)
        : InjectionProcess(id, type) { }; 
      InjectionProcessGaussian(string id, string type, int seed)
        : InjectionProcess(id, type, seed) { }; 

      double get_inj_rate(long cycle);
      int parse_params(vector<string> *str_params);

      InjectionProcess* clone() const
          { return (new InjectionProcessGaussian(*this)); }

    private:
       long m_period;
       long  m_offset;
       double m_rate_peak; // packets per cycle per node.
       double m_std_dev; // in cycles

       double inv_twovar;

};


class InjectionProcessOnOff: public InjectionProcess
{
    public:
      InjectionProcessOnOff(string id, string type)
        : InjectionProcess(id, type) { }; 
      InjectionProcessOnOff(string id, string type, int seed)
        : InjectionProcess(id, type, seed) { }; 

      void init(int group_size);
      double get_inj_rate(long cycle);
      int parse_params(vector<string> *str_params);

      InjectionProcess* clone() const
          { return (new InjectionProcessOnOff(*this)); }

    private:
       double m_alpha;
       double m_beta;
       double m_r1;
       vector<int> m_state;
       string m_init_state;

       long m_prev_cyc;
       long m_counter;
};


#endif
