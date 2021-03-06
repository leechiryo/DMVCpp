﻿#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\Rectangle.h"
#include "..\ViewElements\Text.h"
#include "..\ViewElements\Circle.h"

namespace mvc {
  class Button : public View<Button>
  {
  private:
    // D2D 资源(离开作用域时会自动销毁)
    shared_ptr<Rectangle> m_backRect;
    shared_ptr<Text> m_title;
    shared_ptr<Circle> m_circle;
    shared_ptr<Animation> m_ani;
    float m_maxRadius;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0x666666);
      int xPos = GET_X_LPARAM(lParam);
      int yPos = GET_Y_LPARAM(lParam);

      float fxPos = btn->AbsPixelX2RelX(xPos);
      float fyPos = btn->AbsPixelY2RelY(yPos);
      float maxWidth = tof(max(fxPos, btn->m_calWidth - fxPos));
      float maxHeight = tof(max(fyPos, btn->m_calHeight - fyPos));
      btn->m_maxRadius = tof(sqrt(maxWidth * maxWidth + maxHeight * maxHeight));
      btn->m_circle->SetCirclePosition(btn->AbsPixelX2RelX(xPos), btn->AbsPixelY2RelY(yPos), 0.0f);
      btn->m_circle->SetHidden(false);
      btn->m_ani->PlayAndPauseAtStart();
      return 0;
    }

    static LRESULT Handle_LBUTTONUP(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0x999999);
      return 0;
    }

    static LRESULT Handle_MOUSEENTER(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0x999999);
      return 0;
    }
    static LRESULT Handle_MOUSELEFT(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0xcccccc);
      return 0;
    }

  protected:

    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *title;

    Button(const D2DContext &context, Window *parentWnd, wstring ttl) : View(context, parentWnd) {

      SetInnerClipAreaOffset(0, 0, 0, 0);

      m_backRect = AppendSubView<Rectangle>();
      m_backRect->SetOffset(0, 0, 0, 0);
      m_backRect->SetStroke(0.0f);
      m_backRect->SetBackColor(0xcccccc);

      m_title = AppendSubView<Text>(ttl);

      // 设置点击的动画
      m_circle = AppendSubView<Circle>();
      m_circle->SetOffset(0, 0, 0, 0);
      m_circle->SetWidth("0");
      m_circle->SetHeight("0");
      m_circle->SetBackColor(0xcccccc);
      m_circle->SetColor(0xcccccc);
      m_circle->SetHidden(true);
      m_circle->SetColor(0xcccccc);

      float *addr = &m_maxRadius;
      m_ani = m_circle->AddAnimation([addr](Circle *c, int idx)->bool {

        // 该动画总共有八帧（0-7），如果帧数超过8，则返回true表示动画结束。
        if (idx > 7) return true;

        // 动画表现一个半径从小到大变化，同时有淡出效果的圆。
        float maxRadius = *addr;
        c->SetCircleRadius(maxRadius * idx / 6.0f);
        c->SetBackOpacity((7-idx)/6.0f);
        c->SetOpacity((7-idx)/6.0f);
        return false;

      });

      auto circle = m_circle;
      m_ani->OnFinished = [circle]() {
        circle->SetHidden(true);
      };

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_LBUTTONUP, Handle_LBUTTONUP);
      AddEventHandler(MSG_MOUSEENTER, Handle_MOUSEENTER);
      AddEventHandler(MSG_MOUSELEFT, Handle_MOUSELEFT);

      title = &(m_title->text);
    }

    // 用于XML构造的函数
    Button(const D2DContext &context, Window *parentWnd, const map<string, wstring>& xmlSettings) 
      : Button(context, parentWnd, L"") {
      auto it = xmlSettings.find("text");
      if (it != xmlSettings.end()){
        *(m_title->text.SafePtr()) = it->second;
      }

      it = xmlSettings.find("bind");
      if (it != xmlSettings.end()){
        title->Bind(Utf16To8(it->second.c_str()).get());
      }
    }

    ~Button() {
    }

    virtual float GetDefaultWidth() {
      return m_title->GetDefaultWidth() + 20;
    }

    virtual float GetDefaultHeight() {
      auto height = m_title->GetDefaultHeight();
      return height > 20 ? height + 10 : 30;
    }

    virtual void DrawSelf() {
      m_title->ClearLeftOffset();
    }

  };
}