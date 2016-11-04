#pragma once

#include <d2d1.h>
#include "Types.h"
#include "App.h"

using namespace std;

namespace mvc {

  class ViewBase {

    template<typename T>
    friend class ModelRef;
    friend class App;

    template<typename T>
    friend class View;


  protected:
    weak_ptr<ViewBase> m_wpThis;
    WPViewSet m_subViews;

    double m_left;
    double m_top;
    double m_right;
    double m_bottom;

    bool m_mouseIn = 0;

    // 指向Window对象的D2DRenderTarget字段的指针。每个Window都有一个独立的D2DRenderTarget对象，
    // 其内部的所有subview将共享这一对象，并利用该对象进行绘制。
    ID2D1HwndRenderTarget** m_ppRenderTarget = nullptr;

    virtual void CreateD2DResource() = 0;
    virtual void DestroyD2DResource() = 0;
    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) = 0;

    virtual void MouseEnter(double x, double y) {
    }

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
        if (spv) spv->CreateD2DEnvironment();
      }
    }

    void DestroyD2DEnvironment() {
      for (auto e : m_subViews) {
        auto spv = e.lock();
        if (spv)spv->DestroyD2DEnvironment();
      }

      DestroyD2DResource();
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
      return dipX >= m_left && dipX <= m_right
        && dipY >= m_top && dipY <= m_bottom;
    }

    bool HitTest(int pixelsX, int pixelsY) {
      double dipX = PixelsToDipsX(pixelsX);
      double dipY = PixelsToDipsY(pixelsY);

      return HitTest(dipX, dipY);
    }

  public:

    ViewBase() { }

    virtual ~ViewBase() { }

    ViewBase(ID2D1HwndRenderTarget** ppRndrTgt, const WPViewSet & views) : m_subViews(views) {
      m_ppRenderTarget = ppRndrTgt;
      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (spv) {
          spv->m_ppRenderTarget = m_ppRenderTarget;
        }
      }
    }

    double left, top, width, height;

    virtual void DrawSelf() = 0;

    void Draw() {
      DrawSelf();

      for (auto it = m_subViews.begin(); it != m_subViews.end(); ++it) {
        const auto &v = *it;
        auto ptr = v.lock();

        if (ptr) {
          ptr->Draw();
        }
        else if (v.expired()) {
          m_subViews.erase(it);
        }
      }
    }

    void AddSubView(const WPView &v) {
      m_subViews.insert(v);
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

  };

}

