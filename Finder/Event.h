
#include <string>
#include <time.h>

#include "ps_task.h"

class Event : public PsTask
{
  // Member Variables
  private:
    std::string childzone_;
    time_t      timestamp_;

  // Methods
  public:
    Event();
    virtual ~Event();

    virtual void setTimeStamp(time_t timestamp);
    virtual time_t      getTimeStamp();

    virtual void setChildZone(std::string zone);
    virtual std::string getChildZone();

    virtual bool execute();

};

