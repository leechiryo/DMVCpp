#pragma once

/**
* 一个View类型，作为一个容器存放新的layout，和其中的子view。
*/

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
    Layer(const D2DContext &context, Window * parentWnd): View(context, parentWnd){
      m_leftOffset = 0;
      m_rightOffset = 0;
      m_topOffset = 0;
      m_bottomOffset = 0;
      auto effect = CreateEffect(CLSID_D2D1GaussianBlur, 0);
      effect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 1.0f);
    }

    // 用于XML构造的函数
    Layer(const D2DContext &context, Window * parentWnd, const map<string, wstring> &xmlSettings)
      : Layer(context, parentWnd){
    }

    void Blur(){
      EffectOn();
    }

    void Unblur(){
      EffectOff();
    }

    ~Layer() {
    }

    virtual void DrawSelf() {
    }

  };
}
