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
      m_circle1->SetLeftOffset(0);
      m_circle1->SetWidth("12");
      m_circle1->SetHeight("12");

      m_circle2->SetBackColor(0x333333);
      m_circle2->SetLeftOffset(3);
      m_circle2->SetWidth("6");
      m_circle2->SetHeight("6");

      m_circle2->SetHidden(true);

      m_title->SetLeftOffset(20);

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      m_value = val;
      selectedValue = 0;
    }

    ~Radio() {
    }

    virtual float GetDefaultWidth() {
      return m_title->GetDefaultWidth() + 20;
    }

    virtual float GetDefaultHeight() {
      auto height = m_title->GetDefaultHeight();
      return height > 12 ? height : 12;
    }

    virtual void DrawSelf() {
      if (selectedValue == m_value){
        m_circle2->SetHidden(false);
      }
      else
      {
        m_circle2->SetHidden(true);
      }
    }
  };
}
