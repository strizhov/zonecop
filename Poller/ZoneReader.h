
#include <stdio.h>
#include <vector>

#define LINE_LEN 4096
#define POLLER_FILE_DELIM "|\n"
#define FINDER_FILE_DELIM " \n"

class ZoneReader
{
  private:
    static ZoneReader ZoneReader_obj;
    
    int curr_;
    bool doneflag_;

    std::vector<std::string> poller_vector_;
    std::vector<std::string> finder_vector_;
    
  public:
    ZoneReader();
    virtual ~ZoneReader();
    
    static ZoneReader &getInstance();
    
    int loadPollerZones(const char *poller_filename);
    int loadFinderZones(const char *finder_filename);
    
    void addMissingZones();

    size_t getPollerVectorSize();

    // return zone vector
    std::vector<std::string> getZones(int num);

    std::vector<std::string>& getFinderVector();
    void  setFinderVector(std::string name);
    std::vector<std::string>& getPollerVector();
    void  setPollerVector(std::string name);

    bool getDoneFlag();
    void setDoneFlag(bool doneflag);
};

