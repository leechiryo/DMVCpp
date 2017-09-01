#pragma once

#include <ctime>

namespace mvc{
  
  using namespace std;

  class BarPrice{
  public:
    tm time;
    double bid;
    double ask;
  };

}
