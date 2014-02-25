CXX =    g++
DEFINES = -Wall -Werror -DHAVE_CONFIG_H
CXXFLAGS =  -g -O2 
CPPFLAGS = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LIBS = -lvdns  -lpthread

prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

PROG = finder  

TEST_PROG = test_zone_generator test_query_zone test_queue

OBJS = CrawlTask.o Event.o Event_mgr.o Event_queue.o FileWriter.o ReverseDNS.o ZoneName.o \
       ps_config.o ps_logger.o ps_mutex.o ps_mutex_hdlr.o ps_queue.o ps_rw_mutex.o \
       ps_task.o ps_thrd.o ps_thrd_pool.o

.cc.o:
	$(CXX) $(CXXFLAGS) $(DEFINES) $(CPPFLAGS) -c $<

all: $(PROG)

test: $(TEST_PROG)

$(TEST_PROG): $(OBJS)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) -o $@ $@.cc $(OBJS) $(LIBS)

$(PROG): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) -o $@ $@.cc $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(PROG) $(TEST_PROG)

