#pragma once

#include "Types.h"

using namespace std;

namespace mvc {
  template<typename T>
  class ModelSafePtr {
  private:
    T *m_fieldPtr;     // 这个指针可以指向Model实例本身，也可能指向Model实例的某个字段
    SPModel m_spModel;  // 这个共享指针确保在访问Model时，程序的其它部分不会删除该Model。

  public:
    ModelSafePtr(T *p, const shared_ptr<ModelBase> &pm) {
      m_fieldPtr = p;
      m_spModel = pm;
    }

    bool isValid() const {
      if (m_spModel) return true;
      else return false;
    }

    T* operator->() {
      return m_fieldPtr;
    }

    operator T*() const {
      return m_fieldPtr;
    }

    SPModel get_spModel() {
      return m_spModel;
    }
  };
}
