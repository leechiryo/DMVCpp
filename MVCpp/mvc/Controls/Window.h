#pragma once

#include <vector>
#include "..\View.h"

namespace mvc {
  class Window : public View<Window>
  {
  private:
    HWND m_hwnd;
    HANDLE m_drawThread;
    HANDLE m_drawThreadExitSignal;
    CRITICAL_SECTION m_drawAndResizeSection;

    WPView m_focusedView;

    DxResource<ID3D11Device> m_d3dDevice;
    DxResource<ID3D11DeviceContext> m_d3dContext;
    DxResource<ID2D1Device> m_d2dDevice;
    DxResource<IDXGISwapChain1> m_dxgiSwapChain;
    DxResource<ID2D1Bitmap1> m_d2dBuffer;

    list<shared_ptr<AnimationBase>> m_animations;

    typedef HRESULT(ID2D1DeviceContext::*CreateBitmapFromDxgiSurfaceType)(IDXGISurface*, const D2D1_BITMAP_PROPERTIES1&, ID2D1Bitmap1**);

    // controller method
    static LRESULT Handle_SIZE(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      wnd->m_right = LOWORD(lParam);
      wnd->m_bottom = HIWORD(lParam);
      wnd->UploadLayout();
      for (auto subv : wnd->m_subViews) {
        auto v = subv.lock();
        if (v) {
          v->UpdatePositionAndSize();
        }
      }

      EnterCriticalSection(&wnd->m_drawAndResizeSection);
      Sleep(17);
      LeaveCriticalSection(&wnd->m_drawAndResizeSection);
      return 0;
    }

    static LRESULT Handle_SIZING(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      // 窗口大小发生改变时等待绘制线程重绘画面完成
      EnterCriticalSection(&wnd->m_drawAndResizeSection);
      Sleep(17);
      LeaveCriticalSection(&wnd->m_drawAndResizeSection);
      return 0;
    }

    static LRESULT Handle_DISPLAYCHANGE(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      InvalidateRect(wnd->m_hwnd, NULL, false);
      return 0;
    }

    static LRESULT Handle_DESTROY(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {

      // 当程序结束时，终止绘制窗口的线程。
      SetEvent(wnd->m_drawThreadExitSignal);

      // 等待绘制线程结束
      WaitForSingleObject(wnd->m_drawThread, INFINITE);

      PostQuitMessage(0);
      return 1;

    }

    // 将绘制在缓冲区上的图像切换到前台
    HRESULT PresentBackBuffer() {
      DXGI_PRESENT_PARAMETERS param = { 0 };
      return m_dxgiSwapChain->Present1(0, 0, &param);
    }

    void Resize() {

      auto device1 = m_d3dDevice.Query<ID3D11Device1>();
      auto dxgiDevice = device1.Query<IDXGIDevice>();
      auto dxgiAdapter = dxgiDevice.GetResource(&IDXGIDevice::GetAdapter);
      auto dxgiFactory = dxgiAdapter.GetParent<IDXGIFactory2>();

      // Swap chain
      DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
      RECT rect;
      GetClientRect(m_hwnd, &rect);
      swapChainDesc.Width = rect.right;
      swapChainDesc.Height = rect.bottom;
      swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      swapChainDesc.Stereo = false;
      swapChainDesc.SampleDesc.Count = 1;
      swapChainDesc.SampleDesc.Quality = 0;
      swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      swapChainDesc.BufferCount = 2;
      swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
      swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
      swapChainDesc.Flags = 0;

      m_dxgiSwapChain = dxgiFactory.GetResource<IDXGISwapChain1>(&IDXGIFactory2::CreateSwapChainForHwnd,
        device1.ptr(), m_hwnd, &swapChainDesc, nullptr, nullptr);
      GetClientRect(m_hwnd, &rect);

      auto dxgiBackBuffer = m_dxgiSwapChain.Query<IDXGISurface>(&IDXGISwapChain1::GetBuffer, 0);

      FLOAT dpiX, dpiY;
      App::s_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      D2D1_BITMAP_PROPERTIES1 bmpProp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpiX, dpiY);

      // 更新成员变量。同上这里实现了右值引用的移动而非拷贝
      m_d2dBuffer = m_pContext.CreateBitmap(dxgiBackBuffer.ptr(), bmpProp);

      m_pContext->SetTarget(m_d2dBuffer.ptr());

      // 创建一个特效用的context
      m_pBmpRT = m_pContext.CreateCompatibleRenderTarget();
      m_pEffectContext = m_pBmpRT.Query<ID2D1DeviceContext>();
    }


