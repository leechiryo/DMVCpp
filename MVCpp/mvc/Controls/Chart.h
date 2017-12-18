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
    vector<BarPrice> m_bars;
    size_t m_startBarIndex;
    shared_ptr<Rectangle> m_border;
    shared_ptr<Text> m_info;
    ModelSafePtr<wstring> m_infoText;
    shared_ptr<Line> m_line;
    shared_ptr<Text> m_lastPrice;
    ModelSafePtr<wstring> m_lastPriceText;
    static const int RIGHT_MARGIN = 80;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Chart> cht, WPARAM wParam, LPARAM lParam) {
      if (cht->m_bars.size() > 0 && cht->m_startBarIndex < cht->m_bars.size() - 1){
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
    ModelRef<vector<TickPrice>> ticks;
    ModelRef<TimeFrame> timeFrame;

    Chart(const D2DContext &context, Window * parentWnd) : View(context, parentWnd){

      m_infoText = App::CreateModel<wstring>({});
      m_lastPriceText = App::CreateModel<wstring>({});

      // 设置画面的裁剪区域。
      SetInnerClipAreaOffset(0, 0, 0, 0);

      m_info = AppendSubView<Text>(L"");
      m_info->SetOffset(0, 0);
      m_info->text.Bind(m_infoText);

      m_startBarIndex = 0;

      // 一个画面最多可以表示一百根蜡烛。
      // 每根蜡烛都有一个指向价格数组的指针，蜡烛图中开始价格的索引，自身在所有蜡烛中的索引。
      for (int i = 0; i < 100; i++){
        shared_ptr<Candle> cdl = AppendSubView<Candle>(&m_bars, &m_startBarIndex, i);

        // 横向的位置由蜡烛的索引决定。
        cdl->SetLeftOffset(tof(10 * i + 5));
        m_candles.push_back(cdl);
      }

      m_border = AppendSubView<Rectangle>();
      m_border->SetOffset(0, 0, tof(RIGHT_MARGIN), 0);

      m_line = AppendSubView<Line>();
      m_line->SetLeftOffset(0);
      m_line->SetRightOffset(tof(RIGHT_MARGIN));
      m_line->SetHeight("0");
      m_line->SetColor(0xcccccc);

      m_lastPrice = AppendSubView<Text>(L"");
      m_lastPrice->SetRightOffset(0);
      m_lastPrice->text.Bind(m_lastPriceText);

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_RBUTTONDOWN, Handle_RBUTTONDOWN);
    }

    void AddBar(const BarPrice & bp){
      m_bars.push_back(bp);
    }

    void SetStartIndex(size_t idx){
      m_startBarIndex = idx;
    }

    ~Chart() {
    }

    virtual void DrawSelf() {

      // 根据 ticks 更新价格数组
      if (ticks->size() > 0){
        for (auto & tick : *(ticks.SafePtr())){
          if (m_bars.size() > 0 && m_bars[m_bars.size() - 1].GetDateTime().SameTime(tick.GetDateTime(), timeFrame)){
            m_bars[m_bars.size() - 1].UpdateTick(tick);
          }
          else if (tick.GetBid() != 0 && tick.GetAsk() != 0){
            BarPrice bp{ tick };
            m_bars.push_back(bp);
          }
        }

        wchar_t * weekDays[7] = { L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat" };
        wchar_t buf[100];
        if (ticks->size() > 0){
          auto p = ticks->back();
          DateTime tt = p.GetDateTime();
          swprintf_s(buf, L"%04d/%02d/%02d %02d:%02d:%02d %s", tt.GetYear(), tt.GetMonth(), tt.GetDay(), tt.GetHour(), tt.GetMinute(), tt.GetSecond(), weekDays[tt.GetWeekDay()]);
          *m_infoText = buf;
        }

        ticks->clear();
      }

      // 检查蜡烛图的开始位置是否超出价格数组的边界。
      if (m_bars.size() > 0 && m_startBarIndex > m_bars.size() - 1){
        m_startBarIndex = m_bars.size() - 1;
      }

      // 设置各个蜡烛的纵向位置
      // 纵向位置由最大和最小价格决定以及各蜡烛的价格决定。

      // 算出画面中可以表示的蜡烛数量。
      size_t candleCntInView = static_cast<size_t>((m_calWidth + 5) / 10);
      candleCntInView = candleCntInView > 100 ? 100 : candleCntInView;
      candleCntInView = candleCntInView > (m_bars.size() - m_startBarIndex) ?
        (m_bars.size() - m_startBarIndex) : candleCntInView;

      // 算出画面中表示价格的最大值和最小值
      double min = DBL_MAX;
      double max = -DBL_MAX;

      for (size_t i = 0; i < candleCntInView; i++){
        if (m_bars.size() <= m_startBarIndex + i) break;

        double h = m_bars[m_startBarIndex + i].GetHigh();
        double l = m_bars[m_startBarIndex + i].GetLow();

        max = h > max ? h : max;
        min = l < min ? l : min;
      }

      // 根据价格的最大值和最小值以及各个蜡烛的价格所占比例，算出各蜡烛的纵向位置。
      for (size_t i = 0; i < candleCntInView; i++){
        double h = m_bars[m_startBarIndex + i].GetHigh();
        double l = m_bars[m_startBarIndex + i].GetLow();

        if (max > min){
          double topoffset = (max - h) * m_calHeight / (max - min);
          m_candles[i]->SetTopOffset(tof(topoffset));
          double ratio = (h - l) / (max - min);
          char bufHeight[10];
          sprintf_s(bufHeight, "%.4f%%", ratio * 100);
          m_candles[i]->SetHeight(bufHeight);
        }
        else{
          m_candles[i]->SetTopOffset(tof(m_calHeight / 2));
          m_candles[i]->SetHeight("0");
        }
      }

      // 设定当前价格的水平线
      if (m_bars.size() > 0){
        double lastPrice = m_bars.back().GetClose();
        double topoffset = (max - lastPrice) * m_calHeight / (max - min);
        *m_lastPriceText = std::to_wstring(lastPrice);
        m_line->SetTopOffset(tof(topoffset));
        m_lastPrice->SetTopOffset(tof(topoffset - 10));
      }

    }

    void Reset(){
      m_bars.clear();
    }
  };
}
