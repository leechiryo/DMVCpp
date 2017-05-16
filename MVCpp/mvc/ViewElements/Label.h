#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "Text.h"

namespace mvc {
  class Label : public View<Label>
  {
  private:
    shared_ptr<Text> m_vtext;
  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Label(const D2DContext &context, wstring ttl): View(context){
      m_layout.AddRow("*");
      m_layout.AddCol("*");
      m_vtext = AppendSubView<Text>(ttl);
      m_vtext->SetGridPosition(0, 0);
      m_vtext->SetOffset(0, 0);

      text = &(m_vtext->text);
      // 如果要設置padding，可以在這裏設置内部對象的位置
      //m_vtext->SetPos(5.0, 5.0, 5.0, 5.0);
    }

    ~Label() {
    }

    virtual float GetDefaultWidth() {
      return m_vtext->GetDefaultWidth();
    }

    virtual float GetDefaultHeight() {
      return m_vtext->GetDefaultHeight();
    }

    virtual void DrawSelf() {
    }

  };
}
