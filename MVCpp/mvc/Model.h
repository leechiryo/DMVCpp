#pragma once

/*
 * Model提供了一个方法检测Model的值的变化情况，如果
 * 发现了值发生了变化，该方法将返回True。
*/

#include "Types.h"
#include "ModelSafePtr.h"
#include "ViewBase.h"

namespace mvc {

  class ModelBase{
  };

  template <typename T>
  class Model : public ModelBase {
    friend class App;
    friend class ModelRef<T>;

  private:
    T m_model;

    // 此弱指针用于生成指向自身的共享指针发送给外部。
    // 注意：此处不能用普通指针，否则将会产生多个无关的共享指针副本。
    //      此处也不能直接使用共享指针，否则由于对象内部始终保有自己
    //      的共享指针而导致无法删除该对象
    weak_ptr<Model<T>> m_wpThis;

    ModelSafePtr<T> get_safeptr() {
      auto spThis = m_wpThis.lock();
      if (spThis) {
        return ModelSafePtr<T>{&m_model, spThis};
      }
      else {
        throw runtime_error("The model has been deleted.");
      }
    }

  public:
    template<typename... Args>
    Model(Args... args) : m_model(args...) {
    }

  };
}
