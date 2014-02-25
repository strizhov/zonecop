#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <string>
#include <vector>
#include <iostream>
#include <getopt.h>
#include <fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// libvdns
#include <vantages/dns_ns.h>
#include <vantages/dns_a.h>
#include <vantages/dns_aaaa.h>
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
 * Purpose: destructor
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
ReverseDNS::~ReverseDNS()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: erase memory after we done
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::clearNsIPVector_fromparent()
{
  for (size_t i =0; i< getNsIPVector_fromparent().size(); i++ )
  {
    NsIP *NsIP_obj = getNsIPVector_fromparent()[i];
    delete NsIP_obj;
  }
}
void ReverseDNS::clearNsIPVector_atparent()
{
  for (size_t i =0; i< getNsIPVector_atparent().size(); i++ )
  {
    NsIP *NsIP_obj = getNsIPVector_atparent()[i];
    delete NsIP_obj;
  }
}
void ReverseDNS::clearNsIPVector_atchild()
{
  for (size_t i =0; i< getNsIPVector_atchild().size(); i++ )
  {
    NsIP *NsIP_obj = getNsIPVector_atchild()[i];
    delete NsIP_obj;
  }
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
 * Purpose: save NsIP vector at parent zone 
 * Input: vector of NsIP objects  
 * Output: none
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setNsIPVector_atparent( NsIP *NsIP_obj)
{
  NsIPVector_atparent_.push_back(NsIP_obj);  
}

/*--------------------------------------------------------------------------------------
 * Purpose: return NsIP vector at parent
 * Input: none  
 * Output: vector of NsIP objects
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<NsIP *>& ReverseDNS::getNsIPVector_atparent( )
{
  return NsIPVector_atparent_; 
}

/*--------------------------------------------------------------------------------------
 * Purpose: save NsIP vector at child zone 
 * Input:   vector of NsIP objects
 * Output: none
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setNsIPVector_atchild( NsIP *NsIP_obj  )
{
  NsIPVector_atchild_.push_back(NsIP_obj);
}

/*--------------------------------------------------------------------------------------
 * Purpose: return NsIP vector at child
 * Input:   none
 * Output:  vector of NsIP objects
 * Mikhail Strizhov @ Aug 9, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<NsIP *>& ReverseDNS::getNsIPVector_atchild( )
{
  return NsIPVector_atchild_; 
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
 * Purpose: set child zone
 * Input:  string 
 * Output: none
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setChildZone(std::string childzone)
{
  childzone_ = childzone;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get child zone
 * Input:  none 
 * Output: string
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
std::string ReverseDNS::getChildZone()
{
  return childzone_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set parent zone
 * Input:  string 
 * Output: none
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setParentZone(std::string parentzone)
{
  parentzone_ = parentzone;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get parent zone
 * Input:  none 
 * Output: string
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
std::string ReverseDNS::getParentZone()
{
  return parentzone_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: crop parent zone
 * Input:  child zone  string
 * Output: parent zone string
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
std::string ReverseDNS::cropChildZone(std::string name)
{
  string dot = ".";
  size_t found=name.find(dot);
  if (found!=string::npos)
  {
    name.erase (0, found+1);
  }

  return name;
}


/*--------------------------------------------------------------------------------------
 * Purpose: send DNS_RR_SOA packet
 * Input:  string zone
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::sendSOAquery(std::string zone)
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
int ReverseDNS::checkSOAinAnswerSection(std::string zone )
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
int ReverseDNS::checkSOAinAuthoritySection(std::string zone )
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

/*--------------------------------------------------------------------------------------
 * Purpose: send DNS_RR_NS packet
 * Input:  string zone
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::sendNSquery(std::string zone)
{
  int retval = 1;
  // clean oResp DNS packet
  oResp.clear();
  
  if (!oRes.send(zone, DNS_RR_NS, oResp))
  {
    ps_elog(PSL_CRITICAL, "Unable to retrieve NS set. Zone: %s\n", zone.c_str());
    retval = -1;
  }
  return retval;
} 
    
/*--------------------------------------------------------------------------------------
 * Purpose: check AA bit in DNS header
 * Input:  
 * Output: 1 is not set, -1 is set
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
bool ReverseDNS::checkAAbit()
{
  bool retval = false;
  if (oResp.getHeader().get_aa() == true)
  {
      retval = true;
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: check TC bit in DNS header
 * Input:  
 * Output: 1 is not set, -1 is set
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
bool ReverseDNS::checkTCbit()
{
  bool retval = false;
  if (oResp.getHeader().get_tc() == true)
  {
      retval = true;
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: switch TCP mode
 * Input:  bool flag 
 * Output: 
 * Mikhail Strizhov @ Aug 25, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setTCP(bool flag)
{
  if (flag == true)
  {
    // tcp enable
    getDnsResolver().setTcp(DNS_RES_FORCE);
  }
  else
  { 
    // no tcp
    getDnsResolver().setTcp(DNS_RES_FALLBACK);
  }
} 

/*--------------------------------------------------------------------------------------
 * Purpose: get NS set from answer section
 * Input:  zone string
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getNSinAnswerSection(std::string zone )
{
  vector<string> NameVector;
  
  RRList_t tAns;
  oResp.getAnswers(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
       tAns.end() != tIter;
       tIter++)
  {
    DnsRR *pRR = *tIter;
    // get name in section
    string zone_tmp = pRR->getName();

    // Look for NS set type and check if zone in reply is eq to zone we ask for
    if ( (DNS_RR_NS == pRR->type() ) && (  zone_tmp.compare(zone) == 0) )
    {
      string sNameserver = (static_cast<DnsNs *>(pRR))->getName();
      NameVector.push_back(sNameserver);
    }
  }
  return NameVector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get NS set from authority section
 * Input: string zone 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getNSinAuthoritySection(std::string zone )
{
  vector<string> NameVector;
  
  RRList_t tAns;
  oResp.getAuthority(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
       tAns.end() != tIter;
       tIter++)
  {
    DnsRR *pRR = *tIter;
    // get name in section
    string zone_tmp = pRR->getName();

    // Look for NS set type and check if zone in reply is eq to zone we ask for
    if ( (DNS_RR_NS == pRR->type() ) && (  zone_tmp.compare(zone) == 0) )
    {
      string sNameserver = (static_cast<DnsNs *>(pRR))->getName();
      NameVector.push_back(sNameserver);
    }
  }
  return NameVector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get NS set from authority section
 *          this case is special, because we done check domain name reply,
 *          just retreiving nameset
 * Input: none 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getNSinAuthoritySection_special()
{
  vector<string> NameVector;
  
  RRList_t tAns;
  oResp.getAuthority(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
       tAns.end() != tIter;
       tIter++)
  {
    DnsRR *pRR = *tIter;

    // Look for NS set type 
    if ( DNS_RR_NS == pRR->type()   )
    {
      string sNameserver = (static_cast<DnsNs *>(pRR))->getName();
      NameVector.push_back(sNameserver);
    }
  }
  return NameVector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: send DNS_RR_A packet
 * Input:  string nameserver
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::sendAquery(std::string nameserver)
{
  int retval = 1;
  // clean oResp DNS packet
  oResp.clear();
  
  if (!oRes.send(nameserver, DNS_RR_A, oResp))
  {
    retval = -1;
  }
  return retval;
} 

/*--------------------------------------------------------------------------------------
 * Purpose: get A set from answers section
 * Input: string nameserver 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getAinAnswerSection(std::string nameserver )
{
  vector<string> IpVector;
  
  RRList_t tAns;
  oResp.getAnswers(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
            tAns.end() != tIter;
            tIter++)
  {
    DnsRR *pRR = *tIter;
    
    string nameserver_tmp = pRR->getName();

    // Look for NS set type and check if zone in reply is eq to zone we ask for
    if ( (DNS_RR_A == pRR->type()  ) && (  nameserver.compare(nameserver_tmp) == 0 )  )
    {
      uint32_t uIP = ntohl ( (static_cast<DnsA *>(pRR))->ip() );
      char ip_addr[INET_ADDRSTRLEN];
      if (inet_ntop(AF_INET, &(uIP), ip_addr, INET_ADDRSTRLEN) != NULL)
      {
	string str(ip_addr);        
        IpVector.push_back(str);
      }	
      else
      {
        ps_elog(PSL_CRITICAL, "IPv4 address is invalid!\n");
      }
//     string str =  (static_cast<DnsA *>(pRR))->toString() ;
//      IpVector.push_back(str);
    }
  }
  return IpVector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get A set from authority section
 * Input: string nameserver 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getAinAuthoritySection(std::string nameserver )
{
  vector<string> IpVector;
  
  RRList_t tAns;
  oResp.getAuthority(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
            tAns.end() != tIter;
            tIter++)
  {
    DnsRR *pRR = *tIter;
    string nameserver_tmp = pRR->getName();

    // Look for NS set type and check if zone in reply is eq to zone we ask for
    if ( (DNS_RR_A == pRR->type()  ) && (  nameserver.compare(nameserver_tmp) == 0 )  )
    {
      uint32_t uIP = ntohl ( (static_cast<DnsA *>(pRR))->ip() );
      char ip_addr[INET_ADDRSTRLEN];
      if (inet_ntop(AF_INET, &(uIP), ip_addr, INET_ADDRSTRLEN) != NULL)
      {
	string str(ip_addr);        
        IpVector.push_back(str);
      }	
      else
      {
        ps_elog(PSL_CRITICAL, "IPv4 address is invalid!\n");
      }
//      string str =  (static_cast<DnsA *>(pRR))->toString() ;
//      IpVector.push_back(str);
    }
  }
  return IpVector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: send DNS_RR_AAAA packet
 * Input:  string nameserver
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::sendAAAAquery(std::string nameserver)
{
  int retval = 1;
  // clean oResp DNS packet
  oResp.clear();
  
  if (!oRes.send(nameserver, DNS_RR_AAAA, oResp))
  {
    retval = -1;
  }
  return retval;
} 

/*--------------------------------------------------------------------------------------
 * Purpose: get AAAA set from answers section
 * Input: string nameserver 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getAAAAinAnswerSection(std::string nameserver )
{
  vector<string> Ip6Vector;
  
  RRList_t tAns;
  oResp.getAnswers(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
            tAns.end() != tIter;
            tIter++)
  {
    DnsRR *pRR = *tIter;
    if (DNS_RR_AAAA == pRR->type())
    {
/*      
        uint32_t uIP = ntohl ( (static_cast<DnsA *>(pRR))->ip() );
	char ip_addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(uIP), ip_addr, INET_ADDRSTRLEN);
	string str(ip_addr);        
*/
      string str =  (static_cast<DnsA *>(pRR))->toString() ;
      Ip6Vector.push_back(str);
    }
  }
  return Ip6Vector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get AAAA set from authority section
 * Input: string nameserver 
 * Output: string vector 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ReverseDNS::getAAAAinAuthoritySection(std::string nameserver )
{
  vector<string> Ip6Vector;
  
  RRList_t tAns;
  oResp.getAuthority(tAns);
  RRIter_t tIter;
  for (tIter = tAns.begin();
            tAns.end() != tIter;
            tIter++)
  {
    DnsRR *pRR = *tIter;
    if (DNS_RR_AAAA == pRR->type())
    {
/*      
        uint32_t uIP = ntohl ( (static_cast<DnsA *>(pRR))->ip() );
	char ip_addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(uIP), ip_addr, INET_ADDRSTRLEN);
	string str(ip_addr);        
*/
      string str =  (static_cast<DnsA *>(pRR))->toString() ;
      Ip6Vector.push_back(str);
    }
  }
  return Ip6Vector;
}

