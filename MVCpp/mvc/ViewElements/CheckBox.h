#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  class CheckBox : public View<CheckBox>
  {
  private:

    // D2D 资源(离开作用域时会自动销毁)
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

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<CheckBox> cbx, WPARAM wParam, LPARAM lParam) {
      if (cbx->checked) cbx->checked = false;
      else cbx->checked = true;
      return 0;
    }

  protected:

    virtual void CreateD2DResource() {

      // 背景设为白色透明
      m_pBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xffffff));
      m_pBackgroundBrush->SetOpacity(0.0f);

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_color));
    }

  public:

    ModelRef<bool> checked;
    ModelRef<wstring> title;

    CheckBox(wstring ttl) : title{ ttl } {
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      checked = true;
    }

    ~CheckBox() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left + 20, m_top, m_right, m_bottom);
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      double vstart = (m_bottom + m_top) / 2 - 5;
      double vend = (m_bottom + m_top) / 2 + 5;

      m_pContext->DrawRectangle(RectD(m_left, vstart, m_left + 10, vend), m_pBrush.ptr(), 2.0f);

      if (checked) {
        m_pContext->DrawLine(Point2D(m_left+2, vstart+5), Point2D(m_left+4.5, vend-2), m_pBrush.ptr(), 2.0f);
        m_pContext->DrawLine(Point2D(m_left+4.5, vend-2), Point2D(m_left+8, vstart+2), m_pBrush.ptr(), 2.0f);
      }

      m_pContext->DrawText(
        title->c_str(),
        title->length(),
        m_pTextFormat.ptr(),
        textRect,
        m_pBrush.ptr());
    }

  };
}
