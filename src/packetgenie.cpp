// FILE: src/packetgenie.cpp  Date Modified: March 17, 2015
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



#ifndef _TEST_CPP_
#define _TEST_CPP_

#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <fstream>

#include "misc.cpp"
#include "node.hpp"
#include "network.hpp"
#include "globals.hpp"


// Parse a single line of string. Separates the string by any character in tokens.
static int extract_param(string const& line, string & key, vector<string> &params, string tokens)
{
    unsigned int pos = line.find_first_of(tokens);
    unsigned int initialPos = 0;
    unsigned int n = line.size();
    vector<string> strout;

    // Split line by any of the characters specified in tokens
    while( pos < n)
    {
        strout.push_back(line.substr(initialPos, pos - initialPos));
        initialPos = pos + 1;

        pos = line.find_first_of(tokens, initialPos);
    }

    strout.push_back(line.substr(initialPos, min( pos, n ) - initialPos));
    key = strout.front();
    strout.erase(strout.begin());
    params = strout;
  
    return strout.size();
}

// Parse through the config file to extract the variables and give it to param_list as a map.
static int process_config_file(string const& config_file_path,
                               multimap<string,vector<string> > &param_map)
{
    int num;
    string line, key;
    vector<string> str_param;

    ifstream config_text (config_file_path.c_str());
    while(getline(config_text, line))
    {
        if(line[0] == '-')
        {
            num = extract_param(line, key, str_param, " " );
            // Add to the parameter list, indexed by variable name in key.
            if (num > 0)
                 param_map.insert(pair< string, vector<string> >(key, str_param));
            str_param.clear();
        }
   
    }

    return SUCCESS;
}


int main(int argc, char** argv)
{
  try
  {

    string config_file_path;
    string outfile_path;
    unsigned int randseed = 0;

    // Parsing command line options
    // Get the path for configuration file and the path for the output.
    namespace po = boost::program_options;
    po::options_description desc("Basic Traffic Generator for NoCs.\n");

    // Adding options for the parser.
    desc.add_options()
      ("help,h", "Print help message")
      ("print,p", "Print results")
      ("runtime,r", "Print-as-you-go")
      ("noout,n", "No output file")
      ("seed,s", po::value<unsigned int>(&randseed), "Random seed")
      ("config_file", po::value<string>(&config_file_path),
       "Configuration file for the traffic generator")
      ("outfile", po::value<string>(&outfile_path),
       "Specify the location for the traffic output");
    
    // Specify positional options.
    po::positional_options_description positionalOptions;
    positionalOptions.add("config_file", 1);
    positionalOptions.add("outfile", 1);

    po::variables_map vm;
    

    try
    {
      // When help option is enabled
      po::store(po::command_line_parser(argc, argv).options(desc)
                  .positional(positionalOptions).run(),
                vm);

      if (vm.count("help"))
      {
        cout << "Basic Traffic Generator for simulating NoCs" << endl
             << desc << endl << endl;
        return SUCCESS;
      }

      if (!vm.count("config_file"))
      {
        cout << "ERROR: Need to specify a config file" << endl
             << desc << endl << endl;
        return SUCCESS;
      }

      if (!vm.count("outfile") && !vm.count("noout"))
      {
        cout << "ERROR: Need to specify a output file" << endl
             << desc << endl << endl;
        return SUCCESS;
      }


      po::notify(vm); // Throws an error.

    }
/*  
    catch(boost::program_options::required_option& e)
    {
      cerr << "ERROR: " << e.what() << endl << endl;
      cerr << desc << endl;
      return ERROR_IN_CMDLN;
    }
*/
    catch(po::error& e)
    {
      cerr << "ERROR: " << e.what() << endl << endl;
      cerr << desc << endl;
      return ERROR_IN_CMDLN;
    }


    if (randseed)
        srand(randseed);
    else
        srand(time(NULL));

    if (vm.count("print"))
        gPrint = true;
    else
        gPrint = false;

    if (vm.count("runtime"))
        gRuntime = true;
    else
        gRuntime = false;

    if (vm.count("noout"))
	gNoOut = true;
    else
	gNoOut = false;

    
    multimap<string,vector<string> > param_map;

#if _Debug
        cout << "Parsing success!" << endl;
        cout << "config_file: " << config_file_path << endl;
     if (!gNoOut)
        cout << "Outfile to: " << outfile_path << endl;
#endif
  
    // Process the config file and store the variables to param_list
    // param_list is a map, indexed by the variable name and contains list of values in string.
    process_config_file(config_file_path, param_map);


#if _DEBUG
    // Print the map.
    for (multimap< string,vector<string> >::iterator it=param_map.begin(); 
         it != param_map.end(); ++it)
          cout << it->first << " => " << it->second << endl;
    cout << "MAP SIZE: " << param_map.size() << endl;
#endif

    try
    {
          int error;
          string top_type, traffic_pattern;
          unsigned int top_dim_x, top_dim_y;
          unsigned int simclk_end;
                  
          // Parse basic network and simulation parameters.
          top_type = get_strvec(param_map, "-topology_type")[0];
          top_dim_x = boost::lexical_cast<unsigned int>
                                  (get_strvec(param_map, "-topology_type")[1]);
          top_dim_y = boost::lexical_cast<unsigned int>
                                  (get_strvec(param_map, "-topology_type")[2]);

          simclk_end = boost::lexical_cast<unsigned int>
                                  (get_strvec(param_map, "-simclk_end")[0]);


          if (gPrint)
          {
             cout << "Setting up Network" << endl;

          }

          // Initialize network. ( Declared as X by Y mesh network. )
          Network my_network = Network(NETWORK_MESH, top_dim_x, top_dim_y);


          pair< multimap< string,vector<string> >::iterator, 
                    multimap< string, vector<string> >::iterator> it_range;
          multimap< string, vector<string> >::iterator it_param;


          // Simulation don't finish until all outstanding request-reply packets 
          // have been processed.
          it_param = param_map.find("-wait_for_replies");
          if (it_param != param_map.end())
          {
              gWaitForReply = true;
          }


          if (gPrint)
          {
              cout << "=== Setting up Injection Process ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-traffic_inj_process");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_inj_process(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING INJECTION PROCESS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Traffic Pattern ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-traffic_pattern");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_traffic_pattern(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING TRAFFIC PATTERN" << endl;
                  return error;
              }
          }
          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Senders ===" << endl;
          }

          // Set receivers
          it_range = param_map.equal_range("-traffic_senders");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_sender_nodes(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error
                       << ": ERROR SETTING SENDERS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Receivers ===" << endl;
          }

          // Set receivers
          it_range = param_map.equal_range("-traffic_receivers");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_receiver_nodes(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error
                       << ": ERROR SETTING RECEIVERS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Reply Packets ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-packet_type_reply");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_packet_type_reply(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING REPLY PACKETS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Packets ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-packet_type");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_packet_type(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING PACKETS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Dram Bank ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-dram_bank_config");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_dram_bank_config(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING DRAM BANK" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Dram Timing ===" << endl;
          }

          // Set injection processes
          it_range = param_map.equal_range("-dram_timing_config");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_dram_timing_config(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR " << error 
                       << ": ERROR SETTING DRAM TIMING" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Setting up Node Groups ===" << endl;
          }

          // Set node groups
          it_range = param_map.equal_range("-node_group");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.set_node_group(&(it_param->second));
              if (error & (error != ERROR_IGNORE))
              {
                  cerr << "ERROR: ERROR SETTING GROUPS" << endl;
                  return error;
              }
          }

          if (gPrint)
          {
              cout << endl
                   << "=== Configuring Nodes ===" << endl;
          }

          // Configure nodes
          it_range = param_map.equal_range("-node_config");
          for ( it_param = it_range.first; 
                it_param != it_range.second; ++it_param)
          {
              error = my_network.config_nodes(&(it_param->second));
              if (error)
              {
                  cerr << "ERROR: ERROR CONFIGURING NODES" << endl;
                  return error;
              }
          }

