#pragma once

#include "sqlite3\sqlite3.h"

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

  static LRESULT ImportCSV(shared_ptr<mvc::Button> btn, WPARAM wParam, LPARAM lParam) {
    auto m = mvc::getm<wstring>("csv_path");

    MessageBox(NULL, m->c_str(), L"OK", MB_OK);

    // sqlite3 test
    sqlite3 *db;
    int rc = sqlite3_open("abc.db", &db);
    if (rc){
      MessageBox(NULL, L"Error opening SQLite3 database.", L"ERROR", MB_OK);
    }
    else{
      MessageBox(NULL, L"opened SQLite3 database.", L"OK", MB_OK);
    }

    sqlite3_close(db);

    return 0;
  }
};