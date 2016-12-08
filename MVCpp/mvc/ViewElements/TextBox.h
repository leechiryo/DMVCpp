#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"

namespace mvc{
  class TextBox : public View<TextBox>{

  private:
    ID2D1SolidColorBrush* m_pBackgroundBrush;
    ID2D1SolidColorBrush* m_pTextBrush;
    ID2D1SolidColorBrush* m_pBorderBrush;
    ID2D1RadialGradientBrush* m_pFocusBorderBrush;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    IDWriteTextFormat* m_pTextFormat;

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
      HRESULT hr = m_pContext->CreateSolidColorBrush(
        D2D1::ColorF(0xffffff),
        &m_pBackgroundBrush);

      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to create the background brush.");
      }

      hr = m_pContext->CreateSolidColorBrush(
        D2D1::ColorF(0x333333),
        &m_pTextBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pBackgroundBrush);
        throw std::runtime_error("Failed to create the background brush.");
      }

      hr = m_pContext->CreateSolidColorBrush(
        D2D1::ColorF(0x333333),
        &m_pBorderBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pBackgroundBrush);
        SafeRelease(m_pTextBrush);
        throw std::runtime_error("Failed to create the background brush.");
      }

      hr = App::s_pDWriteFactory->CreateTextFormat(
        m_font,
        NULL,
        m_fontWeight,
        m_fontStyle,
        m_fontStretch,
        m_fontSize,
        L"ja-JP",
        &m_pTextFormat);

      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pBackgroundBrush);
        SafeRelease(m_pTextBrush);
        SafeRelease(m_pBorderBrush);
        throw new std::runtime_error("Failed to create the text format.");
      }

      ID2D1GradientStopCollection *pStops = nullptr;
      D2D1_GRADIENT_STOP stops[2];
      stops[0].color = D2D1::ColorF(D2D1::ColorF::Yellow, 1);
      stops[0].position = 0.0f;
      stops[1].color = D2D1::ColorF(D2D1::ColorF::ForestGreen, 1);
      stops[1].position = 1.0f;

      hr = m_pContext->CreateGradientStopCollection(stops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &pStops);

      hr = m_pContext->CreateRadialGradientBrush(
        D2D1::RadialGradientBrushProperties(
        D2D1::Point2F(75, 75),
        D2D1::Point2F(0, 0),
        75, 75),
        pStops,
        &m_pFocusBorderBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pBackgroundBrush);
        SafeRelease(m_pTextBrush);
        SafeRelease(m_pBorderBrush);
        SafeRelease(m_pTextFormat);
        throw new std::runtime_error("Failed to create the brush.");
      }
    }

    virtual void DestroyD2DResource() {
      SafeRelease(m_pBackgroundBrush);
      SafeRelease(m_pTextBrush);
      SafeRelease(m_pBorderBrush);
      SafeRelease(m_pTextFormat);
      SafeRelease(m_pFocusBorderBrush);
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
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush);
      m_pContext->DrawRectangle(textRect, m_pFocusBorderBrush);

      textRect.left += 10;

      m_pContext->DrawText(
        text->c_str(),
        text->length(),
        m_pTextFormat,
        textRect,
        m_pTextBrush);
    }
  };
}