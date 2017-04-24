#pragma once

#include <map>
#include <string>
#include <memory>
#include "App.h"
#include "Model.h"
#include "ModelRef.h"

#include "..\rapidxml\rapidxml.hpp"

using namespace std;
using namespace rapidxml;

namespace mvc {

  template<typename T>
  shared_ptr<T> v(string id, const ConstructorProxy<T> &cp) {
    return App::CreateView<T>(id, cp);
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

  void load_xml(xml_node<> *node){
    MessageBoxA(NULL, node->name(), "node name", MB_OK);
    for (xml_node<> *child_node = node->first_node(); child_node; child_node = child_node->next_sibling()){
      load_xml(child_node);
    }
  }

  void load_views (int resourceId) {
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId), RT_RCDATA);
    DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hRes);
    HGLOBAL hglob = LoadResource(GetModuleHandle(NULL), hRes);

    const char * pData = reinterpret_cast<const char*>(::LockResource(hglob));

    char * xmlString = new char[dwSize + 1];
    strcpy_s(xmlString, dwSize + 1, pData);
    xmlString[dwSize] = 0;

    xml_document<> doc;
    doc.parse<0>(xmlString);

    load_xml(doc.first_node());
  }
}
