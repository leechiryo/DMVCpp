#pragma once

#include "DateTime.h"
#include "TickPrice.h"

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

    BarPrice(const TickPrice & tp) : m_time{ tp.GetDateTime() }{
      m_high = tp.GetBid();
      m_open = tp.GetBid();
      m_low = tp.GetBid();
      m_close = tp.GetBid();
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

    void UpdateTick(const TickPrice &tp){
      m_close = tp.GetBid();

      if (m_close > m_high){
        m_high = m_close;
      }

      if (m_close < m_low){
        m_low = m_close;
      }
    }

    const DateTime & GetDateTime(){
      return m_time;
    }
  };

}
