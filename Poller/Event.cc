#include <stdio.h>

#include "Event.h"

/*--------------------------------------------------------------------------------------
 * Purpose: constructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
Event::Event()
{

}


/*--------------------------------------------------------------------------------------
 * Purpose: destructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
Event::~Event()
{

}

/*--------------------------------------------------------------------------------------
 * Purpose: set the time stamp in event   
 * Input:  time_t timestamp
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
void Event::setTimeStamp(time_t timestamp)
{
  timestamp_ = timestamp;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get the time stamp in event   
 * Input:  time_t timestamp
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
time_t Event::getTimeStamp()
{
  return timestamp_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set child zone    
 * Input:  string
 * Output: none
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
void Event::setChildZone(std::string zone)
{
  childzone_ = zone;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get child zone    
 * Input:  none
 * Output: string
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
std::string Event::getChildZone()
{
  return childzone_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set parent zone    
 * Input:  string
 * Output: none
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
void Event::setParentZone(std::string zone)
{
  parentzone_ = zone;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get parent zone    
 * Input:  none
 * Output: string
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
std::string Event::getParentZone()
{
  return parentzone_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get vector at parent    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<NsIP *>& Event::getNsIPVector_atparent( )
{
  return NsIPVector_atparent_; 
}

/*--------------------------------------------------------------------------------------
 * Purpose: set vector at parent    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
void Event::setNsIPVector_atparent( std::vector<NsIP *> vector )
{
  NsIPVector_atparent_ = vector;
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: get vector at child    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
std::vector<NsIP *>& Event::getNsIPVector_atchild( )
{
  return NsIPVector_atchild_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set vector at child    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 12, 2011
 * -------------------------------------------------------------------------------------*/
void Event::setNsIPVector_atchild( std::vector<NsIP *> vector   )
{
  NsIPVector_atchild_ = vector;
}
/*--------------------------------------------------------------------------------------
 * Purpose: execute method    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
bool Event::execute()
{
  return false;
}

