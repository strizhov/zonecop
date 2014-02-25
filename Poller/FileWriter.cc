
#include <string>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <errno.h>

#include "ps_logger.h"

#include "FileWriter.h"

#include "NsIP.h"

using namespace std;

FileWriter FileWriter::FileWriter_obj;

/*--------------------------------------------------------------------------------------
 * Purpose: constructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
FileWriter::FileWriter()
  :  file_(stdout)
{

}

/*--------------------------------------------------------------------------------------
 * Purpose: destructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
FileWriter::~FileWriter()
{
  close();

}

/*--------------------------------------------------------------------------------------
 * Purpose: returns file name   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
const std::string &FileWriter::getFileName()
{
  return filename_;
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: set file name   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
int FileWriter::setFileName(const char *filename)
{
  int retval = -1;

  // check filename
  if (NULL != filename && '\0' != filename[0])
  {
    // check FILE pointer
    if (NULL != file_ && stdout != file_)
    {
      fclose(file_);
      file_ = NULL;
    }
    // assign filename
    filename_ = filename;

    file_ = fopen(filename_.c_str(), "w");
    if (NULL == file_)
    {
      fprintf(stderr, "%s [%d]: Unable to open file '%s': %s\n",
              __FILE__,
              __LINE__,
              filename_.c_str(),
              strerror(errno));
      file_ = stdout;
      filename_ = "";
    }
    else
    {
      retval = 1;
    }
  }

  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: returns file name   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
inline FILE *FileWriter::getFile()
{
  return file_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: returns FileWriter object   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
FileWriter &FileWriter::getInstance()
{
  return FileWriter_obj;
}

/*--------------------------------------------------------------------------------------
 * Purpose: close File   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
void FileWriter::close()
{
  if (NULL != file_ && stdout != file_)
  {
    fclose(file_);
    file_ = NULL;
  }
}

/*--------------------------------------------------------------------------------------
 * Purpose: write entries to file   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
void FileWriter::writeEntry(  time_t timestamp, 
                              std::string childzone, 
			      std::string parentzone,
			      std::vector<NsIP *> childVector,
			      std::vector<NsIP *> parentVector
			      )
{
  ostringstream oSS;
  oSS.str("");

  string childset = getNsIPset(childVector);
  string parentset = getNsIPset(parentVector);

  oSS << (int) timestamp 
      << "|"
      << childzone
      << "|"
      << parentzone
      << "|"
      << childset
      << "|"
      << parentset;

  ps_elog(PSL_DEBUG, "%s\n", oSS.str().c_str());    
  
  fprintf(getFile() ,"%s\n", oSS.str().c_str());

}

/*--------------------------------------------------------------------------------------
 * Purpose: create correct format for NsIP vector   
 * Input:  vector
 * Output: string
 * Mikhail Strizhov @ Aug 22, 2011
 * -------------------------------------------------------------------------------------*/
std::string FileWriter::getNsIPset(std::vector<NsIP *> vector)
{
  string set;
  for (size_t i=0; i< vector.size(); i++)
  {
    NsIP *NsIP_obj = vector[i];
    ostringstream oSS_tmp;
    oSS_tmp.str("");
    oSS_tmp << "{" ;
    oSS_tmp << (int)(NsIP_obj->getOnlineFlag());
    oSS_tmp << " " ;
    if (NsIP_obj->getNameServerFlag() == true)
    {
      oSS_tmp << NsIP_obj->getNameServer();
    }
    else
    {
      oSS_tmp << "NULL"; 
    }
    oSS_tmp << " " ;
 
    if (NsIP_obj->getIPFlag() == true)
    {
      oSS_tmp << NsIP_obj->getIP();
    }
    else
    {
      oSS_tmp << "NULL"; 
    }
    
    oSS_tmp << " " ;

    if (NsIP_obj->getSerialFlag() == true)
    {
      oSS_tmp << (int)NsIP_obj->getSerial();
    }
    else
    {
      oSS_tmp << "NULL"; 
    }
    oSS_tmp << " "; 
    oSS_tmp << (int)(NsIP_obj->getAAbitFlag());
    oSS_tmp  << "}";
    set += oSS_tmp.str();
  }
  return set;
}

