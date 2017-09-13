#pragma once

#include "DateTime.h"

namespace mvc{
  
  using namespace std;

  class BarPrice{
  private:
    DateTime m_time;
    double m_high;
    double m_open;
    double m_low;
    double m_close;

  public:
    BarPrice(const char* timeStr, double high, double open, double low, double close) :m_time{ timeStr }{
      m_high = high;
      m_open = open;
      m_low = low;
      m_close = close;
    }

    double GetHigh(){
      return m_high;
    }

    double GetOpen(){
      return m_open;
    }

    double GetLow(){
      return m_low;
    }

    double GetClose(){
      return m_close;
    }

    const DateTime & GetDateTime(){
      return m_time;
    }
  };

}
