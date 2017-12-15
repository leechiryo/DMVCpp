#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\DataModel\TickPrice.h"
#include "..\..\sqlite3\sqlite3.h"

namespace mvc {

  class TickProvider : public View<TickProvider>
  {
  private:
    shared_ptr<Animation> m_aniUpdateTick;
    sqlite3 *db;

  protected:

    virtual void CreateD2DResource() {
    }

  public:

    ModelRef<vector<TickPrice>> updateTarget;
    ModelRef<int> updateSpeed;

    TickProvider(const D2DContext &context, Window *parentWnd, const char *dbpath) : View(context, parentWnd) {

      updateSpeed = 1;

      // open sqlite3 database
      int rc = sqlite3_open(dbpath, &db);
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

      // 从数据库中读取一次lastTick
      if (sqlite3_step(stmt) == SQLITE_ROW){
        const char * time = (char *)sqlite3_column_text(stmt, 0);
        double ask = sqlite3_column_double(stmt, 1);
        double bid = sqlite3_column_double(stmt, 2);

        TickPrice firstTp{ time, ask, bid };
        TickPrice lastTp{ time, ask, bid };
        TickPrice * ptrFirstTp = &firstTp;
        TickPrice * ptrLastTp = &lastTp;

        // 设置一个动画函数用于更新lastTick
        m_aniUpdateTick = AddAnimation([stmt, ptrFirstTp, ptrLastTp](TickProvider *c, int idx)->bool {

          double timelimit = 1000 * (c->updateSpeed) * idx / 60;
          while (ptrLastTp->GetDateTime() - ptrFirstTp->GetDateTime() <= timelimit){
            c->updateTarget->push_back(*ptrLastTp);

            // 从数据库中读取一次lastTick
            if (sqlite3_step(stmt) == SQLITE_ROW) {
              const char * time = (char *)sqlite3_column_text(stmt, 0);
              double ask = sqlite3_column_double(stmt, 1);
              double bid = sqlite3_column_double(stmt, 2);

              ptrLastTp->Update(time, ask, bid);
            }
            else{
              // 如果无法从数据库中读取新的tick，则停止动画处理
              return true;
            }
          }

          return false;
        });
      }

    }

    ~TickProvider() {
      if (!db){
        sqlite3_close(db);
        db = nullptr;
      }
    }

    virtual void DrawSelf() {
    }

    void Start(){
      if (m_aniUpdateTick){
        m_aniUpdateTick->PlayAndPauseAtStart();
      }
    }
  };
}
