#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  class Image : public View<Image>
  {
  private:
    wstring m_fileName;

  protected:

    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1Bitmap1> m_pBitmap;

    virtual void CreateD2DResource() {
      m_pBitmap = m_pContext.LoadImageFile(m_fileName);
    }

  public:

    Image(LPCWSTR fileName) : m_fileName{ fileName } {
    }

    virtual void DrawSelf() {
      m_pContext->DrawBitmap(m_pBitmap.ptr());
    }
  };
}
