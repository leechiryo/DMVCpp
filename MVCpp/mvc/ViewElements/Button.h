#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "AniButtonPressed.h"

namespace mvc {
  class Button : public View<Button>
  {
  private:

    ID2D1SolidColorBrush* m_pNormalBackgroundBrush;
    ID2D1SolidColorBrush* m_pHoverBackgroundBrush;
    ID2D1SolidColorBrush* m_pClickBackgroundBrush;
    ID2D1SolidColorBrush* m_pBackgroundBrush;
    AniButtonPressed m_aniPressed;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    ID2D1SolidColorBrush* m_pBrush;
    IDWriteTextFormat* m_pTextFormat;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_pBackgroundBrush = btn->m_pClickBackgroundBrush;
      return 0;
    }

    static LRESULT Handle_LBUTTONUP(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_pBackgroundBrush = btn->m_pHoverBackgroundBrush;
      return 0;
    }

  protected:
    virtual void MouseEnter(double x, double y) {
      m_pBackgroundBrush = m_pHoverBackgroundBrush;
    }

    virtual void MouseLeft(double x, double y) {
      m_pBackgroundBrush = m_pNormalBackgroundBrush;
    }

    virtual void CreateD2DResource() {
      HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0xcccccc),
        &m_pNormalBackgroundBrush);

      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to create the background brush.");
      }

      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0x999999),
        &m_pHoverBackgroundBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pNormalBackgroundBrush);
        throw std::runtime_error("Failed to create the background brush.");
      }

      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(0x666666),
        &m_pClickBackgroundBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pNormalBackgroundBrush);
        SafeRelease(m_pHoverBackgroundBrush);
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

      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pNormalBackgroundBrush);
        SafeRelease(m_pHoverBackgroundBrush);
        SafeRelease(m_pClickBackgroundBrush);
        throw new std::runtime_error("Failed to create the text format.");
      }

      hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(m_color),
        &m_pBrush);

      if (!SUCCEEDED(hr)) {
        SafeRelease(m_pNormalBackgroundBrush);
        SafeRelease(m_pHoverBackgroundBrush);
        SafeRelease(m_pClickBackgroundBrush);
        SafeRelease(m_pTextFormat);
        throw new std::runtime_error("Failed to create the brush.");
      }

      m_pBackgroundBrush = m_pNormalBackgroundBrush;
    }

    virtual void DestroyD2DResource() {
      SafeRelease(m_pNormalBackgroundBrush);
      SafeRelease(m_pHoverBackgroundBrush);
      SafeRelease(m_pClickBackgroundBrush);
      SafeRelease(m_pTextFormat);
      SafeRelease(m_pBrush);
    }

  public:
    ModelRef<wstring> title;

    Button(wstring ttl) : title{ ttl } {
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_LBUTTONUP, Handle_LBUTTONUP);

      m_aniPressed.SetPos(m_left, m_top, m_right, m_bottom);
      m_subViews.insert(make_shared(&m_aniPressed));
    }

    ~Button() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);
      m_pRenderTarget->FillRectangle(textRect, m_pBackgroundBrush);

      m_pRenderTarget->DrawText(
        title->c_str(),
        title->length(),
        m_pTextFormat,
        textRect,
        m_pBrush);
    }

  };
}