    // 一个描画线程以60FPS的速度描画窗口
    static DWORD WINAPI DrawWindow(LPVOID lpParam) {
      //HDC hdc;
      Window *pWnd = (Window*)lpParam;
      RECT rect{ 0 }, rect1{ 0 };
      HRESULT hr;

      while (true) {

        EnterCriticalSection(&pWnd->m_drawAndResizeSection);
        GetClientRect(pWnd->m_hwnd, &rect1);

        do {
          pWnd->m_right = rect1.right;
          pWnd->m_bottom = rect1.bottom;
          if (rect.right != rect1.right || rect.bottom != rect1.bottom) {
            pWnd->Resize();
            rect.right = rect1.right;
            rect.bottom = rect1.bottom;
          }

          for (auto a : pWnd->m_animations){
            a->Update();
          }

          pWnd->m_pContext->BeginDraw();

          // 调用基类ViewBase的方法绘制.
          pWnd->Draw();

          hr = pWnd->m_pContext->EndDraw();

          if (hr == D2DERR_RECREATE_TARGET) {
            // 出现绘制错误的情况则尝试重建整个D2D环境
            pWnd->RebuildD2DEnvironment();
          }

          GetClientRect(pWnd->m_hwnd, &rect1);
        } while (rect.right != rect1.right
          || rect.bottom != rect1.bottom
          || hr == D2DERR_RECREATE_TARGET);

        hr = pWnd->PresentBackBuffer();
        LeaveCriticalSection(&pWnd->m_drawAndResizeSection);

        // 检查线程是否已经被主线程终止，如果是，则终止绘制循环并退出函数，否则等待17毫秒并进行
        // 下一次绘制循环。（等待17毫秒意味着每秒钟可以绘制1000 / 17 = 60）
        HANDLE signals[] = { pWnd->m_drawThreadExitSignal };
        if (WaitForMultipleObjects(1, signals, false, 17) == WAIT_OBJECT_0) {
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

      // 获取Direct3D设备和环境
      ID3D11Device *pd3dDevice;
      ID3D11DeviceContext *pd3dContext;
      D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE,
        0, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        fls, ARRAYSIZE(fls), D3D11_SDK_VERSION,
        &pd3dDevice, &retFeatureLevel, 
        &pd3dContext);

      m_d3dDevice = pd3dDevice;
      m_d3dContext = pd3dContext;

      // 2. 查询特定的设备和设备环境的接口。
      // 一下返回的都是用DxResource封装的资源，当发生异常时会自动调用SafeRelease函数释放资源。
      auto device1 = m_d3dDevice.Query<ID3D11Device1>();
      auto context1 = m_d3dContext.Query<ID3D11DeviceContext1>();
      auto dxgiDevice = device1.Query<IDXGIDevice>();

      // 3. 创建Direct2D的设备和设备环境
      // 更新成员变量。这里实现了针对右值引用的移动语义，如果赋值前成员变量已经保存有其他资源，
      // 赋值会把这些资源交换给临时变量，并随着临时变量的释放而被释放掉。
      m_d2dDevice = App::s_pDirect2dFactory.GetResource<ID2D1Device>(&ID2D1Factory1::CreateDevice, dxgiDevice.ptr());
      m_pContext = m_d2dDevice.GetResource<ID2D1DeviceContext>(&ID2D1Device::CreateDeviceContext, D2D1_DEVICE_CONTEXT_OPTIONS_NONE);

      // 4. 创建和窗口大小相关的资源
      auto dxgiAdapter = dxgiDevice.GetResource(&IDXGIDevice::GetAdapter);
      auto dxgiFactory = dxgiAdapter.GetParent<IDXGIFactory2>();

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

      // 更新成员变量。同上这里实现了右值引用的移动而非拷贝
      m_dxgiSwapChain = dxgiFactory.GetResource<IDXGISwapChain1>(&IDXGIFactory2::CreateSwapChainForHwnd,
        device1.ptr(), m_hwnd, &swapChainDesc, nullptr, nullptr);

      auto dxgiBackBuffer = m_dxgiSwapChain.Query<IDXGISurface>(&IDXGISwapChain1::GetBuffer, 0);

      FLOAT dpiX, dpiY;
      App::s_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      D2D1_BITMAP_PROPERTIES1 bmpProp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), dpiX, dpiY);

