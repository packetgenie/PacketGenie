#ifndef _MISC_CPP_
#define _MISC_CPP_


#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <ostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include "globals.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

static const boost::regex
    range_specifier( "^(\\d+)-(\\d+)$" );
static const boost::regex
    valid_idx("^\\d+$");
static const boost::regex
    keyword_syntax("^[A-Z]+$");


static int factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

// Allow printing of vector data.
template<typename T>
ostream& operator<< (ostream& out, const vector<T> v) {
    int last = v.size() - 1;
    out << "[";
    for(int i = 0; i < last; i++)
        out << v[i] << ", ";
    out << v[last] << "]";
    return out;
}

static bool lessThan(const int& num1, const int& num2)
{
      return num1 < num2;
}

static int strvec_to_intlist(vector<string> *strvec, list<int> *intlist)
{
    try
    {
        for (vector<string>::iterator it = strvec->begin();
             it != strvec->end(); ++it)
        {
            intlist->push_back(boost::lexical_cast<int>(*it));
        }
        return SUCCESS;
    }
    catch (const boost::bad_lexical_cast &)
    {
        cerr << "ERROR: BAD CASTING" << endl;
        return ERROR;
    }
}

static int strvec_to_doubvec(vector<string> *strvec, vector<double> *doubvec)
{
    try
    {
        for (vector<string>::iterator it = strvec->begin();
             it != strvec->end(); ++it)
        {
            doubvec->push_back(boost::lexical_cast<double>(*it));
        }
        return SUCCESS;
    }
    catch (const boost::bad_lexical_cast &)
    {
        cerr << "ERROR: BAD CASTING" << endl;
        return ERROR;
    }
}


static int write_output(string &filename, string &write_data)
{
      ofstream outfile;
      outfile.open(filename.c_str());
      outfile << write_data;
      outfile.close();

      return 0;

}

static int process_keyword(string token, list<int> *intlist, int ntwk_size)
{

      int init, term, step;
      if (token == "ALL")
      {
          init = 0;
          term = ntwk_size;
          step = 1;
      }
      else if (token == "EVEN")
      {
          init = 0;
          term =  ntwk_size;
          step = 2;
      }
      else if (token == "ODD")
      {
          init = 1;
          term = ntwk_size;
          step = 2;
      }
      else
          return ERROR;

      for (int i = init; i < term; i += step)
          intlist->push_back(i);

      return SUCCESS;
}

static int interpret_range(string token, list<int> *intlist)
{
      int split_pos = token.find_first_of('-');
      int init = boost::lexical_cast<int>(token.substr(0, split_pos));
      int last = boost::lexical_cast<int>(token.substr(split_pos+1));

      for (int i = init; i <= last ; ++i)
           intlist->push_back(i);


      return SUCCESS;

}

//Extract string that are between { }
static	int extract_idxstr(vector<string> *strvec,  vector<string> &node_idxs)
{
      vector<string>::iterator first, last;
      bool brk_found = false;
      bool done = false;

      for ( vector<string>::iterator it = strvec->begin(); it != strvec->end();
            ++it)
      { 
            if (*it == "{")
            {
                if (brk_found)
                    return ERROR;
                first = it;
                ++first; // Exclude the bracket
                brk_found = true;
            }

            if (*it == "}")
            {
                if (!brk_found)
                    return ERROR;
                last = it;
                done = true;
                break;
            }
      }

      if (!done)
          return ERROR;

      node_idxs = vector<string>(first, last);
      return SUCCESS;
}


// Extract strings that are between < >
static	int extract_str(vector<string> *strvec,  vector<string> &extracted)
{
      vector<string>::iterator first, last;
      bool brk_found = false;
      bool done = false;

      for ( vector<string>::iterator it = strvec->begin(); it != strvec->end();
            ++it)
      { 
            if (*it == "<")
            {
                if (brk_found)
                    return ERROR;
                first = it;
                ++first; // Exclude the bracket
                brk_found = true;
            }

            if (*it == ">")
            {
                if (!brk_found)
                    return ERROR;
                last = it;
                done = true;
                break;
            }
      }

      if (!done)
          return ERROR;

      extracted = vector<string>(first, last);
      return SUCCESS;

}


