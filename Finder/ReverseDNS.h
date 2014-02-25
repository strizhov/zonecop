#include <stdio.h>

#include <vantages/dns_resolver.h>
#include <vantages/dns_packet.h>

class ReverseDNS  
{
  private:
    DnsResolver oRes;
    DnsPacket   oResp;
    
    time_t      timestamp_;
    
  public:
    ReverseDNS( );
    virtual ~ReverseDNS();
    
    DnsResolver& getDnsResolver();
    DnsPacket&   getDnsPacket();
    
    std::string  getResolverIPaddress();
    void setResolverIPaddress(std::string ip_string);
    void setResolverIPaddress(uint32_t uIP);
    
    // timestamp
    void setTimeStamp(time_t timestamp);
    time_t getTimeStamp();
    
    // SOA
    int sendSOAquery(std::string& zone);
    int checkHeaderRCODE();
    int checkAnswerSection();
    int checkAuthoritySection();
    int checkSOAinAnswersSection(std::string& zone );
    int checkSOAinAuthoritySection(std::string& zone );

};

