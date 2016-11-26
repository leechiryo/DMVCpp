#pragma once

#include "Animation.h"

namespace mvc {
  class AniButtonPressed : Animation {

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
      // TODO:用10帧在点击的位置作出一个圆形，逐渐填充到整个区域。
      return false;
    }
  };
}
