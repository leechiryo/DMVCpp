#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Candle.h"
#include "..\ViewElements\Rectangle.h"
#include "..\DataModel\BarPrice.h"
#include "..\DataModel\TickPrice.h"

namespace mvc {
  class Chart : public View<Chart>
  {
  private:
    vector<shared_ptr<Candle>> m_candles;
    vector<BarPrice> m_prices;
    size_t m_startBarIndex;
    shared_ptr<Rectangle> m_border;
    shared_ptr<Text> m_info;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Chart> cht, WPARAM wParam, LPARAM lParam) {
      if (cht->m_prices.size() > 0 && cht->m_startBarIndex < cht->m_prices.size() - 1){
        cht->m_startBarIndex++;
      }
      return 0;
    }

    static LRESULT Handle_RBUTTONDOWN(shared_ptr<Chart> cht, WPARAM wParam, LPARAM lParam) {
      if (cht->m_startBarIndex > 0){
        cht->m_startBarIndex--;
      }
      return 0;
    }

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<TickPrice> lastTick;

    Chart(const D2DContext &context, Window * parentWnd): View(context, parentWnd){

      // 设置画面的裁剪区域。
      SetInnerClipAreaOffset(0, 0, 0, 0);

      m_info = AppendSubView<Text>(L"");
      m_info->SetOffset(0, 0);

      m_info->text.Link<TickPrice>(lastTick, [](const TickPrice * iptr, wstring& s){
        wchar_t buf[100];
        DateTime tt = iptr->GetDateTime();
        swprintf_s(buf, L"%04d/%02d/%02d %02d:%02d:%02d %d", tt.GetYear(), tt.GetMonth(), tt.GetDay(), tt.GetHour(), tt.GetMinute(), tt.GetSecond(), tt.GetWeekDay());
        s = buf;
      });


      m_startBarIndex = 0;

      // 一个画面最多可以表示一百根蜡烛。
      // 每根蜡烛都有一个指向价格数组的指针，蜡烛图中开始价格的索引，自身在所有蜡烛中的索引。
      for (int i = 0; i < 100; i++){
        shared_ptr<Candle> cdl = AppendSubView<Candle>(&m_prices, &m_startBarIndex, i);

        // 横向的位置由蜡烛的索引决定。
        cdl->SetLeftOffset(tof(10 * i + 5));
        m_candles.push_back(cdl);
      }

      m_border = AppendSubView<Rectangle>();
      m_border->SetOffset(0, 0, 0, 0);

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_RBUTTONDOWN, Handle_RBUTTONDOWN);
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

      // 检查蜡烛图的开始位置是否超出价格数组的边界。
      if (m_prices.size() > 0 && m_startBarIndex > m_prices.size() - 1){
        m_startBarIndex = m_prices.size() - 1;
      }

      // 设置各个蜡烛的纵向位置
      // 纵向位置由最大和最小价格决定以及各蜡烛的价格决定。

      // 算出画面中可以表示的蜡烛数量。
      size_t candleCntInView = static_cast<size_t>((m_calWidth + 5) / 10);
      candleCntInView = candleCntInView > 100 ? 100 : candleCntInView;
      candleCntInView = candleCntInView > (m_prices.size() - m_startBarIndex) ? 
                        (m_prices.size() - m_startBarIndex) : candleCntInView;

      // 算出画面中表示价格的最大值和最小值
      double min = DBL_MAX;
      double max = -DBL_MAX;

      for (size_t i = 0; i < candleCntInView; i++){
        if (m_prices.size() <= m_startBarIndex + i) break;

        double h = m_prices[m_startBarIndex + i].GetHigh();
        double l = m_prices[m_startBarIndex + i].GetLow();

        max = h > max ? h : max;
        min = l < min ? l : min;
      }

      // 根据价格的最大值和最小值以及各个蜡烛的价格所占比例，算出各蜡烛的纵向位置。
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
