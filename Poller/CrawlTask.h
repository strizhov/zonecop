#include <stdio.h>
#include <stdint.h>
#include <vector>

#include "ps_task.h"

#define DEFAULT_NAMESERVER_TIMEOUT 3
#define DEFAULT_NAMESERVER_RETRY   3

class EventQueue;
class ReverseDNS;
class NsIP;

class CrawlTask : public PsTask
{
  private:
    bool        done_;
    std::vector<std::string> zonenames_;
    EventQueue  *queue_;
    
    // ReverseDNS object
    uint32_t default_nameserver_;
    
    void initDnsResolver(ReverseDNS *obj);

    std::string getZoneName(int i);
    size_t getVectorSize();

    EventQueue* getQueue();

  public:
    CrawlTask( std::vector<std::string> zonenames, EventQueue *queue);
    virtual ~CrawlTask();

    void setDefaultNameserver(uint32_t ip);
    uint32_t getDefaultNameserver();
    
    int checkZone(ReverseDNS *obj, std::string tmp_zone);
    int findParentZone(ReverseDNS *obj, std::string childzone);
    int findNsIPset(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector );
    int findNsIPsetAuth(ReverseDNS *obj, std::string zone, std::string ip, std::vector<NsIP *>& NsIPVector_to );
    int getIPv4address(ReverseDNS *obj, std::string nameserver, std::vector<std::string>& IpVector);

    int findParentNSset(ReverseDNS *obj,  std::string zone,  std::vector<NsIP *>& NsIPVector_from, std::vector<NsIP *>& NsIPVector_to );
    int findChildNSset(ReverseDNS *obj,  std::string zone, std::string ip, std::vector<NsIP *>& NsIPVector_to );

    int findSOAserial(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector);
    int checkSOAserial(ReverseDNS *obj, std::string tmp_zone, NsIP *NsIP_obj );
    int compareSOAserial( std::vector<NsIP *>& NsIPVector, std::string& ip );

    int getCorrectNSset(ReverseDNS *obj, std::string zone, std::vector<NsIP *>& NsIPVector);

    virtual bool execute();
    virtual bool done();
    virtual bool process();
};
