#pragma once

#include "DateTime.h"

namespace mvc{

  using namespace std;

  class TickPrice{
  private:
    DateTime m_time;
    double m_bid;
    double m_ask;

  public:

    // tickStr = time, ask, bid
    TickPrice(const char * tickStr) : m_time{ tickStr }{
      const char * comma1 = strchr(tickStr, ',');
      if (!comma1){
        string errmsg = "Cannot create tick data from tick string: ";
        errmsg = errmsg + tickStr;
        throw std::runtime_error(errmsg);
      }

      int num = sscanf_s(comma1 + 1, "%lf,%lf", &m_ask, &m_bid);

      if (num != 2){
        string errmsg = "Cannot create tick data from tick string: ";
        errmsg = errmsg + tickStr;
        throw std::runtime_error(errmsg);
      }
    }

    TickPrice(const char* timeStr, double ask, double bid) :m_time{ timeStr }{
      m_bid = bid;
      m_ask = ask;
    }

    TickPrice(const TickPrice& tp) : m_time{ tp.m_time }{
      m_bid = tp.m_bid;
      m_ask = tp.m_ask;
    }

    TickPrice(){
      m_bid = 0.0;
      m_ask = 0.0;
    }

    double GetAsk() const{
      return m_ask;
    }

    double GetBid() const{
      return m_bid;
    }

    const DateTime & GetDateTime() const{
      return m_time;
    }
  };

}
