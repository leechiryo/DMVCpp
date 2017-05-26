#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {

  class Rectangle : public Graphic
  {
  public:
    Rectangle(const D2DContext &context, Window * parentWnd) :Graphic(context, parentWnd){
    }

    virtual void DrawSelf() {
      m_pContext->FillRectangle(RectD(m_left, m_top, m_right, m_bottom), m_pBackBrush.ptr());

      if (m_stroke > 0.0f){
        m_pContext->DrawRectangle(RectD(m_left, m_top, m_right, m_bottom), m_pBrush.ptr(), m_stroke);
      }
    }
  };
}
