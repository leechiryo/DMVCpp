#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Windows �w�b�_�[����g�p����Ă��Ȃ����������O���܂��B
// Windows �w�b�_�[ �t�@�C��:
#include <windows.h>

// C �����^�C�� �w�b�_�[ �t�@�C��
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: �v���O�����ɕK�v�Ȓǉ��w�b�_�[�������ŎQ�Ƃ��Ă��������B
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