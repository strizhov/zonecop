#include <stdio.h>

#include "NsIP.h"

/*--------------------------------------------------------------------------------------
 * Purpose: NsIP constructor
 * Input: none 
 * Output: all flags are set to false
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
NsIP::NsIP()
  : online_(false),
    nameserver_flag_(false),
    ip_flag_(false),
    serial_flag_(false),
    aa_bit_flag_(false)
{

}

/*--------------------------------------------------------------------------------------
 * Purpose: NsIP destructor
 * Input:  
 * Output: 
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
NsIP::~NsIP()
{

}
    
/*--------------------------------------------------------------------------------------
 * Purpose: set online flag
 * Input:  bool
 * Output: none
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setOnlineFlag(bool flag)
{
  online_ = flag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set online flag
 * Input:  bool
 * Output: none
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
bool NsIP::getOnlineFlag()
{
  return online_;
}


/*--------------------------------------------------------------------------------------
 * Purpose: set nameserver flag
 * Input:  bool
 * Output: none
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setNameServerFlag(bool flag)
{
  nameserver_flag_ = flag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get nameserver flag
 * Input:  none
 * Output: bool
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
bool NsIP::getNameServerFlag()
{
  return nameserver_flag_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set nameserver
 * Input:  string
 * Output: 
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setNameServer(std::string nameserver)
{
  nameserver_ = nameserver;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get nameserver
 * Input:  
 * Output: string
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
std::string NsIP::getNameServer()
{
  return nameserver_;
}
   
/*--------------------------------------------------------------------------------------
 * Purpose: set ip flag
 * Input: bool  
 * Output: none
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setIPFlag(bool flag)
{
  ip_flag_ = flag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get ip flag
 * Input: none  
 * Output: bool
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
bool NsIP::getIPFlag()
{
  return ip_flag_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set ip
 * Input: ip address string 
 * Output: 
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setIP(std::string ip)
{
  ip_ = ip;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get ip
 * Input:  
 * Output:  string ip address
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
std::string NsIP::getIP()
{
  return ip_;
}
    
/*--------------------------------------------------------------------------------------
 * Purpose: set serial number flag
 * Input:  bool
 * Output:  none
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setSerialFlag(bool flag)
{
  serial_flag_ = flag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get serial number flag
 * Input:  none
 * Output:  bool
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
bool NsIP::getSerialFlag()
{
  return serial_flag_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set serial number
 * Input:  
 * Output:  
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setSerial(uint32_t serial)
{
  serial_ = serial;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get serial number
 * Input:  
 * Output:  
 * Mikhail Strizhov @ Aug 8, 2011
 * -------------------------------------------------------------------------------------*/
uint32_t NsIP::getSerial()
{
  return serial_;
}

/*--------------------------------------------------------------------------------------
 * Purpose: set aa bit flag
 * Input:  bool
 * Output:  none
 * Mikhail Strizhov @ Aug 17, 2011
 * -------------------------------------------------------------------------------------*/
void NsIP::setAAbitFlag(bool flag)
{
  aa_bit_flag_ = flag;
}

/*--------------------------------------------------------------------------------------
 * Purpose: get aa bit flag
 * Input:  none
 * Output:  bool
 * Mikhail Strizhov @ Aug 17, 2011
 * -------------------------------------------------------------------------------------*/
bool NsIP::getAAbitFlag()
{
  return aa_bit_flag_;
}


