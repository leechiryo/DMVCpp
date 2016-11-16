// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "MVCpp.h"
#include <system_error>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace mvc;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  App::Initialize();

  auto view = v<Window>("main_window", {});
  auto model = m<std::string>("def", "abc");

  view->Show();

  App::Uninitialize();
}
