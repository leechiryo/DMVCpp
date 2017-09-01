#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Candle.h"

namespace mvc {
  class Chart : public View<Chart>
  {
  private:
    list<shared_ptr<Candle>> m_candles;
    list<tuple<double, double, double, double>> m_prices;

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Chart(const D2DContext &context, Window * parentWnd): View(context, parentWnd){
      m_leftOffset = 0;
      m_rightOffset = 0;
      m_topOffset = 0;
      m_bottomOffset = 0;
      auto effect = CreateEffect(CLSID_D2D1GaussianBlur, 0);
      effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 1.0f);
    }

    ~Chart() {
    }

    virtual void DrawSelf() {
    }

  };
}
