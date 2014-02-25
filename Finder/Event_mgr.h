

class Event;

class EventMgr
{
  // Member Variables
  private:

  // Methods
  public:
    EventMgr();
    virtual ~EventMgr();

    void handleEvent(Event &p_oEvent);

};

