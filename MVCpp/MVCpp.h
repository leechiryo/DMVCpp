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
#include <initguid.h>


// TODO: プログラムに必要な追加ヘッダーをここで参照してください。
#include "resource.h"

#include "mvc\mvc.h"
#include "mvc\ViewElements\Window.h"
#include "mvc\ViewElements\Button.h"
#include "mvc\ViewElements\TextBox.h"
#include "mvc\ViewElements\Label.h"
#include "mvc\ViewElements\CheckBox.h"
#include "mvc\ViewElements\Radio.h"