#include <stdio.h>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ReverseDNS.h"
#include <vantages/dns_ns.h>
#include <vantages/dns_rr_fact.h>
#include <vantages/dns_name.h>
#include <vantages/dns_rr.h>
#include <vantages/dns_opt.h>
#include <vantages/dns_defs.h>
#include <vantages/dns_a.h>


using namespace std;

int main(int argc, char *argv[])
{
  string childzone = "138.82.129.in-addr.arpa.";
  // correct parent
  string parentzone = "82.129.in-addr.arpa.";
  string ns1 = "dns1.colostate.edu.";
  string ip1 = "129.82.103.121";
  string ns2 = "dns2.colostate.edu.";
  string ip2 = "165.127.125.178";

  string default_resolver = "129.82.138.3";
  
  ReverseDNS *obj = new ReverseDNS();
  
  //set default resolver
  uint32_t uIP = 0;
  struct sockaddr_in  sa;
  inet_pton(AF_INET, (default_resolver.c_str()), &(sa.sin_addr));
  uIP = (htonl)(sa.sin_addr.s_addr);
  // set it
  obj->setResolverIPaddress(uIP);

  string tmp = obj->getResolverIPaddress();
  if (tmp.compare(default_resolver) != 0)
  {
    fprintf(stderr, "Default resolver test failed!\n");
  }
  
  // create ns packet, with nameserver in answer section
  obj->getDnsPacket().clear();
    
  DnsName oName = parentzone;
  DnsNs *nsset1 = (DnsNs *) DnsRrFactory::getInstance().create(DNS_RR_NS);
  nsset1->set_name(oName);
  nsset1->set_class(DNS_CLASS_IN);
  nsset1->setName(ns1);
  obj->getDnsPacket().addAnswer(*nsset1);
  
  DnsNs *nsset2 = (DnsNs *) DnsRrFactory::getInstance().create(DNS_RR_NS);
  nsset2->set_name(oName);
  nsset2->set_class(DNS_CLASS_IN);
  nsset2->setName(ns2);
  obj->getDnsPacket().addAnswer(*nsset2);

  // verify out methods
  
  // check if Answer counter in DNS header is not 0
  if (obj->checkAnswerSection() < 0)
  {
    fprintf(stderr, "Answer section test failed!\n");
  }   
  else
  {
    vector<string> NsVector = obj->getNSinAnswerSection( parentzone );
    if (NsVector.size() == 0)
    {
      fprintf(stderr, "There are no nameservers!\n" );
    }
    else
    {
      fprintf(stderr, "There are %d nameservers in Answer section!\n", (int)(NsVector.size()) );
      for (size_t i=0; i< NsVector.size(); i++)
      {
        fprintf(stderr, "Nameserver: %s\n", NsVector[i].c_str() );    
      }
    }
  } 
 
  // test authority section 
  obj->getDnsPacket().clear();
    
  DnsNs *nsset3 = (DnsNs *) DnsRrFactory::getInstance().create(DNS_RR_NS);
  nsset3->set_name(oName);
  nsset3->set_class(DNS_CLASS_IN);
  nsset3->setName(ns1);
  obj->getDnsPacket().addAuthority(*nsset1);
  
  DnsNs *nsset4 = (DnsNs *) DnsRrFactory::getInstance().create(DNS_RR_NS);
  nsset4->set_name(oName);
  nsset4->set_class(DNS_CLASS_IN);
  nsset4->setName(ns2);
  obj->getDnsPacket().addAuthority(*nsset2);

  // check if Answer counter in DNS header is not 0
  if (obj->checkAuthoritySection() < 0)
  {
    fprintf(stderr, "Answer section test failed!\n");
  }   
  else
  {
    vector<string> NsVector = obj->getNSinAuthoritySection( parentzone );
    if (NsVector.size() == 0)
    {
      fprintf(stderr, "There are no nameservers!\n" );
    }
    else
    {
      fprintf(stderr, "There are %d nameservers in Authority section!\n", (int)(NsVector.size()) );
      for (size_t i=0; i< NsVector.size(); i++)
      {
        fprintf(stderr, "Nameserver: %s\n", NsVector[i].c_str() );    
      }
    }
  } 


  // test A section 
  obj->getDnsPacket().clear();
    
  DnsA *ip = (DnsA *) DnsRrFactory::getInstance().create(DNS_RR_A);
  DnsName nsname = ns1;
  ip->set_name(nsname);
  ip->set_class(DNS_CLASS_IN);
  
  uint32_t ns1IP = 0;
  struct sockaddr_in  sa1;
  inet_pton(AF_INET, (ip1.c_str()), &(sa1.sin_addr));
  ns1IP = (htonl)(sa1.sin_addr.s_addr);
  
  ip->set_ip(ns1IP);
  obj->getDnsPacket().addAnswer(*ip);

  // check if Answer counter in DNS header is not 0
  if (obj->checkAnswerSection() < 0)
  {
    fprintf(stderr, "Answer section is empty!\n" );
    // Otherwise, check Authority counter in DNS header
  }
  else
  {
    vector<string> IpVector = obj->getAinAnswerSection(ns1);
    for (size_t i=0; i< IpVector.size(); i++)
    {
      fprintf(stderr, "%s IP: %s\n", ns1.c_str(), IpVector[i].c_str() );    
    }
  }


  // test NsIP object

  // NsIPVector from parent
  NsIP *NsIP_obj1 = new NsIP();
  NsIP_obj1->setNameServer(ns1);
  NsIP_obj1->setIP(ip1);

  NsIP *NsIP_obj2 = new NsIP();
  NsIP_obj2->setNameServer(ns2);
  NsIP_obj2->setIP(ip2);
  
  obj->getNsIPVector_fromparent().push_back(NsIP_obj1);
  obj->getNsIPVector_fromparent().push_back(NsIP_obj2);

  fprintf(stderr, "NsIP vector size: %d\n",(int)(obj->getNsIPVector_fromparent().size()));
  for (size_t i=0; i< (obj->getNsIPVector_fromparent().size()); i++)
  {
    NsIP  *tmp = obj->getNsIPVector_fromparent()[i];
    fprintf(stderr, "Nameserver: %s with IP %s\n", tmp->getNameServer().c_str(), tmp->getIP().c_str());
  }

  delete obj;
  return 0;
}
