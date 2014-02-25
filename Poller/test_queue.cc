#include <stdio.h>
#include <string>
#include <time.h>

#include "Event.h"
#include "Event_queue.h"
#include "Event_mgr.h"
#include "FileWriter.h"
#include "ReverseDNS.h"

using namespace std;

int main(int argc, char *argv[])
{
  string childzone = "138.82.129.in-addr.arpa.";
  // correct parent
  string parentzone = "82.129.in-addr.arpa.";
  char outFile[] = "/tmp/test_queue2.txt";
  
  // Queue manager
  EventMgr *Mgr = new EventMgr();
  // Queue of events
  EventQueue *Queue = new EventQueue(Mgr);
  if (! (Queue->init(1000)) )
  {
    fprintf(stderr, "Could not initiate queue!\n");
  }
  
  // start QUEUE
  if (! Queue->start() )
  {
    fprintf(stderr, "Unable to start event queue.\n");
  }

  sleep(1);

  int value = FileWriter::getInstance().setFileName(outFile);
  if (value == -1)
  {
    fprintf(stderr, "Unable to set file name '%s'\n", outFile);
  }
  
  for (size_t i=0; i< 10; i++)
  {
    Event *pEvent = new Event( );
    pEvent->setTimeStamp( time(NULL) );
    pEvent->setChildZone( childzone );
    pEvent->setParentZone( parentzone );

    // enqueue Event object
    if (!Queue->enqueue(*pEvent))
    {
      fprintf(stderr, "Unable to enqueue with zone: %s\n",childzone.c_str() );
      delete pEvent;
    }
  }

  
  sleep(2);

  if (!Queue->kill())
  {
    fprintf(stderr, "Could not kill queue!\n");
  }

  delete Mgr;
  delete Queue;
  
  return 0;
}
