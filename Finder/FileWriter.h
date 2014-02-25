
#include <string>
#include <stdio.h>
#include <iostream> 
#include <fstream> 


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

    void writeEntry(time_t timestamp, std::string zone);

};

