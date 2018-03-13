
namespace mvc{

  // 一段用于初始化的代码，注册所有控件的XML tag定义。
  class XmlTagInitializer{
  public:
    XmlTagInitializer(){

      // 此处的代码会在main函数之前执行。
      Label::SetXmlTag("label");
      Button::SetXmlTag("button");
      CheckBox::SetXmlTag("checkbox");
      Dialog::SetXmlTag("dialog");
      Layer::SetXmlTag("layer");
      Radio::SetXmlTag("radio");
      TextBox::SetXmlTag("textbox");
      Image::SetXmlTag("img");
      Line::SetXmlTag("line");
      Chart::SetXmlTag("chart");
      TickProvider::SetXmlTag("tickprovider");

    }
  } _xmlTagInitializer;
}
