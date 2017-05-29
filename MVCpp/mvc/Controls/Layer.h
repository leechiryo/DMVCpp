#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {
  class Layer : public View<Layer>
  {
  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Layer(const D2DContext &context, Window * parentWnd): View(context, parentWnd){
      m_leftOffset = 0;
      m_rightOffset = 0;
      m_topOffset = 0;
      m_bottomOffset = 0;
    }

    ~Layer() {
    }

    virtual void DrawSelf() {
    }

  };
}
