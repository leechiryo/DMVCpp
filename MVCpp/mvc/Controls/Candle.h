#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Line.h"

namespace mvc {
  class Candle : public View<Candle>
  {
  private:
    shared_ptr<Rectangle> m_rect;
    shared_ptr<Line> m_topShadow;
    shared_ptr<Line> m_bottomShadow;
  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<double> open;
    ModelRef<double> close;
    ModelRef<double> high;
    ModelRef<double> low;

    Candle(const D2DContext &context, Window * parentWnd, double h, double o, double l, double c): View(context, parentWnd){
      open = o;
      close = c;
      high = h;
      low = l;
      m_topShadow = AppendSubView<Line>();
      m_bottomShadow = AppendSubView<Line>();
      m_rect = AppendSubView<Rectangle>();

      double p1 = max(c, o);
      double p2 = min(c, o);

      double ratio1 = (h - p1) / (h - l);
      double ratio2 = (p1 - p2) / (h - l);

      m_layout.AddRow(tof(ratio1));
      m_layout.AddRow(tof(ratio2));
      m_layout.AddRow("*");
      m_layout.AddCol("*");

      m_topShadow->SetGridPosition(0, 0);
      m_topShadow->SetWidth("0");
      m_topShadow->SetTopOffset(0);
      m_topShadow->SetBottomOffset(0);

      m_rect->SetGridPosition(1, 0);
      m_rect->SetOffset(0.5, 0.5, 0.5, 0.5);

      m_bottomShadow->SetGridPosition(2, 0);
      m_bottomShadow->SetWidth("0");
      m_bottomShadow->SetTopOffset(0);
      m_bottomShadow->SetBottomOffset(0);

      if (o > c) {
        m_rect->SetBackColor(0x00ff00);
      }
      else if (o < c) {
        m_rect->SetBackColor(0xff0000);
      }
    }

    ~Candle() {
    }

    virtual float GetDefaultWidth() {
      return 5.0f;
    }

    virtual void DrawSelf() {
    }

  };
}
