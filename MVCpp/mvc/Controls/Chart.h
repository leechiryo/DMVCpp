#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Candle.h"
#include "..\ViewElements\Rectangle.h"
#include "..\DataModel\BarPrice.h"
#include "..\DataModel\TickPrice.h"
#include "sqlite3\sqlite3.h"

namespace mvc {

  class Chart : public View<Chart>
  {
  private:
    vector<shared_ptr<Candle>> m_candles;
    vector<BarPrice> m_prices;
    size_t m_startBarIndex;
    shared_ptr<Rectangle> m_border;
    shared_ptr<Text> m_info;

    shared_ptr<Animation> m_aniUpdateTick;
    sqlite3 *db;

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
    ModelRef<TimeFrame> timeFrame;

    Chart(const D2DContext &context, Window * parentWnd): View(context, parentWnd){

      // 设置画面的裁剪区域。
      SetInnerClipAreaOffset(0, 0, 0, 0);

      m_info = AppendSubView<Text>(L"");
      m_info->SetOffset(0, 0);

      m_info->text.Link<TickPrice>(lastTick, [](ModelRef<TickPrice> * iptr, wstring& s){
        wchar_t * weekDays[7] = {L"Sun", L"Mon", L"Tue", L"Wed", L"Thu", L"Fri", L"Sat"};
        wchar_t buf[100];
        
        DateTime tt = iptr->SafePtr()->GetDateTime();
        swprintf_s(buf, L"%04d/%02d/%02d %02d:%02d:%02d %s", tt.GetYear(), tt.GetMonth(), tt.GetDay(), tt.GetHour(), tt.GetMinute(), tt.GetSecond(), weekDays[tt.GetWeekDay()]);
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

      // open sqlite3 database
      int rc = sqlite3_open("data.db", &db);
      if (rc){
        MessageBox(NULL, L"Error opening SQLite3 database.", L"ERROR", MB_OK);
        return;
      }

      sqlite3_stmt * stmt = nullptr;

      rc = sqlite3_prepare_v2(db, "select time, ask, bid from ticks;", -1, &stmt, nullptr);
      if (rc){
        MessageBox(NULL, L"Error executing sql.", L"ERROR", MB_OK);
        return;
      }

      // 设置一个动画函数用于更新lastTick
      auto * pTick = &lastTick;
      m_aniUpdateTick = AddAnimation([stmt, pTick](Chart *c, int idx)->bool {

        if (idx > 1000) return true;

        // 每10帧(0.16秒)从数据库读取一次价格信息
        if (idx % 10 != 0) return false;

        // 在动画的每一帧，从数据库中读取一次lastTick
        if (sqlite3_step(stmt) == SQLITE_ROW){
          const char * time = (char *)sqlite3_column_text(stmt, 0);
          double ask = sqlite3_column_double(stmt, 1);
          double bid = sqlite3_column_double(stmt, 2);

          pTick->SafePtr()->Update(time, ask, bid);
          return false;
        }
        else{
          // 如果无法从数据库中读取新的tick，则停止动画处理
          return true;
        }

      });

    }

    void AddBar(const BarPrice & bp){
      m_prices.push_back(bp);
    }

    void SetStartIndex(size_t idx){
      m_startBarIndex = idx;
    }

    void AutoPlay(){
      m_aniUpdateTick->PlayAndPauseAtStart();
    }

    ~Chart() {
      if (!db){
        sqlite3_close(db);
        db = nullptr;
      }
    }

    virtual void DrawSelf() {

      // 根据 lastTick 更新价格数组
      if (m_prices.size() > 0 && m_prices[m_prices.size() - 1].GetDateTime().SameTime(lastTick->GetDateTime(), timeFrame)){
        m_prices[m_prices.size() - 1].UpdateTick(lastTick);
      }
      else if(lastTick->GetBid() != 0 && lastTick->GetAsk() != 0){
        BarPrice bp{ lastTick };
        m_prices.push_back(bp);
      }

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

    }

  };
}
