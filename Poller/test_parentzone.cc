#include <stdio.h>
#include <string>
#include <vector>

#include "ReverseDNS.h"
#include <vantages/dns_soa.h>
#include <vantages/dns_rr_fact.h>
#include <vantages/dns_name.h>

using namespace std;

int main(int argc, char *argv[])
{
  string childzone = "138.82.129.in-addr.arpa.";
  // correct parent
  string parentzone = "82.129.in-addr.arpa.";

  
  ReverseDNS *obj = new ReverseDNS();

  string tmp_parent = obj->cropChildZone(childzone);
  if (tmp_parent.compare(parentzone) !=0 )
  {
    fprintf(stderr, "Parent zone test failed!\n");
  }

  obj->getDnsPacket().clear();
    
  DnsName oName = parentzone;
  DnsSoa *soa = (DnsSoa *) DnsRrFactory::getInstance().create(DNS_RR_SOA);
  soa->setRName(parentzone);
  soa->setMName(parentzone);
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
      if (obj->checkSOAinAnswersSection(parentzone ) < 0)
      { 
        fprintf(stderr, "FAILURE!!\n");
      }
      else
      {
          fprintf(stderr, "SUCCESS!\n"); 
      }
    }

  }

  obj->setParentZone(parentzone);
  string tmp = obj->getParentZone();
  if (tmp.compare(parentzone) != 0)
  {
    fprintf(stderr, "Could not set Parent Zone in reverse dns class!\n");
  }

  delete obj;
  return 0;
}
