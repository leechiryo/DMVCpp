#include "Label.h"
#include "Button.h"
#include "CheckBox.h"
#include "Dialog.h"
#include "Layer.h"
#include "Radio.h"
#include "TextBox.h"

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

    }
  } _xmlTagInitializer;
}
