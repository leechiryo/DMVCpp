#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {
  class Text : public View<Text>
  {
  private:

    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1SolidColorBrush> m_pBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    void ResetTextFormat(){
      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

  protected:

    virtual void CreateD2DResource() {
      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_color));
    }

  public:

    ModelRef<wstring> text;

    void SetFontName(const WCHAR *fontName){
      wcscpy_s(m_font, MAX_CHARS + 1, fontName);
      ResetTextFormat();
    }

    void SetFontSize(float fontSize){
      m_fontSize = fontSize;
      ResetTextFormat();
    }

    void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight){
      m_fontWeight = fontWeight;
      ResetTextFormat();
    }

    void SetFontStyle(DWRITE_FONT_STYLE fontStyle){
      m_fontStyle = fontStyle;
      ResetTextFormat();
    }

    void SetFontStretch(DWRITE_FONT_STRETCH fontStretch){
      m_fontStretch = fontStretch;
      ResetTextFormat();
    }

    void SetColor(unsigned color){
      m_color = color;
      m_pBrush->SetColor(D2D1::ColorF(m_color));
    }

    Text(const D2DContext &context, wstring ttl) : View(context), text{ ttl }{
      m_left = 0.0f;
      m_right = 0.0f;
      m_top = 0.0f;
      m_bottom = 0.0f;
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;
    }

    float GetSubstrWidth(int length) {
      D2D1_RECT_F textRect = RectD(0, 0, 0, 0);
      auto layout = m_pContext.GetTextLayout(text.SafePtr(), length, m_pTextFormat.ptr(), textRect, m_pBrush.ptr());
      DWRITE_TEXT_METRICS tm;
      layout->GetMetrics(&tm);
      return tm.widthIncludingTrailingWhitespace;
    }

    float GetSubstrHeight(int length) {
      D2D1_RECT_F textRect = RectD(0, 0, 0, 0);
      auto layout = m_pContext.GetTextLayout(text.SafePtr(), length, m_pTextFormat.ptr(), textRect, m_pBrush.ptr());
      DWRITE_TEXT_METRICS tm;
      layout->GetMetrics(&tm);
      return tm.height;
    }

    virtual float GetDefaultWidth() {
      return GetSubstrWidth(text->size());
    }

    virtual float GetDefaultHeight() {
      return GetSubstrHeight(text->size());
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);
      auto layout = m_pContext.DrawText(text.SafePtr(), m_pTextFormat.ptr(), textRect, m_pBrush.ptr());
      DWRITE_TEXT_METRICS tm;
      layout->GetMetrics(&tm);
    }
  };
}
