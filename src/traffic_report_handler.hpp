// FILE: src/traffic_report_handler.hpp  Date Modified: March 17, 2015
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
