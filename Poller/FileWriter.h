
#include <string>
#include <stdio.h>
#include <iostream> 
#include <fstream> 

class NsIP;

class FileWriter
{
  private:
    static FileWriter FileWriter_obj;
    
    FILE *file_;
    
    std::string filename_;

  public:
    FileWriter();
    virtual ~FileWriter();
    
    void close();


    static FileWriter &getInstance();
    
    const std::string &getFileName();
    int   setFileName(const char *filename);
    
    FILE *getFile();

    void writeEntry(  time_t timestamp, std::string childzone,std::string parentzone,std::vector<NsIP *> childVector,std::vector<NsIP *> parentVector);

    std::string getNsIPset(std::vector<NsIP *> vector);
};

