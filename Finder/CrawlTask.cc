#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ps_logger.h"

#include "Event.h"
#include "Event_queue.h"
#include "ReverseDNS.h"
#include "ps_config.h"
#include "finder_def.h"
#include "CrawlTask.h"

using namespace std;

/*--------------------------------------------------------------------------------------
 * Purpose: comstructor   
 * Input:  zone string
 * Output: none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
CrawlTask::CrawlTask( std::vector<std::string> zonenames, EventQueue *queue)
  : done_(false),
    zonenames_(zonenames),
    queue_(queue)
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: destructor
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
CrawlTask::~CrawlTask()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: get zone name
 * Input: 
 * Output: string
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
std::string CrawlTask::getZoneName(int i)
{
  return zonenames_[i];
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: get size of zonearray vector
 * Input: none
 * Output: size_t
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
size_t CrawlTask::getVectorSize()
{
  return zonenames_.size();
}

/*--------------------------------------------------------------------------------------
 * Purpose: get queue pointer
 * Input: none
 * Output: eventqueue
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
EventQueue* CrawlTask::getQueue()
{
  return queue_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: initialize Dns Resolver
 * Input: config file  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void CrawlTask::initDnsResolver(ReverseDNS *obj)
{
  // IP ADDRESS value
  const char *nameserver_ = PsConfig::getInstance().getValue(FINDER_NAMESERVER);
  {
    if ( nameserver_ != NULL)
    {
      uint32_t uIP = 0;
      struct sockaddr_in  sa;
      inet_pton(AF_INET, nameserver_, &(sa.sin_addr));
      uIP = (htonl)(sa.sin_addr.s_addr);
      if (uIP != 0)
      {
	// init resolver
        obj->setResolverIPaddress(uIP);
      }
      else
      {
        ps_elog(PSL_CRITICAL, "Could not convert IP address for nameserver!\n");     
      }
    }
    else
    {
        ps_elog(PSL_CRITICAL, "Could not read IP address for nameserver from configuration file. Using /etc/resolv.conf!\n");     
    }
  }

  // TIMEOUT value
  const char *timeout_ = PsConfig::getInstance().getValue(FINDER_NAMESERVER_TIMEOUT);
  if (timeout_ != NULL)
  {
    int val = atoi(timeout_); 
    if (val != 0)
    {
      obj->getDnsResolver().setTimeout(val);
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Could not convert nameserver's timeout value %s to integer!\n", timeout_);     
      obj->getDnsResolver().setTimeout(DEFAULT_NAMESERVER_TIMEOUT);
    }
  }
  else
  {
    ps_elog(PSL_DEBUG, "Could not read nameserver's timeout value from configuration file, using default!\n");     
    obj->getDnsResolver().setTimeout(DEFAULT_NAMESERVER_TIMEOUT);
  }
  
  // RETRY value 
  const char *retry_ = PsConfig::getInstance().getValue(FINDER_NAMESERVER_RETRY);
  if (retry_ != NULL)
  {
    int val = atoi(retry_); 
    if (val != 0)
    {
      obj->getDnsResolver().setRetries(val);
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Could not convert nameserver's retry value %s to integer!\n", retry_);     
      obj->getDnsResolver().setRetries(DEFAULT_NAMESERVER_RETRY);
    }
  }
  else
  {
    ps_elog(PSL_DEBUG, "Could not read nameserver's retry value from configuration file, using default!\n");     
    obj->getDnsResolver().setRetries(DEFAULT_NAMESERVER_RETRY);
  }
}

/*--------------------------------------------------------------------------------------
 * Purpose: execute method, start crawling  
 * Input:  none
 * Output:  none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
bool CrawlTask::execute()
{
  bool retval = false;

  ps_elog(PSL_DEBUG, "Number of zones received: %d\n", (int)getVectorSize());
 
  for (size_t index=0; index <  getVectorSize(); index++)
  {
    // ReverseDNS object
    ReverseDNS *RDNS_obj = new ReverseDNS();
    
    // init DNS resolver
    initDnsResolver(RDNS_obj);

    string tmp_zone = getZoneName(index);
    ps_elog(PSL_CRITICAL, "Incomming Zone: %s\n", tmp_zone.c_str() );
    
    // set timestamp
    RDNS_obj->setTimeStamp( time(NULL) );

    if (checkZone(RDNS_obj, tmp_zone) > 0)
    {
      Event *pEvent = new Event( );
      pEvent->setTimeStamp( RDNS_obj->getTimeStamp() );
      pEvent->setChildZone( tmp_zone );
      // enqueue Event object
      if (!getQueue()->enqueue(*pEvent))
      {
        ps_elog(PSL_CRITICAL, "Unable to enqueue with zone: %s\n", tmp_zone.c_str() );
        delete pEvent;
      }
      else
      {
       retval = true;
       done_ = true;
      }
    }
    delete RDNS_obj;
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose:   
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
bool CrawlTask::done()
{
  return done_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: find Child zone existence
 * Input:  zone string
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::checkZone(ReverseDNS *obj, std::string& zone)
{    
  int retval = -1;

  // send SOA query torecursive resolver
  if (obj->sendSOAquery(zone) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to send SOA query for '%s' zone\n", zone.c_str());
  }
  else
  {
    // check RCODE in header, should not contain ERROR
    if (obj->checkHeaderRCODE() < 0)
    {
      ps_elog(PSL_CRITICAL, "RCODE in DNS header contains error! Zone: %s\n", zone.c_str());
    }
    else
    {
      // check if Answer counter in DNS header is not 0
      if (obj->checkAnswerSection() < 0)
      {
        ps_elog(PSL_CRITICAL, "Answer section is empty. Zone: %s\n", zone.c_str());
        // Otherwise, check Authority counter in DNS header
        if (obj->checkAuthoritySection() < 0)
        {
          ps_elog(PSL_CRITICAL, "Authority section is empty. Zone: %s\n", zone.c_str() );
	}
	else
	{
	  // Compare zone we ask and zone in reply in Auth section
          if (obj->checkSOAinAuthoritySection(zone) < 0)
	  {
	   ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", zone.c_str());
	  }
	  else
	  {
            // zone exist!
            retval = 1;
	  }
	}
      }
      else
      {
        // Compare zone we ask and zone in reply in Answer section
        if (obj->checkSOAinAnswersSection(zone) < 0)
        {
          ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", zone.c_str());
        }
        else
        {
          // zone exist!
          retval = 1;
        }
      }
    }// if (obj->checkHeaderRCODE() < 0)
  }// if (obj->sendSOAquery(zone) < 0)

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose:   
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
bool CrawlTask::process()
{
  bool bRet = false;
  if (done())
  {
//    printf("Done w/ %s\n", getZoneName().c_str());
    printf("Done\n");
    bRet = true;
  }
  else
  {
//    printf("NOT DONE: %s\n", getZoneName().c_str() );
    printf("NOT DONE\n");
  }
  return bRet;
}

