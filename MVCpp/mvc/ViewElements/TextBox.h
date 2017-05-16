#pragma once

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
      float tbxw = static_cast<float>(m_right - m_left - 10);
      if (w > tbxw + m_hTranslation) {
        m_hTranslation = w - tbxw;
      }
      else if (w < m_hTranslation) {
        m_hTranslation = w;
      }

      m_vtext->SetLeftOffset(5 - m_hTranslation);
      m_spAniCaret->SetCaretPos(w - m_hTranslation, 30);
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
    }

  public:

    ModelRef<wstring> *text;

    TextBox(const D2DContext &context, wstring text) : View(context) {

      // 内部的layout为一行一列
      m_layout.AddRow("*");
      m_layout.AddCol("*");

      m_focused = false;

      m_color = 0x333333;
      m_fontWeight = DWRITE_FONT_WEIGHT_REGULAR;
      m_fontStyle = DWRITE_FONT_STYLE_NORMAL;
      m_fontStretch = DWRITE_FONT_STRETCH_NORMAL;
      wcscpy_s(m_font, MAX_CHARS + 1, L"Source Code Pro");
      m_fontSize = 16.0;

      m_insertPos = text.length();
      m_hTranslation = 0.0f;

      // 按照从后到前的顺序生成子View。
      m_shadowEffect2 = AppendSubView<Effect>(CLSID_D2D1Shadow);
      m_backRect = AppendSubView<Rectangle>();
      m_vtext = AppendSubView<Text>(text);
      m_spAniCaret = AppendSubView<AniCaretFlicker>();

      m_shadowEffect2->SetOffset(0, 0, 0, 0);
      m_shadowEffect2->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
      m_shadowEffect2->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 5.0f);
      m_shadowEffect2->AddLayoutRow("*");
      m_shadowEffect2->AddLayoutCol("*");
      m_shadowRect = m_shadowEffect2->AppendSubView<Rectangle>();
      m_shadowRect->SetOffset(0, 0, 0, 0);
      m_shadowRect->SetBackColor(0x0);
      m_shadowRect->SetBackOpacity(1.0f);
      m_shadowRect->SetStroke(0.0f);

      m_backRect->SetOffset(0, 0, 0, 0);
      m_backRect->SetBackColor(0xfdfdfd);
      m_backRect->SetBackOpacity(1.0f);
      m_backRect->SetStroke(1.0f);
      m_backRect->SetColor(0x555555);

      this->text = &(m_vtext->text);
      m_vtext->SetOffset(5, 12);
      m_vtext->SetClipArea(0.0, 0.0, m_right - m_left - 10.0, m_bottom - m_top);

      // 设置光标的动画
      m_spAniCaret->PlayRepeatly();

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

    virtual float GetDefaultWidth() {
      auto textWidth = m_vtext->GetDefaultWidth() + 10;
      if (textWidth > 100){
        return textWidth;
      }
      else{
        return 100;
      }
    }

    virtual float GetDefaultHeight() {
      auto textHeight = m_vtext->GetDefaultHeight() + 10;
      if (textHeight > 40){
        return textHeight;
      }
      else{
        return 40;
      }
    }

    virtual void DrawSelf() {

      if (m_focused) {

        // 显示阴影效果和输入光标
        m_shadowEffect2->SetHidden(false);
        m_backRect->SetColor(0x66afe9);
        m_spAniCaret->SetHidden(false);
      }
      else {
        // 改变边框的颜色
        m_backRect->SetColor(0x555555);

        // 隐藏输入光标
        m_spAniCaret->SetHidden(true);

        // 取消阴影效果
        m_shadowEffect2->SetHidden(true);
      }
    }
  };
}