#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Text.h"
#include "Rectangle.h"
#include "Line.h"

namespace mvc {

  class CheckBox : public View<CheckBox>
  {
  private:

    shared_ptr<Text> m_title;
    shared_ptr<Rectangle> m_rect;
    shared_ptr<Line> m_line1;
    shared_ptr<Line> m_line2;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<CheckBox> cbx, WPARAM wParam, LPARAM lParam) {
      if (cbx->checked) {
        cbx->checked = false;
        cbx->m_line1->SetHidden(true);
        cbx->m_line2->SetHidden(true);
      }
      else {
        cbx->checked = true;
        cbx->m_line1->SetHidden(false);
        cbx->m_line2->SetHidden(false);
      }
      return 0;
    }

  protected:

    virtual void CreateD2DResource() {
    }

  public:

    ModelRef<bool> checked;

    CheckBox(const D2DContext &context, wstring ttl) : View(context) {

      m_layout.AddRow("*");
      m_layout.AddCol("*");

      m_rect = AppendSubView<Rectangle>();
      m_title = AppendSubView<Text>(ttl);
      m_line1 = m_rect->AppendSubView<Line>();
      m_line2 = m_rect->AppendSubView<Line>();

      m_rect->AddLayoutRow("*");
      m_rect->AddLayoutCol("*");
      m_rect->SetStroke(2.0f);
      m_rect->SetLeftOffset(0);
      m_rect->SetWidth("12");
      m_rect->SetHeight("12");
      
      m_line1->SetStroke(2.0f);
      m_line2->SetStroke(2.0f);
      m_line1->SetOffset(2, 5, 6.5, 2);
      m_line2->SetOffset(5.5, 10, 2, 10);

      m_title->SetLeftOffset(20);

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      checked = true;
    }

    ~CheckBox() {
    }

    virtual float GetDefaultWidth() {
      return m_title->GetDefaultWidth() + 20;
    }

    virtual float GetDefaultHeight() {
      auto height = m_title->GetDefaultHeight();
      return height > 12 ? height : 12;
    }

    virtual void DrawSelf() {
    }

  };
}
