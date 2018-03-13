#include "mvc.h"

namespace mvc{

  void regv(string id, SPView v){
    App::RegisterSubView(id, v);
  }

  void load_layout(xml_node<> *layoutNode, Layout * layout) {
    xml_node<> *rowsNode = layoutNode->first_node("rows");
    for (xml_node<> *row_node = rowsNode->first_node(); row_node; row_node = row_node->next_sibling()) {
      xml_attribute<> * attr = row_node->first_attribute("height");
      char * heightStr = attr->value();
      layout->AddRow(heightStr);
    }

    xml_node<> * colsNode = layoutNode->first_node("cols");
    for (xml_node<> *col_node = colsNode->first_node(); col_node; col_node = col_node->next_sibling()) {
      xml_attribute<> * attr = col_node->first_attribute("width");
      char * widthStr = attr->value();
      layout->AddCol(widthStr);
    }
  }

  void load_xml(xml_node<> *node, SPView currentView){
    if (!strcmp(node->name(), "layout")) {
      Layout layout;
      load_layout(node, &layout);
    }
    else {

      if (!strcmp(node->name(), "window")) {
        xml_attribute<> * idAttr = node->first_attribute("id");
        xml_attribute<> * textAttr = node->first_attribute("text");
        xml_attribute<> * widthAttr = node->first_attribute("width");
        xml_attribute<> * heightAttr = node->first_attribute("height");
        if (idAttr){

          char * id = idAttr->value();
          wchar_t ttl[200] = L"MVC Application";
          int width = 800;
          int height = 600;

          if (textAttr){
            char *text = textAttr->value();
            MultiByteToWideChar(CP_UTF8, 0, text, -1, ttl, 200);
          }
          if (widthAttr){
            char * widthVal = widthAttr->value();
            width = stoi(widthVal);
          }
          if (heightAttr){
            char * heightVal = heightAttr->value();
            height = stoi(heightVal);
          }

          currentView = v(id, ttl, width, height);
        }
      }
      else{

        auto & s_xmlLoaders = ViewBase::GetXmlLoaders();

        auto it = s_xmlLoaders.find(node->name());

        if (s_xmlLoaders.end() != it){

          char * id = "";
          xml_attribute<> * idAttr = node->first_attribute("id");
          if (idAttr){
            char * id = idAttr->value();
          }

          map<string, wstring> xmlSettings;
          for (const xml_attribute<>* a = node->first_attribute(); a; a = a->next_attribute()){
            char * aName = a->name();
            char * aVal = a->value();

            int size = MultiByteToWideChar(CP_UTF8, 0, aVal, -1, NULL, 0);
            wchar_t * wide = new wchar_t[size];
            MultiByteToWideChar(CP_UTF8, 0, aVal, -1, &wide[0], size);

            wstring value = wide;
            xmlSettings.insert({ aName, value });
          }

          currentView = it->second(currentView, id, xmlSettings);
        }
      }

      for (xml_node<> *child_node = node->first_node(); child_node; child_node = child_node->next_sibling()){
        load_xml(child_node, currentView);
      }

    }
  }

  void load_views(int resourceId) {
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(resourceId), RT_RCDATA);
    DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hRes);
    HGLOBAL hglob = LoadResource(GetModuleHandle(NULL), hRes);

    const char * pData = reinterpret_cast<const char*>(::LockResource(hglob));

    char * xmlString = new char[dwSize + 1];
    strcpy_s(xmlString, dwSize + 1, pData);
    xmlString[dwSize] = 0;

    xml_document<> doc;
    doc.parse<0>(xmlString);

    load_xml(doc.first_node(), nullptr);
  }

}

