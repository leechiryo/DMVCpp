// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "mvc\mvc.h"
#include "mvc\Controls\Window.h"
#include "mvc\Controls\Button.h"
#include "mvc\Controls\TextBox.h"
#include "mvc\Controls\Label.h"
#include "mvc\Controls\CheckBox.h"
#include "mvc\Controls\Radio.h"
#include "mvc\Controls\Layer.h"
#include "mvc\Controls\Dialog.h"
#include "mvc\Controls\Chart.h"
#include "mvc\ViewElements\Image.h"
#include "mvc\ViewElements\Line.h"
#include <system_error>
#include "mvc/DataModel/DateTime.h"
#include "mvc/DataModel/TickPrice.h"

#include "MyController.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "Windowscodecs.lib")
#pragma comment(lib, "sqlite3.lib")

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
  m<wstring>("csv_path", L"");

  // 准备 View
  auto view = v("main_window", L"MVC++ テスト", 800, 600);
  auto layer = view->AppendLayer();
  regv("layer1", layer);
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

  auto line = layer->AppendSubView<Line>();

  auto lbl = layer->AppendSubView<Label>(L"CSV File:");
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

  auto btn2 = layer->AppendSubView<Button>(L"IMPORT CSV");
  btn2->SetGridPosition(2, 1);
  btn2->SetLeftOffset(150);

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

  line->SetColor(0x23ff00);
  line->SetGridPosition(7, 2);
  line->SetOffset(0, 0, 0, 0);

  BarPrice bp1{ "2017.01.23 12:22:23.221", 1.12345, 1.12124, 1.11843, 1.11957 };
  BarPrice bp2{ "2017.01.23 12:22:23.221", 1.12034, 1.11957, 1.11331, 1.11423 };
  BarPrice bp3{ "2017.01.23 12:22:23.221", 1.11658, 1.11423, 1.11212, 1.11212 };
  BarPrice bp4{ "2017.01.23 12:22:23.221", 1.11443, 1.11212, 1.11021, 1.11323 };

  //auto cht = layer->AppendSubView<Chart>();
  //cht->AddBar(bp1);
  //cht->AddBar(bp2);
  //cht->AddBar(bp3);
  //cht->AddBar(bp4);

  //cht->SetGridPosition(8, 1);
  //cht->SetOffset(100, 0, 0, 10);

  // 设置直线的阴影效果
  // 阴影效果通过以下特效的组合来达成。
  //
  // ┌───┐                                ┌────────┐
  // │src│ ─→ shadow → affine transfrom → │  com-  │
  // │img│ ─────────────────────────────→ │ posite │ → 最终结果
  // └───┘                                └────────┘
  //
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
  dialog->SetWidth("400");
  dialog->SetHeight("300");

  // 绑定 Model 和 View

  // 直接绑定。绑定的Model对象类型和自身表达的类型一致。
  // *直接绑定是双向绑定。即Model值的改变会自动反映到View上，
  // *反之View上的改变，也会自动反映到Model上。
  btn->title->Bind("my_model");
  rdo1->selectedValue.Bind("groupVal");
  rdo2->selectedValue.Bind("groupVal");
  rdo3->selectedValue.Bind("groupVal");
  tbx->text->Bind("csv_path");

  // 将lbl2的text属性绑定到id为groupVal的model上。
  // 但是，这不是一个直接绑定，因为groupVal的类型为int型
  // 而lbl2的text表达的是wstring型，所以为了绑定到int型需要附加一个转换函数
  // 负责将int类型的值转换成wstring。这种绑定方式称为间接绑定。
  // 转换函数可以使用函数指针，或者lambda表达式。
  // 编译器不能识别出模板类型，所以要添加一个模板提示（即Bind<int>(...)）。
  // *间接绑定是单向绑定。即Model值的改变会自动反映到View上，
  // *但是View上的改变不会反映到Model上。
  lbl2->text->Bind<int>("groupVal", [](const int * iptr, wstring& s){
    if (!(*iptr)){
      s = L"Select radio value.";
    }
    else{
      wchar_t buf[] = L"You selected: x";
      int index = sizeof(buf)/2 - 2;
      buf[index] = to_wstring(*iptr)[0];
      s = buf;
    }
  });

  // 设置事件处理 Controller
  btn->AddEventHandler(WM_LBUTTONUP, MyController::UpdateTitle);
  dialog->closebtn->AddEventHandler(WM_LBUTTONUP, MyController::CloseDialog);
  btn2->AddEventHandler(WM_LBUTTONUP, MyController::ImportCSV);

  view->Show();
}
