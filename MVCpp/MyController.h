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

    auto layer = mvc::getv<mvc::Layer>("layer1");
    layer->Blur();
    return 0;
  }

  static LRESULT CloseDialog(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {
    auto layer = mvc::getv<mvc::Layer>("layer1");
    layer->Unblur();
    return 0;
  }
};