#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

#include "..\ViewElements\Line.h"
#include "..\ViewElements\RoundRectangle.h"
#include "..\Controls\Button.h"

namespace mvc {
  class Dialog : public View<Dialog>
  {
  private:

    shared_ptr<RoundRectangle> m_backrect;
    shared_ptr<Button> m_closebtn;
    shared_ptr<Line> m_line1;
    shared_ptr<Line> m_line2;

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Dialog(const D2DContext &context, Window * parentWnd): View(context, parentWnd){
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

      SetHidden(false);
    }

    ~Dialog() {
    }

    void Show(){
      SetHidden(true);
    }

    virtual void DrawSelf() {
    }

  };
}
