#pragma once

#include "Types.h"
#include "ModelSafePtr.h"
#include "Model.h"

using namespace std;

namespace mvc {
  template<typename T>
  class ModelRef {

    friend class ViewBase;

  private:
    T m_fallback;
    T *m_fieldPtr;
    WPModel m_wpModel;
    ViewBase *m_pMyView;

  public:

    template<typename ... Args>
    ModelRef(ViewBase * pView, Args ... args) : m_fallback(args...), m_wpModel() {
      m_fieldPtr = &m_fallback;
      m_pMyView = pView;
    }

    ~ModelRef() {
      m_wpModel.reset();
    }

    template<typename M>
    void Bind(string modelId, T M::*mPtr) {
      auto spModel = App::GetModel<M>(modelId);
      m_wpModel = spModel.get_spModel();
      if (spModel.isValid()) {
        m_fieldPtr = &(spModel->*mPtr);
        if (m_pMyView) {
          spModel.get_spModel()->AddBindedView(m_pMyView->m_wpThis);
        }
      }
      else {
        throw runtime_error("Can not bind to an object which is destroied.");
      }
    }

    void Bind(string modelId) {
      auto spModel = App::GetModel<T>(modelId);
      m_wpModel = spModel.get_spModel();
      if (spModel.isValid()) {
        m_fieldPtr = spModel;
        if (m_pMyView) {
          spModel.get_spModel()->AddBindedView(m_pMyView->m_wpThis);
        }
      }
      else {
        throw runtime_error("Can not bind to an object which is destroied.");
      }
    }

    void UnBind() {
      auto spModel = m_wpModel.lock();
      if (spModel) {
        spModel->RemoveBindedView(m_pMyView->m_wpThis);
      }
      m_fieldPtr = &m_fallback;
    }

    ModelSafePtr<T> SafePtr() {
      auto spModel = m_wpModel.lock();
      if (!spModel) UnBind();
      return ModelSafePtr<T>{m_fieldPtr, spModel};
    }

    ModelRef<T>& operator=(const T &m) {
      auto spModel = m_wpModel.lock();
      if (!spModel) UnBind();
      *m_fieldPtr = m;
      return *this;
    }

    ModelSafePtr<T> operator->() {
      return SafePtr();
    }

  };

}
