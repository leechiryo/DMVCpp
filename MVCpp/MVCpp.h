#pragma once

#include "resource.h"

template<class Interface>
inline void SafeRelease(Interface *ppInterfaceToRelease) {
  if (ppInterfaceToRelease != NULL)
  {
    (ppInterfaceToRelease)->Release();

    (ppInterfaceToRelease) = NULL;
  }
}