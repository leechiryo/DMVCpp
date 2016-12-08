#pragma once

#include "Animation.h"

namespace mvc {
  class AniButtonPressed : public Animation {

  private:
    ID2D1SolidColorBrush* m_pBrush;
    float m_clickX;
    float m_clickY;

  protected:
    virtual void CreateD2DResource() {
      HRESULT hr = m_pContext->CreateSolidColorBrush(
        D2D1::ColorF(0xcccccc),
        &m_pBrush);

      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to create the background brush.");
      }

      m_pBrush->SetOpacity(0.2f);
    }

    virtual void DestroyD2DResource() {
      SafeRelease(m_pBrush);
    }
  public:
    void SetCenter(double x, double y) {
      m_clickX = tof(x);
      m_clickY = tof(y);
    }

    virtual bool DrawFrame(int frameIdx) {
      // TODO:用6帧(0.1秒)在点击的位置作出一个半透明的圆形，逐渐填充到整个区域。
      double maxWidth = max(m_clickX - m_left, m_right - m_clickX);
      double maxHeight = max(m_clickY - m_top, m_bottom - m_clickY);
      double max_radius = sqrt(maxWidth * maxWidth + maxHeight * maxHeight);
      double radius = max_radius * frameIdx / 6.0;

      D2D1_ELLIPSE ellipse = D2D1::Ellipse(
          D2D1::Point2F(m_clickX, m_clickY),
          tof(radius), tof(radius));

      m_pContext->PushAxisAlignedClip(RectD(m_left, m_top, m_right, m_bottom), D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
      m_pContext->FillEllipse(ellipse, m_pBrush);
      m_pContext->PopAxisAlignedClip();

      if(frameIdx == 6) return true;
      else return false;
    }
  };
}
