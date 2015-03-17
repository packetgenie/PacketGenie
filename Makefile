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

########## BOOST LINKING OPTIONS. ################################
#### IF - you have boost installed in a custom directory.#########
##################################################################
#BOOSTDIR   = include/boost_1_56_0
#BOOSTLIB   = $(BOOSTDIR)/stage/lib
#BOOST_LINK = $(BOOSTLIB)/libboost_program_options.a \
#  						$(BOOSTLIB)/libboost_regex.a
##################################################################
#### OR - you have boost installed in your system already ########
#####     such as /usr/lib64														  ########
##################################################################
BOOST_FLAG = -lboost
BOOST_LINK = -lboost_program_options -lboost_regex
##################################################################


BUILD_ROOT=$(shell pwd)
BOOKSIMDIR      = src/intersim/
export BOOKSIM_OBJDIR=$(BUILD_ROOT)/obj/intersim/

CC        = g++
CFLAGS    = -c -Wall -Wno-unused-function
LDFLAGS   = 
INCLUDE   = -I $(BOOSTDIR) $(BOOST_FLAG)
OBJDIR    = obj/
BINDIR    = bin/
SRCDIR    = src/
OBJLIST   = reply_job.o \
						packet.o \
						port.o icnt_wrapper.o \
						node_configs.o packet_handler.o node.o misc.o \
						traffic_report_handler.o \
						node_group.o globals.o traffic_pattern.o \
						injection_process.o network.o test.o 
OBJECTS   = $(addprefix $(OBJDIR), $(OBJLIST) )

all: booksim test

test: $(OBJECTS) booksim
	$(CC) $(LDFLAGS) $(OBJECTS) $(BOOKSIM_OBJDIR)/*.o \
		-o $(BINDIR)/test $(BOOST_LINK)


$(OBJECTS): obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $? -o $@ $(INCLUDE)

booksim: $(BOOKSIM_OBJDIR)/%.o: $(BOOKSIMDIR)/%.cpp
	$(MAKE)	"CREATE_LIBRARY=1" -C ./$(BOOKSIMDIR)

clean:
	rm -rf obj/*.o
	rm -rf $(BOOKSIM_OBJDIR)/*.o
