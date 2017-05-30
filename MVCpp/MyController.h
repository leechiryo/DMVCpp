#pragma once

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

    auto dialog = mvc::getv<mvc::Dialog>("dialog1");
    dialog->SlideIn(mvc::SlideInDir::fromTop);
    return 0;
  }

  static LRESULT ShowSelectRadio(shared_ptr<mvc::Radio> btn, WPARAM wParam, LPARAM lParam) {
    auto m1 = mvc::getm<int>("groupVal");
    auto m2 = mvc::getm<wstring>("strGroupVal");
    (*m2) = move(to_wstring(*m1));
    return 0;
  }
};