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
}

using namespace std;
using namespace tf_gen;

class TrafficReportHandler;


extern bool gBookSim;
extern bool gPrint;
extern bool gRuntime;
extern bool gWaitForReply;

extern size_t g_clock;
extern TrafficReportHandler* g_traffic_report;

extern tf_gen::Network* g_network;


#endif
