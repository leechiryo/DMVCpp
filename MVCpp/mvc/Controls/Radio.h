#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Text.h"
#include "..\ViewElements\Circle.h"

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

    Radio(const D2DContext &context, Window *parentWnd, int val, wstring ttl) : View(context, parentWnd) {

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

    // 用于XML构造的函数
    Radio(const D2DContext &context, Window *parentWnd, const map<string, wstring> &xmlSettings) 
      : Radio(context, parentWnd, 0, L"") {

      auto it = xmlSettings.find("text");
      if (it != xmlSettings.end()){
        *(m_title->text.SafePtr()) = it->second;
      }

      auto it2 = xmlSettings.find("value");
      if (it2 != xmlSettings.end()){
        m_value = stoi(it2->second);
      }

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
