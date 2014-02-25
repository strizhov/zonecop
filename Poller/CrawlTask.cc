#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <string>
#include <stdint.h>
#include <vector>
// network libs
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ps_config.h"
#include "ps_logger.h"

#include "Event.h"
#include "Event_queue.h"
#include "ReverseDNS.h"
#include "poller_def.h"
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
 * Purpose: initialize Dns Resolver
 * Input: RerevseDNS class pointer  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void CrawlTask::initDnsResolver(ReverseDNS *obj)
{
  // IP ADDRESS value
  const char *nameserver_ = PsConfig::getInstance().getValue(POLLER_NAMESERVER);
  {
    if ( nameserver_ != NULL)
    {
      uint32_t uIP = 0;
      struct sockaddr_in  sa;
      inet_pton(AF_INET, nameserver_, &(sa.sin_addr));
      uIP = (htonl)(sa.sin_addr.s_addr);
      if (uIP != 0)
      {
        setDefaultNameserver(uIP);
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
  const char *timeout_ = PsConfig::getInstance().getValue(POLLER_NAMESERVER_TIMEOUT);
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
  const char *retry_ = PsConfig::getInstance().getValue(POLLER_NAMESERVER_RETRY);
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
 * Purpose: set ip address for default resolver
 * Input: uint32_t  
 * Output: 
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void CrawlTask::setDefaultNameserver(uint32_t ip)
{
  default_nameserver_ = ip;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get ip address for default resolver
 * Input:   
 * Output:  uint32_t
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
uint32_t CrawlTask::getDefaultNameserver()
{
  return default_nameserver_;
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
 * Purpose: get queue
 * Input: none
 * Output: eventqueue
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
EventQueue* CrawlTask::getQueue()
{
  return queue_;
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
 
  // go over each zone in vector and perform analysis
  for (size_t index=0; index <  getVectorSize(); index++)
  {
    ReverseDNS *RDNS_obj = new ReverseDNS();
    
    // init default settings for dns resolver
    initDnsResolver(RDNS_obj);

    string tmp_zone = getZoneName(index);
    ps_elog(PSL_CRITICAL, "Incomming Zone: %s\n", tmp_zone.c_str() );
    // set timestamp
    RDNS_obj->setTimeStamp( time(NULL) );

    // check existence of given zone
    if ( checkZone(RDNS_obj, tmp_zone) > 0)
    {
     // set Child Zone
     RDNS_obj->setChildZone( tmp_zone );

     // get parent and check his existence
     if (findParentZone( RDNS_obj, RDNS_obj->getChildZone() ) > 0)
     {
       ps_elog(PSL_DEBUG, "Child: %s, Parent: %s\n", RDNS_obj->getChildZone().c_str(), RDNS_obj->getParentZone().c_str());
       // find NS;IP pairs and push back them to NsIPVector_fromparent
       if (findNsIPset( RDNS_obj, RDNS_obj->getParentZone(), RDNS_obj->getNsIPVector_fromparent() ) > 0)
       {
	 // go over NS;IP pairs in fromparent vector  and fill up NS;IP pairs to  atparent
	 findParentNSset(  RDNS_obj, RDNS_obj->getChildZone(), RDNS_obj->getNsIPVector_fromparent(), RDNS_obj->getNsIPVector_atparent()  );
//	 {
	   // retrieve SOA serial numbers at parent and check AA bit from each nameserver
           findSOAserial( RDNS_obj, RDNS_obj->getChildZone(), RDNS_obj->getNsIPVector_atparent() );
//	   {
	     string ip = "NULL"; // pass ip by reference
	     compareSOAserial( RDNS_obj->getNsIPVector_atparent(), ip); 
//	     {
	       findChildNSset( RDNS_obj,
	                           RDNS_obj->getChildZone(), 
	                           ip, 
				   RDNS_obj->getNsIPVector_atchild()
				 ) ;
//	       {
	         // retrieve SOA serial numbers at child
                 findSOAserial( RDNS_obj, RDNS_obj->getChildZone(), RDNS_obj->getNsIPVector_atchild() ) ;
//		 {
		   Event *pEvent = new Event( );
                   pEvent->setTimeStamp( RDNS_obj->getTimeStamp() );
		   pEvent->setChildZone( RDNS_obj->getChildZone() );
		   pEvent->setParentZone( RDNS_obj->getParentZone() );
		   pEvent->setNsIPVector_atparent( RDNS_obj->getNsIPVector_atparent() );
		   pEvent->setNsIPVector_atchild( RDNS_obj->getNsIPVector_atchild() );
                   // enqueue Event object
                   if (!getQueue()->enqueue(*pEvent))
                   {
                     ps_elog(PSL_CRITICAL, "Unable to enqueue zone: %s\n", RDNS_obj->getChildZone().c_str() );
		     RDNS_obj->clearNsIPVector_atparent();
		     RDNS_obj->clearNsIPVector_atchild();
                     delete pEvent;
                   }
		   else
		   {
                     done_ = true;
		     retval = true;
		   }
//                 }// findSOAserial
//		 else
//		 {
//		   RDNS_obj->clearNsIPVector_atparent();
//		   RDNS_obj->clearNsIPVector_atchild();
//		 }
//	       }// findChildNSset
//	       else
//	       {
//	         RDNS_obj->clearNsIPVector_atparent();
//	       }
//	     }// compareSOAserial  
//	     else
//	     {
//	       RDNS_obj->clearNsIPVector_atparent();
//	     }
//           }// findSOAserial 
//	   else
//	   {
//	     RDNS_obj->clearNsIPVector_atparent();
//	   }
//	 }// findParentNSset
         // if NsIP pairs are found at parent, free memory
         RDNS_obj->clearNsIPVector_fromparent();          
       }// findNsIPset
     }
   }
   delete RDNS_obj;

  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: find Child zone existence
 * Input: RerevseDNS class pointer,  zone string
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::checkZone(ReverseDNS *obj, std::string tmp_zone)
{    
  int retval = -1;

  // send SOA query torecursive resolver
  if (obj->sendSOAquery(tmp_zone) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to send SOA query for '%s' zone\n", tmp_zone.c_str());
  }
  else
  {
    // check RCODE in header, should not contain ERROR
    if (obj->checkHeaderRCODE() < 0)
    {
      ps_elog(PSL_CRITICAL, "RCODE in DNS header contains error! Zone: %s\n", tmp_zone.c_str());
    }
    else
    {
      // check if Answer counter in DNS header is not 0
      if (obj->checkAnswerSection() < 0)
      {
        ps_elog(PSL_CRITICAL, "Answer section is empty. Zone: %s\n", tmp_zone.c_str());
        // Otherwise, check Authority counter in DNS header
        if (obj->checkAuthoritySection() < 0)
        {
          ps_elog(PSL_CRITICAL, "Authority section is empty. Zone: %s\n", tmp_zone.c_str() );
	}
	else
	{
	  // Compare zone we ask and zone in reply in Auth section
          if (obj->checkSOAinAuthoritySection(tmp_zone) < 0)
	  {
	   ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", tmp_zone.c_str());
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
        if (obj->checkSOAinAnswerSection(tmp_zone) < 0)
        {
          ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", tmp_zone.c_str());
        }
        else
        {
          // zone exist!
          retval = 1;
        }
      }
    }// if (obj->checkHeaderRCODE() < 0)
  }// if (obj->sendSOAquery(tmp_zone) < 0)

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: find Parent zone
 * Input: RerevseDNS class pointer,  string zone (child zone)
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findParentZone(ReverseDNS *obj, std::string childzone)
{
  int retval = -1;

  bool loop = true;
  // assign child zone to child_tmp
  // we will cut some parts of child_tmp
  string child_tmp = childzone;
  string parent_tmp;
  
  while(loop == true)
  {
    parent_tmp = obj->cropChildZone(child_tmp);

    if (parent_tmp.compare( child_tmp ) == 0)
    {
      ps_elog(PSL_CRITICAL, "Child and Parent domain names are same!\n");
      return retval;
    }
    ps_elog(PSL_DEBUG,  "Parent_tmp zone: %s\n", parent_tmp.c_str() );
    
    // check SOA record
    if ( checkZone(obj, parent_tmp) > 0)
    {
      // parent found yay!
      obj->setParentZone(parent_tmp);
      retval = 1;
      loop = false;
    }
    else
    { 
      // parent doesnt exist? lets crop more!
      child_tmp = parent_tmp;
    }
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: find NS set
 * Input: RerevseDNS class pointer,  string zone, NsIP object vector  
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findNsIPset(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector )
{
  int retval = -1;

  // send NS query to recursive resolver
  if (obj->sendNSquery(zone) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to send NS query for '%s' zone\n", zone.c_str());
  }
  else
  {
    // check if TC bit is enabled
    if (obj->checkTCbit() == true)
    {
       ps_elog(PSL_CRITICAL, "TCP flag is enabled in DNS header!!\n");
       obj->setTCP(true);
       if (findNsIPset( obj,  zone, NsIPVector) > 0)
       {
         retval = 1;
       }
       // disable TCP flag at resolver
       obj->setTCP(false);
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
          vector<string> NsVector = obj->getNSinAuthoritySection( zone );
          if (NsVector.size() == 0)
          {
	    ps_elog(PSL_CRITICAL, "There are no nameservers! Zone: %s\n", zone.c_str() );
	    ps_elog(PSL_CRITICAL, "Going to special case scenario!\n");
	    if (getCorrectNSset(obj, zone, NsIPVector ) < 0)
	    {
	      ps_elog(PSL_CRITICAL, "No nameserver could be reached! Zone: %s\n", zone.c_str());
	    }
	    else
	    {
	      retval = 1;
	    }
          }
	  else
	  {
	    ps_elog(PSL_DEBUG, "There are %d nameservers in Authority section! Zone: %s\n", (int)(NsVector.size()), zone.c_str() );
	    for (size_t i=0; i< NsVector.size(); i++)
	    {
	      vector<string> IpVector;
	      // pass IpVector by reference
	      if ( getIPv4address(obj, NsVector[i], IpVector) > 0)
	      {
	        if (IpVector.size() == 0)
	        {
	          ps_elog(PSL_CRITICAL, "No IP address is found for nameserver: %s! Zone: %s\n", NsVector[i].c_str(), zone.c_str() );
	          // Even there is no IP address, push back NsIP object
	          // We dont want to loose data anyway!
	          NsIP *NsIP_obj = new NsIP();
	          NsIP_obj->setNameServerFlag(true);
	          NsIP_obj->setNameServer(NsVector[i]);
	        
	          // add NsIP obj to Vector of NsIP objects from parent
	          NsIPVector.push_back(NsIP_obj);
	          retval = 1;
	        }	
	        else
	        {
	          for (size_t j=0; j< IpVector.size(); j++)
	          {
	            ps_elog(PSL_DEBUG, "Nameserver: %s  IPv4: %s\n", NsVector[i].c_str(), IpVector[j].c_str());    
	            // create NsIP object and fill it up
	            // serial ,aabit, etc are FALSE by NsIP default constructor
	            NsIP *NsIP_obj = new NsIP();
	            NsIP_obj->setNameServerFlag(true);
	            NsIP_obj->setNameServer(NsVector[i]);
                    NsIP_obj->setIPFlag(true);
		    NsIP_obj->setIP(IpVector[j]);

	            // add NsIP obj to Vector of NsIP objects from parent
	            NsIPVector.push_back(NsIP_obj);
	            retval = 1;
	          }
	        }
	      }  
	    }
	  }
        }  
      }// answer section == 0
      else
      {
        vector<string> NsVector = obj->getNSinAnswerSection( zone );
        if (NsVector.size() == 0)
        {
          ps_elog(PSL_CRITICAL, "There are no nameservers! Zone: %s\n", zone.c_str() );
        }
        else
        {
          ps_elog(PSL_DEBUG, "There are %d nameservers in Answer section! Zone: %s\n", (int)(NsVector.size()) , zone.c_str());
          for (size_t i=0; i< NsVector.size(); i++)
          {
            vector<string> IpVector;
	    // pass IpVector by reference
	    // check if recursive resolver send A query
	    if ( getIPv4address(obj, NsVector[i], IpVector) > 0)
	    {
	      // check size of received data in vector
	      if (IpVector.size() == 0)
	      {
	        ps_elog(PSL_CRITICAL, "No IP address is found for nameserver: %s! Zone: %s\n", NsVector[i].c_str(), zone.c_str() );
	        // Even there is no IP address, push back NsIP object
	        // We dont want to loose data anyway!
	        NsIP *NsIP_obj = new NsIP();
	        NsIP_obj->setNameServerFlag(true);
	        NsIP_obj->setNameServer(NsVector[i]);
	        
	        // add NsIP obj to Vector of NsIP objects from parent
	        NsIPVector.push_back(NsIP_obj);
	        retval = 1;
	      }
	      else
	      {
	        for (size_t j=0; j< IpVector.size(); j++)
	        {
	          ps_elog(PSL_DEBUG, "Nameserver: %s  IPv4: %s\n", NsVector[i].c_str(), IpVector[j].c_str());    
	          // create NsIP object and fill it up
	          // serial ,aabit, etc are FALSE by NsIP default constructor
	          NsIP *NsIP_obj = new NsIP();
	          NsIP_obj->setNameServerFlag(true);
	          NsIP_obj->setNameServer(NsVector[i]);
                  NsIP_obj->setIPFlag(true);
	          NsIP_obj->setIP(IpVector[j]);

	          // add NsIP obj to Vector of NsIP objects from parent
	          NsIPVector.push_back(NsIP_obj);
	          retval = 1;
	        }
	      }
	    }  
	  }  
        }
      } // answer section header check
    }//tc if set
  }//if (obj->sendNSquery(zone) < 0)
  
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get Ns;IP pairs that are stored from authority name server 
 * Input:   RerevseDNS class pointer,   zone to lookup, 
 *          IP of authority nameserver 
 *	    Ns;IP object vector to fill up  
 * Output: -1 failure, 1 success 
 * Mikhail Strizhov @ Aug 11, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findNsIPsetAuth(ReverseDNS *obj, std::string zone, std::string ip, std::vector<NsIP *>& NsIPVector_to )
{
  int retval = -1;
  string null_str = "NULL";

  if (ip.compare(null_str) != 0)
  {
    // set authoritative nameserver IP 
    obj->setResolverIPaddress(ip);
    ps_elog(PSL_DEBUG, "Using %s IP address for queries\n", obj->getResolverIPaddress().c_str());

    if (findNsIPset( obj,  zone, NsIPVector_to) > 0)
    {
      retval = 1;
    }
  }  

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: create and send DNS packet for A record
 * Input: RerevseDNS class pointer,  string  nameserver, vector with ip addresses by reference
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::getIPv4address(ReverseDNS *obj, std::string nameserver, std::vector<std::string>& IpVector)
{
  int retval = -1;

  // always use recursive resolver for getting A records
  obj->setResolverIPaddress( getDefaultNameserver() );
  
  // send A query to recursive resolver
  if (obj->sendAquery(nameserver) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to send A query for '%s' nameserver\n", nameserver.c_str());
  }
  else
  {
    // check if Answer counter in DNS header is not 0
    if (obj->checkAnswerSection() < 0)
    {
      ps_elog(PSL_CRITICAL, "Answer section is empty. Nameserver: %s\n", nameserver.c_str());
      // Otherwise, check Authority counter in DNS header
      if (obj->checkAuthoritySection() < 0)
      {
        ps_elog(PSL_CRITICAL, "Authority section is empty. Nameserver: %s\n", nameserver.c_str());
      }
      else
      {
        IpVector = obj->getAinAuthoritySection(nameserver);
	retval = 1;
      }
    }
    else
    {
      IpVector = obj->getAinAnswerSection(nameserver);
      retval = 1;
    }
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get NS;IP set that is delegated at parent zone
 * Input: RerevseDNS class pointer,  zone string,   two NS;IP object vectors
 * Output:  -1 failure, 1 success
 * Mikhail Strizhov @ Aug 10, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findParentNSset(  ReverseDNS *obj,  
                                 std::string zone,  
				 std::vector<NsIP *>& NsIPVector_from, 
				 std::vector<NsIP *>& NsIPVector_to
				 )
{
  int retval = -1;
  
  for (size_t i=0; i< NsIPVector_from.size(); i++)
  {
    // unpack
    NsIP *NsIP_obj = NsIPVector_from[i];
    // check if IP address Flag is enabled 
    if (NsIP_obj->getIPFlag() == true)
    {
      ps_elog(PSL_DEBUG, "Choosen nameserver: %s with IP: %s\n", 
	                                         NsIP_obj->getNameServer().c_str(),
						 NsIP_obj->getIP().c_str()
						 );
      // set authoritative nameserver IP 
      string ip = NsIP_obj->getIP();
      if (findNsIPsetAuth(obj, zone, ip, NsIPVector_to) > 0)
      {
        retval = 1;
	break;
      }
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Nameserver: %s does not have IP address!\n", NsIP_obj->getNameServer().c_str());
    }
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: go over NS;IP pairs and get SOA serial numbers
 * Input: RerevseDNS class pointer,  zone string,  NsIP object vector
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 10, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findSOAserial(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector)
{
  int retval = -1;
  
  for (size_t i=0; i< NsIPVector.size(); i++)
  {
    // unpack
    NsIP *NsIP_obj = NsIPVector[i];

    if (NsIP_obj->getIPFlag() == true)
    {
      ps_elog(PSL_DEBUG, "Choosen nameserver: %s with IP: %s\n", 
	                                         NsIP_obj->getNameServer().c_str(),
						 NsIP_obj->getIP().c_str()
						 );
      // set authoritative nameserver IP 
      obj->setResolverIPaddress(NsIP_obj->getIP());
 
      // fill up NsIP object serial and AA bit flags and values
      if (checkSOAserial(obj, zone,  NsIP_obj) > 0)
      {
        ps_elog(PSL_DEBUG, "Serial number: %d, AA bit: %d\n", 
	                                         NsIP_obj->getSerial(),
						 (int)(NsIP_obj->getAAbitFlag()));
        retval = 1;
      }
    }
    else
    {
      ps_elog(PSL_CRITICAL, "Nameserver: %s does not have IP address!\n", NsIP_obj->getNameServer().c_str());
    }
  }  
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: create and send DNS packet for SOA Serial number
 * Input: RerevseDNS class pointer,  string zone,  reference to serial, reference to aa bit 
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 10, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::checkSOAserial(ReverseDNS *obj, std::string tmp_zone, NsIP *NsIP_obj )
{
  int retval = -1;

  // send SOA query 
  if (obj->sendSOAquery(tmp_zone) < 0)
  {
    ps_elog(PSL_CRITICAL, "Unable to send SOA query for '%s' zone\n", tmp_zone.c_str());
  }
  else
  {
    // if SOA was send successfull, set online flag to online
    NsIP_obj->setOnlineFlag(true);
    
    // AA bit is enabled?
    if (obj->checkAAbit() == true)
    {
      NsIP_obj->setAAbitFlag(true);
    }

    // check RCODE in header, should not contain ERROR
    if (obj->checkHeaderRCODE() < 0)
    {
      ps_elog(PSL_CRITICAL, "RCODE in DNS header contains error! Zone: %s\n", tmp_zone.c_str());
    }
    else
    {
      // check if Answer counter in DNS header is not 0
      if (obj->checkAnswerSection() < 0)
      {
        ps_elog(PSL_CRITICAL, "Answer section is empty. Zone: %s\n", tmp_zone.c_str());
	if (obj->checkAuthoritySection() < 0)
	{
          ps_elog(PSL_CRITICAL, "Authority section is empty. Zone: %s\n", tmp_zone.c_str());
	}
	else
	{
	  if (obj->checkSOAinAuthoritySection(tmp_zone) < 0)
	  {
            ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", tmp_zone.c_str());
	  }
	  else
	  {
	    // get serial
	    uint32_t serial = -1;
	    if (obj->getSOAserialAuthoritySection(tmp_zone, serial) < 0)
	    {
	      ps_elog(PSL_CRITICAL, "Could not get SOA serial number! Zone: %s\n", tmp_zone.c_str());
	    }
	    else
	    {
	      NsIP_obj->setSerialFlag(true);
	      NsIP_obj->setSerial(serial);
              retval = 1;
	    }
	  }
	}
      }
      else
      {
        // Compare zone we ask and zone in reply in Answer section
        if (obj->checkSOAinAnswerSection(tmp_zone) < 0)
        {
          ps_elog(PSL_CRITICAL, "Zone %s does not exist!\n", tmp_zone.c_str());
        }
        else
        {
	  // get serial
	  uint32_t serial = -1;
	  if (obj->getSOAserialAnswerSection(tmp_zone, serial) < 0)
	  {
	    ps_elog(PSL_CRITICAL, "Could not get SOA serial number! Zone: %s\n", tmp_zone.c_str());
	  }
	  else
	  {
	    NsIP_obj->setSerialFlag(true);
	    NsIP_obj->setSerial(serial);
            retval = 1;
	  }
        }
      }
    }// if (obj->checkHeaderRCODE() < 0)
  }// if (obj->sendSOAquery(tmp_zone) < 0)

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: Compare serial numbers from NsIPVector
 *          if all serials are equal, choose randmon NS;IP pair
 *          if not, choose MAX serial and choose corresponding NS;IP pair
 * Input: vector of NsIP object,   reference to choosen index
 * Output: -1 failure, 1 - success
 * Mikhail Strizhov @ Aug 11, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::compareSOAserial( std::vector<NsIP *>& NsIPVector, std::string& ip )
{
  int retval = -1;

  vector<uint32_t> SerialVector;
  vector<NsIP *> ChoosenPairs;
  int difference_flag = 0;
  
  // put all NsIP pairs to special vector
  for (size_t i=0; i< NsIPVector.size(); i++)
  {
    // unpack
    NsIP *NsIP_obj = NsIPVector[i];
    // put only nameserver with IP and Serial numbers
    if ( (NsIP_obj->getIPFlag() == true) && (NsIP_obj->getSerialFlag() == true) )
    {
      ChoosenPairs.push_back(NsIP_obj);
    }  
  }
  
  // now get all serial numbers
  for (size_t i= 0; i< ChoosenPairs.size(); i++)
  {
    NsIP *NsIP_obj = ChoosenPairs[i];
    uint32_t serial_num = NsIP_obj->getSerial();
    SerialVector.push_back(serial_num);
  }
  
  // check if we have something inside
  if (SerialVector.size() != 0)
  {
    // find max element in vector  
    uint32_t max_serial = *max_element( SerialVector.begin(), SerialVector.end());
    // get index of max_serial  in choosen vector
    int index = find( SerialVector.begin(), SerialVector.end(), max_serial) - SerialVector.begin();
    ps_elog(PSL_DEBUG, "Max serial number: %d\n", max_serial);
 
    // compare max_serial to other elements
    for (size_t i=0; i< SerialVector.size(); i++)
    {
      if (max_serial != SerialVector[i])
      {
        difference_flag = 1;
        break;
      }
    }

    // if difference_flag set to 1, we pick index of max_serial number
    // if difference_flag set to 0, we pick random index in SerialVector
    if (difference_flag == 1)
    {
      // assign ip
      NsIP *NsIP_obj = ChoosenPairs[index];
      ip = NsIP_obj->getIP();
      ps_elog(PSL_CRITICAL, "Serial numbers are different!\n");
      ps_elog(PSL_CRITICAL, "Choosen nameserver: %s, IP: %s\n", NsIP_obj->getNameServer().c_str(),
                                                              NsIP_obj->getIP().c_str());
      retval = 1;
    }
    else
    {
      // choose random
      srand ( time(NULL) );
      int number = rand() % (int)SerialVector.size(); 
      NsIP *NsIP_obj = ChoosenPairs[number];
      ip = NsIP_obj->getIP();
      ps_elog(PSL_DEBUG, "Serial numbers are equal!\n" );
      ps_elog(PSL_DEBUG, "Choosen nameserver: %s with IP: %s\n", NsIP_obj->getNameServer().c_str(),
                                                               NsIP_obj->getIP().c_str());
      retval = 1;
    }
  }// size !=0  
  
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get Ns;IP pairs that are stored at child zone 
 * Input:   RerevseDNS class pointer,   zone to lookup, 
 *          IP of authority nameserver 
 *	    Ns;IP object vector to fill up  
 * Output: -1 failure, 1 success 
 * Mikhail Strizhov @ Aug 11, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::findChildNSset(ReverseDNS *obj,  std::string zone, std::string ip, std::vector<NsIP *>& NsIPVector_to )
{
  return findNsIPsetAuth(obj, zone, ip, NsIPVector_to);
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
    printf("Done\n");
    bRet = true;
  }
  else
  {
    printf("NOT DONE\n");
  }
  return bRet;
}

/*--------------------------------------------------------------------------------------
 * Purpose: special case for handling retarded zone owners 
 * Input:    
 * Output: -1 failure, 1 success 
 * Mikhail Strizhov @ Aug 11, 2011
 * -------------------------------------------------------------------------------------*/
int CrawlTask::getCorrectNSset(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector)
{
  int retval =  -1;

  vector<string> NsVector = obj->getNSinAuthoritySection_special();
  for (size_t i=0; i< NsVector.size(); i++)
  {
    if (retval == 1)
    {
      break;
    }

    vector<string> IpVector;
    // pass IpVector by reference
    if ( getIPv4address(obj, NsVector[i], IpVector) < 0)
    {
      ps_elog(PSL_CRITICAL, "No IP addresses could be found. Nameserver: %s\n", NsVector[i].c_str());
    }
    else
    {
      if (IpVector.size() == 0)
      {
        ps_elog(PSL_CRITICAL, "Vector of IP addresses is empty!\n");
      }
      else
      {
        for (size_t j=0; j<IpVector.size(); j++)
        {
          string ip = IpVector[j];
          if (findNsIPsetAuth(obj, zone, ip, NsIPVector) < 0)
	  {
            ps_elog(PSL_CRITICAL, "Could not get list of nameservers!\n");
	  }
	  else
	  {
	    retval = 1;
	    break;
	  }
        }
      }
    }
  }  

  return retval;
}  
