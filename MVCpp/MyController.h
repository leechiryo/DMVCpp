#pragma once
#include "MVCpp.h"

class MyController {
public:
  static LRESULT UpdateTitle(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {
    auto m = mvc::getm<wstring>("my_model");
    if (*m == L"Hello!") {
      (*m) = L"Hi";
    }
    else {
      (*m) = L"Hello!";
    }
    return 0;
  }
};