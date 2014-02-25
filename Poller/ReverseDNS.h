#include <stdio.h>
#include <vector>

#include <vantages/dns_resolver.h>
#include <vantages/dns_packet.h>

#include "NsIP.h"

class ReverseDNS  
{
  private:
    DnsResolver oRes;
    DnsPacket   oResp;
    
    time_t      timestamp_;
    std::string childzone_;
    std::string parentzone_;

    std::vector<NsIP *> NsIPVector_fromparent_;
    std::vector<NsIP *> NsIPVector_atparent_;
    std::vector<NsIP *> NsIPVector_atchild_;

  public:
    ReverseDNS();
    virtual ~ReverseDNS();
   
    // Resolver methods
    DnsResolver& getDnsResolver();
    DnsPacket&   getDnsPacket();
    std::string  getResolverIPaddress();
    void setResolverIPaddress(std::string ip_string);
    void setResolverIPaddress(uint32_t uIP);
    void setTCP(bool flag);

    // timestamp
    void setTimeStamp(time_t timestamp);
    time_t getTimeStamp();

    // Child 
    void setChildZone(std::string childzone);
    std::string getChildZone();
    // Parent
    void setParentZone(std::string parentzone);
    std::string getParentZone();
    // crop Child Zone
    std::string cropChildZone(std::string name);
    
    // SOA
    int sendSOAquery(std::string zone);
    int checkHeaderRCODE();
    int checkAnswerSection();
    int checkAuthoritySection();
    int checkSOArecord(std::string zone);
    int checkSOAinAnswerSection(std::string zone );
    int checkSOAinAuthoritySection(std::string zone );
    int getSOAserialAnswerSection(std::string zone, uint32_t& serial );
    int getSOAserialAuthoritySection(std::string zone, uint32_t& serial );
 
    // NS  
    int sendNSquery(std::string zone);
    bool checkAAbit();
    bool checkTCbit();
    std::vector<std::string> getNSinAnswerSection(std::string zone );
    std::vector<std::string> getNSinAuthoritySection(std::string zone );
    std::vector<std::string> getNSinAuthoritySection_special();

    // IPv4
    int sendAquery(std::string nameserver);
    std::vector<std::string> getAinAnswerSection(std::string nameserver );
    std::vector<std::string> getAinAuthoritySection(std::string nameserver );
    // IPv6
    int sendAAAAquery(std::string nameserver);
    std::vector<std::string> getAAAAinAnswerSection(std::string nameserver );
    std::vector<std::string> getAAAAinAuthoritySection(std::string nameserver );
    
    // NsIPVector from parent
    std::vector<NsIP *>& getNsIPVector_fromparent();
    void setNsIPVector_fromparent(NsIP *NsIP_obj);

    // NsIPVector at parent 
    std::vector<NsIP *>& getNsIPVector_atparent( );
    void setNsIPVector_atparent( NsIP *NsIP_obj );
    
    // NsIPVector at child 
    std::vector<NsIP *>& getNsIPVector_atchild( );
    void setNsIPVector_atchild( NsIP *NsIP_obj);
    
    // free memory
    void clearNsIPVector_fromparent();
    void clearNsIPVector_atparent();
    void clearNsIPVector_atchild();
};

