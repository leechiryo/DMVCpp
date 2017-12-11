#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Line.h"
#include "..\DataModel\BarPrice.h"

namespace mvc {
  class Candle : public View<Candle>
  {
  private:
    shared_ptr<Rectangle> m_rect;
    shared_ptr<Line> m_topShadow;
    shared_ptr<Line> m_bottomShadow;

    vector<BarPrice>* m_prices;
    size_t* m_startIndex;
    size_t m_offset;

    double m_oldH;
    double m_oldO;
    double m_oldL;
    double m_oldC;
    double m_oldCalHeight;

  protected:
    virtual void CreateD2DResource() {
    }

  public:

    Candle(const D2DContext &context, Window * parentWnd, vector<BarPrice>* prices, size_t* startIndex, size_t offset) : View(context, parentWnd){
      m_prices = prices;
      m_offset = offset;
      m_startIndex = startIndex;

      m_topShadow = AppendSubView<Line>();
      m_bottomShadow = AppendSubView<Line>();
      m_rect = AppendSubView<Rectangle>();

      m_topShadow->SetWidth("0");
      m_topShadow->SetTopOffset(0);

      m_bottomShadow->SetWidth("0");
      m_bottomShadow->SetBottomOffset(0);

      m_rect->SetLeftOffset(0.5);
      m_rect->SetRightOffset(0.5);

      m_oldH = 0.0;
      m_oldO = 0.0;
      m_oldL = 0.0;
      m_oldC = 0.0;
      m_oldCalHeight = 0.0;
    }

    ~Candle() {
    }

    virtual float GetDefaultWidth() {
      return 5.0f;
    }

    virtual void DrawSelf() {
      if (!m_prices || m_prices->size() <= (*m_startIndex) + m_offset){
        m_topShadow->SetHidden(true);
        m_rect->SetHidden(true);
        m_bottomShadow->SetHidden(true);
        return;
      }
      else{
        m_topShadow->SetHidden(false);
        m_rect->SetHidden(false);
        m_bottomShadow->SetHidden(false);
      }

      BarPrice& bp = m_prices->at((*m_startIndex) + m_offset);

      double h = bp.GetHigh();
      double o = bp.GetOpen();
      double l = bp.GetLow();
      double c = bp.GetClose();

      if (m_oldH != h || m_oldO != o || m_oldL != l || m_oldC != c || m_oldCalHeight != m_calHeight){
        if (h == l || m_calHeight == 0){
          m_topShadow->SetHeight("0");
          m_bottomShadow->SetHeight("0");
          m_rect->SetTopOffset(tof(0));
          m_rect->SetHeight("0");
        }
        else{
          double p1 = max(c, o);
          double p2 = min(c, o);

          double topHeight = (h - p1) * m_calHeight / (h - l);
          double rectHeight = (p1 - p2) * m_calHeight / (h - l);
          double bottomHeight = m_calHeight - topHeight - rectHeight;

          char buf[20] = { 0 };

          sprintf_s(buf, "%f", topHeight);
          m_topShadow->SetHeight(buf);

          sprintf_s(buf, "%f", rectHeight);
          m_rect->SetTopOffset(tof(topHeight));
          m_rect->SetHeight(buf);

          sprintf_s(buf, "%f", bottomHeight);
          m_bottomShadow->SetHeight(buf);

          if (o > c) {
            m_rect->SetBackColor(0xff0000);
          }
          else if (o < c) {
            m_rect->SetBackColor(0x00ff00);
          }
        }

        m_oldH = h;
        m_oldO = o;
        m_oldL = l;
        m_oldC = c;
        m_oldCalHeight = m_calHeight;
      }

    }

  };
}
