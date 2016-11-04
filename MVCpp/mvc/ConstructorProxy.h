#pragma once
#include <memory>

/*
 * 此类利用接收到的构造参数，在全局堆上生成(new)相应的View和Model对象。
 * 它的用途是作为v函数和m函数的参数。
 * 如果v函数和m函数的参数不使用该对象，而是直接接受View对象和Model对象，
 * 则参数将存在于函数的局部栈上，难以将生命期扩展到全局。
 * 如果v函数和m函数的参数不使用该对象，而是直接传递View对象和Model对象
 * 的构造参数，那么又无法使用{}-list写法。
 * 让该对象作为v函数和m函数的参数，我们在调用时可以使用{}-list写法构造出
 * 一个VMObjCreator对象，进而在内部new出一个View/Model对象。
 * 在v函数和m函数的内部，我们又可以将View/Model对象连同id注册给全局的map。
 * 解决了用{}-list调用v/m函数的问题。
 */

namespace mvc {
  template<typename T>
  class ConstructorProxy {
  private:
    std::shared_ptr<T> m_spT;

  public:
    template<typename... Args>
    ConstructorProxy(Args... args) {
      m_spT = make_shared<T>(args...);
    }

    std::shared_ptr<T> GetSP() const {
      return m_spT;
    }
  };
}
