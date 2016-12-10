#pragma once

#include "Animation.h"

namespace mvc {
  class AniCaretFlicker : public Animation {

  private:
    DxResource<ID2D1SolidColorBrush> m_pBrush;
    float m_posX;
    float m_posY;

  protected:
    virtual void CreateD2DResource() {
      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x333333));
      m_pBrush->SetOpacity(0.8f);
    }

  public:
    void SetPosition(double x, double y) {
      m_posX = tof(x);
      m_posY = tof(y);
    }

    virtual bool DrawFrame(int frameIdx) {
      // TODO:用30帧(0.5秒)在指定的位置绘制一条竖线。
      m_pContext->DrawLine(Point2D(m_posX, m_posY), Point2D(m_posX, m_posY), m_pBrush.ptr());

      if(frameIdx == 18) return true;
      else return false;
    }
  };
}
