// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "mvc\mvc.h"
#include "mvc\ViewElements\Window.h"
#include "mvc\ViewElements\Button.h"
#include "mvc\ViewElements\TextBox.h"
#include "mvc\ViewElements\Label.h"
#include "mvc\ViewElements\CheckBox.h"
#include "mvc\ViewElements\Radio.h"
#include "MyController.h"
#include <system_error>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")

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
  auto view = v<Window>("main_window", { L"MVC++ テスト", 800, 600 });
  auto lbl = view->AddSubView<Label>("lblInput", L"Some item:");
  lbl->SetPos(100, 110, 200, 140);

  auto tbx = view->AddSubView<TextBox>("tbxInput", L"Input ...");
  tbx->SetPos(250, 100, 600, 140);

  auto btn = view->AddSubView<Button>("btnStart", L"START");
  btn->SetPos(100, 200, 200, 240);

  auto cbx = view->AddSubView<CheckBox>("cbxInput", L"这是一个CheckBox.");
  cbx->SetPos(100, 160, 500, 180);

  auto rdo1 = view->AddSubView<Radio>("rdo1Input", { 1, L"这是一个RadioBox1." });
  rdo1->SetPos(100, 260, 500, 280);

  auto rdo2 = view->AddSubView<Radio>("rdo2Input", { 2, L"这是一个RadioBox2." });
  rdo2->SetPos(100, 300, 500, 320);

  auto rdo3 = view->AddSubView<Radio>("rdo3Input", { 3, L"这是一个RadioBox3." });
  rdo3->SetPos(100, 340, 500, 360);

  auto lbl2 = view->AddSubView<Label>("lbl2", L"");
  lbl2->SetPos(100, 400, 500, 420);

  // 绑定 Model 和 View
  btn->title.Bind("my_model");
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

  App::Uninitialize();
}