#if _BOOKSIM
          string booksim_config_file;
        
          it_param = param_map.find("-booksim_enable");
          if (it_param != param_map.end())
          {
              cout << "Initializing Booksim" << endl;
              booksim_config_file = get_strvec(param_map, "-booksim_config_file")[0];
              error = my_network.config_BookSim(booksim_config_file.c_str());
              if (error)
              {
                  cerr << "ERROR: ERROR CONFIGURING BOOKSIM INTERCONNECT" << endl;
                  return error;
              }
          }
#endif

          // Initialize simuation
          my_network.init_sim(simclk_end);


          if (gPrint)
          {
              cout << ">>> Simulation Starts >>>" << endl;
          }

          ///////////////////////////////////// 
          // Step through until we are done! //
          // //////////////////////////////////
          do
          {
              error = my_network.step();
          }
          while((my_network.get_status() != NETWORK_STATUS_DONE) || error);

          if (error)
              cerr << "ERROR: ERROR OCCURED WHILE STEPPING THROUGH" << endl 
                   << "=== traffic generaton aborted === " << endl;


          /////////////////////////////////////
          //       Print Final Outputs       //
          /////////////////////////////////////
          if (gPrint)
          {
              cout << "<<< Simulation Complete <<< " << endl << endl;
          }

          if (gPrint)
              my_network.print_traffic_records();
   

          // Write result to file
     if (!gNoOut) {
          string output;
          my_network.get_traffic_records(output);
          write_output(outfile_path, output);
     }


        // Run traffic generator
        //    (1) Go through each node and test for traffic
        //          - if the node will generate traffic, determine the destination and record.
        //    (2) Increment clock
        //    (3) Repeat 1-2 until clock reaches the end.
        //    (4) Write traffic log to a file, create any statistical data if necessary.

    }
    catch (const boost::bad_lexical_cast &)
    {
        cerr << "ERROR: BAD CASTING" << endl;
    }

    // In case of exceptions
  }
  catch(exception& e)
  { 
    cerr << "Unhandled Exception reached the top of main: "
         << e.what() << ", application will now exit" << endl;
    return ERROR;
  }
  

  return SUCCESS;
}



#endif
