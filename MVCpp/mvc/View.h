#pragma once

#include <Windowsx.h>
#include <vector>
#include "ViewBase.h"
#include "App.h"

using namespace std;

namespace mvc {
  class XmlTagInitializer;

  template<typename DerivedType>
  class View : public ViewBase {
    typedef std::function<LRESULT(shared_ptr<DerivedType>, WPARAM, LPARAM)> ControllerMethod;
    friend class App;
    friend class XmlTagInitializer;

  private:
    map<int, vector<ControllerMethod> > m_eventHandlers;

    static SPView CreateSubViewFromXML(SPView parentView, string id, const XmlSettings & xmlSettings) {
      auto settings = xmlSettings.settings;
      if (id == "") {
        return parentView->AppendSubView<DerivedType>(settings);
      }
      else {
        auto newSubView = parentView->AppendSubView<DerivedType>(settings);
        regv(id, newSubView);
        return newSubView;
      }
    }

  protected:

    static int SetXmlTag(string tag) {
      auto& s_xmlLoaders = GetXmlLoaders();
      auto it = s_xmlLoaders.find(tag);
      if (s_xmlLoaders.end() != it) {
        it->second = CreateSubViewFromXML;
        return 0;
      }
      else {
        s_xmlLoaders.insert({ tag, CreateSubViewFromXML });
        return 1;
      }
    }


    virtual WPView GetClickedSubView(int pixelX, int pixelY) {

      // 查询所有的子view，看其是否被选中。
      // 如果鼠标坐标同时落在多个view上，则只有最上层（zOrder最大）的view被选中
      WPView retval;
      int maxZOrder = 0;

      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;
        if (spv->HitTest(pixelX, pixelY)) {
          // 如果鼠标事件发生时的坐标在子 View 的内部，
          // 则在该子view中进一步查询内部的子view。
          auto wphsv = spv->GetClickedSubView(pixelX, pixelY);
          auto sphsv = wphsv.lock();
          if (sphsv && sphsv->m_zOrder > maxZOrder) {
            retval = wphsv;
            maxZOrder = sphsv->m_zOrder;
          }
          else if (spv->m_zOrder > maxZOrder) {
            retval = spv;
            maxZOrder = spv->m_zOrder;
          }
        }
      }

      return retval;
    }


    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result, WPView &eventView) {

      bool isMouseEvent = 0;
      int pixelX = 0;
      int pixelY = 0;
      double dipX = 0.0;
      double dipY = 0.0;
      SPView sphitv;
      if (msg == WM_MOUSEMOVE || msg == WM_LBUTTONDOWN || msg == WM_LBUTTONUP) {
        // 当鼠标事件发生时，先获取鼠标的坐标信息。
        pixelX = GET_X_LPARAM(lParam);
        pixelY = GET_Y_LPARAM(lParam);
        dipX = AbsPixelX2RelX(pixelX);
        dipY = AbsPixelY2RelY(pixelY);
        isMouseEvent = true;

        auto wphitv = GetHitSubView(pixelX, pixelY);
        sphitv = wphitv.lock();

        if (sphitv) {
          UpdateMouseInState(sphitv, pixelX, pixelY);
          sphitv->FireEvent(msg, wParam, lParam);
        }
        return 1;
      }

      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;

        // 子元素先尝试处理该消息。
        char processed = spv->HandleMessage(msg, wParam, lParam, result, eventView);

        // 如果子元素处理了该消息，则跳过本元素的消息处理并直接返回。
        if (processed) return 1;
      }

      // 如果子元素未处理该消息，则本元素尝试处理它。
      // 但，字符或者键盘事件需要本元素处在聚焦状态才处理。
      if ((msg != WM_CHAR && msg != WM_KEYDOWN) || m_focused) {
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
      }

      // 如果本元素也未处理该消息，则向父元素返回0表示自己以及自己的子元素未能处理此消息。
      return 0;
    }

  public:

    View(const D2DContext &context, Window *parentWnd) : ViewBase(context, parentWnd) {

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

    void FireEvent(int msg, WPARAM wParam, LPARAM lParam) {
      if (m_eventHandlers.find(msg) != m_eventHandlers.end()) {
        // 如果本view可以处理该事件，则处理之。
        auto spThis = m_wpThis.lock();
        if (spThis) {
          for (auto handler : m_eventHandlers[msg]) {
            handler(dynamic_pointer_cast<DerivedType>(spThis), wParam, lParam);
          }
        }
      }
      else {
        // 如果本view不能处理该事件，则交给父view处理。
        if (m_parentView) {
          m_parentView->FireEvent(msg, wParam, lParam);
        }
      }
    }

    shared_ptr<Animation> AddAnimation(std::function<bool(DerivedType*, int)> updateFunc) {
      if (m_parentWnd) {
        return m_parentWnd->CreateAnimation<DerivedType>(this, updateFunc);
      }
      return nullptr;
    }

  };
}
