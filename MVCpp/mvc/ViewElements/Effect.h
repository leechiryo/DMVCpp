#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  class Effect : public View<Effect>
  {
  private:
    REFCLSID m_effectClsId;
    // D2D 资源(离开作用域时会自动销毁)
    DxResource<ID2D1Effect> m_effect;

  protected:

    virtual void CreateD2DResource() {
      m_effect = m_pContext.CreateEffect(m_effectClsId);
    }

  public:

    Effect(const D2DContext &context, REFCLSID effectClsId) : View(context), m_effectClsId(effectClsId) {
    }

    template<typename T, typename U>
    HRESULT SetValue(U index, const T &value) {
      return m_effect->SetValue(index, value);
    }

    virtual void DrawSelf() {

      for (auto v : m_subViews){

        auto srcView = v.lock();

        if (srcView){

          srcView->SetHidden(false);

          auto bmpRT = m_pContext.CreateCompatibleRenderTarget();
          srcView->m_pContext = bmpRT.Query<ID2D1DeviceContext>();
          srcView->m_pContext->BeginDraw();
          srcView->m_pContext->Clear(D2D1::ColorF(0xffffff, 0.0f));
          srcView->Draw();
          srcView->m_pContext->EndDraw();

          srcView->SetHidden(true);

          auto bmp = bmpRT.GetResource<ID2D1Bitmap>(&ID2D1BitmapRenderTarget::GetBitmap);
          m_effect->SetInput(0, bmp.ptr());
          m_pContext->DrawImage(m_effect.ptr());

        }
      }

    }
  };
}
