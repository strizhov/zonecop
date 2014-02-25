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

// finder classes
#include "ZoneName.h"
#include "CrawlTask.h"
#include "Event_queue.h"
#include "Event_mgr.h"
#include "FileWriter.h"
#include "finder_def.h"

#define ZONES_TO_CRAWL 2000

using namespace std;

/*--------------------------------------------------------------------------------------
 * Purpose: print usage info and exit
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int usage()
{
  printf("Finder application [ OPTIONS ]\n");
  printf("\n");
  printf("OPTIONS:\n");
  printf("      -c <config file>          This file overrides the default location\n");
  printf("                                of the config file (/etc/finder.conf)\n");
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
  const char *ConfLogFile = oConfig.getValue(FINDER_LOG_FILE);
  
  if ( NULL != ConfLogFile
      && PsLogger::getInstance().getFileName() == "")
  {
    if (!(PsLogger::getInstance().setFileName(ConfLogFile)))
    {
      ps_elog(PSL_CRITICAL, "Unable to set file '%s' as output file, using stderr\n", ConfLogFile);
      retval = -1;
    }
  }

  const char *ConfLogLevel = oConfig.getValue(FINDER_LOG_LEVEL);
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
  const char *outFile = oConfig.getValue(OUT_FILE_NAME);
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
 * Purpose: main function
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 1, 2011
 * -------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
  std::string ConfigFile = "/etc/finder.conf";
  
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

    ps_elog(PSL_DEBUG, "Adding crawling tasks...\n");
    
    // create zone generator
    ZoneName ZoneName_obj;
    
    PsTaskList_t oList;


//    for (int i = 0; i < ZONES_TO_CRAWL; i++)
    while (ZoneName_obj.getDoneFlag() != true ) 
    {
      vector<string> zones = ZoneName_obj.getZone(zones_per_worker);
      
      CrawlTask *pTask = new CrawlTask(zones, Queue);
      oList.push_back(pTask);
      PsThread *pThrd = oPool.checkOut();
      pThrd->setTask(pTask);
      
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
  }// pool init

  if (! Queue->kill() )
  {
    ps_elog(PSL_CRITICAL, "Unable to kill event queue.\n");
  }

  // Queue manager
  delete Mgr;
  // Queue of events
  delete Queue;
  
  return 0;
}
