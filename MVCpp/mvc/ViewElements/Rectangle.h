#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {

  class Rectangle : public Graphic
  {
  public:
    virtual void DrawSelf() {
      m_pContext->FillRectangle(RectD(m_left, m_top, m_right, m_top), m_pBackBrush.ptr());
      m_pContext->DrawRectangle(RectD(m_left, m_top, m_right, m_top), m_pBrush.ptr(), m_stroke);
    }
  };
}
