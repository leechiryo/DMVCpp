#pragma once

#include <ctime>

namespace mvc{
  
  using namespace std;

  class TickPrice{
  public:
    tm time;
    double bid;
    double ask;
  };

}
