#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {

  class Circle : public Graphic
  {
  public:
    Circle(const D2DContext &context) : Graphic(context){}

    virtual void DrawSelf() {
      double width = m_right - m_left;
      double height = m_bottom - m_top;
      double radius = min(width, height) / 2.0;
      double centerX = m_left + width / 2.0;
      double centerY = m_top + height / 2.0;

      D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(tof(centerX), tof(centerY)),
        tof(radius), tof(radius));

      m_pContext->FillEllipse(ellipse, m_pBrush.ptr());
      m_pContext->DrawEllipse(ellipse, m_pBrush.ptr(), m_stroke);
    }
  };
}
