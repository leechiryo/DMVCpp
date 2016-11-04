#include "App.h"
#include "View.h"
#include "Model.h"

namespace mvc {
  map<string, SPView> App::s_views;
  map<string, SPModel> App::s_models;

  double App::DPI_SCALE_X = 1.0f;
  double App::DPI_SCALE_Y = 1.0f;

  ID2D1Factory* App::s_pDirect2dFactory = nullptr;
  IDWriteFactory* App::s_pDWriteFactory = nullptr;

  void App::UpdateViews()
  {
    for (auto m : s_models) {
      if (m.second->ModelChanged()) {
        m.second->UpdateBindedViews();
      }
    }
  }
}
