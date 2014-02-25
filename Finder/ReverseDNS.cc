#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <string>
#include <list>
#include <iostream>
#include <getopt.h>
#include <fstream>

// libvdns
#include <vantages/dns_ns.h>
#include <vantages/dns_a.h>
#include <vantages/dns_soa.h>
#include <vantages/dns_header.h>
#include <vantages/dns_defs.h>

#include "ps_logger.h"

#include "ReverseDNS.h"

using namespace std;


/*--------------------------------------------------------------------------------------
 * Purpose: constructor
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
ReverseDNS::ReverseDNS( )
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: desstructor
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
ReverseDNS::~ReverseDNS()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: get ip address as string
 * Input: uint32_t  
 * Output: string
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
std::string ReverseDNS::getResolverIPaddress()
{
  uint32_t tmp = oRes.getNameserver();

  std::string sRet;
  char szIP[16];
  memset(szIP, 0, 16);
  sprintf(szIP, "%d.%d.%d.%d",
            (tmp >> 24) & 0x00ff,
            (tmp >> 16) & 0x00ff,
            (tmp >> 8) & 0x00ff,
            (tmp) & 0x00ff);
  
  sRet = szIP;
  return sRet;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set IP address for auth/recursive  queries
 * Input: IP address string  
 * Output: none
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setResolverIPaddress(std::string ip_string)
{
  uint32_t uIP = 0;
  struct sockaddr_in sa;
  inet_pton(AF_INET, (ip_string.c_str()), &(sa.sin_addr));
  uIP = (htonl)(sa.sin_addr.s_addr);
  // set auth nameserver 
  oRes.setNameserver(uIP);
}

/*--------------------------------------------------------------------------------------
 * Purpose: set IP address for auth/recursive  queries
 * Input: IP address uint32_t  
 * Output: none
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setResolverIPaddress(uint32_t uIP)
{
  oRes.setNameserver(uIP);
}
/*--------------------------------------------------------------------------------------
 * Purpose: set timestamp
 * Input:   time_t
 * Output:  none
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setTimeStamp(time_t timestamp)
{
  timestamp_ = timestamp;
}

/*--------------------------------------------------------------------------------------
 * Purpose: return DnsResolver object
 * Input:  none
 * Output: reference to DnsResolver object 
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
DnsResolver& ReverseDNS::getDnsResolver()
{
  return oRes;
}

/*--------------------------------------------------------------------------------------
 * Purpose: return DnsPacket object
 * Input:  none
 * Output: reference to DnsPacket object 
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
DnsPacket& ReverseDNS::getDnsPacket()
{
  return oResp; 
}
/*--------------------------------------------------------------------------------------
 * Purpose: get timestamp
 * Input:   none
 * Output:  time_t
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
time_t ReverseDNS::getTimeStamp()
{
  return timestamp_;
}
/*--------------------------------------------------------------------------------------
 * Purpose: send DNS_RR_SOA packet
 * Input:  string zone
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::sendSOAquery(std::string& zone)
{
  int retval = 1;
  // clean oResp DNS packet
  oResp.clear();
  if (!oRes.send(zone, DNS_RR_SOA, oResp))
  {
    retval = -1;
  }
  return retval;
} 
  
/*--------------------------------------------------------------------------------------
 * Purpose: check RCODE in dns header
 * Input:  string zone
 * Output: -1 error, 1 - no error
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::checkHeaderRCODE()
{
  int retval = 1;
  
  if (oResp.getHeader().get_rcode() != DNS_NOERROR)
  {
    retval = -1;
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: check if Answer section exist
 * Input:  
 * Output: -1 is 0, 1 - not eq to 0
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::checkAnswerSection()
{
  int retval = 1;

  if (oResp.getHeader().an_count() == 0)
  {
    retval = -1;
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: check if Authority section exist
 * Input:  
 * Output: -1 error, 1 - no error
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::checkAuthoritySection()
{
  int retval = 1;
  if (oResp.getHeader().ns_count() == 0)
  {
    retval = -1;
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: check check SOA record existence in answers section
 * Input:  
 * Output: -1 error, 1 - success, 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::checkSOAinAnswersSection(std::string& zone )
{
  int retval = -1;

  RRList_t tAns;
  oResp.getAnswers(tAns);    
  RRIter_t tIter;
  for (tIter = tAns.begin(); tAns.end() != tIter; tIter++)
  {
    DnsRR *pRR = *tIter;
    if ( (DNS_RR_SOA == pRR->type() )  )
    {
      string zone_tmp = pRR->getName();
      if (zone_tmp.compare(zone) == 0)
      {
        retval = 1;
      }
    }
  }// for loop

  return retval;
}	  

/*--------------------------------------------------------------------------------------
 * Purpose: check check SOA record existence in authority section
 * Input:  
 * Output: -1 error, 1 - success, 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::checkSOAinAuthoritySection(std::string& zone )
{
  int retval = -1;

  RRList_t tAns;
  oResp.getAuthority(tAns);    
  RRIter_t tIter;
  for (tIter = tAns.begin(); tAns.end() != tIter; tIter++)
  {
    DnsRR *pRR = *tIter;
    if ( (DNS_RR_SOA == pRR->type() )  )
    {
      string zone_tmp = pRR->getName();
      if (zone_tmp.compare(zone) == 0)
      {
        retval = 1;
      }
    }
  }// for loop

  return retval;
}	  
