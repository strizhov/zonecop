
#include "ps_queue.h"

class EventMgr;

class EventQueue : public PsQueue
{
  // Member Variables
  private:
    EventMgr *Mgr_;

    EventMgr* getEventMgr();

  // Methods
  public:
    EventQueue(EventMgr *Mgr);
    virtual ~EventQueue();

    virtual bool run();

};

