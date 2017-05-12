﻿#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"
#include "AniCaretFlicker.h"
#include "Rectangle.h"
#include "Effect.h"
#include "Text.h"

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
    shared_ptr<Effect> m_shadowEffect2;
    shared_ptr<Rectangle> m_shadowRect;
    shared_ptr<Rectangle> m_backRect;
    shared_ptr<Text> m_vtext;

    size_t m_insertPos;
    float m_hTranslation;

    static const int MAX_CHARS = 256;
    wchar_t m_font[MAX_CHARS + 1];
    float m_fontSize;
    DWRITE_FONT_WEIGHT m_fontWeight;
    DWRITE_FONT_STYLE m_fontStyle;
    DWRITE_FONT_STRETCH m_fontStretch;

    UINT32 m_color;

    void UpdateCaretPos() {
      float w = m_vtext->GetSubstrWidth(m_insertPos);
      float tbxw = static_cast<float>(m_right - m_left);
      if (w > tbxw + m_hTranslation) {
        m_hTranslation = w - tbxw;
      }
      else if (w < m_hTranslation) {
        m_hTranslation = w;
      }

      m_vtext->SetPos(-m_hTranslation, 12, 0, 12);
      m_spAniCaret->SetCaretPos(w + 5, 30);
    }

    static LRESULT Handle_MOUSEMOVE(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      // 变更鼠标图标
      HCURSOR cursor = LoadCursor(NULL, IDC_IBEAM);
      SetCursor(cursor);
      return 0;
    }

    static LRESULT Handle_CHAR(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      if (wParam > 31 && wParam != 127) {
        (*(tbx->text))->insert(tbx->m_insertPos, 1, wParam);
        tbx->m_insertPos++;
        tbx->UpdateCaretPos();
      }
      return 0;
    }

    static LRESULT Handle_KEYDOWN(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      switch (wParam) {
      case VK_BACK:
        if (tbx->m_insertPos > 0) {
          (*(tbx->text))->erase(tbx->m_insertPos - 1, 1);
          tbx->m_insertPos--;
        }
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      case VK_DELETE:
        if (tbx->m_insertPos < (*(tbx->text))->length()) {
          (*(tbx->text))->erase(tbx->m_insertPos, 1);
        }
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      case VK_HOME:
        tbx->m_insertPos = 0;
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      case VK_END:
        tbx->m_insertPos = (*(tbx->text))->length();
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      case VK_LEFT:
        tbx->m_insertPos = tbx->m_insertPos > 0 ? tbx->m_insertPos - 1 : 0;
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      case VK_RIGHT:
        tbx->m_insertPos = tbx->m_insertPos < (*(tbx->text))->length() ? tbx->m_insertPos + 1 : (*(tbx->text))->length();
        tbx->m_spAniCaret->SetFrameIndex(9);
        break;
      }
      tbx->UpdateCaretPos();
      return 0;
    }

  protected:

    virtual void CreateD2DResource() {

      m_pBackgroundBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0xfdfdfd));
      m_pTextBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x333333));
      m_pBorderBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x555555));
      m_pBorderFocusedBrush = m_pContext.CreateSolidColorBrush(D2D1::ColorF(0x66afe9));

      m_pTextFormat = App::CreateTextFormat(m_font, m_fontSize, m_fontWeight, m_fontStyle, m_fontStretch);
      m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
      m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

      m_shadowEffect = m_pContext.CreateEffect(CLSID_D2D1Shadow);
      //m_shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
    }

  public:

    ModelRef<wstring> *text;

    TextBox(const D2DContext &context, wstring text) : View(context) {

      m_focused = false;

      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      m_insertPos = text.length();
      m_hTranslation = 0.0f;

      m_shadowEffect2 = CreateSubView<Effect>(CLSID_D2D1Shadow);
      m_shadowEffect2->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
      m_shadowEffect2->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 5.0f);
      m_shadowRect = m_shadowEffect2->CreateSubView<Rectangle>();
      m_shadowRect->SetBackColor(0x0);
      m_shadowRect->SetBackOpacity(1.0f);
      m_shadowRect->SetStroke(0.0f);

      m_backRect = CreateSubView<Rectangle>();
      m_backRect->SetBackColor(0xfdfdfd);
      m_backRect->SetBackOpacity(1.0f);
      m_backRect->SetStroke(1.0f);
      m_backRect->SetColor(0x555555);

      m_vtext = CreateSubView<Text>(text);
      this->text = &(m_vtext->text);
      m_vtext->SetPos(0, 12, 0, 12);

      // 设置光标的动画
      m_spAniCaret = CreateSubView<AniCaretFlicker>();
      m_spAniCaret->PlayRepeatly();

      //UpdateCaretPos();

      AddEventHandler(WM_CHAR, Handle_CHAR);
      AddEventHandler(WM_KEYDOWN, Handle_KEYDOWN);
      AddEventHandler(WM_MOUSEMOVE, Handle_MOUSEMOVE);
    }

    ~TextBox() {
    }

    // 重载了该方法，直接返回本节点，不再向子view查询是否聚焦
    virtual WPView GetClickedSubView(int pixelX, int pixelY) {
      return m_wpThis;
    }

    virtual void DrawSelf() {
      D2D1_RECT_F textRect = RectD(m_left, m_top, m_right, m_bottom);

      m_backRect->SetPos(0, 0, m_right - m_left, m_bottom - m_top);
      m_vtext->SetClipArea(0.0, 0.0, m_right - m_left, m_bottom - m_top);

      if (m_focused) {
        m_shadowEffect2->SetPos(0, 0, 0, 0);
        m_shadowRect->SetPos(0, 0, m_right - m_left, m_bottom - m_top);
        m_shadowEffect2->SetHidden(false);
        m_backRect->SetColor(0x66afe9);
        // 如果处在选中状态，则在边框周围绘制一个阴影。
        //auto bmpRT = m_pContext.CreateCompatibleRenderTarget();
        //auto bmpContext = bmpRT.Query<ID2D1DeviceContext>();

        //bmpContext->BeginDraw();
        //bmpContext->Clear(D2D1::ColorF(0xffffff, 0.0f));
        //bmpContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());
        //bmpContext->EndDraw();

        //auto bmp = bmpRT.GetResource<ID2D1Bitmap>(&ID2D1BitmapRenderTarget::GetBitmap);

        //m_shadowEffect->SetInput(0, bmp.ptr());
        //m_pContext->DrawImage(m_shadowEffect.ptr());

        // 改变边框的颜色
        m_pContext->DrawRectangle(textRect, m_pBorderFocusedBrush.ptr());

        // 显示输入光标
        m_spAniCaret->SetHidden(false);
      }
      else {
        // 改变边框的颜色
        m_pContext->DrawRectangle(textRect, m_pBorderBrush.ptr());

        m_backRect->SetColor(0x555555);

        // 隐藏输入光标
        m_spAniCaret->SetHidden(true);

        m_shadowEffect2->SetHidden(true);
      }
      m_pContext->FillRectangle(textRect, m_pBackgroundBrush.ptr());

      textRect.left += 10;
      textRect.right -= 10;

      m_spAniCaret->SetPos(5, 0, m_right - m_left - 5, m_bottom - m_top);


      //m_pContext->PushAxisAlignedClip(textRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
      //m_pContext->SetTransform(D2D1::Matrix3x2F::Translation(-m_hTranslation, 0.0f));
      //m_pContext.DrawText(text.SafePtr(), m_pTextFormat.ptr(), textRect, m_pTextBrush.ptr());
      //m_pContext->PopAxisAlignedClip();

    }
  };
}