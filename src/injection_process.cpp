// FILE: src/injection_process.cpp  Date Modified: March 17, 2015
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



#include "injection_process.hpp"
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <math.h>


InjectionProcess * InjectionProcess::New(vector<string> *str_params)
{
    int error;
    // Injection Process ID is the first element
    string id = str_params->at(0);

    // Process type is the second element
    string process_type = str_params->at(1);

#if _DEBUG
    cout << "Injection Proces: id=" << id << ", "
         << "process_type=" << process_type << endl;
#endif

    InjectionProcess *inj_proc;

    if (process_type == "uniform")
        inj_proc = new InjectionProcessUniform(id, process_type);
    else if (process_type == "burst")
        inj_proc = new InjectionProcessBurst(id, process_type);
    else if (process_type == "gaussian")
        inj_proc = new InjectionProcessGaussian(id, process_type);
    else if (process_type == "onoff")
        inj_proc = new InjectionProcessOnOff(id, process_type);
    else
        inj_proc = NULL;

    if (inj_proc != NULL)
        error = inj_proc->parse_params(str_params);

    if (error)
        return NULL;
    else
        return inj_proc;

}


InjectionProcess::~InjectionProcess()
{
  
}

void InjectionProcess::init(int group_size)
{ 
      m_grp_size = group_size;
      m_ready = true; 
}

// Get the per-cycle injection rate of a network at current cycle.
double InjectionProcess::get_inj_rate(int cycle)
{
    assert(m_ready);
    return 0;
}


int InjectionProcess::parse_params(vector<string> *str_params)
{
    return SUCCESS; 
}


// InjectionProcess - Uniform
double InjectionProcessUniform::get_inj_rate(int cycle)
{
    assert(m_ready);
    return m_rate;
}


int InjectionProcessUniform::parse_params(vector<string> *str_params)
{
    // Uniform should have had three parameters
    //   <inj_id> <type> <inj_rate>
    if ( str_params->size() < 3) 
        return ERROR;

    m_rate = boost::lexical_cast<double>(str_params->at(2));

    return SUCCESS;
  
}


// InjectionProcess - Burst
double InjectionProcessBurst::get_inj_rate(int cycle)
{
    assert(m_ready);

    int cycle_pos = cycle % m_period;  

    if (m_start_low)
        cycle_pos = m_period - cycle_pos;
    
    if ( cycle_pos  <= m_cycle_high )
        return m_rate_high;
    else
        return m_rate_base;

}

int InjectionProcessBurst::parse_params(vector<string> *str_params)
{
    // Burst should have had seven parameters (may also have empty strings at end)
    //   <inj_id> <type> <period> <dut_cycle> <start_low> <rate_base> <rate_high>
    if ( str_params->size() < 7)
        return ERROR;


    m_period = boost::lexical_cast<int>(str_params->at(2));

    double dut =  boost::lexical_cast<double>(str_params->at(3));
    m_cycle_high = (dut * m_period);

    string token = str_params->at(4);
    if (token == "low" || token == "L")
        m_start_low = true;
    else if (token == "high" || token == "H")
        m_start_low = false;
    else
        return ERROR;
    

    m_rate_base = boost::lexical_cast<double>(str_params->at(5));
    m_rate_high = boost::lexical_cast<double>(str_params->at(6));
    
    return SUCCESS;
}

// InjectionProcess - Gaussian
double InjectionProcessGaussian::get_inj_rate(int cycle)
{
    assert(m_ready);
    // Compensate for offset and periodic behavior.
    // Center around the period.
    double x_pos = ((cycle - m_offset) % m_period);
    if (x_pos > (m_period/2.0))
        x_pos = m_period - x_pos;

    // Gaussian function:
    // f(x,sigma) = A * exp^(-(x^2)/(2*sigma^2))
    return m_rate_peak *  exp(-1.0 * pow(x_pos,2) * inv_twovar);


}

int InjectionProcessGaussian::parse_params(vector<string> *str_params)
{
    // Burst should have had seven parameters (may also have empty strings at end)
    //   <inj_id> <type> <period> <start_offset> <rate_peak> <std_dev>
    if ( str_params->size() < 6)
        return ERROR;


    m_period = boost::lexical_cast<int>(str_params->at(2));

    // Offset should not exceed the period.
    m_offset = ( boost::lexical_cast<int>(str_params->at(3)) ) % m_period;

    m_rate_peak = boost::lexical_cast<double>(str_params->at(4));
    m_std_dev = boost::lexical_cast<double>(str_params->at(5));

    inv_twovar = 1.0 / (2 * pow(m_std_dev, 2.0));

    return SUCCESS;
}

// InjectionProcess - OnOff
void InjectionProcessOnOff::init(int group_size)
{ 
      m_grp_size = group_size;
      m_ready = true; 

      m_counter = group_size-1;
      m_prev_cyc = -1; // Not started yet

      if (m_init_state == "all_on")
      {
           m_state =  vector<int>(m_grp_size, 1);
      }
      else if (m_init_state == "all_off")
      {
           m_state =  vector<int>(m_grp_size, 0);
      }
      else if (m_init_state == "random")
      {
           for (int i = 0; i < m_grp_size; ++i)
                m_state.push_back(rand()%2);
      }
      else
          assert(false);

}


double InjectionProcessOnOff::get_inj_rate(int cycle)
{
    assert(m_ready);
    assert(cycle >= 0);

    ++m_counter;

    if (cycle != m_prev_cyc)
    {
        assert(m_counter==m_grp_size);
        m_prev_cyc = cycle; 
        m_counter = 0;
    }

    assert(m_counter < m_grp_size);
   
    // Test for state change.
    if (m_state[m_counter]) // in On State
        m_state[m_counter] = get_rand_decimal() >= m_beta;
    else // in Off State
        m_state[m_counter] = get_rand_decimal() < m_alpha;

    if (m_state[m_counter]) // On state
        return m_r1;
    else
        return 0;
}

int InjectionProcessOnOff::parse_params(vector<string> *str_params)
{
    // Burst should have had seven parameters (may also have empty strings at end)
    //   <inj_id> <type> <alpha> <beta> <r1> <initial_state>
    if ( str_params->size() < 6)
        return ERROR;


    m_alpha = boost::lexical_cast<double>(str_params->at(2));
    m_beta  = boost::lexical_cast<double>(str_params->at(3));
    m_r1 = boost::lexical_cast<double>(str_params->at(4));
    
    m_init_state = str_params->at(5);

    if (m_init_state == "all_on" 
        || m_init_state == "all_off" 
        || m_init_state == "random" )
            return SUCCESS;

    return ERROR;
}
