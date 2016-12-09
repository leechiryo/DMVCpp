#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"

namespace mvc{
  class TextBox : public View<TextBox>{

  private:
    DxResource<ID2D1SolidColorBrush> m_pBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pTextBrush;
    DxResource<ID2D1SolidColorBrush> m_pBorderBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;


    static LRESULT Handle_LBUTTONDOWN(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      // 设置Focus，改变边框的样式。
      return 0;
    }

  protected:
    virtual void MouseEnter(double x, double y) {
      // 变更鼠标图标
    }

    virtual void MouseLeft(double x, double y) {
      // 变更鼠标图标
    }

    virtual void CreateD2DResource() {

      m_pBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xeeeeee));
      m_pTextBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x333333));
      m_pBorderBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x555555));

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    virtual void DestroyD2DResource() {
    }

  public:

    ModelRef<wstring> text;

    TextBox(wstring text) : text{ text }{
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
    }

    ~TextBox() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());
      m_pContext->DrawRectangle(textRect, m_pBorderBrush.ptr());

      textRect.left += 10;

      m_pContext->DrawText(
        text->c_str(),
        text->length(),
        m_pTextFormat.ptr(),
        textRect,
        m_pTextBrush.ptr());
    }
  };
}