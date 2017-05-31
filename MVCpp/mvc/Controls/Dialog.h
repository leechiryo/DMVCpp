#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

#include "..\ViewElements\Line.h"
#include "..\ViewElements\RoundRectangle.h"
#include "..\Controls\Button.h"

namespace mvc {

  enum class SlideInDir{ fromLeft, fromRight, fromTop, fromBottom };

  class Dialog : public View<Dialog>
  {
  private:


    shared_ptr<RoundRectangle> m_backrect;
    shared_ptr<Button> m_closebtn;
    shared_ptr<Line> m_line1;
    shared_ptr<Line> m_line2;
    shared_ptr<AnimationBase> m_aniSlideInFromLeft;
    shared_ptr<AnimationBase> m_aniSlideInFromRight;
    shared_ptr<AnimationBase> m_aniSlideInFromTop;
    shared_ptr<AnimationBase> m_aniSlideInFromBottom;

    float m_slideStartVal;
    float m_slideEndVal;

    static LRESULT CloseClicked(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      return 0;
    }

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Dialog(const D2DContext &context, Window * parentWnd) : View(context, parentWnd){
      m_backrect = AppendSubView<RoundRectangle>();
      m_backrect->SetOffset(0, 0, 0, 0);
      m_backrect->SetRoundRadius(3, 3);
      m_backrect->SetBackColor(0xeeeeee);

      m_closebtn = AppendSubView<Button>(L"");
      m_closebtn->SetRightOffset(3);
      m_closebtn->SetTopOffset(3);
      m_closebtn->SetWidth("15");
      m_closebtn->SetHeight("15");

      m_line1 = m_closebtn->AppendSubView<Line>();
      m_line1->SetOffset(3, 3, 3, 3);
      m_line2 = m_closebtn->AppendSubView<Line>();
      m_line2->SetOffset(3, 12, 3, 12);

      SetHidden(true);

      auto me = this;
      m_closebtn->AddEventHandler(WM_LBUTTONUP, [me](shared_ptr<Button> btn, WPARAM wp, LPARAM lp)->LRESULT{
        me->Close();
        return 0;
      });

      m_aniSlideInFromLeft = AddAnimation<Dialog>([](Dialog *d, int idx)->bool {
        if (idx >= 7) return true;
        d->SetLeftOffset(d->m_slideStartVal + (d->m_slideEndVal - d->m_slideStartVal)*idx / 6);
        return false;
      });

      m_aniSlideInFromRight = AddAnimation<Dialog>([](Dialog *d, int idx)->bool {
        if (idx >= 7) return true;
        d->SetRightOffset(d->m_slideStartVal + (d->m_slideEndVal - d->m_slideStartVal)*idx / 6);
        return false;
      });

      m_aniSlideInFromTop = AddAnimation<Dialog>([](Dialog *d, int idx)->bool {
        if (idx >= 7) return true;
        d->SetTopOffset(d->m_slideStartVal + (d->m_slideEndVal - d->m_slideStartVal)*idx / 6);
        return false;
      });

      m_aniSlideInFromBottom = AddAnimation<Dialog>([](Dialog *d, int idx)->bool {
        if (idx >= 7) return true;
        d->SetBottomOffset(d->m_slideStartVal + (d->m_slideEndVal - d->m_slideStartVal)*idx / 6);
        return false;
      });
    }

    ~Dialog() {
    }

    void Open(){
      SetHidden(false);
    }

    void SlideIn(SlideInDir dir){
      char buf[10];
      LayoutInfo oldLayout;
      SaveLayout(&oldLayout);
      Dialog *me = this;
      const GridCell * cell = m_parentLayout->GetCell(m_row, m_col);

      switch (dir){
      case SlideInDir::fromLeft:
        m_slideStartVal = -m_calWidth;
        m_slideEndVal = tof(m_left);
        sprintf_s(buf, "%.0f", m_calWidth);
        SetWidth(buf);
        ClearRightOffset();
        SetLeftOffset(-m_calWidth);
        SetHidden(false);
        m_aniSlideInFromLeft->Stop();
        m_aniSlideInFromLeft->PlayAndPauseAtEnd();
        m_aniSlideInFromLeft->OnFinished = [me, oldLayout](){
          me->RestoreLayout(oldLayout);
        };
        break;
      case SlideInDir::fromRight:
        m_slideStartVal = -m_calWidth;
        m_slideEndVal = tof(cell->width - m_right);
        sprintf_s(buf, "%.0f", m_calWidth);
        SetWidth(buf);
        ClearLeftOffset();
        SetRightOffset(-m_calWidth);
        SetHidden(false);
        m_aniSlideInFromRight->Stop();
        m_aniSlideInFromRight->PlayAndPauseAtEnd();
        m_aniSlideInFromRight->OnFinished = [me, oldLayout](){
          me->RestoreLayout(oldLayout);
        };
        break;
      case SlideInDir::fromTop:
        m_slideStartVal = -m_calHeight;
        m_slideEndVal = tof(m_top);
        sprintf_s(buf, "%.0f", m_calHeight);
        SetHeight(buf);
        ClearBottomOffset();
        SetTopOffset(-m_calHeight);
        SetHidden(false);
        m_aniSlideInFromTop->Stop();
        m_aniSlideInFromTop->PlayAndPauseAtEnd();
        m_aniSlideInFromTop->OnFinished = [me, oldLayout](){
          me->RestoreLayout(oldLayout);
        };
        break;
      case SlideInDir::fromBottom:
        m_slideStartVal = -m_calHeight;
        m_slideEndVal = tof(cell->height - m_bottom);
        sprintf_s(buf, "%.0f", m_calHeight);
        SetHeight(buf);
        ClearTopOffset();
        SetBottomOffset(-m_calHeight);
        SetHidden(false);
        m_aniSlideInFromBottom->Stop();
        m_aniSlideInFromBottom->PlayAndPauseAtEnd();
        m_aniSlideInFromBottom->OnFinished = [me, oldLayout](){
          me->RestoreLayout(oldLayout);
        };
        break;
      default:
        return;
      }
    }

    void Close(){
      SetHidden(true);
    }

    virtual void DrawSelf() {
    }

  };
}