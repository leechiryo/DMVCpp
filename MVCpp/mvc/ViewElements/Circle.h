#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Graphic.h"

namespace mvc {

  class Circle : public Graphic<Circle>
  {
  private:
    float m_centerX = NAN;
    float m_centerY = NAN;
    float m_radius = NAN;

    bool CirclePositionSet(){
      return !isnan(m_centerX) && !isnan(m_centerY) && !isnan(m_radius);
    }

  public:
    Circle(const D2DContext &context, Window *parentWnd) : Graphic(context, parentWnd){}

    void SetCirclePosition(float centerX, float centerY, float radius){
      m_centerX = centerX;
      m_centerY = centerY;
      m_radius = radius;
    }

    void SetCircleRadius(float radius) {
      m_radius = radius;
    }

    virtual void DrawSelf() {
      float centerX, centerY, radius;

      if (CirclePositionSet()){
        centerX = m_centerX;
        centerY = m_centerY;
        radius = m_radius;
      }
      else{
        double width = m_right - m_left;
        double height = m_bottom - m_top;
        radius = tof(min(width, height) / 2.0);
        centerX = tof(m_left + width / 2.0);
        centerY = tof(m_top + height / 2.0);
      }

      D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(centerX, centerY),
        radius, radius);

      m_pContext->FillEllipse(ellipse, m_pBackBrush.ptr());
      m_pContext->DrawEllipse(ellipse, m_pBrush.ptr(), m_stroke);
    }
  };
}
