
PROG = poller

CC =    gcc
CFLAGS = -g -O2 
#CFLAGS += -Wall -Werror 

CXX =    g++
LDFLAGS =  -lpthread 
CXXFLAGS = -g -O2 

CPPFLAGS =  

RM = rm -f
AR = @AR@
ARFLAGS = @ARFLAGS@
RANLIB = ranlib

INSTALL = @INSTALL@
INSTALL_PROGRAM = @INSTALL_PROGRAM@
INSTALL_MODE= -m 0755
DOC_MODE= -m 0644

# Include some boilerplate Gnu makefile definitions.
prefix = /usr/local

exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
infodir = ${datarootdir}/info
includedir = ${prefix}/include
datarootdir = ${prefix}/share
datadir = ${datarootdir}
docdir = $(datadir)/doc/poller
localedir = $(datadir)/locale

mandir = $(prefix)/man
manpfx = man

man1ext = .1
man1dir = $(mandir)/$(manpfx)1
man5ext = .5
man5dir = $(mandir)/$(manpfx)5
man8ext = .8
man8dir = $(mandir)/$(manpfx)8

htmldir = ${docdir}


OBJS = CrawlTask.o Event.o Event_mgr.o Event_queue.o FileWriter.o NsIP.o ReverseDNS.o ZoneReader.o \
       ps_config.o ps_logger.o ps_mutex.o ps_mutex_hdlr.o ps_queue.o ps_rw_mutex.o \
       ps_task.o ps_thrd.o ps_thrd_pool.o


.cc.o:
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $<


all: $(PROG)

$(PROG): $(OBJS) 
	$(CXX) $(CXXFLAGS) $(DEFINES) $(CPPFLAGS) $(LDFLAGS) -o $@ $@.cc $(OBJS) $(LIBS)

clean:
	rm -f $(PROG) $(OBJS)  
