#include <stdio.h>
#include <string>
#include <stdint.h>

class NsIP
{
  private:
    bool        online_;

    bool        nameserver_flag_;
    std::string nameserver_;

    bool        ip_flag_;
    std::string ip_;

    bool        serial_flag_;
    uint32_t    serial_;

    bool        aa_bit_flag_;

  public:
    NsIP();
    virtual ~NsIP();
    
    void setOnlineFlag(bool flag);
    bool getOnlineFlag();

    void setNameServerFlag(bool flag);
    bool getNameServerFlag();

    void setNameServer(std::string nameserver);
    std::string getNameServer();
   
    void setIPFlag(bool flag);
    bool getIPFlag();

    void setIP(std::string ip);
    std::string getIP();

    void setSerialFlag(bool flag);
    bool getSerialFlag();
    
    void setSerial(uint32_t serial);
    uint32_t getSerial();

    void setAAbitFlag(bool flag);
    bool getAAbitFlag();


};
