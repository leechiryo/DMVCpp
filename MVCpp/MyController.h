#pragma once

class MyController {
public:
  static LRESULT UpdateTitle(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {
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

    //auto cht = mvc::getv<mvc::Chart>("cht1");

    //auto & tp = cht->LastTick();
    //mvc::OrderInfo oi;
    //oi.open = tp.GetBid();
    //oi.direction = mvc::OrderDirection::Buy;
    //oi.stop = oi.open - 0.0020;
    //oi.limit = oi.open + 0.0100;
    //oi.status = mvc::OrderStatus::Open;
    //oi.openTime = tp.GetDateTime();

    //cht->AddOrder(oi);

    return 0;
  }

  static LRESULT ImportCSV(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto m = mvc::getm<wstring>("btnStart");
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