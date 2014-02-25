
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <set>

#include "ps_logger.h"
#include "ZoneReader.h"


using namespace std;

ZoneReader ZoneReader::ZoneReader_obj;

/*--------------------------------------------------------------------------------------
 * Purpose: constructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
ZoneReader::ZoneReader()
 : curr_(0),
   doneflag_(false) 
{

}

/*--------------------------------------------------------------------------------------
 * Purpose: destructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
ZoneReader::~ZoneReader()
{

}

/*--------------------------------------------------------------------------------------
 * Purpose: returns ZoneReader object   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
ZoneReader &ZoneReader::getInstance()
{
  return ZoneReader_obj;
}

/*--------------------------------------------------------------------------------------
 * Purpose: load pollers zones in vector   
 * Input:  file name of file
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
int ZoneReader::loadPollerZones(const char *poller_filename)
{
  int retval = -1;
  FILE *File = NULL;

  // Make sure the user specified a file.
  if (NULL == poller_filename)
  {
    ps_elog(PSL_CRITICAL, "No file with poller zones specified\n");
  }
  // Try to open it for reading.
  else if (NULL == (File = fopen(poller_filename, "r")))
  {
    ps_elog(PSL_CRITICAL, "Unable to open file with poller's zones\n");
  }
  else
  {
    char line[LINE_LEN + 1];
    memset(line, 0, LINE_LEN + 1);

    // Get the first line of the file
    while ( fgets(line, LINE_LEN, File) != NULL )
    {
      char *token = NULL;
      // first token is timestamp
      token = strtok( line, POLLER_FILE_DELIM );
      if (token == NULL)
      {
        ps_elog(PSL_CRITICAL, "Failed to read timestamp from pollers zone file\n");
      }
      else
      {
        // second - zone name
        token = strtok( NULL, POLLER_FILE_DELIM );
        if (token == NULL)
        {
          ps_elog(PSL_CRITICAL, "Failed to read zonename from pollers zone file\n");
        }
	else
	{
	  char tmp[LINE_LEN+1];
	  memset(tmp, 0, LINE_LEN+1);
	  memcpy(tmp, token, strlen(token));
	  string tmp_string(tmp);
          // fill up poller_vector
          setPollerVector(tmp_string);
          // update retval
          retval = 1;
	}
      }	
      // clean line.
      memset( line, 0, LINE_LEN + 1);
    }
    // Close the file.
    fclose(File);
  }

  return retval;
}


/*--------------------------------------------------------------------------------------
 * Purpose: load finders zones in vector   
 * Input:  file name of file
 * Output: -1 failure, 1 success
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
int ZoneReader::loadFinderZones(const char *finder_filename)
{
  int retval = -1;
  FILE *File = NULL;

  // Make sure the user specified a file.
  if (NULL == finder_filename)
  {
    ps_elog(PSL_CRITICAL, "No file with finder zones specified\n");
  }
  // Try to open it for reading.
  else if (NULL == (File = fopen(finder_filename, "r")))
  {
    ps_elog(PSL_CRITICAL, "Unable to open file with finder's zones\n");
  }
  else
  {
    char line[LINE_LEN + 1];
    memset(line, 0, LINE_LEN + 1);

    // Get the first line of the file
    while ( fgets(line, LINE_LEN, File) != NULL )
    {
      char *token = NULL;
      // first token is timestamp
      token = strtok( line, FINDER_FILE_DELIM );
      if (token == NULL)
      {
        ps_elog(PSL_CRITICAL, "Failed to read timestamp from finders zone file\n");
      }
      else
      {
        // second - zone name
	token = NULL;
        token = strtok( NULL, FINDER_FILE_DELIM );
        if (token == NULL)
        {
          ps_elog(PSL_CRITICAL, "Failed to read zonename from finders zone file\n");
        }
	else
	{
          // fill up finder_vector
	  char tmp[LINE_LEN+1];
	  memset(tmp, 0, LINE_LEN+1);
	  memcpy(tmp, token, strlen(token));
	  string tmp_string(tmp);
          setFinderVector(tmp_string);
          // update retval
          retval = 1;
	}
      }	
      // clean line.
      memset( line, 0, LINE_LEN + 1);
    }

    // Close the file.
    fclose(File);
  }
  return retval;
}

/*--------------------------------------------------------------------------------------
 * Purpose: find difference in two string vectors   
 * Input:  poller and finder zone vectors
 * Output:  none
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneReader::addMissingZones( )
{
  // sort both  vectors
  sort(getPollerVector().begin(), getPollerVector().end());
  sort(getFinderVector().begin(), getFinderVector().end());

  ps_elog(PSL_CRITICAL, "Size of pollers vector is %d, size of finders vector is %d\n", 
                                                getPollerVector().size(),
						getFinderVector().size()
						);
  
  // create set of both vectors and find difference
  std::set<std::string> poller_set( getPollerVector().begin(), getPollerVector().end() );
  std::set<std::string> finder_set( getFinderVector().begin(), getFinderVector().end() );
  std::vector<std::string> result;

  std::set_difference( 
		       finder_set.begin(), 
		       finder_set.end(),
		       poller_set.begin(), 
                       poller_set.end(),
                       std::back_inserter( result )
		       );

  ps_elog(PSL_CRITICAL, "Number of zones would be added: %d\n", result.size() );

  // add difference to poller_vector
  getPollerVector().insert( getPollerVector().end(), result.begin(), result.end() );
  
}

/*--------------------------------------------------------------------------------------
 * Purpose: return size of pollers vector    
 * Input:  
 * Output:  size_t
 * Mikhail Strizhov @ Aug 4, 2011
 * -------------------------------------------------------------------------------------*/
