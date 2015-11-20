// FILE: src/globals.hpp  Date Modified: March 17, 2015
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



#ifndef _GLOBALS_HPP_
#define _GLOBALS_HPP_

#define _DEBUG 0 

#define _BOOKSIM 1 

#include <string>
#include <climits>
#include <cstdlib>
#include <cassert>


namespace
{
    const extern int SUCCESS = 0;
    const extern int ERROR = -1;
    const extern int ERROR_IN_CMDLN = -2;
}

namespace tf_gen
{
    class Node;
    class NodeGroup;
    class Network;
    class Packet;
    class Port;
    class PacketBuffer;
    class Dram;
}

using namespace std;
using namespace tf_gen;

class TrafficReportHandler;


extern bool gBookSim;
extern bool gPrint;
extern bool gRuntime;
extern bool gWaitForReply;

extern size_t g_clock;
extern size_t g_clock_end;
extern TrafficReportHandler* g_traffic_report;

extern tf_gen::Network* g_network;


#endif
