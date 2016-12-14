#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"
#include "AniCaretFlicker.h"

namespace mvc{
  class TextBox : public View<TextBox>{

  private:
    DxResource<ID2D1SolidColorBrush> m_pBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pTextBrush;
    DxResource<ID2D1SolidColorBrush> m_pBorderBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;
    DxResource<ID2D1Effect> m_shadowEffect;

    shared_ptr<AniCaretFlicker> m_spAniCaret;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    bool m_focused;

    static LRESULT Handle_LBUTTONDOWN(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      // 设置Focus，改变边框的样式。
      tbx->m_focused = true;

      // 改变边框的颜色
      tbx->m_pBorderBrush = tbx->m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x66afe9));
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

      m_pBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xfdfdfd));
      m_pTextBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x333333));
      m_pBorderBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x555555));

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_shadowEffect = m_pContext.CreateEffect(CLSID_D2D1Shadow);
      m_shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
    }

  public:

    ModelRef<wstring> text;

    TextBox(wstring text) : text{ text }{

      m_focused = false;

      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);

      // 设置光标的动画
      m_spAniCaret = make_shared<AniCaretFlicker>();
      m_subViews.insert(m_spAniCaret);

      m_spAniCaret->SetPosition(100, 30);
      m_spAniCaret->PlayRepeatly();
    }

    ~TextBox() {
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);

      if (m_focused) {
        // 如果处在选中状态，则在边框周围绘制一个阴影。
        auto bmpRT = m_pContext.CreateCompatibleRenderTarget();

        bmpRT->BeginDraw();
        bmpRT->Clear(D2D1::ColorF(0xffffff, 0.0f));
        bmpRT->FillRectangle(textRect, m_pBackgroundBrush.ptr());
        bmpRT->EndDraw();

        auto bmp = bmpRT.GetResource<ID2D1Bitmap>(&ID2D1BitmapRenderTarget::GetBitmap);

        m_shadowEffect->SetInput(0, bmp.ptr());
        m_pContext->DrawImage(m_shadowEffect.ptr());
      }

      m_pContext->DrawRectangle(textRect, m_pBorderBrush.ptr());
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      textRect.left += 10;

      m_pContext->DrawText(
        text->c_str(),
        text->length(),
        m_pTextFormat.ptr(),
        textRect,
        m_pTextBrush.ptr());

      m_spAniCaret->SetPos(m_left + 5, m_top, m_right, m_bottom);
    }
  };
}