﻿#pragma once

#include <vector>

#include "..\View.h"

namespace mvc {
  class Window : public View<Window>
  {
  private:
    HWND m_hwnd;
    HANDLE m_drawThread;
    HANDLE m_drawThreadExitSignal;

    // controller method
    static LRESULT Handle_SIZE(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      wnd->m_right = LOWORD(lParam);
      wnd->m_bottom = HIWORD(lParam);
      return 0;
    }

    static LRESULT Handle_DISPLAYCHANGE(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      InvalidateRect(wnd->m_hwnd, NULL, false);
      return 0;
    }

    static LRESULT Handle_DESTROY(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      PostQuitMessage(0);
      return 1;
    }

    // 一个描画线程以60FPS的速度描画窗口
    static DWORD WINAPI DrawWindow(LPVOID lpParam) {
      //HDC hdc;
      Window *pWnd = (Window*)lpParam;
      RECT rect, rect1;
      HRESULT hr;

      while (true) {

        do {
          GetClientRect(pWnd->m_hwnd, &rect);
          pWnd->m_pRenderTarget->Resize(D2D1::SizeU(rect.right, rect.bottom));

          pWnd->m_pRenderTarget->BeginDraw();

          // 调用基类ViewBase的方法.
          pWnd->Draw();

          hr = pWnd->m_pRenderTarget->EndDraw();

          if (hr == D2DERR_RECREATE_TARGET) {
            // 出现绘制错误的情况则尝试重建整个D2D环境
            pWnd->DestroyD2DEnvironment();
            pWnd->CreateD2DEnvironment();
          }

          GetClientRect(pWnd->m_hwnd, &rect1);
        } while (rect.right != rect1.right
          || rect.bottom != rect1.bottom
          || hr == D2DERR_RECREATE_TARGET);

        // 检查线程是否已经被主线程终止，如果是，则终止绘制循环并推出函数，否则等待17毫秒并进行
        // 下一次绘制循环。（等待17毫秒意味着每秒钟可以绘制1000 / 17 = 60）
        if (WaitForSingleObject(pWnd->m_drawThreadExitSignal, 17) == WAIT_OBJECT_0) {
          break;
        }
      }

      return 0;
    }


  protected:
    // 初始化D2D的设备相关的资源
    virtual void CreateD2DResource() {

      // 1. 创建Direct3D的设备和设备环境
      HRESULT hr = S_OK;
      D3D_FEATURE_LEVEL fls[] = {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
      };

      D3D_FEATURE_LEVEL retFeatureLevel;

      // 用DxResource封装的资源，当发生异常时会自动调用SafeRelease函数释放资源。
      DxResource<ID3D11Device> device;
      DxResource<ID3D11DeviceContext> context;

      D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 
        0, D3D11_CREATE_DEVICE_BGRA_SUPPORT, 
        fls, ARRAYSIZE(fls), D3D11_SDK_VERSION,
        &device, &retFeatureLevel, &context);

      // 2. 获取特定的设备和设备环境的接口。
      auto device1 = device.Query<ID3D11Device1>(); 
      auto context1 = context.Query<ID3D11DeviceContext1>();
      auto dxgiDevice = device1.Query<IDXGIDevice>();

      // 3. 创建Direct2D的设备和设备环境
      auto d2dDevice = App::s_pDirect2dFactory.GetResource<ID2D1Device>(&ID2D1Factory1::CreateDevice, dxgiDevice.GetPtr());

      // 实验：用模板把参数传递和错误处理等不自然的地方封装起来
      // d2dDevice = GetXResource<ID2D1Device>(App::s_pDirect2dFactory, &ID2D1Factory1::CreateDevice, dxgiDevice);
      d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pContext);

      // 4. 创建和窗口大小相关的资源
      auto dxgiAdapter = dxgiDevice.GetResource(&IDXGIDevice::GetAdapter);
      auto dxgiFactory = dxgiAdapter.GetParent<IDXGIFactory2>();
      //dxgiDevice->GetAdapter(&dxgiAdapter);
      //dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));

      // Swap chain
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
      swapChainDesc.Width = 0;
      swapChainDesc.Height = 0;
      swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      swapChainDesc.Stereo = false;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.BufferCount = 2;
      swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
      swapChainDesc.Flags = 0;

      IDXGISwapChain1 *dxgiSwapChain;
      dxgiFactory->CreateSwapChainForHwnd(device1.GetPtr(), m_hwnd, 
        &swapChainDesc, nullptr, nullptr, &dxgiSwapChain);

