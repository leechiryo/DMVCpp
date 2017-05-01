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
    Label(wstring ttl){
      m_vtext = make_shared<Text>(ttl);
      m_subViews.insert(m_vtext);
      text = &(m_vtext->text);
      // 如果要設置padding，可以在這裏設置内部對象的位置
      //m_vtext->SetPos(5.0, 5.0, 5.0, 5.0);
    }

    ~Label() {
    }

    virtual void DrawSelf() {
    }

  };
}
