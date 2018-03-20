#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーから使用されていない部分を除外します。
// Windows ヘッダー ファイル:
#include <windows.h>

// C ランタイム ヘッダー ファイル
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include <initguid.h>
#include <map>
#include <set>
#include <string>
#include <memory>
#include <wincodec.h>
#include <functional>

#include "resource.h"
#include "rapidxml\rapidxml.hpp"

#include "mvc\mvc.h"
#include "mvc\Controls\Window.h"
#include "mvc\Controls\Button.h"
#include "mvc\Controls\TextBox.h"
#include "mvc\Controls\Label.h"
#include "mvc\Controls\CheckBox.h"
#include "mvc\Controls\Radio.h"
#include "mvc\Controls\Layer.h"
#include "mvc\Controls\Dialog.h"
#include "mvc\Controls\Chart.h"
#include "mvc\Controls\TickProvider.h"
#include "mvc\ViewElements\Image.h"
#include "mvc\ViewElements\Line.h"
#include <system_error>
#include "mvc/DataModel/DateTime.h"
#include "mvc/DataModel/TickPrice.h"

#include "MyController.h"