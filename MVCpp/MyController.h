#pragma once
#include "MVCpp.h"

class MyController {
public:
  static LRESULT UpdateTitle(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {
    auto m = mvc::getm<wstring>("my_model");
    (*m) = L"Hi";
    return 0;
  }
};