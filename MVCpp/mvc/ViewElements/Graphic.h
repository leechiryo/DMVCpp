#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  class Graphic : public View<Graphic>
  {
  protected:

    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1SolidColorBrush> m_pBackBrush;
    DxResource<ID2D1SolidColorBrush> m_pBrush;

    UINT32 m_color;
    float m_opacity;
    UINT32 m_backColor;
    float m_backOpacity;
    float m_stroke;

    virtual void CreateD2DResource() {
      m_pBackBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_backColor));
      m_pBackBrush->SetOpacity(m_backOpacity);
      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_color));
      m_pBrush->SetOpacity(m_opacity);
      m_stroke = 1.0f;
    }

  public:

    Graphic() {
      m_color = 0x333333;
      m_backColor = 0xffffff;
      m_opacity = 1.0f;
      m_backOpacity = 0.0f;
    }

    void SetColor(UINT32 color) {
      m_color = color;
      m_pBrush->SetColor(D2D1::ColorF(m_color));
    }

    void SetOpacity(float opacity) {
      m_opacity = opacity;
      m_pBrush->SetOpacity(m_opacity);
    }

    void SetBackColor(UINT32 color) {
      m_backColor = color;
      m_pBackBrush->SetColor(D2D1::ColorF(m_backColor));
      m_opacity = 1.0f;
      m_pBackBrush->SetOpacity(m_opacity);
    }

    void SetBackOpacity(float opacity) {
      m_backOpacity = opacity;
      m_pBackBrush->SetOpacity(m_backOpacity);
    }

    void SetStroke(float stroke) {
      m_stroke = stroke;
    }

    ~Graphic() {
    }

  };
}
