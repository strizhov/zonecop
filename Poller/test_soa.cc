#include <stdio.h>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ReverseDNS.h"
#include "CrawlTask.h"
#include <vantages/dns_soa.h>
#include <vantages/dns_rr_fact.h>
#include <vantages/dns_name.h>

using namespace std;

int main(int argc, char *argv[])
{
  
  string childzone = "138.82.129.in-addr.arpa.";
  uint32_t serial = 2011082208;
  string ns1 = "flea.netsec.colostate.edu.";
  string ip1 = "129.82.138.19";
  string ns2 = "ns.shinkuro.com.";
  string ip2 = "66.92.164.104";
  uint32_t serial1 = 2011082207;
  uint32_t serial2 = 2011082210;

  string ns3 = "errornameserver.";
  
  ReverseDNS *obj = new ReverseDNS();


  obj->getDnsPacket().clear();
    
  DnsName oName = childzone;
  DnsSoa *soa = (DnsSoa *) DnsRrFactory::getInstance().create(DNS_RR_SOA);
  soa->setRName(childzone);
  soa->setMName(childzone);
  soa->setSerial(serial);
  soa->set_name(oName);
  obj->getDnsPacket().getHeader().set_aa(true);
  obj->getDnsPacket().addAnswer(*soa);

//  obj->getDnsPacket().print();
    
  // NsIP pair   
  NsIP *NsIP_obj = new NsIP();
  NsIP_obj->setNameServer(ns1);
  NsIP_obj->setIP(ip1);
  // AA bit is enabled?
  if (obj->checkAAbit() == true)
  {
    NsIP_obj->setAAbitFlag(true);
  }

  // check RCODE in header, should not contain ERROR
  if (obj->checkHeaderRCODE() < 0)
  {
    fprintf(stderr, "RCODE in DNS header contains error!\n");
  }
  else
  {
    // check if Answer counter in DNS header is not 0
    if (obj->checkAnswerSection() < 0)
    {
      fprintf(stderr, "Answer section is empty.\n");
    }
    else
    {
      // Compare zone we ask and zone in reply in Answer section
      if (obj->checkSOAinAnswersSection(childzone) < 0)
      {
        fprintf(stderr, "Zone %s does not exist!\n", childzone.c_str());
      }
      else
      {
        // get serial
	uint32_t tmpserial = -1;
	if (obj->getSOAserial(childzone, tmpserial) < 0)
	{
	  fprintf(stderr, "Could not get SOA serial number\n!"); 
	}
	else
	{
	  NsIP_obj->setSerialFlag(true);
	  NsIP_obj->setSerial(serial);
	}
      }
    }
  }
  

  fprintf(stderr, "Nameserver: %s IP: %s Serial: %d AA flag: %d\n",
                                              NsIP_obj->getNameServer().c_str(),
					      NsIP_obj->getIP().c_str(),
					      NsIP_obj->getSerial(),
					      (int)(NsIP_obj->getAAbitFlag())
					      );
  


  std::vector<std::string> tmpvec;
  CrawlTask *task = new    CrawlTask( tmpvec, NULL);

  // NsIPVector at parent
  NsIP *NsIP_obj1 = new NsIP();
  NsIP_obj1->setIPFlag(true);
  NsIP_obj1->setNameServer(ns1);
  NsIP_obj1->setIPFlag(true);
  NsIP_obj1->setIP(ip1);
  NsIP_obj1->setSerialFlag(true);
  NsIP_obj1->setSerial(serial1);
  NsIP_obj1->setAAbitFlag(true);

  NsIP *NsIP_obj2 = new NsIP();
  NsIP_obj2->setNameServerFlag(true);
  NsIP_obj2->setNameServer(ns2);
  NsIP_obj2->setIPFlag(true);
  NsIP_obj2->setIP(ip2);
  NsIP_obj2->setSerialFlag(true);
  NsIP_obj2->setSerial(serial2);
  NsIP_obj2->setAAbitFlag(true);
  
  NsIP *NsIP_obj3 = new NsIP();
  NsIP_obj3->setNameServerFlag(true);
  NsIP_obj3->setNameServer(ns3);
  
  obj->getNsIPVector_atparent().push_back(NsIP_obj1);
  obj->getNsIPVector_atparent().push_back(NsIP_obj2);
  obj->getNsIPVector_atparent().push_back(NsIP_obj3);

  fprintf(stderr, "NsIP vector size: %d\n",(int)(obj->getNsIPVector_atparent().size()));
  for (size_t i=0; i< (obj->getNsIPVector_atparent().size()); i++)
  {
    NsIP  *tmp = obj->getNsIPVector_atparent()[i];
    fprintf(stderr, "Nameserver: %s with IP %s\n", tmp->getNameServer().c_str(), tmp->getIP().c_str());
  }

  string ip;
  if ( task->compareSOAserial( obj->getNsIPVector_atparent(), ip ) < 0)
  {
    fprintf(stderr, "Could not find right Serial number!\n");
  }
  else
  {
    fprintf(stderr, "Choosen IP: %s\n", ip.c_str());
  }



  delete obj;
  delete task;
  return 0;
}
