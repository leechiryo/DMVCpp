#pragma once

#include <d2d1.h>
#include <vector>

#include "..\View.h"

namespace mvc {
  class Window : public View<Window>
  {
  private:
    HWND m_hwnd;
    ID2D1HwndRenderTarget* m_pRenderTarget = nullptr;

    // controller method
    static LRESULT Handle_SIZE(shared_ptr<Window> wnd, WPARAM wParam, LPARAM lParam) {
      wnd->m_right = LOWORD(lParam);
      wnd->m_bottom = HIWORD(lParam);
      if (wnd->m_pRenderTarget) {
        wnd->m_pRenderTarget->Resize(D2D1::SizeU((UINT)wnd->m_right, (UINT)wnd->m_bottom));
      }
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


    void CreateMe() {
      m_hwnd = nullptr;

      // Register message handler methods.
      AddEventHandler(WM_SIZE, Handle_SIZE);
      AddEventHandler(WM_DISPLAYCHANGE, Handle_DISPLAYCHANGE);
      AddEventHandler(WM_DESTROY, Handle_DESTROY);

      // Create window using windows api.
      WNDCLASSEX wcex;

      wcex.cbSize = sizeof(WNDCLASSEX);

      wcex.style = CS_HREDRAW | CS_VREDRAW;
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
        this); // pass the this pointer to window parameter

      // 在Constructor中调用虚函数。本来，如此调用虚函数并不会激发
      // 对象的多态调用（即调用派生对象的虚函数）。但在此处每个类都负责
      // 自身所需的D2D资源的创建，所以并不需要调用派生对象的虚函数。
      // 这么做并没有什么问题。
      CreateD2DEnvironment();
    }

  protected:
    virtual void CreateD2DResource() {
      HRESULT hr = S_OK;

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
      CreateMe();
    }
    Window(const WPViewSet & subViews) : View(&m_pRenderTarget, subViews) {
      CreateMe();
    }

    void Show() {
      if (m_hwnd) {

        ShowWindow(m_hwnd, SW_SHOWNORMAL);
        UpdateWindow(m_hwnd);

        // Process and dispatch messages
        MSG msg = { 0 };
        while (true){
          if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) break;
          }
          else{
            // Update the screen.
            Sleep(17);  // Update the screen 60 times every second.
          }
        }
      }
    }

    virtual ~Window() {
      DestroyD2DEnvironment();
    }

    void DrawSelf() {
      m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
      m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    }

    void Update() {
      InvalidateRect(m_hwnd, NULL, false);
    }

    void Close() {
      PostQuitMessage(0);
    }

    // The windows procedure.
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

      LRESULT result = 0;

      if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
        Window *pDemoApp = (Window *)pcs->lpCreateParams;
        ::SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pDemoApp);

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
        Window *pDemoApp = reinterpret_cast<Window *>(static_cast<LONG_PTR>(
          ::GetWindowLongPtr(hwnd, GWLP_USERDATA)));

        if (pDemoApp) {
          if (message == WM_PAINT) {
            // WM_PAINT will never be process by message handler function.
            // It will be process in a special way.
            pDemoApp->m_pRenderTarget->BeginDraw();

            // Method from base class (Element).
            pDemoApp->Draw();

            HRESULT hr = pDemoApp->m_pRenderTarget->EndDraw();

            if (hr == D2DERR_RECREATE_TARGET) {
              // Discard all of the device resources in the child window
              // and recreate them.
              pDemoApp->DestroyD2DEnvironment();
              pDemoApp->CreateD2DEnvironment();
            }

            ValidateRect(hwnd, NULL);

            result = 0;
          }
          else if (message == WM_NCCALCSIZE) {
            /*
            ** From MSDN:
            ** If the wParam parameter is FALSE, the application should return zero.
            ** When wParam is TRUE, simply returning 0 without processing the NCCALCSIZE_PARAMS rectangles
            ** will cause the client area to resize to the size of the window, including the window frame.
            ** This will remove the window frame and caption items from your window, leaving only the client
            ** area displayed.
            */
            return 0;
          }
          else {
            char processed = pDemoApp->HandleMessage(message, wParam, lParam, result);
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
