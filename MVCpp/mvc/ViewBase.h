#pragma once

#include "Types.h"
#include "D2DContext.h"
#include "App.h"

using namespace std;

namespace mvc {

  class ViewBase {

    template<typename T>
    friend class ModelRef;
    friend class App;

    template<typename T>
    friend class View;

    friend class Window;

  private:
    bool m_hidden;

  protected:
    WPView m_wpThis;
    WPViewSet m_subViews;

    double m_absLeft;
    double m_absTop;

    double m_left;
    double m_top;
    double m_right;
    double m_bottom;

    bool m_mouseIn = 0;
    bool m_canBeFocused = true;
    bool m_focused = false;

    // 指向Window对象的D2DContext字段的指针。每个Window都有一个独立的D2DContext对象，
    // 其内部的所有subview将共享这一对象，并利用该对象进行绘制。
    D2DContext m_pContext;

    virtual void CreateD2DResource() = 0;
    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result, WPView &eventView) = 0;
    virtual WPView GetClickedSubView(int pixelX, int pixelY) = 0;

    // 以前是需要的，在引入了DxResource以后就变得不太需要了。
    // 但是谨慎起见，暂时先保留。
    virtual void DestroyD2DResource() {};

    // 如果需要处理鼠标进入事件，可以重载此函数
    virtual void MouseEnter(double x, double y) {
    }

    // 如果需要处理鼠标离开事件，可以重载此函数
    virtual void MouseLeft(double x, double y) {
    }

    void GetPixelRect(RECT &rect) {
      rect.left = DipsXToPixels(m_left);
      rect.top = DipsYToPixels(m_top);
      rect.right = DipsXToPixels(m_right);
      rect.bottom = DipsYToPixels(m_bottom);
    }

    void CreateD2DEnvironment() {
      CreateD2DResource();

      for (auto e : m_subViews) {
        auto spv = e.lock();
        if (spv) {
          spv->m_pContext = m_pContext;
          spv->CreateD2DEnvironment();
        }
      }
    }

    void DestroyD2DEnvironment() {
      for (auto e : m_subViews) {
        auto spv = e.lock();
        if (spv)spv->DestroyD2DEnvironment();
      }

      DestroyD2DResource();
    }

    double AbsX2RelX(int absX) {
      return (absX - m_absLeft) / App::DPI_SCALE_X;
    }

    double AbsY2RelY(int absY) {
      return (absY - m_absTop) / App::DPI_SCALE_Y;
    }

    template <typename T>
    double PixelsToDipsX(T x) {
      return static_cast<double>(x) / App::DPI_SCALE_X;
    }

    template <typename T>
    double PixelsToDipsY(T y) {
      return static_cast<double>(y) / App::DPI_SCALE_Y;
    }

    int DipsXToPixels(double x) {
      return static_cast<int>(x * App::DPI_SCALE_X);
    }

    int DipsYToPixels(double y) {
      return static_cast<int>(y * App::DPI_SCALE_Y);
    }

    virtual bool HitTest(double dipX, double dipY) {
      return dipX >= 0 && dipX <= (m_right - m_left)
        && dipY >= 0 && dipY <= (m_bottom - m_top);
    }

    bool HitTest(int pixelsX, int pixelsY) {
      double dipX = AbsX2RelX(pixelsX);
      double dipY = AbsY2RelY(pixelsY);

      return HitTest(dipX, dipY);
    }

  public:

    ViewBase() {
      m_hidden = false;
    }

    virtual ~ViewBase() { }

    double left, top, width, height;

    virtual void DrawSelf() = 0;

    void SetHidden(bool hidden) {
      m_hidden = hidden;
    }

    void Draw() {

      if (m_hidden) return;

      DrawSelf();

      // 開始繪製内部元素，首先移動坐標原點到父元素的左上角
      m_pContext->SetTransform(TranslationMatrix(m_left, m_top));

      for (auto it = m_subViews.begin(); it != m_subViews.end(); ++it) {
        const auto &v = *it;
        auto ptr = v.lock();

        if (ptr) {
          ptr->m_absLeft = m_absLeft + ptr->m_left;
          ptr->m_absTop = m_absTop + ptr->m_top;
          ptr->Draw();
        }
        else if (v.expired()) {
          m_subViews.erase(it);
        }
      }

      // 内部元素繪製完成，將坐標移回
      m_pContext->SetTransform(TranslationMatrix(-m_left, -m_top));

      m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
    }

    template <typename T>
    shared_ptr<T> AddSubView(string id, const ConstructorProxy<T> &cp) {
      auto v = App::CreateView<T>(id, cp);
      m_subViews.insert(v);
      return v;
    }

    void RemoveSubView(const WPView &v) {
      m_subViews.erase(v);
    }


    void SetPos(double left, double top, double right, double bottom) {
      m_left = left;
      m_top = top;
      m_right = right;
      m_bottom = bottom;
    }

    DxResource<ID2D1Effect> DrawEffect(REFCLSID effectId, D2DContext &context) {

      auto effect = context.CreateEffect(effectId);
      D2DContext temp;
      temp = m_pContext;
      auto bmpRT = context.CreateCompatibleRenderTarget();
      m_pContext = bmpRT.Query<ID2D1DeviceContext>();

      m_pContext->BeginDraw();
      m_pContext->Clear(D2D1::ColorF(0xffffff, 0.0f));
      DrawSelf();
      m_pContext->EndDraw();

      m_pContext = temp;

      auto bmp = bmpRT.GetResource<ID2D1Bitmap>(&ID2D1BitmapRenderTarget::GetBitmap);
      effect->SetInput(0, bmp.ptr());

      return effect;
    }
  };
}
