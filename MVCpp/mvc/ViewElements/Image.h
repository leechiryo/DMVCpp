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

    Image(const D2DContext &context, LPCWSTR fileName) : View(context), m_fileName( fileName ) {
    }

    virtual void DrawSelf() {
      auto size = m_pBitmap->GetSize();
      m_right = m_left + size.width;
      m_bottom = m_top + size.height;
      D2D1_RECT_F rect = RectD(m_left, m_top, m_right, m_bottom);
      m_pContext->DrawBitmap(m_pBitmap.ptr(), &rect);
    }
  };
}
