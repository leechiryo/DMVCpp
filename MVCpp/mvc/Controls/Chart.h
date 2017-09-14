#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Candle.h"
#include "..\ViewElements\Rectangle.h"
#include "..\DataModel\BarPrice.h"

namespace mvc {
  class Chart : public View<Chart>
  {
  private:
    vector<shared_ptr<Candle>> m_candles;
    vector<BarPrice> m_prices;
    size_t m_startBarIndex;
    shared_ptr<Rectangle> m_border;

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Chart(const D2DContext &context, Window * parentWnd): View(context, parentWnd){

      m_startBarIndex = 0;

      for (int i = 0; i < 100; i++){
        shared_ptr<Candle> cdl = AppendSubView<Candle>(&m_prices, &m_startBarIndex, i);
        m_candles.push_back(cdl);
      }

      m_border = AppendSubView<Rectangle>();
      m_border->SetOffset(0, 0, 0, 0);
    }

    void AddBar(const BarPrice & bp){
      m_prices.push_back(bp);
    }

    void SetStartIndex(size_t idx){
      m_startBarIndex = idx;
    }

    ~Chart() {
    }

    virtual void DrawSelf() {

      // 更新所有的蜡烛位置。
      // 横向的位置由蜡烛的索引决定。
      for (size_t i = 0; i < m_candles.size(); i++){
        m_candles[i]->SetLeftOffset(tof(10 * i + 5));
      }

      // 纵向位置由最大和最小价格决定以及各蜡烛的价格决定。

      // 算出画面中可以表示的蜡烛数量。
      size_t candleCntInView = static_cast<size_t>((m_calWidth + 5) / 10);
      candleCntInView = candleCntInView > 100 ? 100 : candleCntInView;
      candleCntInView = candleCntInView > (m_prices.size() - m_startBarIndex) ? 
                        (m_prices.size() - m_startBarIndex) : candleCntInView;

      double min = DBL_MAX;
      double max = -DBL_MAX;


      for (size_t i = 0; i < candleCntInView; i++){
        if (m_prices.size() <= m_startBarIndex + i) break;

        double h = m_prices[m_startBarIndex + i].GetHigh();
        double l = m_prices[m_startBarIndex + i].GetLow();

        max = h > max ? h : max;
        min = l < min ? l : min;
      }

      for (size_t i = 0; i < candleCntInView; i++){
        double h = m_prices[m_startBarIndex + i].GetHigh();
        double l = m_prices[m_startBarIndex + i].GetLow();
        double topoffset = (max - h) * m_calHeight / (max - min);
        m_candles[i]->SetTopOffset(tof(topoffset));
        double ratio = (h - l) / (max - min);
        char bufHeight[10];
        sprintf_s(bufHeight, "%.4f%%", ratio * 100);
        m_candles[i]->SetHeight(bufHeight);
      }

    }

  };
}
