#pragma once

#include <ctime>
#include <chrono>
#include "../Types.h"

namespace mvc{

  using namespace std;

  class DateTime{
  private:

    time_t m_sectime;
    int m_year;
    int m_month;  // 1~12
    int m_day;    // 1~31
    int m_hour;   // 0~23
    int m_min;    // 0~59
    int m_second; // 0~59
    int m_millisecond; // 0~999
    int m_weekday;     //0~6: 周日到周六

  public:
    // timestr = 2011.01.02 22:00:26.739
    DateTime(const char * timestr){

      if (!timestr || strlen(timestr) < 23){
        string errmsg = "Cannot create date time from time string: ";
        if (!timestr) errmsg = errmsg + "[null]";
        else errmsg = errmsg + timestr;
        throw std::runtime_error(errmsg);
      }

      int num = sscanf_s(timestr, "%d.%d.%d %d:%d:%d.%d", &m_year, &m_month, &m_day, &m_hour, &m_min, &m_second, &m_millisecond);

      if (num != 7){
        string errmsg = "Cannot create date time from time string: ";
        if (!timestr) errmsg = errmsg + "[null]";
        else errmsg = errmsg + timestr;
        throw std::runtime_error(errmsg);
      }
      
      tm tm_ = {0};
      tm_.tm_year = m_year - 1900;
      tm_.tm_mon = m_month - 1;
      tm_.tm_mday = m_day;
      tm_.tm_hour = m_hour;
      tm_.tm_min = m_min;
      tm_.tm_sec = m_second;

      m_sectime = mktime(&tm_);

      if (m_sectime == -1){
        string errmsg = "Cannot create date time from time string: ";
        errmsg = errmsg + timestr;
        throw std::runtime_error(errmsg);
      }

      m_weekday = tm_.tm_wday;
    }

    // 计算两个时间结构之间的差。结果是long long类型，单位是微秒。
    long long operator-(const DateTime & t){
      time_t diffsec = m_sectime - t.m_sectime;
      int diffmilli = m_millisecond - t.m_millisecond;
      long long retval = diffsec * 1000 + diffmilli;
      return retval;
    }

    int GetYear(){
      return m_year;
    }

    int GetMonth(){
      return m_month;
    }

    int GetDay(){
      return m_day;
    }

    int GetHour(){
      return m_hour;
    }

    int GetMinute(){
      return m_min;
    }

    int GetSecond(){
      return m_second;
    }

    int GetMillisecond(){
      return m_millisecond;
    }

    int GetWeekDay(){
      return m_weekday;
    }

  };

}
