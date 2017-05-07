#include "App.h"
#include "View.h"
#include "Model.h"

namespace mvc {
  map<string, SPView> App::s_views;
  map<string, SPModel> App::s_models;

  double App::DPI_SCALE_X = 1.0f;
  double App::DPI_SCALE_Y = 1.0f;

  DxResource<ID2D1Factory1> App::s_pDirect2dFactory;
  DxResource<IDWriteFactory1> App::s_pDWriteFactory;
  DxResource<IWICImagingFactory> App::s_pImagingFactory;
}
