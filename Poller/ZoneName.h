#include <stdio.h>
#include <vector>


class ZoneName 
{
  private:
    std::string  name_;
    std::vector<std::string> names_;
    int          index_;
    bool         doneflag_;

    // name
    std::string getName();
    void setName(std::string name);

    // index
    int getIndex();
    void setIndex(int index);
    
    // done flag
    void setDoneFlag(bool doneflag);
    
    // generate zone method
    void genZone();

  public:
    ZoneName();
    virtual ~ZoneName();

    bool getDoneFlag();
    // return zone vector
    std::vector<std::string> getZone(int num);

};
