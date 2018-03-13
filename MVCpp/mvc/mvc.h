#pragma once

#include <map>
#include <string>
#include <memory>
#include "App.h"
#include "Model.h"
#include "ModelRef.h"
#include "Layout.h"

#include "..\rapidxml\rapidxml.hpp"

using namespace std;
using namespace rapidxml;

namespace mvc {

  class Window;

  template<typename ...Args>
  shared_ptr<Window> v(string id, Args ... args) {
    auto w = App::CreateMainWindow(id, args...);
    return w;
  }

  template<typename T>
  ModelSafePtr<T> m(string id, const ConstructorProxy<Model<T>> &cp) {
    return App::CreateModel<T>(id, cp);
  }

  template<typename T>
  shared_ptr<T> getv(string id) {
    return App::GetView<T>(id);
  }

  template<typename T>
  ModelSafePtr<T> getm(string id) {
    return App::GetModel<T>(id);
  }

  void regv(string id, SPView v);

  void load_layout(xml_node<> *layoutNode, SPView * view);

  void load_xml(xml_node<> *node, SPView currentView);

  void load_views(int resourceId);

}
