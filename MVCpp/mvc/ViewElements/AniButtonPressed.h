#pragma once

#include "Animation.h"

namespace mvc {
  class AniButtonPressed : public Animation {

  private:
    ID2D1SolidColorBrush* m_pBrush;
  protected:
    virtual void CreateD2DResource() {
      HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0xcccccc),
        &m_pBrush);

      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to create the background brush.");
      }

    }

    virtual void DestroyD2DResource() {
      SafeRelease(m_pBrush);
    }
  public:
    virtual bool DrawFrame(int frameIdx) {
      // TODO:用10帧(1/6秒)在点击的位置作出一个圆形，逐渐填充到整个区域。
      double width = m_right - m_left;
      double height = m_bottom - m_top;
      double circle_x = m_left + width / 2.0;
      double circle_y = m_top + height / 2.0;
      double max_radius = sqrt(width * width + height * height) / 2.0;
      double radius = max_radius * frameIdx / 10.0;

      D2D1_ELLIPSE ellipse = D2D1::Ellipse(
          D2D1::Point2F(circle_x, circle_y),
          radius, radius );

      m_pRenderTarget->FillEllipse(ellipse, m_pBrush);

      if(frameIdx == 10) return true;
      else return false;
    }
  };
}
