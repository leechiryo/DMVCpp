#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Text.h"
#include "Circle.h"

namespace mvc {

  class Radio : public View<Radio>
  {
  private:

    int m_value;
    shared_ptr<Text> m_title;
    shared_ptr<Circle> m_circle1;
    shared_ptr<Circle> m_circle2;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Radio> rdo, WPARAM wParam, LPARAM lParam) {
      rdo->selectedValue = rdo->m_value;
      return 0;
    }

  protected:

    virtual void CreateD2DResource() {
    }

  public:

    ModelRef<int> selectedValue;

    Radio(const D2DContext &context, int val, wstring ttl) : View(context) {
      m_layout.AddRow("*");
      m_layout.AddCol("*");

      m_circle1 = AppendSubView<Circle>();
      m_circle2 = AppendSubView<Circle>();
      m_title = AppendSubView<Text>(ttl);

      m_circle1->SetStroke(2.0f);
      m_circle2->SetBackColor(0x333333);
      m_circle1->SetOffset(0, 0);
      m_circle1->SetWidth("10");
      m_circle1->SetHeight("10");
      m_circle2->SetOffset(3, 3);
      m_circle2->SetWidth("5");
      m_circle2->SetHeight("5");

      m_title->SetLeftOffset(15);

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      m_value = val;
      selectedValue = 0;
    }

    ~Radio() {
    }

    virtual void DrawSelf() {
    }
  };
}
