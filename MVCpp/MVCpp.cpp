// MVCpp.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include "MVCpp.h"
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;								// 現在のインターフェイス
TCHAR szTitle[MAX_LOADSTRING];					// タイトル バーのテキスト
TCHAR szWindowClass[MAX_LOADSTRING];			// メイン ウィンドウ クラス名
HANDLE drawThread;
HANDLE drawThreadExitSignal;

ID2D1Factory1 *d2dFactory;
ID3D11Device *device;
ID3D11DeviceContext *context;
ID2D1Device *d2dDevice;
ID2D1DeviceContext *d2dContext;
IDXGIDevice1 *dxgiDevice;
IDXGIAdapter *dxgiAdapter;
IDXGIFactory2 *dxgiFactory;
IDXGISwapChain1 *swapChain;
ID3D11Texture2D *backBuffer;
ID3D11RenderTargetView *renderTarget;
IDXGISurface *dxgiBackBuffer;
ID2D1Bitmap1 *bmp;


// このコード モジュールに含まれる関数の宣言を転送します:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void InitD2D(HWND hWnd){
  // 初始化DirectX
  HRESULT hr = 0;

  D3D_FEATURE_LEVEL featureLevel;
  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
  };

  hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
    featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, &featureLevel, &context);
  hr = device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice);

  D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory);
  hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
  hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);

  DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
  scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  scd.BufferCount = 2;
  scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  scd.SampleDesc.Count = 1;

  hr = dxgiDevice->GetAdapter(&dxgiAdapter);
  hr = dxgiAdapter->GetParent(_uuidof(IDXGIFactory2), (void**)&dxgiFactory);
  hr = dxgiFactory->CreateSwapChainForHwnd(device, hWnd, &scd, nullptr, nullptr, &swapChain);
  hr = dxgiDevice->SetMaximumFrameLatency(1);
  hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
  hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTarget);

  D2D1_BITMAP_PROPERTIES1 bmpProp;
  bmpProp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
  bmpProp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
  bmpProp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
  d2dFactory->GetDesktopDpi(&bmpProp.dpiX, &bmpProp.dpiY);
  bmpProp.colorContext = nullptr;

  hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));
  hr = d2dContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bmpProp, &bmp);
  d2dContext->SetTarget(bmp);

}

void DeinitD2D(){
  SafeRelease(d2dFactory);
  SafeRelease(device);
  SafeRelease(context);
  SafeRelease(d2dDevice);
  SafeRelease(d2dContext);
  SafeRelease(dxgiDevice);
  SafeRelease(dxgiAdapter);
  SafeRelease(dxgiFactory);
  SafeRelease(swapChain);
  SafeRelease(backBuffer);
  SafeRelease(renderTarget);
  SafeRelease(dxgiBackBuffer);
  SafeRelease(bmp);
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

  wcex.style = CS_HREDRAW | CS_VREDRAW;
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
DWORD WINAPI DrawWindow(LPVOID lpParam){
  //HDC hdc;
  HWND hWnd = *((HWND*)lpParam);
  RECT rect;

  ID2D1SolidColorBrush *blackBrush;
  d2dContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &blackBrush);

  while (true){
    GetClientRect(hWnd, &rect);
    //hdc = GetDC(hWnd);

    //DrawText(hdc, L"I like windows.", 15, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    //ReleaseDC(hWnd, hdc);

    context->OMSetRenderTargets(1, &renderTarget, nullptr);

    d2dContext->BeginDraw();


    d2dContext->DrawRectangle(
      D2D1::RectF(rect.left + 100.0f,
                  rect.top + 100.0f,
                  rect.right - 100.0f,
                  rect.bottom - 100.0f),
      blackBrush);

    d2dContext->EndDraw();

    swapChain->Present(1, 0);

    if (WaitForSingleObject(drawThreadExitSignal, 0) == WAIT_OBJECT_0){
      break;
    }

    Sleep(17);
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
  PAINTSTRUCT ps;
  HDC hdc;
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
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);
    // TODO: 描画コードをここに追加してください...
    EndPaint(hWnd, &ps);
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
