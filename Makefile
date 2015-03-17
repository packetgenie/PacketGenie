## FILE: Makefile  Date Modified: March 17, 2015
##
## PacketGenie
## Copyright 2014 The Regents of the University of Michigan
## Dong-Hyeon Park, Ritesh Parikh, Valeria Bertacco
## 
##  PacketGenie is licensed under the Apache License, Version 2.0
##  (the "License"); you may not use this file except in compliance
##  with the License. You may obtain a copy of the License at
##     http://www.apache.org/licenses/LICENSE-2.0
##     
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.

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
						injection_process.o network.o packetgenie.o 
OBJECTS   = $(addprefix $(OBJDIR), $(OBJLIST) )

all: booksim packetgenie

packetgenie: $(OBJECTS) booksim
	$(CC) $(LDFLAGS) $(OBJECTS) $(BOOKSIM_OBJDIR)/*.o \
		-o $(BINDIR)/packetgenie $(BOOST_LINK)


$(OBJECTS): obj/%.o: src/%.cpp
	$(CC) $(CFLAGS) $? -o $@ $(INCLUDE)

booksim: $(BOOKSIM_OBJDIR)/%.o: $(BOOKSIMDIR)/%.cpp
	$(MAKE)	"CREATE_LIBRARY=1" -C ./$(BOOKSIMDIR)

clean:
	rm -rf obj/*.o
	rm -rf $(BOOKSIM_OBJDIR)/*.o
