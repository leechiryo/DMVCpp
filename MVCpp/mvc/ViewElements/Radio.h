#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  class Radio : public View<Radio>
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

    int m_value;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Radio> rdo, WPARAM wParam, LPARAM lParam) {
      rdo->selectedValue = rdo->m_value;
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

    ModelRef<int> selectedValue;
    ModelRef<wstring> title;

    Radio(int val, wstring ttl) : title{ ttl } {
      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      m_value = val;
      selectedValue = 0;
    }

    ~Radio() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left + 20, m_top, m_right, m_bottom);
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      double radius = 5.0;
      double radiusChecked = 3.0;
      double centerX = m_left + radius;
      double centerY = (m_bottom + m_top) / 2;

      D2D1_ELLIPSE ellipse = D2D1::Ellipse(
        D2D1::Point2F(tof(centerX), tof(centerY)),
        tof(radius), tof(radius));
      m_pContext->DrawEllipse(ellipse, m_pBrush.ptr(), 2.0f);

      if (m_value == selectedValue) {
        D2D1_ELLIPSE ellipseChecked = D2D1::Ellipse(
          D2D1::Point2F(tof(centerX), tof(centerY)),
          tof(radiusChecked), tof(radiusChecked));
        m_pContext->FillEllipse(ellipseChecked, m_pBrush.ptr());
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
