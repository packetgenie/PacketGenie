#include "packet_handler.hpp"
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <math.h>


ReplyPacketHandler* ReplyPacketHandler::New(vector<string> *str_params)
{
    string new_id, new_type;
    size_t new_size, path_delay;
    ReplyPacket* new_pckt;
    if (str_params->size() < 3)
        return NULL;

    new_id = str_params->at(0);
    new_size = boost::lexical_cast<size_t>(str_params->at(2));
    new_pckt = new ReplyPacket(new_id, new_size, 0,
                               NULL, NULL, NULL, "UNSPECIFIED");

    new_type = str_params->at(1);
    path_delay = 0;

    // Instantiate the appropriate Packet Handler
    ReplyPacketHandler* new_handler;
    if (new_type == "erlang")
        new_handler = new ReplyPacketHandlerErlang(new_pckt, path_delay);
    else if (new_type == "dram")
        new_handler = new ReplyPacketHandlerDram(new_pckt, path_delay);
    else
        new_handler = new ReplyPacketHandler("fixed", new_pckt, path_delay);

    int error = new_handler->init(str_params);
    if (error)
        return NULL;
    else
        return new_handler;

}

int ReplyPacketHandler::init(vector<string> *str_params)
{
    m_process_delay = boost::lexical_cast<double>(str_params->at(3));
    return SUCCESS;
}

// NOTE: ready_to_reply should be getting invoked based on the period of
//       node's internal clock, not the global clock
bool ReplyPacketHandler::ready_to_reply(size_t node_clock,
                                        ReplyJob *test_job,
                                        receivers_t *replier_pool)
{
    if(test_job->get_wait_count() == (m_path_delay+m_process_delay))
        return true; // Consumed.
   
    test_job->inc_count();
    return false;
}

ReplyJob* ReplyPacketHandler::create_reply_job(RequestPacket* request_packet)
{
    // ReplyPacket gets created (allocated to heap) here,
    // deleted at traffic_report_handler.
    return ReplyJob::New(this, request_packet, m_packet->clone(), "Default");
}

int ReplyPacketHandlerErlang::init(vector<string> *str_params)
{
    //    -packet_type_reply <name> <type> <size> 
    //                       <arrival_rate> <service_time> 
    if ( str_params->size() < 5)
        return ERROR;

    m_arrival_rate =  boost::lexical_cast<double>(str_params->at(3));
    m_service_time =  boost::lexical_cast<double>(str_params->at(4));

    m_erlangC = erlang_delay_constant(m_arrival_rate, m_service_time);
    return SUCCESS;
}


bool ReplyPacketHandlerErlang::ready_to_reply(size_t node_clock,
                                              ReplyJob *test_job,
                                              receivers_t *replier_pool)
{
    size_t packet_age;
    double rand_val, thresh;
    packet_age = test_job->get_wait_count();

    if (packet_age >= m_path_delay)
    {
        rand_val = get_rand_decimal();
        thresh = erlang_prob_wait(packet_age-m_path_delay,
                                  m_arrival_rate, m_service_time,
                                  m_erlangC);

        if (rand_val > thresh)
            return true;
    }
  
    test_job->inc_count();
    return false;
}

ReplyJob* ReplyPacketHandlerDram::create_reply_job(RequestPacket* request_packet)
{
    assert((request_packet->get_dest())->is_dram());
    return ReplyJob::New(this, request_packet, m_packet->clone(), "Dram");
}


bool ReplyPacketHandlerDram::ready_to_reply(size_t node_clock,
                                            ReplyJob *test_job,
                                            receivers_t *replier_pool)
{
    assert(test_job->get_type() == "Dram");
    ReplyJobDram* dram_job = (ReplyJobDram*)test_job;

    DramJobStatus status;
    size_t packet_age;
    Node* mc; 
    bool change_status = false;

    status = dram_job->get_status();
    packet_age = dram_job->get_wait_count();
    mc = dram_job->get_replier();

    if (status == IN_NETWORK)
    {
        // If the upcoming increment will change the status
        if (packet_age+1 >= m_path_delay)
            change_status = true;
    }
    else if (status == WAITING_BANK)
    {
        change_status = mc->bank_ready(dram_job->get_bank_count());
        if (change_status)
            dram_job->set_access_time(mc->generate_access_time());
    }
    else if (status == WAITING_ACCESS)
    {
        change_status = dram_job->check_access_status();
    }
    else if (status == DONE_ACCESS)
    {
        return true;
    }

    // Increment the wait counter
    dram_job->inc_count();

    // Because of how inc_count is setup in ReplyJobDram,
    // we need to change status after the count.
    if (change_status)
        dram_job->advance_status();
    
    return false;
}
