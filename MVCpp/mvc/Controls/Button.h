#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"
#include "..\ViewElements\AniButtonPressed.h"
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
    shared_ptr<AniButtonPressed> m_spAniPressed;
    shared_ptr<Circle> m_circle;
    shared_ptr<AnimationBase> m_ani;

    // controller method
    static LRESULT Handle_LBUTTONDOWN(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0x666666);
      int xPos = GET_X_LPARAM(lParam);
      int yPos = GET_Y_LPARAM(lParam);
      btn->m_spAniPressed->SetCenter(btn->AbsPixelX2RelX(xPos), btn->AbsPixelY2RelY(yPos));
      btn->m_spAniPressed->Stop();
      btn->m_spAniPressed->PlayAndHideAtEnd();
      return 0;
    }

    static LRESULT Handle_LBUTTONUP(shared_ptr<Button> btn, WPARAM wParam, LPARAM lParam) {
      btn->m_backRect->SetBackColor(0x999999);
      return 0;
    }

  protected:
    virtual void MouseEnter(double x, double y) {
      m_backRect->SetBackColor(0x999999);
    }

    virtual void MouseLeft(double x, double y) {
      m_backRect->SetBackColor(0xcccccc);
    }

    virtual void CreateD2DResource() {
    }

  public:
    ModelRef<wstring> *title;

    Button(const D2DContext &context, Window *parentWnd, wstring ttl) : View(context, parentWnd) {

      m_backRect = AppendSubView<Rectangle>();
      m_backRect->SetOffset(0, 0, 0, 0);
      m_backRect->SetStroke(0.0f);
      m_backRect->SetBackColor(0xcccccc);

      m_title = AppendSubView<Text>(ttl);

      // 设置点击的动画
      m_spAniPressed = AppendSubView<AniButtonPressed>();
      m_spAniPressed->SetOffset(0, 0, 0, 0);

      m_circle = AppendSubView<Circle>();
      m_circle->SetOffset(0, 0);
      m_circle->SetWidth("0");
      m_circle->SetHeight("0");

      m_ani = m_circle->AddAnimation<Circle>([](Circle *c, int idx)->bool{
        return true;
      });

      AddEventHandler(WM_LBUTTONDOWN, Handle_LBUTTONDOWN);
      AddEventHandler(WM_LBUTTONUP, Handle_LBUTTONUP);

      title = &(m_title->text);
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