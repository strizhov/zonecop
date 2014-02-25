#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

// vantages 
#include "ps_thrd_pool.h"
#include "ps_thrd.h"
#include "ps_config.h"
#include "ps_logger.h"

#include "ZoneReader.h"
#include "CrawlTask.h"
#include "Event_queue.h"
#include "Event_mgr.h"
#include "FileWriter.h"
#include "poller_def.h"

using namespace std;

/*--------------------------------------------------------------------------------------
 * Purpose: print usage info and exit
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int usage()
{
  printf("Poller application [ OPTIONS ]\n");
  printf("\n");
  printf("OPTIONS:\n");
  printf("      -c <config file>          This file overrides the default location\n");
  printf("                                of the config file (/etc/poller.conf)\n");
  printf("      -d                        Run daemon in the foreground.\n");
  printf("      -h                        Usage help.\n");

  exit(0);
}

/*--------------------------------------------------------------------------------------
 * Purpose: run app in daemon world
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int daemonize()
{
  int iRet = -1;
  pid_t tPid = -1;

  if ((tPid = fork()) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to fork: %s\n", strerror(errno));
  }
  else if (tPid > 0)
  {
    iRet = tPid;
  }
  else if (setsid() < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to create new process group: %s\n", strerror(errno));
  }
  else
  {
    int iFD = open("/dev/null", O_RDWR);
    if (iFD < 0)
    {
      ps_elog(PSL_CRITICAL, "Unable to open /dev/null: %s\n", strerror(errno));
    }
    else
    {
      int iRetVal = dup(iFD);
      iRetVal = dup(iFD);
      iRetVal = 0;

      if (chdir("/") < 0)
      {
        ps_elog(PSL_CRITICAL, "Unable to chdir to /: %s\n", strerror(errno));
      }
      else
      {
        iRet = 0;
      }
    }
  }

  return iRet;
}
/*--------------------------------------------------------------------------------------
 * Purpose: load logging configuration
 * Input:  
 * Output: 1 success, -1 failure
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int initLogging()
{
  int retval = 1; 

  PsConfig &oConfig = PsConfig::getInstance();
  const char *ConfLogFile = oConfig.getValue(LOG_FILE);
  
  if ( NULL != ConfLogFile
      && PsLogger::getInstance().getFileName() == "")
  {
    if (!(PsLogger::getInstance().setFileName(ConfLogFile)))
    {
      ps_elog(PSL_CRITICAL, "Unable to set file '%s' as output file, using stderr\n", ConfLogFile);
      retval = -1;
    }
  }

  const char *ConfLogLevel = oConfig.getValue(LOG_LEVEL);
  if (NULL != ConfLogLevel)
  {
    string sLevel = ConfLogLevel;
    int iLogLevel = PsLogger::getInstance().strToLevel(sLevel);
    if (-1 == iLogLevel)
    {
      ps_elog(PSL_CRITICAL, "Unable to set log level '%s', using stderr\n", ConfLogLevel);
      retval = -1;
    }
    else
    {
      PsLogger::getInstance().setLevel(iLogLevel);
    }
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: init File Writer
 * Input:  
 * Output: 1 success, -1 failure
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int initFileWriter()
{
  int retval = 1;

  PsConfig &oConfig = PsConfig::getInstance();
  const char *outFile = oConfig.getValue(POLLER_OUT_FILE_NAME);
  if (outFile != NULL)
  {
    int value = FileWriter::getInstance().setFileName(outFile);
    if (value == -1)
    {
      ps_elog(PSL_CRITICAL, "Unable to set file name '%s', using stderr\n", outFile);
      retval = -1;
    }
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: init ZoneReader 
 * Input:  
 * Output: 1 success, -1 failure
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int initZoneReader()
{
  int retval = 1;

  // ZoneReader object
  ZoneReader &obj = ZoneReader::getInstance(); 
    
  // read poller file
  const char *poller_out_file = PsConfig::getInstance().getValue(POLLER_OUT_FILE_NAME);
  if (poller_out_file != NULL)
  {
    if (obj.loadPollerZones(poller_out_file) == -1)
    {
      ps_elog(PSL_CRITICAL, "Could not load Poller's zones!\n");    
      // print error, but dont close application
    }
  }

  // read finder zones
  const char *finder_out_file = PsConfig::getInstance().getValue(FINDER_OUT_FILE_NAME);
  if (finder_out_file != NULL)
  {
    if (obj.loadFinderZones(finder_out_file) == -1)
    {
      ps_elog(PSL_CRITICAL, "Could not load Finder's zones!\n");   
      retval = -1;
    }
    else
    {
      // Once zones loaded, find the difference in both files
      // and add missing zones to pollers vector of zones
      obj.addMissingZones();
    }
  }
    
  return retval;
}
/*--------------------------------------------------------------------------------------
 * Purpose: main function
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  std::string ConfigFile = "/etc/poller.conf";
  
  int daemon = 0;


  int c;
  while ((c = getopt(argc, argv, "c:dh")) != -1)
  {
    switch (c)
    {
      case 'c':
        ConfigFile = optarg;
        break;
      case 'd':
        daemon = 1;
        break;
      case 'h':
        usage();
        break;
      case '?':
      default:
        ps_elog(PSL_CRITICAL, "Unable to parse command line.\n");
        usage();
        break;
    }
  }
  
  // load config file
  PsConfig &oConfig = PsConfig::getInstance();
  if (!oConfig.load(ConfigFile.c_str()))
  {
    ps_elog(PSL_CRITICAL, "Unable to load config file '%s'.\n", ConfigFile.c_str());
    usage();
  }
  // init Logger
  if ( initLogging() == -1)
  {
    ps_elog(PSL_CRITICAL, "Could not init logging\n");
  }

  // read Finder's and Poller's zones from file
  if (initZoneReader() == -1)
  {
    ps_elog(PSL_CRITICAL, "Could not load zone files\n");
    usage();
  }

  // init FileWriter
  if (initFileWriter() == -1)
  {
    ps_elog(PSL_CRITICAL, "Could not init File Writer\n" );
  }
  
  // daemonize ?
  if (daemon == 1)
  {
    int id = 0;
    
    // get PID FILE
    const char *pidfile = oConfig.getValue(PID_FILE);
    if (pidfile == NULL)
    {
      ps_elog(PSL_CRITICAL, "Unable to load PID file location from configuration file\n");
      usage();
    }
    
    // go daemon!
    PsLogger::getInstance().close();
    if ( (id = daemonize() ) == -1)
    {
      ps_elog(PSL_CRITICAL, "Could not go in daemon world!\n");
      usage();
    }
    
    // write ID to file
    FILE *PidFile_file = fopen(pidfile, "w");
    if (PidFile_file == NULL)
    {
      ps_elog(PSL_CRITICAL, "Unable to create PID file: %s\n", strerror(errno));
      usage();
    }
    else
    {
      fprintf(PidFile_file, "%d", id);
    }
  }
  
  //
  // QUEUE INIT
  //
  
  // Queue manager
  EventMgr *Mgr = new EventMgr();
  // Queue of events
  EventQueue *Queue = new EventQueue(Mgr);
 
  // get MAX value from config file
  const char *queue_max_ = PsConfig::getInstance().getValue(EVENT_QUEUE_MAX);
  {
    if (NULL != queue_max_)
    {
      int val = atoi(queue_max_); 
      if (! (Queue->init(val)) )
      {
        ps_elog(PSL_CRITICAL, "Unable to init event queue.\n");
        usage();
      }
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Could not get queue size\n");
      usage();
    }
  }
  // start QUEUE
  if (! Queue->start() )
  {
    ps_elog(PSL_CRITICAL, "Unable to start event queue.\n");
    usage();
  }

  //
  // THREAD POOL INIT 
  //
  
  PsThreadPool oPool;
  // get number of workers in thread pool
  const char *workers_number_ = PsConfig::getInstance().getValue(WORKERS_NUMBER);
  {
    if ( workers_number_ != NULL)
    {
      int val = atoi(workers_number_); 
      oPool.setSize(val);
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Could not get thread pool size\n");
      usage();
    }
  }
  if (!oPool.init())
  {
    ps_elog(PSL_CRITICAL, "Could not init pool.\n");
  }
  else
  {
    // wait all threads
    sleep(1);

    //
    // START ADDING JOBS
    //
    int zones_per_worker = 0;
    const char *zones_per_worker_ = PsConfig::getInstance().getValue(ZONES_PER_WORKER);
    {
      if ( zones_per_worker_ != NULL)
      {
        zones_per_worker = atoi(zones_per_worker_); 
      }
      else
      {
        ps_elog(PSL_CRITICAL, "Could not get number of zones per crawler\n");
        // kill pool
	oPool.kill();
        usage();
      }
    } 

    // Task list
    PsTaskList_t oList;

    // ZoneReader object
    ZoneReader &ZoneReader_obj = ZoneReader::getInstance(); 

    while (ZoneReader_obj.getDoneFlag() != true)
    {
      vector<string> zones = ZoneReader_obj.getZones(zones_per_worker);
      if (zones.size() != 0)
      {
        CrawlTask *pTask = new CrawlTask(zones, Queue);
        oList.push_back(pTask);
        PsThread *pThrd = oPool.checkOut();
        pThrd->setTask(pTask);
      }
      else
      {
        ps_elog(PSL_CRITICAL, "Number of received zones equal to 0!\n");
      }
    }
    
    ps_elog( PSL_DEBUG, "Waiting for pool to finish\n");
    while (oPool.getNumFree() < oPool.getSize())
    {
      sleep(1);
    }
    ps_elog( PSL_DEBUG, "Killing pool.\n");
    oPool.kill();

    for (PsTaskIter_t tIter = oList.begin();
         oList.end() != tIter;
         tIter++)
    {
      delete (*tIter);
    }


  }

  if (! Queue->kill() )
  {
    ps_elog(PSL_CRITICAL, "Unable to kill event queue.\n");
    usage();
  }
  

  // Queue manager
  delete Mgr;
  // Queue of events
  delete Queue;
  

  // wait untill everything would be killed
  sleep(1);
  
  return 0;
}
