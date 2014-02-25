
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "ps_logger.h"

#include "Event.h"
#include "FileWriter.h"
#include "NsIP.h"
#include "Event_mgr.h"

using namespace std;

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
//  string  childzone = p_oEvent.getChildZone();
//  string  parentzone = p_oEvent.getParentZone();

//  fprintf(stderr, "Event: child: %s, parent: %s\n", childzone.c_str(), parentzone.c_str() );
//  fprintf(stderr, "At child size: %d,  at parent size: %d\n", 
//                             (int)(p_oEvent.getNsIPVector_atchild().size()),
//                             (int)(p_oEvent.getNsIPVector_atparent().size())
//			     );
  
  FileWriter::getInstance().writeEntry( p_oEvent.getTimeStamp(),
                                        p_oEvent.getChildZone(),
					p_oEvent.getParentZone(),
					p_oEvent.getNsIPVector_atchild(),
					p_oEvent.getNsIPVector_atparent()
                                        );
  
  // free NsIP at child and parent
  for (size_t i =0; i< p_oEvent.getNsIPVector_atchild().size(); i++ )
  {
    NsIP *NsIP_obj = p_oEvent.getNsIPVector_atchild()[i];
    delete NsIP_obj;
  }
  for (size_t i =0; i< p_oEvent.getNsIPVector_atparent().size(); i++ )
  {
    NsIP *NsIP_obj = p_oEvent.getNsIPVector_atparent()[i];
    delete NsIP_obj;
  }
  
  return;  
}

