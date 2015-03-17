// FILE: src/traffic_report_handler.cpp  Date Modified: March 17, 2015
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



#include "traffic_report_handler.hpp"
#include <cstdlib>
#include <sstream>



TrafficReportHandler::TrafficReportHandler(size_t ntwk_size, size_t sim_length)
  : m_sim_length(sim_length), m_ntwk_size(ntwk_size)
{
    m_records = new list<traffic_incident_t>();

//    m_node_stat = new vector<size_t>(ntwk_size, 0); 
    // Just sent packets? should it be a struct?
}

TrafficReportHandler::~TrafficReportHandler()
{
   for (list<traffic_incident_t>::iterator it = m_records->begin();
         it != m_records->end(); ++it)
    {
        delete it->packet;
    }
    delete m_records;
//    delete m_node_stat;
}


void TrafficReportHandler::add_incident(Packet* packet)
{
    traffic_incident_t new_incident;
    new_incident.incident_cycle = g_clock;
    new_incident.packet = packet;
    m_records->push_back(new_incident);

    if (gRuntime)  // Print traffic as we go.
    {
        cout << get_incident_str(new_incident);
    }
}

void TrafficReportHandler::get_all_records(list<traffic_incident_t> &record_out)
{
    for (list<traffic_incident_t>::iterator it = m_records->begin();
         it != m_records->end(); ++it)
    {
        record_out.push_back(*it);
    }
}

void TrafficReportHandler::get_all_records(string &strout)
{
   ostringstream temp;

   for (list<traffic_incident_t>::iterator it = m_records->begin();
         it != m_records->end(); ++it)
    {
         temp << get_incident_str(*it);
    }

   strout.append(temp.str());

}

/*
void TrafficReportHandler::get_all_node_stat(string &strout)
{ 
   ostringstream temp;

   int node_idx=0;

   for (vector<size_t>::iterator it = m_node_stat->begin();
         it != m_node_stat->end(); ++it)
    {
         temp << "Node " << node_idx << " :\t" 
              << "Sent: " << *it << endl;
         ++node_idx;
    }

   strout.append(temp.str());
}
*/

double TrafficReportHandler::get_avg_inj_rate()
{
   return ((double) m_records->size())/m_sim_length; 
}

size_t TrafficReportHandler::get_total_num_packets()
{
   return m_records->size(); 
}

string TrafficReportHandler::generate_str_report()
{
   ostringstream temp;

   string recordlog = "";
   get_all_records(recordlog);

//   string node_stat_log = "";
//   get_all_node_stat(node_stat_log);

   temp << "<< << << Traffic Report >> >> >>" << endl;
   if (!gRuntime)
   {
   temp << "= = = Traffic Log = = =" << endl
        << recordlog
        << "= = = End of Log = = =" << endl;
   }
//   temp << "= = = Node Statistics = = =" << endl
//        << node_stat_log
//        << "= = = End of Statistics = = =" << endl
   temp   << "<< << << End Traffic Report >> >> >>" << endl;

   return temp.str();
}
        

string TrafficReportHandler::get_incident_str(const traffic_incident_t &incident)
{
    ostringstream out;
    out << "Cycle\t" << incident.incident_cycle << " :\t" 
        << (incident.packet->get_src())->get_idx() << "\t --> \t "
        << (incident.packet->get_dest())->get_idx()
        << "\tSize: " << (incident.packet)->get_size()
        << "   \tPacket: " << (incident.packet)->get_id() << endl;

    return out.str();
}

