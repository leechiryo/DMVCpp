#pragma once

#include <ctime>
#include <chrono>

namespace mvc{
  
  using namespace std;

  class DateTime{
  private:

  public:
    int year;
    int month;  // 1~12
    int day;    // 1~31
    int hour;   // 0~23
    int min;    // 0~59
    int second; // 0~59
    int millisecond; // 0~999
    int weekday;     //1~7: Žüˆê“žŽü“ú
  };

}
