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

  static LRESULT OnTextChanged(shared_ptr<mvc::TextBox> tbx, WPARAM wParam, LPARAM lParam) {
    auto pubno = mvc::getm<wstring>("pubno");
    auto csvPath = mvc::getm<wstring>("csv_path");
    *pubno = L"event: " + *csvPath;
    return 0;
  }

  static LRESULT ImportCSV(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto m = mvc::getm<wstring>("btn2");
    auto tp = mvc::getv<mvc::TickProvider>("tick_provider1");
    auto cht = mvc::getv<mvc::Chart>("cht1");

    if (*m == L"START") {
      (*m) = L"STOP";
      cht->Reset();
      tp->Start();
    }
    else {
      (*m) = L"START";
      tp->Stop();
      cht->Reset();
    }

    return 0;
  }
};