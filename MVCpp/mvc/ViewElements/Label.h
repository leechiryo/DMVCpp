#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {
  class Label : public View<Label>
  {
  private:

    // D2D 资源(离开作用域是会自动销毁)
    DxResource<ID2D1SolidColorBrush> m_pBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

  protected:

    virtual void CreateD2DResource() {

      // 背景设为白色透明
      m_pBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xffffff));
      m_pBackgroundBrush->SetOpacity(0.0f);

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_color));
    }

  public:

    ModelRef<wstring> text;

    Label(wstring ttl) : text{ ttl }{
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;
    }

    ~Label() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      m_pContext->DrawText(
        text->c_str(),
        text->length(),
        m_pTextFormat.ptr(),
        textRect,
        m_pBrush.ptr());
    }

  };
}
