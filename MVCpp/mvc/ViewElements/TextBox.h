#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"
#include "AniCaretFlicker.h"

namespace mvc {
  class TextBox : public View<TextBox> {

  private:
    DxResource<ID2D1SolidColorBrush> m_pBackgroundBrush;
    DxResource<ID2D1SolidColorBrush> m_pTextBrush;
    DxResource<ID2D1SolidColorBrush> m_pBorderBrush;
    DxResource<ID2D1SolidColorBrush> m_pBorderFocusedBrush;
    DxResource<IDWriteTextFormat> m_pTextFormat;
    DxResource<ID2D1Effect> m_shadowEffect;

    shared_ptr<AniCaretFlicker> m_spAniCaret;
    size_t m_insertPos;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    static LRESULT Handle_CHAR(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      if (wParam > 31 && wParam != 127) {
        tbx->text->insert(tbx->m_insertPos, 1, wParam);
        tbx->m_insertPos++;
      }
      return 0;
    }

    static LRESULT Handle_KEYDOWN(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      switch (wParam) {
      case VK_BACK:
        if (tbx->m_insertPos > 0) {
          tbx->text->erase(tbx->m_insertPos - 1, 1);
          tbx->m_insertPos--;
        }
        break;
      case VK_DELETE:
        if (tbx->m_insertPos < tbx->text->length()) {
          tbx->text->erase(tbx->m_insertPos, 1);
        }
        break;
      case VK_HOME:
        tbx->m_insertPos = 0;
        break;
      case VK_END:
        tbx->m_insertPos = tbx->text->length();
        break;
      case VK_LEFT:
        tbx->m_insertPos = tbx->m_insertPos > 0 ? tbx->m_insertPos - 1 : 0;
        break;
      case VK_RIGHT:
        tbx->m_insertPos = tbx->m_insertPos < tbx->text->length() ? tbx->m_insertPos + 1 : tbx->text->length();
        break;
      }
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
      m_pBorderFocusedBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x66afe9));

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_shadowEffect = m_pContext.CreateEffect(CLSID_D2D1Shadow);
      m_shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
    }

  public:

    ModelRef<wstring> text;

    TextBox(wstring text) : text{ text } {

      m_focused = false;

      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      m_insertPos = text.length();

      // 设置光标的动画
      m_spAniCaret = make_shared<AniCaretFlicker>();
      m_spAniCaret->PlayRepeatly();
      m_subViews.insert(m_spAniCaret);

      AddEventHandler(WM_CHAR, Handle_CHAR);
      AddEventHandler(WM_KEYDOWN, Handle_KEYDOWN);
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

        // 改变边框的颜色
        m_pContext->DrawRectangle(textRect, m_pBorderFocusedBrush.ptr());

        // 显示输入光标
        m_spAniCaret->SetHidden(false);
      }
      else {
        // 改变边框的颜色
        m_pContext->DrawRectangle(textRect, m_pBorderBrush.ptr());

        // 隐藏输入光标
        m_spAniCaret->SetHidden(true);
      }
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      textRect.left += 10;

      m_spAniCaret->SetPos(m_left + 5, m_top, m_right, m_bottom);
      auto layout = m_pContext.DrawText(text.SafePtr(), m_pTextFormat.ptr(), textRect, m_pTextBrush.ptr());

      if (m_insertPos == 0) {
        m_spAniCaret->SetCaretPos(5, 30);
      }
      else if (m_insertPos == text->length()) {
        DWRITE_TEXT_METRICS tm;
        layout->GetMetrics(&tm);
        m_spAniCaret->SetCaretPos(tm.widthIncludingTrailingWhitespace + 5, 30);
      }
      else {
        auto layout = m_pContext.GetTextLayout(text.SafePtr(), m_insertPos, m_pTextFormat.ptr(), textRect, m_pTextBrush.ptr());
        DWRITE_TEXT_METRICS tm;
        layout->GetMetrics(&tm);
        m_spAniCaret->SetCaretPos(tm.widthIncludingTrailingWhitespace + 5, 30);
      }
    }
  };
}