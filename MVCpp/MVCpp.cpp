// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "MVCpp.h"
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

  auto view = v<Window>("main_window", { L"MVC++ テスト", 800, 600 });
  auto model = m<std::wstring>("my_model", L"Hello!");

  auto lbl = view->AddSubView<Label>("lblInput", L"Some item:");
  lbl->SetPos(100, 100, 200, 140);

  auto tbx = view->AddSubView<TextBox>("tbxInput", L"Input ...");
  tbx->SetPos(250, 100, 600, 140);

  auto btn = view->AddSubView<Button>("btnStart", L"START");
  btn->SetPos(100, 200, 200, 240);

  auto cbx = view->AddSubView<CheckBox>("cbxInput", L"这是一个CheckBox.");
  cbx->SetPos(100, 160, 500, 180);

  btn->title.Bind("my_model");
  btn->AddEventHandler(WM_LBUTTONUP, MyController::UpdateTitle);
  view->Show();

  App::Uninitialize();
}
