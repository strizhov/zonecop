#include <stdio.h>
#include <string>
#include <time.h>

#include "ReverseDNS.h"

#include <vantages/dns_soa.h>
#include <vantages/dns_rr_fact.h>
#include <vantages/dns_name.h>

using namespace std;

// checks if ReverseDNS methods works correctly
int main(int argc, char *argv[])
{
  string zone = "138.82.129.in-addr.arpa.";
  
  DnsName oName = zone;

  string rec_resolver_str = "129.82.138.3";
  char   rec_resolver_char[] = "129.82.138.3";
  

  ReverseDNS *obj = new ReverseDNS();

  // test resolver
  obj->setResolverIPaddress(rec_resolver_str) ;
  string tmp = obj->getResolverIPaddress();
  if (rec_resolver_str.compare(tmp) != 0)
  {
    fprintf(stderr, "Resolver is not set up!\n");
  }

  obj->setResolverIPaddress(rec_resolver_char);
  string tmp2 (rec_resolver_char);
  tmp = obj->getResolverIPaddress();
  if (tmp2.compare(tmp) != 0)
  {
    fprintf(stderr, "Resolver is not set up!\n");
  }

  // test timestamp
  time_t testtime = time(NULL);
  obj->setTimeStamp(testtime) ;
  time_t tmptime = obj->getTimeStamp();
  if (tmptime != testtime)
  {
    fprintf(stderr, "Time in object is different!\n");
  }


  obj->getDnsPacket().clear();
    
  DnsSoa *soa = (DnsSoa *) DnsRrFactory::getInstance().create(DNS_RR_SOA);
  soa->setRName(zone);
  soa->setMName(zone);
  soa->set_name(oName);
  obj->getDnsPacket().addAnswer(*soa);
  
  if (obj->checkHeaderRCODE() < 0)
  {
    fprintf(stderr, "FAILURE! Error in RCODE in DNS Header\n");
  }
  else
  {
    if (obj->checkAnswerSection() < 0)
    {
      fprintf(stderr, "FAILURE! Can not check answer section\n");
    }
    else
    {
      if (obj->checkSOAinAnswersSection(zone ) < 0)
      { 
        fprintf(stderr, "FAILURE!!\n");
      }
      else
      {
          fprintf(stderr, "SUCCESS!\n"); 
      }
    }

  }
  
  obj->getDnsPacket().clear();
    
  DnsSoa *soa_auth = (DnsSoa *) DnsRrFactory::getInstance().create(DNS_RR_SOA);
  soa_auth->setRName(zone);
  soa_auth->setMName(zone);
  soa_auth->set_name(oName);
  obj->getDnsPacket().addAuthority(*soa_auth);

  obj->getDnsPacket().getHeader().set_rcode(DNS_NOERROR);


  if (obj->checkHeaderRCODE() < 0)
  {
    fprintf(stderr, "FAILURE! Error in RCODE in DNS Header\n");
  }
  else
  {
    if (obj->checkAuthoritySection() < 0)
    {
      fprintf(stderr, "FAILURE! Can not check answer section\n");
    }
    else
    {
      if (obj->checkSOAinAuthoritySection(zone ) < 0)
      { 
        fprintf(stderr, "FAILURE!!\n");
      }
      else
      {
          fprintf(stderr, "SUCCESS!\n"); 
      }
    }

  }
  
  return 0;
}