      // 更新成员变量。同上这里实现了右值引用的移动而非拷贝
      m_d2dBuffer = m_pContext.CreateBitmap(dxgiBackBuffer.ptr(), bmpProp);

      m_pContext->SetTarget(m_d2dBuffer.ptr());

      // 创建一个特效用的context
      m_pBmpRT = m_pContext.CreateCompatibleRenderTarget();
      m_pEffectContext = m_pBmpRT.Query<ID2D1DeviceContext>();
    }

  public:
    Window(const WCHAR * title, int width, int height) : View({}, this) {
      m_hwnd = nullptr;
      m_canBeFocused = false;

      int a = m_animations.size();

      // Register message handler methods.
      AddEventHandler(WM_SIZE, Handle_SIZE);
      AddEventHandler(WM_SIZING, Handle_SIZING);
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
      wcex.lpszClassName = L"MVC++ Window";
      wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

      RegisterClassEx(&wcex);

      m_hwnd = CreateWindowEx(
        0,
        L"MVC++ Window",
        title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        HINST_THISCOMPONENT,
        this); // 将this指针传递给Window创建的参数

      m_left = 0.0f;
      m_right = width;
      m_top = 0.0f;
      m_bottom = height;

      m_absLeft = m_left;
      m_absTop = m_top;

      CreateD2DResource();
    }

    void UploadLayout() {
      m_layout.SetWidth(tof(m_right));
      m_layout.SetHeight(tof(m_bottom));
    }

    void Show() {
      if (m_hwnd) {

        // 创建线程同步用的信号量
        m_drawThreadExitSignal = CreateEvent(NULL, false, false, nullptr);
        InitializeCriticalSection(&m_drawAndResizeSection);

        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);

        // 启动一个新的线程以60FPS的速度更新画面。
        m_drawThread = CreateThread(NULL, 0, DrawWindow, this, 0, NULL);

        // 处理消息
        MSG msg = { 0 };

        // 主消息循环
        while (GetMessage(&msg, NULL, 0, 0))
        {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
    }

    virtual ~Window() {
    }

    virtual void DrawSelf() {
      m_pContext->Clear(D2D1::ColorF(D2D1::ColorF::White));
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
          else {

            if (message == WM_LBUTTONDOWN) {
              int pixelX = GET_X_LPARAM(lParam);
              int pixelY = GET_Y_LPARAM(lParam);
              // 获得选中的子view对象
              auto clickedV = pWnd->GetClickedSubView(pixelX, pixelY);
              auto v = clickedV.lock();
              if (v) {
                // 如果子view对象可以聚焦，则将当前焦点转移到选中的子view对象上
                if (v->m_canBeFocused) {
                  // 放弃以前的焦点
                  auto v2 = pWnd->m_focusedView.lock();
                  if (v2) {
                    v2->m_focused = false;
                    v2->FocusChanged();
                  }
                  pWnd->m_focusedView = clickedV;
                  v->m_focused = true;
                  v->FocusChanged();
                }
              }
            }

            // 如果是键盘事件，则之间将消息发给当前的焦点view上，并指定其处理该消息
            // 如果不存在焦点view，或者该view无法处理此事件，也不会在转发给下一级view。
            if (message == WM_CHAR || message == WM_KEYDOWN) {
              auto v = pWnd->m_focusedView.lock();
              if (v) {
                WPView ev;
                v->HandleMessage(message, wParam, lParam, result, ev);
              }
              return 0;
            }

            WPView ev;
            char processed = pWnd->HandleMessage(message, wParam, lParam, result, ev);
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

    template <typename T>
    shared_ptr<Animation2<T>> CreateAnimation(shared_ptr<T> resource, std::function<bool(T*, int)> updateFunc){
      auto ani = make_shared<Animation2<T>>(resource, updateFunc);
      m_animations.push_back(ani);
      return ani;
    }
  };
}
