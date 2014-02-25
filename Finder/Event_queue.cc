#include <stdio.h>

#include "ps_mutex_hdlr.h"
#include "ps_task.h"
#include "ps_logger.h"

#include "Event_queue.h"
#include "Event_mgr.h"
#include "Event.h"

/*--------------------------------------------------------------------------------------
 * Purpose: event queue constructor 
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
EventQueue::EventQueue(EventMgr *Mgr)
  : Mgr_(Mgr)
{
}

/*--------------------------------------------------------------------------------------
 * Purpose: event queue destructor 
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
EventQueue::~EventQueue()
{
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: get event mgr   
 * Input: 
 * Output: EventMgr
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
EventMgr* EventQueue::getEventMgr()
{
  return Mgr_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: dequeue stuff   
 * Input: 
 * Output: 
 * Mikhail Strizhov @ July 26, 2011
 * -------------------------------------------------------------------------------------*/
bool EventQueue::run()
{
  bool bRet = false;

  while (getRun())
  {
    bRet = true;
    
    PsTask *pTask = dequeue();
    Event *pEvent = static_cast<Event *>(pTask);
    if (NULL == pTask)
    //
    // CRITICAL SECTION BEGIN
    //
    {
      PsMutexHandler oMH(getMutex());
      getMutex().wait(5000);
    }
    //
    // CRITICAL SECTION END
    //
    else if (NULL == pEvent)
    {
      ps_elog(PSL_ERROR, "Unable to cast task for some reason.\n");
    }
    else 
    {
      getEventMgr()->handleEvent(*pEvent);
    }

    if (NULL != pTask)
    {
      delete pTask;
    }
  }

  return bRet;
}