/*--------------------------------------------------------------------------------------
 * Purpose: return vector of NsIP objects
 * Input:  string zone
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<NsIP *>& ReverseDNS::getNsIPVector_fromparent()
{
  return NsIPVector_fromparent_; 
}

/*--------------------------------------------------------------------------------------
 * Purpose: push back NsIP object to NsIP vector from parent zone
 * Input:  NsIP object
 * Output: none
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
void ReverseDNS::setNsIPVector_fromparent(NsIP *NsIP_obj)
{
  NsIPVector_fromparent_.push_back(NsIP_obj);
}

/*--------------------------------------------------------------------------------------
 * Purpose: get SOA SERIAL number
 * Input:  string zone
 * Output: -1 error, 1 - success, 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::getSOAserialAnswerSection(std::string zone, uint32_t& serial )
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
      serial = ((DnsSoa *)(pRR))->getSerial();
      retval = 1;
    }
  }// for loop

  return retval;
}	  

/*--------------------------------------------------------------------------------------
 * Purpose: get SOA SERIAL number
 * Input:  string zone
 * Output: -1 error, 1 - success, 
 * Mikhail Strizhov @ June 23, 2011
 * -------------------------------------------------------------------------------------*/
int ReverseDNS::getSOAserialAuthoritySection(std::string zone, uint32_t& serial )
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
      serial = ((DnsSoa *)(pRR))->getSerial();
      retval = 1;
    }
  }// for loop

  return retval;
}

