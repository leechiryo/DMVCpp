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

    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result) {

      bool isMouseEvent = 0;
      int pixelX = 0;
      int pixelY = 0;
      if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
        // 当鼠标事件发生时，先获取鼠标的坐标信息。
        pixelX = GET_X_LPARAM(lParam);
        pixelY = GET_Y_LPARAM(lParam);
        isMouseEvent = true;
      }

      for (auto v : m_subViews) {
        auto spv = v.lock(); 
        if (!spv) continue; 
        if (isMouseEvent) {
          // TODO: 遇到mouse事件，判断该事件是否表示进入某个子view，
          // 如果是，则向该子view发送WM_MOUSEHOVER消息或WM_MOUSELEAVE消息
          double dipX = PixelsToDipsX(pixelX);
          double dipY = PixelsToDipsY(pixelY);

          if (!spv->HitTest(dipX, dipY)) {
            // 仅当鼠标事件发生时的坐标在元素所在区域内才处理它。
            if (spv->m_mouseIn) {
              spv->m_mouseIn = 0;
              spv->MouseLeft(dipX, dipY);
            }
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
        char processed = spv->HandleMessage(msg, wParam, lParam, result);

        // 如果子元素处理了该消息，则跳过本元素的消息处理并直接返回。
        if (processed) return 1;
      }

      // 如果子元素未处理该消息，则本元素尝试处理它。
      auto it = m_eventHandlers.find(msg);
      if (it != m_eventHandlers.end()) {
        auto spThis = m_wpThis.lock();
        if (spThis){
          for (auto handler : it->second){
            result = handler(dynamic_pointer_cast<DerivedType>(spThis), wParam, lParam);
          }
          return 1;  // 向父元素返回1表示本元素已经处理该消息。
        }
      }

      // 如果本元素也未处理该消息，则向父元素返回0表示自己以及自己的子元素未能处理此消息。
      return 0;
    }

  public:

    View() : ViewBase() {
    }

    void AddEventHandler(int msg, ControllerMethod method) {

      if (m_eventHandlers.find(msg) == m_eventHandlers.end()) {
        vector<ControllerMethod>* handlers = new vector<ControllerMethod>();
        handlers->push_back(method);
        m_eventHandlers.insert({ msg, *handlers });
      }
      else{
        m_eventHandlers[msg].push_back(method);
      }

    }

    void FireEvent(int msg) {
      if (m_eventHandlers.find(msg) != m_eventHandlers.end()) {
        auto spThis = m_wpThis.lock();
        if (spThis) {
          for (auto handler : m_eventHandlers[msg]){
            handler(dynamic_pointer_cast<DerivedType>(spThis), msg, 0);
          }
        }
        App::UpdateViews();
      }
    }
  };
}
