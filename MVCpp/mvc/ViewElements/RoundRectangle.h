#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {

  class RoundRectangle : public Graphic
  {
  private:
    float m_radiusX;
    float m_radiusY;

  public:

    RoundRectangle(const D2DContext &context, Window *parentWnd) :Graphic(context, parentWnd){}

    void SetRoundRadius(float radiusX, float radiusY) {
      m_radiusX = radiusX;
      m_radiusY = radiusY;
    }

    virtual void DrawSelf() {
      D2D1_ROUNDED_RECT rect;
      rect.rect.left = tof(m_left);
      rect.rect.right = tof(m_right);
      rect.rect.top = tof(m_top);
      rect.rect.bottom = tof(m_bottom);

      rect.radiusX = m_radiusX;
      rect.radiusY = m_radiusY;

      m_pContext->FillRoundedRectangle(rect, m_pBackBrush.ptr());
      m_pContext->DrawRoundedRectangle(rect, m_pBrush.ptr(), m_stroke);
    }
  };
}
