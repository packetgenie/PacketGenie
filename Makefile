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