// Looks at a string vector and parse out the node index values inside
// { } into an integer list 
static int process_strvec(vector<string> *raw_strvec, 
                          list<int> *intlist, int ntwk_size)
{

      vector<string> str_vec;


      boost::smatch str_matches;
      size_t size_max = ntwk_size;
      int error;

      error = extract_idxstr(raw_strvec, str_vec);
      if (error)
          return error;

      // Look for any special keyword
      vector<string>::iterator it = str_vec.begin();
      if ( boost::regex_match(*it, str_matches, keyword_syntax) )
      {
            // Process Keyword
            process_keyword(*it, intlist, ntwk_size);
            ++it;
      }

      unsigned int idx;
      string idx_str;

      // NEED TO LOOP THROUGH AND CHECK FOR RANGE. 
      while(it != str_vec.end())
      {
          idx_str = *it;

          if ( boost::regex_match(idx_str, str_matches, range_specifier) )
          {
                // Process range syntax
                interpret_range(idx_str, intlist);

          }
          else if (boost::regex_match(idx_str, str_matches, valid_idx) )
          {
              // Process a regular single integer index
              idx = boost::lexical_cast<int>(idx_str);

              if ( idx <  size_max )
              {
                    intlist->push_back(idx);
              }
              else
              {
                  cerr << "ERROR node index out of bounds " << endl
                       << "Index parsed: " << idx  
                       << "Max allowed: " << size_max <<endl;
                  return ERROR;
              }
          }
          else
          {
              cerr << "ERROR: Invalid node index - " << idx_str << endl;
              return ERROR;
          }
          ++it;
    }
   
    // Sort the list from smallest to largest.
    intlist->sort(lessThan);
    // Remove duplicates
    intlist->unique(); 
    return SUCCESS;
  
}

static vector<string>& get_strvec(multimap< string,vector<string> > &mmap, 
                                  const string &key)
{
      return mmap.find(key)->second;

}

/*
static int generate_node_prob(string name, size_t m, list<double> *prob_dist)
{
    double temp;

    if (prob_dist->size() != 0)
        return ERROR;

    if ( name == "uniform")
    {
        temp = 1.0;
        for (size_t i = 0; i < m; ++i)
            prob_dist->push_back(temp);
        
        return SUCCESS;
    }

    return ERROR;
}
*/

// Give random number between 0 and 1. (1 not included)
static double get_rand_decimal()
{
    return rand() / (RAND_MAX+1.0); 
}


static double factorial(size_t val)
{
    size_t ret = 1;
    for (size_t i = 1; i <= val; ++i)
        ret *= i;
    return ret;
}

// For s=1
static double erlang_delay_constant(double arrival_rate, 
                                    double service_time)
{
    double a, util_fac;
//          size_t num_server, s_fac;
          
//          num_server = 1;
//          s_fac = factorial(num_server);

    a = arrival_rate * service_time;
    if (a >= 1)
        return 1; // Utilization factor becomes 1.

    util_fac = a;

    return (a/(1-util_fac)) / ( a + a/(1-util_fac));
}

static double erlang_delay_constant(size_t num,
                                    double arrival_rate, 
                                    double service_time)
{
    double numerator, denominator;
    double a, a_pow_s, util_fac;
    int num_factorial = factorial(num);

    a = arrival_rate * service_time;
    a_pow_s = pow(a,num);
    if (a >= 1)
        return 1; // Utilization factor becomes 1.

    util_fac = a;

    numerator = a_pow_s/(num_factorial*(1-util_fac));
    denominator = numerator;
    for (size_t k=0; k<num; k++)
        denominator += pow(a,k)/factorial(k);

    return (numerator /denominator);
}

static double erlang_prob_wait(size_t wait_time, 
                               double arrival_rate,
                               double service_time,
                               double erlangC)
{
    double a, util_fac;
    a = arrival_rate * service_time;
    if (a >= 1)
        return erlangC;

    util_fac = a;
    return erlangC*exp(-1*(1-util_fac)*(1/service_time)*wait_time);
}

static double erlang_prob_wait(size_t wait_time, 
                               size_t num,
                               double arrival_rate,
                               double service_time,
                               double erlangC)
{
    double a, util_fac;
    a = arrival_rate * service_time;
    if (a >= 1)
        return erlangC;

    util_fac = a;
    return erlangC*exp(-1*(1-util_fac)*(num/service_time)*wait_time);
}

















#endif
