
#include <string>
#include <time.h>
#include <vector>

#include "ps_task.h"

class NsIP;

class Event : public PsTask
{
  // Member Variables
  private:
    time_t      timestamp_;
    std::string childzone_;
    std::string parentzone_;
    
    std::vector<NsIP *> NsIPVector_atparent_;
    std::vector<NsIP *> NsIPVector_atchild_;

  // Methods
  public:
    Event();
    virtual ~Event();

    virtual void setTimeStamp(time_t timestamp);
    virtual time_t getTimeStamp();
    
    virtual void setChildZone(std::string zone);
    virtual std::string getChildZone();

    virtual void setParentZone(std::string zone);
    virtual std::string getParentZone();
    
    // NsIPVector at parent 
    virtual std::vector<NsIP *>& getNsIPVector_atparent( );
    virtual void setNsIPVector_atparent( std::vector<NsIP *> vector );
    
    // NsIPVector at child 
    virtual std::vector<NsIP *>& getNsIPVector_atchild( );
    virtual void setNsIPVector_atchild( std::vector<NsIP *> vector   );
    
    virtual bool execute();
};

