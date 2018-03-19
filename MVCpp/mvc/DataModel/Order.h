#pragma once

#include "DateTime.h"

namespace mvc{
  using namespace std;

  enum class OrderStatus{Pending, Open, Close};

  struct Order{

    // 仓位状态
    OrderStatus status;

    // 开仓时间(只有状态为Open或Close时才有意义)
    DateTime openTime;

    // 开仓价位(如状态为Penging，则为预定开仓价位，否则为实际开仓价位)
    double open;

    // 平仓价位(如果状态不是Close，则为零)
    double close;

    // 止损(如未设置，则为零)
    double stop;

    // 止盈(如未设置，则为零)
    double limit;

  };
}
