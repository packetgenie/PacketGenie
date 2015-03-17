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

#ifndef _TRAFFIC_REPORT_HANDLER_HPP_
#define _TRAFFIC_REPORT_HANDLER_HPP_


#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <list>
#include <ostream>
#include "globals.hpp"
#include "node.hpp"
#include "packet.hpp"
//#include "misc.cpp"
//
struct traffic_incident_t
{
    size_t incident_cycle;
    Packet* packet;
};
class TrafficReportHandler
{
    public:
        TrafficReportHandler(size_t ntwk_size, size_t sim_length);
        virtual ~TrafficReportHandler();

        void add_incident(Packet* new_packet);

        void get_all_records(list<traffic_incident_t>& record_out);
        void get_all_records(string &strout);
        
 //       void get_all_node_stat(string &strout);

        double get_avg_inj_rate();
        size_t get_total_num_packets();

        string generate_str_report();

      
    private:

        string get_incident_str(const traffic_incident_t & incident); 

        list<traffic_incident_t> *m_records;
//        vector<size_t> * m_node_stat;
        size_t m_sim_length;

        size_t m_ntwk_size;
        


};


#endif
