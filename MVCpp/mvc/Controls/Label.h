#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Text.h"

namespace mvc {
  class Label : public View<Label>
  {
  private:

    shared_ptr<Text> m_vtext;
    shared_ptr<Rectangle> m_labelBack;

    float m_leftPadding;
    float m_rightPadding;
    float m_topPadding;
    float m_bottomPadding;

  protected:
    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *text;
    Label(const D2DContext &context, Window * parentWnd, wstring ttl): View(context, parentWnd){
      m_labelBack = AppendSubView<Rectangle>();
      m_labelBack->SetOpacity(0.0);
      m_labelBack->SetBackOpacity(0.0);
      m_labelBack->SetHidden(false);
      m_labelBack->SetOffset(0, 0, 0, 0);

      m_vtext = AppendSubView<Text>(ttl);
      m_vtext->SetGridPosition(0, 0);
      m_vtext->SetOffset(0, 0);

      text = &(m_vtext->text);

      // 如果要設置padding，可以在這裏設置内部對象的位置
      m_leftPadding = 0;
      m_topPadding = 0;
      m_rightPadding = 0;
      m_bottomPadding = 0;
    }

    // 用于XML构造的函数
    Label(const D2DContext &context, Window * parentWnd, const map<string, wstring> & xmlSettings)
      : Label(context, parentWnd, L""){

      auto it = xmlSettings.find("text");
      if (it != xmlSettings.end()){
        *(m_vtext->text.SafePtr()) = it->second;
      }

    }

    ~Label() {
    }

    void SetBackColor(UINT32 color){
      m_labelBack->SetBackOpacity(1);
      m_labelBack->SetBackColor(color);
    }

    void SetColor(UINT32 color){
      m_vtext->SetColor(color);
    }

    void SetBorderColor(UINT32 color){
      m_labelBack->SetOpacity(1);
      m_labelBack->SetColor(color);
    }

    void SetPadding(float l, float t, float r, float b){
      m_leftPadding = l;
      m_topPadding = t;
      m_rightPadding = r;
      m_bottomPadding = b;

      m_vtext->SetOffset(l, t, r, b);
    }

    virtual float GetDefaultWidth() {
      return m_vtext->GetDefaultWidth() + m_leftPadding + m_rightPadding;
    }

    virtual float GetDefaultHeight() {
      return m_vtext->GetDefaultHeight() + m_topPadding + m_bottomPadding;
    }

    virtual void DrawSelf() {
    }

  };
}
