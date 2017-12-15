#pragma once

#include <ctime>
#include <chrono>
#include "../Types.h"

namespace mvc{

  using namespace std;

  enum class TimeFrame {
    ct1Min, ct5Min, ct15Min, ct1Hour, ct4Hour, ct1Day, ct1Week, ct1Mon
  };

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
    int m_weekCount; // 0~52 or 53

  public:

    DateTime(){
      // create current time.
      struct tm tm_;
      time(&m_sectime);
      localtime_s(&tm_, &m_sectime);
      m_second = tm_.tm_sec;
      m_year  =tm_.tm_year + 1900;
      m_month = tm_.tm_mon + 1;
      m_day = tm_.tm_mday;
      m_hour = tm_.tm_hour;
      m_min = tm_.tm_min;
      m_second = tm_.tm_sec;
      m_weekday = tm_.tm_wday;
      m_weekCount = (tm_.tm_yday + 7 - (tm_.tm_wday ? (tm_.tm_wday - 1) : 6)) / 7;
    }

    bool SameTime(const DateTime& time, TimeFrame tf) const {
      bool retval = true;

      switch (tf){
      case TimeFrame::ct1Min:
        retval = retval && (m_min == time.m_min);
      case TimeFrame::ct5Min:
        retval = retval && ((m_min / 5) == (time.m_min / 5));
      case TimeFrame::ct15Min:
        retval = retval && ((m_min / 15) == (time.m_min / 15));
      case TimeFrame::ct1Hour:
        retval = retval && (m_hour == time.m_hour);
      case TimeFrame::ct4Hour:
        retval = retval && ((m_hour / 4) == (time.m_hour / 4));
      case TimeFrame::ct1Day:
        retval = retval && (m_day == time.m_day);
      case TimeFrame::ct1Mon:
        retval = retval && (m_year == time.m_year && m_month == time.m_month);
        break;

      case TimeFrame::ct1Week:
        retval = retval && (m_year == time.m_year && m_weekCount == time.m_weekCount);
        break;
      }

      return retval;
    }

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
      m_weekCount = (tm_.tm_yday + 7 - (tm_.tm_wday ? (tm_.tm_wday - 1) : 6)) / 7;
    }

    // 计算两个时间结构之间的差。结果是long long类型，单位是微秒。
    long long operator-(const DateTime & t) const{
      time_t diffsec = m_sectime - t.m_sectime;
      int diffmilli = m_millisecond - t.m_millisecond;
      long long retval = diffsec * 1000 + diffmilli;
      return retval;
    }

    int GetYear() const{
      return m_year;
    }

    int GetMonth() const{
      return m_month;
    }

    int GetDay() const{
      return m_day;
    }

    int GetHour() const{
      return m_hour;
    }

    int GetMinute() const{
      return m_min;
    }

    int GetSecond() const{
      return m_second;
    }

    int GetMillisecond() const{
      return m_millisecond;
    }

    int GetWeekDay() const{
      return m_weekday;
    }

  };

}
