#pragma once

#include "Types.h"
#include "ModelSafePtr.h"
#include "Model.h"

using namespace std;

namespace mvc {
  template<typename T>
  class ModelRef {

    friend class ViewBase;
    typedef void(*ConvertFunc)(void *, T&);

  private:
    void *m_source;  // 仅在引用的对象类型与希望表现的类型(T)不同时使用。指向实际引用的对象。
    T m_fallback;
    T *m_fieldPtr;
    WPModel m_wpModel;

    std::function<void(void*, T&)> m_convertFunc;

  public:

    template<typename ... Args>
    ModelRef(Args ... args) : m_fallback(args...), m_wpModel() {
      m_fieldPtr = &m_fallback;
      m_source = nullptr;
      m_convertFunc = nullptr;
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
      }
      else {
        throw runtime_error("Can not bind to an object which is destroied.");
      }
    }

    template<typename M, typename S>
    void Bind(string modelId, S M::*mPtr, std::function<void(S*, T&)> convertFunc) {
      auto spModel = App::GetModel<M>(modelId);
      m_wpModel = spModel.get_spModel();
      if (spModel.isValid()) {
        m_source = &(spModel->*mPtr);
        m_convertFunc = *(reinterpret_cast<decltype(m_convertFunc)*>(&convertFunc));
        m_fieldPtr = &m_fallback;  // unbind
      }
      else {
        throw runtime_error("Can not bind to an object which is destroied.");
      }
    }

    template<typename S>
    void Bind(string modelId, std::function<void(S*, T&)> convertFunc) {
      auto spModel = App::GetModel<S>(modelId);
      m_wpModel = spModel.get_spModel();
      if (spModel.isValid()) {
        m_source = spModel;
        m_convertFunc = *(reinterpret_cast<decltype(m_convertFunc)*>(&convertFunc));
        m_fieldPtr = &m_fallback;  // unbind
      }
      else {
        throw runtime_error("Can not bind to an object which is destroied.");
      }
    }

    template<typename S>
    void Link(ModelRef<S> &ref, std::function<void(ModelRef<S>*, T&)> convertFunc) {
      m_source = &ref;
      m_convertFunc = *(reinterpret_cast<decltype(m_convertFunc)*>(&convertFunc));
      m_fieldPtr = &m_fallback;  // unbind
    }

    void UnBind() {
      m_fieldPtr = &m_fallback;
    }

    void UpdateConvertedValue(){
      if (m_source && m_convertFunc){
        m_convertFunc(m_source, m_fallback);
      }
    }

    ModelSafePtr<T> SafePtr() {
      auto spModel = m_wpModel.lock();
      if (!spModel) UnBind();
      UpdateConvertedValue();
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

    operator T () {
      auto spModel = m_wpModel.lock();
      if (!spModel) UnBind();
      UpdateConvertedValue();
      return *m_fieldPtr;
    }

  };
}
