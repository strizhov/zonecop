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
 * Purpose: execute method    
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
bool Event::execute()
{
  return false;
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
