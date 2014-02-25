
#include <stdio.h>

#include "ps_logger.h"

#include "Event.h"
#include "Event_mgr.h"
#include "FileWriter.h"

/*--------------------------------------------------------------------------------------
 * Purpose: constructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
EventMgr::EventMgr()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: destructor   
 * Input:  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
EventMgr::~EventMgr()
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: handle events from a queue   
 * Input: event object  
 * Output: 
 * Mikhail Strizhov @ July 29, 2011
 * -------------------------------------------------------------------------------------*/
void EventMgr::handleEvent(Event &p_oEvent)
{
  time_t event_timestamp = p_oEvent.getTimeStamp();
  std::string event_zone = p_oEvent.getChildZone();

  FileWriter::getInstance().writeEntry(event_timestamp, event_zone);
  
  return;  
}

