#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <vector>

#include "ZoneName.h"

using namespace std;

/*--------------------------------------------------------------------------------------
 * Purpose: constructor of ZoneName class 
 * Input: none
 * Output: set index to -1, set done flag to false
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
ZoneName::ZoneName()
 : index_(-1),
   doneflag_(false) 
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: destructor
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
ZoneName::~ZoneName()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: returns zone name
 * Input: 
 * Output:  string Name 
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
std::string ZoneName::getName()
{
  return name_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set up zone name
 * Input:  string name
 * Output:  none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneName::setName(std::string name)
{
  name_ = name;
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: returns current index
 * Input:  none
 * Output: int index
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
int ZoneName::getIndex()
{
  return index_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set up index value
 * Input:   integer string
 * Output:  none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneName::setIndex(int index)
{
  index_ = index;
}

/*--------------------------------------------------------------------------------------
 * Purpose: returns done flag
 * Input:  none
 * Output:  flag, false or true
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
bool ZoneName::getDoneFlag()
{
  return doneflag_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set up done flag
 * Input:  bool, flase or true
 * Output: none
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneName::setDoneFlag(bool doneflag)
{
  doneflag_ = doneflag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: first, generate zone, then return it
 * Input:  none 
 * Output: returns the name for zone
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<std::string> ZoneName::getZone(int num)
{
  // clean all elements in vector
  names_.clear();

  // fill up vector
  for (int i=0; i<num; i++)
  {
    // generate zone and save it 
    genZone();
    // retrive zone and put it to vector
    names_.push_back( getName()  );
  }
  // return current zone
  return names_; 
}

/*--------------------------------------------------------------------------------------
 * Purpose: generate reverse dns zone
 * Input:  none 
 * Output: returns the name for zone
 * Mikhail Strizhov @ July 18, 2011
 * -------------------------------------------------------------------------------------*/
void ZoneName::genZone()
{
  string dot = ".";
  string arpa = ".in-addr.arpa.";
  ostringstream oss;
  
  // check if start cycle
  if (getIndex() == -1)
  {
    // 0.in-addr.arpa
    oss.str("");
    oss << "0" << arpa;
      
//    fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
    
    setName(oss.str());
    setIndex(1);
    return;
  }

  // zone represented as "a.b.c.in-addr.arpa."
  string a, b, c;
  int a_val = -1, b_val = -1, c_val = -1;

  string tmpname = getName();
  // lookup for .in-addr.arpa.  and erase it
  size_t arpa_found = tmpname.find(arpa);
  if (arpa_found!=string::npos)
  {
    tmpname.erase (arpa_found+1, string::npos);
  }
 
  // get values of a,b,c
  size_t a_found = tmpname.find(dot);
  if (a_found != string::npos)
  {
    a = tmpname.substr(0, a_found);
    a_val = atoi(a.c_str());
    
    tmpname.erase(0, a_found+1);
    
    size_t b_found = tmpname.find(dot);
    if (b_found != string::npos)
    {
      b = tmpname.substr(0, b_found);
      b_val = atoi(b.c_str());
      
      tmpname.erase(0, b_found+1);
    
      size_t c_found = tmpname.find(dot);
      if (c_found != string::npos)
      {
        c = tmpname.substr(0, c_found);
        c_val = atoi(c.c_str());
	
	tmpname.erase(0, c_found+1);
      }
    }
  }
  
//  fprintf(stderr, "a: %d, b: %d, c: %d\n", a_val, b_val, c_val);

  // if name is a.in-addr.arpa
  if (getIndex() == 1)
  {
    if (a_val >= 0 && a_val <= 255)
    {
      int new_a_val = a_val + 1;
      if (new_a_val > 255)
      {
        oss.str("");
        oss << "0" << dot << "0" << arpa;
      
//        fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
        setName(oss.str());
        setIndex(2);
      }
      else
      {
        oss.str("");
        oss << new_a_val << arpa;

//        fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
        setName(oss.str());
      }
    }
    else
    {
      fprintf(stderr,"Format of X.in-addr.arpa INCORRECT!\n");
    }
  }

  // if name is a.b.in-addr.arpa
  if (getIndex() == 2)
  {
    if ( (a_val >= 0) && ( a_val <= 255)  )
    {
      if ( (b_val >= 0) && (b_val <= 255) )
      {
        int new_b_val = b_val + 1;

	// in case if b is out of 0-255 range
	if (new_b_val > 255)
	{
	  int new_a_val = a_val + 1; 
	  // in case, we need to switch to x.x.x.in-addr.arpa
	  if (new_a_val > 255)
	  {
            oss.str("");
            oss << "0" << dot << "0" << dot << "0" << arpa;

//            fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
            setName(oss.str());
            setIndex(3);
	  
	  }
	  else // in case the we need to switch to a+1.0.in-addr.arpa
	  {
            oss.str("");
            oss << new_a_val << dot << "0" << arpa;

//            fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
            setName(oss.str());
	  }
	}
	else
	{
	    // keep same a value, but increase b
            oss.str("");
            oss << a_val << dot << new_b_val << arpa;
//            fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
            setName(oss.str());
	  
	}
      }
    }
  }  


  // a.b.c.in-addr.arpa
  if (getIndex() == 3)
  {
    if ( (a_val >= 0) && ( a_val <= 255)  )
    {
      if ( (b_val >= 0) && (b_val <= 255) )
      {
        if ( (c_val >= 0) && (c_val <= 255)  )
	{
          int new_c_val = c_val + 1;
	  if (new_c_val > 255)
	  {
            int new_b_val = b_val + 1;
	    if (new_b_val > 255)
	    {
	      int new_a_val = a_val + 1;
	      if (new_a_val > 255)
	      {
//	        fprintf(stderr, "DONE!!!!\n");
		setDoneFlag(true);
		return;
//		setIndex(-1);
	      }
	      else
	      {
                oss.str("");
                oss << new_a_val << dot << "0" << dot << "0" << arpa;
//                fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
                setName(oss.str());
	      
	      }
	    
	    }
	    else
	    { // a.new_b.0.in-addr.arpa
              oss.str("");
              oss << a_val << dot << new_b_val << dot << "0" << arpa;
//              fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
              setName(oss.str());
	    
	    }

	  
	  }
          else
	  {
	    // keep same a value, but increase b
            oss.str("");
            oss << a_val << dot << b_val << dot << new_c_val << arpa;
//            fprintf(stderr, "New zone: %s\n", oss.str().c_str()  );
            setName(oss.str());
	  
	  }



	
	} // c_val
      } // b_val
    } // a_val 
  }

 
}


