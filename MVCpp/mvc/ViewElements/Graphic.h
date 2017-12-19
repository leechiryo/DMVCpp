#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  template<typename DerivedType>
  class Graphic : public View<DerivedType>
  {
  protected:

    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1SolidColorBrush> m_pBackBrush;
    DxResource<ID2D1SolidColorBrush> m_pBrush;
    DxResource<ID2D1StrokeStyle> m_pStrokeStyle;

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

    Graphic(const D2DContext &context, Window *parentWnd) : View(context, parentWnd) {
      m_color = 0x333333;
      m_backColor = 0xffffff;
      m_opacity = 1.0f;
      m_backOpacity = 0.0f;
    }

    void SetColor(UINT32 color) {
      m_color = color;
      if (m_pBrush.ptr()){
        m_pBrush->SetColor(D2D1::ColorF(m_color));
      }
    }

    void SetOpacity(float opacity) {
      m_opacity = opacity;
      if (m_pBrush.ptr()){
        m_pBrush->SetOpacity(m_opacity);
      }
    }

    void SetBackColor(UINT32 color) {
      m_backColor = color;
      m_opacity = 1.0f;
      if (m_pBackBrush.ptr()){
        m_pBackBrush->SetColor(D2D1::ColorF(m_backColor));
        m_pBackBrush->SetOpacity(m_opacity);
      }
    }

    void SetBackOpacity(float opacity) {
      m_backOpacity = opacity;
      if (m_pBackBrush.ptr()){
        m_pBackBrush->SetOpacity(m_backOpacity);
      }
    }

    void SetStroke(float stroke) {
      m_stroke = stroke;
    }

    typedef HRESULT(ID2D1Factory1::*CreateStrokeStyle_TYPE)(const D2D1_STROKE_STYLE_PROPERTIES&, const FLOAT*, UINT, ID2D1StrokeStyle**);
    void SetStrokeStyle(const D2D1_STROKE_STYLE_PROPERTIES &ss, const FLOAT *dashes, UINT dashesCount){
      m_pStrokeStyle = App::s_pDirect2dFactory.GetResource<ID2D1StrokeStyle, CreateStrokeStyle_TYPE>(&ID2D1Factory1::CreateStrokeStyle, ss, dashes, dashesCount);
    }

    ~Graphic() {
    }

  };
}
