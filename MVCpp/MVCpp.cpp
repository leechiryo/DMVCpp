// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "mvc\mvc.h"
#include "mvc\Controls\Window.h"
#include "mvc\Controls\Button.h"
#include "mvc\Controls\TextBox.h"
#include "mvc\Controls\Label.h"
#include "mvc\Controls\CheckBox.h"
#include "mvc\Controls\Radio.h"
#include "mvc\Controls\Candle.h"
#include "mvc\Controls\Layer.h"
#include "mvc\Controls\Dialog.h"
#include "mvc\ViewElements\Image.h"
#include "mvc\ViewElements\Line.h"
#include "MyController.h"
#include <system_error>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")

using namespace mvc;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  App::Initialize();

  load_views(IDR_RCDATA1);

  // 准备 Model
  m<wstring>("my_model", L"Hello!");
  m<int>("groupVal", 0);
  m<wstring>("strGroupVal", L"Selected radio value.");

  // 准备 View
  auto view = v("main_window", L"MVC++ テスト", 800, 600);
  auto layer = view->AppendLayer();
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("50");
  layer->AddLayoutRow("*");
  layer->AddLayoutCol("200");
  layer->AddLayoutCol("*");
  layer->AddLayoutCol("100");
  view->UploadLayout();

  auto lbl = layer->AppendSubView<Label>(L"Some item:");
  regv("abc", lbl); // 将lbl注册到全局，以后程序的其他部分就可以用ID（getv<Label>("abc")）来取得lbl了
  lbl->SetGridPosition(1, 0);
  lbl->SetRightOffset(20);

  auto tbx = layer->AppendSubView<TextBox>(L"Input ...");
  tbx->SetGridPosition(1, 1);
  tbx->SetLeftOffset(0);
  tbx->SetRightOffset(50);

  auto btn = layer->AppendSubView<Button>(L"START");
  btn->SetGridPosition(2, 1);
  btn->SetLeftOffset(0);

  auto cbx = layer->AppendSubView<CheckBox>(L"这是一个CheckBox.");
  cbx->SetGridPosition(3, 1);
  cbx->SetLeftOffset(0);

  auto rdo1 = layer->AppendSubView<Radio>(1, L"这是一个RadioBox1.");
  rdo1->SetGridPosition(4, 1);
  rdo1->SetLeftOffset(0);

  auto rdo2 = layer->AppendSubView<Radio>(2, L"这是一个RadioBox2.");
  rdo2->SetGridPosition(5, 1);
  rdo2->SetLeftOffset(0);

  auto rdo3 = layer->AppendSubView<Radio>(3, L"这是一个RadioBox3.");
  rdo3->SetGridPosition(6, 1);
  rdo3->SetLeftOffset(0);

  auto lbl2 = layer->AppendSubView<Label>(L"");
  lbl2->SetGridPosition(7, 1);
  lbl2->SetLeftOffset(0);

  auto img = layer->AppendSubView<Image>(L"01.png");
  img->SetGridPosition(8, 1);
  img->SetOffset(0, 0);

  auto line = layer->AppendSubView<Line>();
  line->SetColor(0x23ff00);
  line->SetGridPosition(7, 2);
  line->SetOffset(0, 0, 0, 0);

  //auto candle1 = view->AppendSubView<Candle>(1.12345, 1.12124, 1.11843, 1.11957);
  //candle1->SetGridPosition(6, 2);
  //candle1->SetOffset(0, 0);
  //candle1->SetBottomOffset(0);

  //auto candle2 = view->AppendSubView<Candle>(1.12345, 1.11957, 1.11843, 1.12124);
  //candle2->SetGridPosition(6, 2);
  //candle2->SetOffset(10, 0);
  //candle2->SetBottomOffset(0);

  // 设置直线的阴影效果
  auto shadowEffect = line->CreateEffect(CLSID_D2D1Shadow, 0);
  auto affineTransEffect = line->CreateEffect(CLSID_D2D12DAffineTransform);
  auto compositeEffect = line->CreateEffect(CLSID_D2D1Composite, 1);

  affineTransEffect->SetInputEffect(0, shadowEffect.ptr());
  D2D1_MATRIX_3X2_F matrix = D2D1::Matrix3x2F::Translation(-5, 5);
  affineTransEffect->SetValue(D2D1_2DAFFINETRANSFORM_PROP_TRANSFORM_MATRIX, matrix);

  compositeEffect->SetInputEffect(0, affineTransEffect.ptr());
  line->EffectOn();

  auto dialog = view->AppendSubView<Dialog>();
  regv("dialog1", dialog);
  dialog->SetLeftOffset(0);
  dialog->SetTopOffset(0);
  dialog->SetBottomOffset(0);
  dialog->SetWidth("200");

  // 绑定 Model 和 View
  btn->title->Bind("my_model");
  rdo1->selectedValue.Bind("groupVal");
  rdo2->selectedValue.Bind("groupVal");
  rdo3->selectedValue.Bind("groupVal");
  lbl2->text->Bind("strGroupVal");

  // 设置事件处理 Controller
  rdo1->AddEventHandler(WM_LBUTTONDOWN, MyController::ShowSelectRadio);
  rdo2->AddEventHandler(WM_LBUTTONDOWN, MyController::ShowSelectRadio);
  rdo3->AddEventHandler(WM_LBUTTONDOWN, MyController::ShowSelectRadio);
  btn->AddEventHandler(WM_LBUTTONUP, MyController::UpdateTitle);

  view->Show();
}
