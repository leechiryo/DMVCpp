#include "App.h"
#include "View.h"
#include "Model.h"

namespace mvc {
  ComLibrary App::s_comLib;
  map<string, SPView> App::s_viewsWithId;
  list<SPView> App::s_allViews;
  map<string, SPModel> App::s_models;

  double App::DPI_SCALE_X = 1.0f;
  double App::DPI_SCALE_Y = 1.0f;

  DxResource<ID2D1Factory1> App::s_pDirect2dFactory;
  DxResource<IDWriteFactory1> App::s_pDWriteFactory;
  DxResource<IWICImagingFactory> App::s_pImagingFactory;

  class AppInitializer{
  public:
    AppInitializer(){

      // 此处的代码会在main函数之前执行。
      App::Initialize();

    }
  } _appInitializer;
}
