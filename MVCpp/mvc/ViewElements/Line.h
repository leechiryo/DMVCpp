#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {
  class Line : public Graphic
  {
  public:
    Line(const D2DContext &context) : Graphic(context){
    }

    virtual void DrawSelf() {
      D2D1_POINT_2F p1 = Point2D(m_left, m_top);
      D2D1_POINT_2F p2 = Point2D(m_right, m_bottom);
      m_pContext->DrawLine(p1, p2, m_pBrush.ptr(), m_stroke);
    }
  };
}
