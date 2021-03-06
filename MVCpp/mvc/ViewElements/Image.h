﻿#pragma once

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

    Image(const D2DContext &context, Window *parentWnd, LPCWSTR fileName) : View(context, parentWnd), m_fileName( fileName ) {
    }

    // 用于XML构造的函数
    Image(const D2DContext &context, Window *parentWnd, const map<string, wstring> &xmlSettings) 
      : Image(context, parentWnd, L""){

      auto it = xmlSettings.find("src");
      if (it != xmlSettings.end()){
        m_fileName = it->second;
      }

    }

    virtual float GetDefaultWidth(){
      auto size = m_pBitmap->GetSize();
      return size.width;
    }

    virtual float GetDefaultHeight(){
      auto size = m_pBitmap->GetSize();
      return size.height;
    }

    virtual void DrawSelf() {
      m_right = m_left + m_calWidth;
      m_bottom = m_top + m_calHeight;

      D2D1_RECT_F rect = RectD(m_left, m_top, m_right, m_bottom);
      m_pContext->DrawBitmap(m_pBitmap.ptr(), &rect);
    }
  };
}