size_t ZoneReader::getPollerVectorSize()
{
  return poller_vector_.size();
}

/*--------------------------------------------------------------------------------------
 * Purpose: return "bag" of zones
 * Input:  number of zones per worker 
 * Output: vector of zones
 * Mikhail Strizhov @ Aug 5, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ZoneReader::getZones(int num)
{
  std::vector<std::string> retvector;

  int div_value = (int) (getPollerVector().size() / num);
  int mod_value = (int) (getPollerVector().size() % num);

/*
  ps_elog(PSL_DEBUG, "Size of vector %d, Per worker %d, div %d, mod %d\n", 
                                              (int)getPollerVectorSize(),
					      num,
					      div_value,
					      mod_value
                                              ); 
*/

  // first, fill up even number zones from pollers vector
  if ( curr_ < div_value * num  )
  {
    for (int i = curr_; i < curr_ + num; i++ )
    {
      retvector.push_back( getPollerVector()[i]  );    
    }
    curr_ = curr_ + num;
  }
  else // add rest
  {
    for (int i = curr_; i< curr_ + mod_value; i++)
    {
      retvector.push_back( getPollerVector()[i] );
    }
    // set done!!
    setDoneFlag(true);
  }
  
  // return current zone
  return retvector; 
}

/*--------------------------------------------------------------------------------------
 * Purpose: returns done flag
 * Input:  none
 * Output:  flag, false or true
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
bool ZoneReader::getDoneFlag()
{
  return doneflag_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set up done flag
 * Input:  bool, flase or true
 * Output: none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneReader::setDoneFlag(bool doneflag)
{
  doneflag_ = doneflag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: 
 * Input:   
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string>& ZoneReader::getFinderVector()
{
  return finder_vector_;
}

void ZoneReader::setFinderVector(std::string name)
{
  finder_vector_.push_back(name);
}

/*--------------------------------------------------------------------------------------
 * Purpose: 
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string>& ZoneReader::getPollerVector()
{
  return poller_vector_;
}

void  ZoneReader::setPollerVector(std::string name)
{
  poller_vector_.push_back(name);
}
