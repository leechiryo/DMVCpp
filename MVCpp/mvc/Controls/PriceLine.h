#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Line.h"
#include "..\DataModel\BarPrice.h"

namespace mvc {

  enum class PriceType { Entry, StopLoss, TakeProfit };

  class PriceLine : public View<PriceLine>
  {
  private:
    shared_ptr<Line> m_priceLine;
    shared_ptr<Text> m_label;

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    const OrderInfo orderInfo;
    PriceType priceType;


    PriceLine(const D2DContext &context, Window * parentWnd, PriceType type, const OrderInfo & oi) : View(context, parentWnd), orderInfo{ oi } {
      priceType = type;
      m_priceLine = AppendSubView<Line>();
      m_priceLine->SetLeftOffset(0.0f);
      m_priceLine->SetRightOffset(0.0f);
      m_priceLine->SetTopOffset(5.0f);
      m_priceLine->SetBottomOffset(5.0f);
      m_priceLine->SetStrokeStyle(
        D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_FLAT,
          D2D1_CAP_STYLE_FLAT,
          D2D1_LINE_JOIN_MITER,
          10.0f,
          D2D1_DASH_STYLE_DASH,
          0.0f),
        nullptr, 0);

      m_label = AppendSubView<Text>(L"");
      m_label->SetLeftOffset(20.0f);

    }

    ~PriceLine() {
    }

    virtual float GetDefaultHeight() {
      return 10.0f;
    }

    virtual void DrawSelf() {

      auto & labelTxt = *(m_label->text.SafePtr());
      wchar_t buf[100];

      switch (priceType) {

      case PriceType::Entry:
        m_priceLine->SetColor(0x123456);

        if (orderInfo.direction == OrderDirection::Buy) {
          labelTxt = L"BUY";
        }
        else if (orderInfo.direction == OrderDirection::Sell) {
          labelTxt = L"SELL";
        }
        break;

      case PriceType::StopLoss:
        if (orderInfo.stop == 0.0) {
          break;
        }

        m_priceLine->SetColor(0xff3456);
        swprintf_s(buf, L"%f", abs(orderInfo.stop - orderInfo.open));
        labelTxt = buf;
        break;

      case PriceType::TakeProfit:
        if (orderInfo.limit == 0.0) {
          break;
        }

        m_priceLine->SetColor(0x12ff56);
        swprintf_s(buf, L"%f", abs(orderInfo.limit - orderInfo.open));
        labelTxt = buf;
        break;
      }
    }

  };
}
