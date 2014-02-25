
#include <string>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "ps_logger.h"

#include "FileWriter.h"


//#define DEBUG

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
#ifdef _PS_DEBUG
      fprintf(stderr, "%s [%d]: Unable to open file '%s': %s\n",
              __FILE__,
              __LINE__,
              filename_.c_str(),
              strerror(errno));
#endif
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
void FileWriter::writeEntry(time_t timestamp, std::string zone)
{
  ostringstream oSS;
  oSS << (int) timestamp 
      << " "
      << zone;

#ifdef DEBUG      
  ps_elog(PSL_DEBUG, "%s\n", oSS.str().c_str());    
#endif  
  
  fprintf(getFile() ,"%s\n", oSS.str().c_str());

}



