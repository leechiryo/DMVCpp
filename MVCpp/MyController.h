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
    auto startDate = mvc::getm<wstring>("start_date");
    *pubno = L"event: " + *startDate;

    return 0;
  }

  static LRESULT StartChart(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto m = mvc::getm<wstring>("btnStart");
    auto tp = mvc::getv<mvc::TickProvider>("tick_provider1");
    auto cht = mvc::getv<mvc::Chart>("cht1");
    auto date = mvc::getm<wstring>("start_date");

    string startDate = mvc::Utf16To8(date->c_str()).get();
    if (startDate != "") {
      if (*m == L"START") {
        (*m) = L"STOP";
        cht->Reset();
        tp->Start(startDate);
      }
      else {
        (*m) = L"START";
        tp->Stop();
        cht->Reset();
      }
    }

    return 0;
  }

  static LRESULT ChangeFrame(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto m = mvc::getm<wstring>("time_frame");

    if (*m == L"M1") {
      (*m) = L"M5";
    }
    else if (*m == L"M5") {
      (*m) = L"M15";
    }
    else if (*m == L"M15") {
      (*m) = L"M30";
    }
    else if (*m == L"M30") {
      (*m) = L"H1";
    }
    else if (*m == L"H1") {
      (*m) = L"H4";
    }
    else if (*m == L"H4") {
      (*m) = L"D1";
    }
    else if (*m == L"D1") {
      (*m) = L"W1";
    }
    else if (*m == L"W1") {
      (*m) = L"Mon1";
    }
    else if (*m == L"Mon1") {
      (*m) = L"M1";
    }


    return 0;
  }

  static LRESULT OnBuy(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto cht = mvc::getv<mvc::Chart>("cht1");
    auto btnSell = mvc::getv<mvc::Button>("btnSell");

    auto & tp = cht->LastTick();
    auto & text = *(btn->title->SafePtr());
    auto & text2 = *(btnSell->title->SafePtr());

    if (tp.GetAsk() != 0.0 && tp.GetBid() != 0.0) {

      if (text2 == L"Sell") {
        if (text == L"Buy") {
          mvc::OrderInfo oi;
          oi.open = tp.GetBid();
          oi.direction = mvc::OrderDirection::Buy;
          oi.stop = oi.open - 0.0020;
          oi.limit = oi.open + 0.0100;
          oi.status = mvc::OrderStatus::Open;
          oi.openTime = tp.GetDateTime();

          cht->AddOrder(oi);
          text = L"Close";
        }
        else {
          cht->CloseOrder();
          text = L"Buy";
        }
      }
    }

    return 0;
  }

  static LRESULT OnSell(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {

    auto cht = mvc::getv<mvc::Chart>("cht1");
    auto btnBuy = mvc::getv<mvc::Button>("btnBuy");

    auto & tp = cht->LastTick();
    auto & text = *(btn->title->SafePtr());
    auto & text2 = *(btnBuy->title->SafePtr());

    if (tp.GetAsk() != 0.0 && tp.GetBid() != 0.0) {
      if (text2 == L"Buy") {
        if (text == L"Sell") {
          mvc::OrderInfo oi;
          oi.open = tp.GetBid();
          oi.direction = mvc::OrderDirection::Sell;
          oi.stop = oi.open + 0.0020;
          oi.limit = oi.open - 0.0100;
          oi.status = mvc::OrderStatus::Open;
          oi.openTime = tp.GetDateTime();

          cht->AddOrder(oi);
          text = L"Close";
        }
        else {
          cht->CloseOrder();
          text = L"Sell";
        }
      }
    }

    return 0;
  }
};