#pragma once

#include "../Types.h"
#include "../View.h"
#include "../ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Text.h"
#include "..\ViewElements\Line.h"

namespace mvc {
  class TextBox : public View<TextBox> {

  public:
    static const int MSG_TEXTCHANGED = WM_USER + 100;

  private:
    shared_ptr<Rectangle> m_backRect;
    shared_ptr<Text> m_vtext;
    shared_ptr<Line> m_caret;
    shared_ptr<Animation> m_aniCaret;

    size_t m_insertPos;
    float m_hTranslation;

    UINT32 m_color;

    void UpdateCaretPos() {

      if (m_insertPos > text->SafePtr()->length()){
        m_insertPos = text->SafePtr()->length();
      }

      float w = m_vtext->GetSubstrWidth(m_insertPos);
      float tbxw = static_cast<float>(m_right - m_left - 10);
      if (w > tbxw + m_hTranslation) {
        m_hTranslation = w - tbxw;
      }
      else if (w < m_hTranslation) {
        m_hTranslation = w;
      }

      m_vtext->SetLeftOffset(5 - m_hTranslation);
      m_caret->SetLeftOffset(w - m_hTranslation + 5);
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
        tbx->m_aniCaret->SetFrameIndex(9);
        tbx->FireEvent(MSG_TEXTCHANGED, 0, 0);
      }
      return 0;
    }

    static LRESULT Handle_KEYDOWN(shared_ptr<TextBox> tbx, WPARAM wParam, LPARAM lParam) {
      switch (wParam) {
      case VK_BACK:
        if (tbx->m_insertPos > 0) {
          (*(tbx->text))->erase(tbx->m_insertPos - 1, 1);
          tbx->m_insertPos--;
          tbx->FireEvent(MSG_TEXTCHANGED, 0, 0);
        }
        tbx->m_aniCaret->SetFrameIndex(9);
        break;
      case VK_DELETE:
        if (tbx->m_insertPos < (*(tbx->text))->length()) {
          (*(tbx->text))->erase(tbx->m_insertPos, 1);
          tbx->FireEvent(MSG_TEXTCHANGED, 0, 0);
        }
        tbx->m_aniCaret->SetFrameIndex(9);
        break;
      case VK_HOME:
        tbx->m_insertPos = 0;
        tbx->m_aniCaret->SetFrameIndex(9);
        break;
      case VK_END:
        tbx->m_insertPos = (*(tbx->text))->length();
        tbx->m_aniCaret->SetFrameIndex(9);
        break;
      case VK_LEFT:
        tbx->m_insertPos = tbx->m_insertPos > 0 ? tbx->m_insertPos - 1 : 0;
        tbx->m_aniCaret->SetFrameIndex(9);
        break;
      case VK_RIGHT:
        tbx->m_insertPos = tbx->m_insertPos < (*(tbx->text))->length() ? tbx->m_insertPos + 1 : (*(tbx->text))->length();
        tbx->m_aniCaret->SetFrameIndex(9);
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

    TextBox(const D2DContext &context, Window *parentWnd, wstring text) : View(context, parentWnd) {

      // 按照从后到前的顺序生成子View。
      m_backRect = AppendSubView<Rectangle>();
      m_caret = AppendSubView<Line>();
      m_caret->SetStroke(2.0f);
      m_caret->SetOffset(0, 10);
      m_caret->SetWidth("0");
      m_caret->SetHeight("20");

      m_aniCaret = m_caret->AddAnimation([](Line * l, int i)->bool{
        if (i <= 48)
        {
          if (i <= 8) {
            // 前8帧淡入效果
            l->SetOpacity(i / 8.0f);
          }
          else if (i > 40) {
            // 后8帧淡出效果
            l->SetOpacity((48 - i) / 8.0f);
          }
          else {
            // 中间24帧完全显示
            l->SetOpacity(1.0f);
          }
        }

        if (i == 72) return true;
        else return false;
      });

      m_caret->SetHidden(true);

      m_backRect->SetOffset(0, 0, 0, 0);
      m_backRect->SetBackColor(0xfdfdfd);
      m_backRect->SetBackOpacity(1.0f);
      m_backRect->SetStroke(1.0f);
      m_backRect->SetColor(0x555555);
      m_backRect->SetInnerClipAreaOffset(5, 0, 5, 0);
      m_vtext = m_backRect->AppendSubView<Text>(text);
      m_vtext->SetLeftOffset(5);

      // 设置边框的阴影（当处于聚焦状态时显示）
      auto shadowEffect = m_backRect->CreateEffect(CLSID_D2D1Shadow, 0);
      auto compositeEffect = m_backRect->CreateEffect(CLSID_D2D1Composite, 1);
      shadowEffect->SetValue(D2D1_SHADOW_PROP_COLOR, D2D1::ColorF(0x66afe9));
      shadowEffect->SetValue(D2D1_SHADOW_PROP_BLUR_STANDARD_DEVIATION, 4.0f);
      compositeEffect->SetInputEffect(0, shadowEffect.ptr());
      m_backRect->EffectOff();

      // 设置光标的动画

      AddEventHandler(WM_CHAR, Handle_CHAR);
      AddEventHandler(WM_KEYDOWN, Handle_KEYDOWN);
      AddEventHandler(WM_MOUSEMOVE, Handle_MOUSEMOVE);

      m_focused = false;
      m_insertPos = text.length();
      m_hTranslation = 0.0f;

      this->text = &(m_vtext->text);
    }

    // 用于XML构造的函数
    TextBox(const D2DContext &context, Window *parentWnd, const map<string, wstring> &xmlSettings)
      : TextBox(context, parentWnd, L"") {

      auto it = xmlSettings.find("text");
      if (it != xmlSettings.end()){
        *(m_vtext->text.SafePtr()) = it->second;
      }

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

    virtual void FocusChanged(){
      if (m_focused) {
        // 显示阴影效果和输入光标
        m_backRect->SetColor(0x66afe9);
        m_backRect->EffectOn();
        m_caret->SetHidden(false);
        m_aniCaret->PlayRepeatly();
        UpdateCaretPos();
      }
      else {
        // 改变边框的颜色
        m_backRect->SetColor(0x555555);
        m_backRect->EffectOff();

        // 隐藏输入光标
        m_caret->SetHidden(true);
        m_aniCaret->Stop();
      }
    }

    virtual void DrawSelf() {
    }
  };
}