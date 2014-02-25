#include <stdio.h>
#include <vector>


class ZoneName 
{
  private:
    std::string  name_;
    std::vector<std::string> names_;
    int          index_;
    bool         doneflag_;

  public:
    ZoneName();
    virtual ~ZoneName();

    // index
    int getIndex();
    void setIndex(int index);
    
    // name
    std::string getName();
    void setName(std::string name);
    
    // generate zone method
    int genZone();
    
    // done flag
    void setDoneFlag(bool doneflag);
    bool getDoneFlag();
    
    // return zone vector
    std::vector<std::string> getZone(int num);

};
