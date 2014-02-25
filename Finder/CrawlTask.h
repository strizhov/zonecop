#include <stdio.h>

#include "ps_task.h"

#define DEFAULT_NAMESERVER_RETRY 3
#define DEFAULT_NAMESERVER_TIMEOUT 3

class EventQueue;
class ReverseDNS;

class CrawlTask : public PsTask
{
  private:
    bool        done_;
    std::vector<std::string> zonenames_;
    EventQueue  *queue_;
    
    std::string getZoneName(int i);
    size_t getVectorSize();

    EventQueue* getQueue();

  public:
    CrawlTask( std::vector<std::string> zonenames, EventQueue *queue);
    virtual ~CrawlTask();

    int checkZone(ReverseDNS *obj, std::string& zone);

    
    void initDnsResolver(ReverseDNS *obj);

    virtual bool execute();
    virtual bool done();
    virtual bool process();
};
