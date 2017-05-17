#pragma once

#include <Windowsx.h>
#include <vector>
#include "ViewBase.h"
#include "App.h"

using namespace std;

namespace mvc {
  template<typename DerivedType>
  class View : public ViewBase {
    typedef LRESULT(*ControllerMethod)(shared_ptr<DerivedType>, WPARAM, LPARAM);
    friend class App;

  private:
    map<int, vector<ControllerMethod> > m_eventHandlers;

  protected:

    virtual WPView GetClickedSubView(int pixelX, int pixelY) {

      // 查询所有的子view，看其是否被选中。
      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;
        if (spv->HitTest(pixelX, pixelY)) {
          // 如果鼠标事件发生时的坐标在子 View 的内部，
          // 则在该子view中进一步查询内部的子view。
          auto csv = spv->GetClickedSubView(pixelX, pixelY);
          if (!csv.expired()) return csv;
        }
      }

      // 如果内部的所有子view都没有被选中，则说明自身被选中，
      // 于是返回自身。
      if(m_canBeFocused) return m_wpThis;
      else {
        WPView emptyV;
        return emptyV;
      }
    }

    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result, WPView &eventView) {

      bool isMouseEvent = 0;
      int pixelX = 0;
      int pixelY = 0;
      double dipX = 0.0;
      double dipY = 0.0;
      if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
        // 当鼠标事件发生时，先获取鼠标的坐标信息。
        pixelX = GET_X_LPARAM(lParam);
        pixelY = GET_Y_LPARAM(lParam);
        dipX = AbsPixelX2RelX(pixelX);
        dipY = AbsPixelY2RelY(pixelY);
        isMouseEvent = true;
      }

      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;
        if (isMouseEvent) {
          // 遇到 mouse 事件，判断该事件是否表示进入某个子 view
          if (!spv->HitTest(pixelX, pixelY)) {
            if (spv->m_mouseIn) {
              spv->m_mouseIn = 0;
              spv->MouseLeft(dipX, dipY);
            }
            // 如果鼠标事件发生时的坐标不在子 View 的内部，则跳过之后的事件处理(continue)，
            // 仅当鼠标的坐标在元素所在区域内才处理它。
            continue;
          }
          else {
            if (!spv->m_mouseIn) {
              spv->m_mouseIn = 1;
              spv->MouseEnter(dipX, dipY);
            }
          }
        }

        // 子元素先尝试处理该消息。
        char processed = spv->HandleMessage(msg, wParam, lParam, result, eventView);

        // 如果子元素处理了该消息，则跳过本元素的消息处理并直接返回。
        if (processed) return 1;
      }

      // 如果子元素未处理该消息，则本元素尝试处理它。
      auto it = m_eventHandlers.find(msg);
      if (it != m_eventHandlers.end()) {
        auto spThis = m_wpThis.lock();
        if (spThis) {
          for (auto handler : it->second) {
            result = handler(dynamic_pointer_cast<DerivedType>(spThis), wParam, lParam);
          }
          eventView = m_wpThis;
          return 1;  // 向父元素返回1表示本元素已经处理该消息。
        }
      }

      // 如果本元素也未处理该消息，则向父元素返回0表示自己以及自己的子元素未能处理此消息。
      return 0;
    }

  public:

    View(const D2DContext &context) : ViewBase(context) {
    }

    void AddEventHandler(int msg, ControllerMethod method) {

      if (m_eventHandlers.find(msg) == m_eventHandlers.end()) {
        vector<ControllerMethod>* handlers = new vector<ControllerMethod>();
        handlers->push_back(method);
        m_eventHandlers.insert({ msg, *handlers });
      }
      else {
        m_eventHandlers[msg].push_back(method);
      }

    }

    void FireEvent(int msg) {
      if (m_eventHandlers.find(msg) != m_eventHandlers.end()) {
        auto spThis = m_wpThis.lock();
        if (spThis) {
          for (auto handler : m_eventHandlers[msg]) {
            handler(dynamic_pointer_cast<DerivedType>(spThis), msg, 0);
          }
        }
        App::UpdateViews();
      }
    }
  };
}
