#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "ZoneName.h"

#define NUM_ZONES 20 


using namespace std;

int main(int argc, char *argv[])
{
  string arpa = ".in-addr.arpa.";
  string dot = ".";

  ZoneName obj;
  
//  for (int i =0; i< NUM_ZONES; i++)
  while (obj.getDoneFlag() != true) 
  {
    if (obj.genZone() < 0)
    {
      fprintf(stderr, "FAILURE! Could not generate zone!\n");
      return 0;
    }
    else
    {
      string zone = obj.getName();
      
      size_t arpa_found = zone.find(arpa);
      if (arpa_found!=string::npos)
      {
        int index = obj.getIndex();
	if ((index >= 1) && (index <= 3) )
	{
	  // a.in-addr.arpa.
	  if (index == 1)
	  {
	    string a;
	    int a_val = -1;
            size_t a_found = zone.find(dot);
            if (a_found != string::npos)
            {
              a = zone.substr(0, a_found);
              a_val = atoi(a.c_str());
              if (a_val < 0 || a_val > 255)
	      {
	        fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", a_val);
	      }
	    }
	    else
	    {
	      fprintf(stderr, "FAILURE: %s is NOT found in zone: %s ", dot.c_str(), zone.c_str() );
	    }
	  }

	  // a.b.in-addr.arpa.
	  if (index == 2)
	  {
            string a,b;
	    int a_val, b_val;

            size_t a_found = zone.find(dot);
            if (a_found != string::npos)
            {
              a = zone.substr(0, a_found);
              a_val = atoi(a.c_str());
	      if (a_val < 0 || a_val >255)
	      {
	        fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", a_val);
	      }
	      else
	      {
                zone.erase(0, a_found+1);
    
                size_t b_found = zone.find(dot);
                if (b_found != string::npos)
                {
                  b = zone.substr(0, b_found);
                  b_val = atoi(b.c_str());
	          if (b_val < 0 || b_val >255)
	          {
	            fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", b_val);
	          }

                }
	        else
	        {
	          fprintf(stderr, "FAILURE: %s is NOT found in zone: %s ", dot.c_str(), zone.c_str() );
	        }
	      }
	    }
	    else
	    {
	      fprintf(stderr, "FAILURE: %s is NOT found in zone: %s ", dot.c_str(), zone.c_str() );
	    }
	  }
      
	  
	  
          // a.b.c.in-addr.arpa.
	  if (index == 3)
	  {
            string a,b,c;
	    int a_val, b_val, c_val;

            size_t a_found = zone.find(dot);
            if (a_found != string::npos)
            {
              a = zone.substr(0, a_found);
              a_val = atoi(a.c_str());
	      if (a_val < 0 || a_val >255)
	      {
	        fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", a_val);
	      }
	      else
	      {
                zone.erase(0, a_found+1);
    
                size_t b_found = zone.find(dot);
                if (b_found != string::npos)
                {
                  b = zone.substr(0, b_found);
                  b_val = atoi(b.c_str());
	          if (b_val < 0 || b_val >255)
	          {
	            fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", b_val);
	          }
		  else
		  {
                    zone.erase(0, b_found+1);
    
                    size_t c_found = zone.find(dot);
                    if (c_found != string::npos)
                    {
                      c = zone.substr(0, c_found);
                      c_val = atoi(c.c_str());
	              if (c_val < 0 || c_val >255)
	              {
	                fprintf(stderr, "FAILURE: %d value is out of range [0;255]\n", b_val);
	              }
		  
		    }
		  }

                }
	        else
	        {
	          fprintf(stderr, "FAILURE: %s is NOT found in zone: %s ", dot.c_str(), zone.c_str() );
	        }
	      }
	    }
	    else
	    {
	      fprintf(stderr, "FAILURE: %s is NOT found in zone: %s ", dot.c_str(), zone.c_str() );
	    }

	  }
	}
	else
	{
	  fprintf(stderr,"FAILURE: %d index is not possible!\n ", index);
	}

      }
      else
      {
        fprintf(stderr, "FAILURE! %s is NOT found in zone: %s\n", arpa.c_str(), zone.c_str());
        return 0;
      }
    }
  }
  return 0;
}
