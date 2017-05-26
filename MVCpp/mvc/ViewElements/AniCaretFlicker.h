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
      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x666666));
    }

  public:
    AniCaretFlicker(const D2DContext &context, Window *parentWnd) : Animation(context, parentWnd){
    }

    void SetCaretPos(double x, double y) {
      m_posX = tof(x);
      m_posY = tof(y);
    }

    virtual bool DrawFrame(int frameIdx) {
      // TODO:用60帧(1秒)在指定的位置绘制一条竖线。
      if (frameIdx <= 48)
      {
        if (frameIdx <= 8) {
          // 前8帧淡入效果
          m_pBrush->SetOpacity(frameIdx / 8.0f);
        }
        else if (frameIdx > 40) {
          // 后8帧淡出效果
          m_pBrush->SetOpacity((48 - frameIdx) / 8.0f);
        }
        else {
          // 中间24帧完全显示
          m_pBrush->SetOpacity(1.0f);
        }

        m_pContext->DrawLine(
          Point2D(m_left+m_posX, m_top+m_posY-20), 
          Point2D(m_left + m_posX, m_top+m_posY), 
          m_pBrush.ptr(),
          2.0f);
      }

      if (frameIdx == 72) return true;
      else return false;
    }
  };
}
