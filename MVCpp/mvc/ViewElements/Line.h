#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {
  class Line : public Graphic<Line>
  {
  private:

  public:
    Line(const D2DContext &context, Window * parentWnd) : Graphic(context, parentWnd){
    }

    // 用于XML构造的函数
    Line(const D2DContext &context, Window * parentWnd, const map<string, wstring> &xmlSettings) 
      : Line(context, parentWnd){
    }

    virtual void DrawSelf() {
      D2D1_POINT_2F p1 = Point2D(m_left, m_top);
      D2D1_POINT_2F p2 = Point2D(m_right, m_bottom);
      m_pContext->DrawLine(p1, p2, m_pBrush.ptr(), m_stroke, m_pStrokeStyle.ptr());
    }
  };
}