      IDXGISurface *dxgiBackBuffer;
      dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

      FLOAT dpiX, dpiY;
      App::s_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      ID2D1Bitmap1 *d2dBuffer;
      D2D1_BITMAP_PROPERTIES1 bmpProp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpiX, dpiY);
      m_pContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bmpProp, &d2dBuffer);
      m_pContext->SetTarget(d2dBuffer);

      if (!m_pRenderTarget) {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        hr = App::s_pDirect2dFactory->CreateHwndRenderTarget(
          D2D1::RenderTargetProperties(),
          D2D1::HwndRenderTargetProperties(m_hwnd, size),
          &m_pRenderTarget);
      }

    }

    virtual void DestroyD2DResource() {
      SafeRelease(m_pRenderTarget);
    }

  public:
    Window() {
      m_hwnd = nullptr;

      // Register message handler methods.
      AddEventHandler(WM_SIZE, Handle_SIZE);
      AddEventHandler(WM_DISPLAYCHANGE, Handle_DISPLAYCHANGE);
      AddEventHandler(WM_DESTROY, Handle_DESTROY);

      // Create window using windows api.
      WNDCLASSEX wcex;

      wcex.cbSize = sizeof(WNDCLASSEX);

      wcex.style = 0;
      wcex.lpfnWndProc = WndProc;
      wcex.cbClsExtra = 0;
      wcex.cbWndExtra = sizeof(LONG_PTR);
      wcex.hInstance = HINST_THISCOMPONENT;
      wcex.hIcon = NULL;
      wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
      wcex.lpszMenuName = NULL;
      wcex.lpszClassName = L"NativeWPFWindow";
      wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

      RegisterClassEx(&wcex);

      m_hwnd = CreateWindowEx(
        0,
        L"NativeWPFWindow",
        TEXT("Native WPF Demo"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this); // 将this指针传递给Window创建的参数
    }

    void Show() {
      if (m_hwnd) {

        // 创建D2D的绘制环境
        CreateD2DEnvironment();

        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);

        // 启动一个新的线程以60FPS的速度更新画面。
        m_drawThreadExitSignal = CreateEvent(NULL, false, false, nullptr);
        m_drawThread = CreateThread(NULL, 0, DrawWindow, this, 0, NULL);

        // 处理消息
        MSG msg = { 0 };

        // 主消息循环
        while (GetMessage(&msg, NULL, 0, 0))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }

        // 当程序结束时，终止绘制窗口的线程。
        SetEvent(m_drawThreadExitSignal);

        // 等待绘制线程结束
        WaitForSingleObject(m_drawThread, INFINITE);
      }
    }

    virtual ~Window() {
      DestroyD2DEnvironment();
    }

    virtual void DrawSelf() {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
      m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    }

    void Update() {
      InvalidateRect(m_hwnd, NULL, false);
    }

    void Close() {
      PostQuitMessage(0);
    }

    // 窗口消息函数
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

      LRESULT result = 0;

      if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Window *pWnd = (Window *)pcs->lpCreateParams;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pWnd);

        // inform the application of the frame change.
        RECT rcClient;
        GetWindowRect(hwnd, &rcClient);
        SetWindowPos(
          hwnd,
          NULL,
          rcClient.left,
          rcClient.top,
          rcClient.right - rcClient.left,
          rcClient.bottom - rcClient.top,
          SWP_FRAMECHANGED);

        result = 1;
      }
      else {
        Window *pWnd = reinterpret_cast<Window *>(static_cast<LONG_PTR>(
          ::GetWindowLongPtr(hwnd, GWLP_USERDATA)));

        if (pWnd) {
          if (message == WM_NCCALCSIZE) {
            /*
            ** From MSDN:
            ** If the wParam parameter is FALSE, the application should return zero.
            ** When wParam is TRUE, simply returning 0 without processing the NCCALCSIZE_PARAMS rectangles
            ** will cause the client area to resize to the size of the window, including the window frame.
            ** This will remove the window frame and caption items from your window, leaving only the client
            ** area displayed.
            */
            // return 0;
            return DefWindowProc(hwnd, message, wParam, lParam);
          }
          else if (message == WM_SIZE){
            return DefWindowProc(hwnd, message, wParam, lParam);
          }
          else {
            char processed = pWnd->HandleMessage(message, wParam, lParam, result);
            if (!processed) {
              result = DefWindowProc(hwnd, message, wParam, lParam);
            }
          }
        }
        else {
          result = DefWindowProc(hwnd, message, wParam, lParam);
        }
      }

      return result;
    }


  };
}
