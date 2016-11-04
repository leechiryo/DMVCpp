// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "MVCpp.h"
#include <system_error>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HANDLE drawThread;
HANDLE drawThreadExitSignal;

ID2D1Factory* s_pDirect2dFactory;
IDWriteFactory* s_pDWriteFactory;
ID2D1HwndRenderTarget* m_pRenderTarget = nullptr;


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void InitD2D(HWND hWnd) {
  // 初始化DirectX
  HRESULT hr = CoInitialize(NULL);
  if (!SUCCEEDED(hr)) {
    throw std::system_error(EINTR, std::system_category(), "COM environment is not initialized successfully.");
  }
  // create the d2d factory.
  hr = D2D1CreateFactory(
    D2D1_FACTORY_TYPE_MULTI_THREADED,
    &s_pDirect2dFactory);

  if (!SUCCEEDED(hr)) {
    CoUninitialize();
    throw std::system_error(EINTR, std::system_category(), "Direct2D is not initialized successfully.");
  }

  // create the dwrite factory.
  hr = DWriteCreateFactory(
    DWRITE_FACTORY_TYPE_SHARED,
    __uuidof(IDWriteFactory),
    reinterpret_cast<IUnknown**>(&s_pDWriteFactory));

  if (!SUCCEEDED(hr)) {
    SafeRelease(s_pDirect2dFactory);
    CoUninitialize();
    throw std::system_error(EINTR, std::system_category(), "DirectWrite is not initialized successfully.");
  }

  if (!m_pRenderTarget) {
    RECT rc;
    GetClientRect(hWnd, &rc);

    D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

    hr = s_pDirect2dFactory->CreateHwndRenderTarget(
      D2D1::RenderTargetProperties(),
      D2D1::HwndRenderTargetProperties(hWnd, size),
      &m_pRenderTarget);
  }
}

void DeinitD2D() {
  SafeRelease(m_pRenderTarget);
  SafeRelease(s_pDirect2dFactory);
  SafeRelease(s_pDWriteFactory);
  CoUninitialize();
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: ここにコードを挿入してください。
  MSG msg;
  HACCEL hAccelTable;

  // グローバル文字列を初期化しています。
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_MVCPP, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // アプリケーションの初期化を実行します:
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MVCPP));

  // メイン メッセージ ループ:
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  // 当程序结束时，终止绘制窗口的线程。
  SetEvent(drawThreadExitSignal);

  // 等待绘制线程结束
  WaitForSingleObject(drawThread, INFINITE);

  DeinitD2D();

  return (int)msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = 0;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MVCPP));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MVCPP);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

  return RegisterClassEx(&wcex);
}


// 一个描画线程以60FPS的速度描画窗口
DWORD WINAPI DrawWindow(LPVOID lpParam) {
  //HDC hdc;
  HWND hWnd = *((HWND*)lpParam);
  RECT rect, rect1;

  ID2D1SolidColorBrush *blackBrush;
  m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackBrush);

  while (true) {

    do {
      GetClientRect(hWnd, &rect);

      m_pRenderTarget->Resize(D2D1::SizeU(rect.right, rect.bottom));

      m_pRenderTarget->BeginDraw();
      m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
      m_pRenderTarget->DrawRectangle(
        D2D1::RectF(100.0f,
          100.0f,
          rect.right - 100.0f,
          rect.bottom - 100.0f),
        blackBrush);
      m_pRenderTarget->EndDraw();

      GetClientRect(hWnd, &rect1);

      // 如果绘制前与绘制后的窗口大小发生了变化，则立刻调整render target的大小重新绘制。
      // 这样做是为了防止由于render target的拉伸而造成的画面发生闪烁
    } while (rect.right != rect1.right || rect.bottom != rect1.bottom);

    if (WaitForSingleObject(drawThreadExitSignal, 17) == WAIT_OBJECT_0) {
      break;
    }
  }

  return 0;
}


//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します。
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  static HWND hWnd;

  hInst = hInstance; // グローバル変数にインスタンス処理を格納します。

  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  InitD2D(hWnd);

  // 启动一个新的线程以60FPS的速度更新画面。
  drawThreadExitSignal = CreateEvent(NULL, false, false, L"_DRAW_THREAD_EXIT_SIGNAL_");
  drawThread = CreateThread(NULL, 0, DrawWindow, &hWnd, 0, NULL);

  return TRUE;
}


//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND	- アプリケーション メニューの処理
//  WM_PAINT	- メイン ウィンドウの描画
//  WM_DESTROY	- 中止メッセージを表示して戻る
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  RECT rect;
  rect.left = 50;
  rect.top = 50;
  rect.right = 250;
  rect.bottom = 100;

  switch (message)
  {
  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    // 選択されたメニューの解析:
    switch (wmId)
    {
    case IDM_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
      break;
    case IDM_EXIT:
      DestroyWindow(hWnd);
      break;
    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    return (INT_PTR)TRUE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  }
  return (INT_PTR)FALSE;
}
