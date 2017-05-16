#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "AniButtonPressed.h"

namespace mvc {
  class Button : public View<Button>
  {
  private:

    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1SolidColorBrush> m_pNormalBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pHoverBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pClickBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;

    shared_ptr<AniButtonPressed> m_spAniPressed;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_pBackgroundBrush = btn->m_pClickBackgroundBrush;
      int xPos = GET_X_LPARAM(lParam);
      int yPos = GET_Y_LPARAM(lParam);
      btn->m_spAniPressed->SetCenter(btn->AbsX2RelX(xPos), btn->AbsY2RelY(yPos));
      btn->m_spAniPressed->Stop();
      btn->m_spAniPressed->PlayAndStopAtEnd();
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

      m_pNormalBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xcccccc));

      m_pHoverBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x999999));

      m_pClickBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x666666));

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_pBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(m_color));

      m_pBackgroundBrush = m_pNormalBackgroundBrush;
    }

  public:
    ModelRef<wstring> title;

    Button(const D2DContext &context, wstring ttl) : View(context), title{ ttl } {

      // 内部的layout为一行一列
      m_layout.AddRow("100%");
      m_layout.AddCol("100%");

      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_LBUTTONUP, Handle_LBUTTONUP);

      // 设置点击的动画
      m_spAniPressed = AppendSubView<AniButtonPressed>();
      m_spAniPressed->SetOffset(0, 0, 0, 0);
    }

    ~Button() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      m_pContext->DrawText(
        title->c_str(),
        title->length(),
        m_pTextFormat.ptr(),
        textRect,
        m_pBrush.ptr());
    }

  };
}