#pragma once

#include "Types.h"

namespace mvc {
  class D2DContext : public DxResource<ID2D1DeviceContext> {
  public:
    D2DContext& operator=(const D2DContext& dxR){
      m_pResource = dxR.m_pResource;
      sp = dxR.sp;
      return *this;
    }

    D2DContext& operator=(D2DContext&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
      return *this;
    }

    D2DContext& operator=(DxResource<ID2D1DeviceContext>&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
      return *this;
    }

    template <typename... Args>
    DxResource<ID2D1SolidColorBrush> CreateSolidColorBrush(Args... args) {
      ID2D1SolidColorBrush *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateSolidColorBrush(args..., &resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to create the solid color brush");
      }

      return resource;
    }

    DxResource<ID2D1Bitmap1> CreateBitmap(IDXGISurface* surface, const D2D1_BITMAP_PROPERTIES1& prop)
    {
      ID2D1Bitmap1 *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateBitmapFromDxgiSurface(surface, prop, &resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to create the bitmap from DXGI surface.");
      }

      return resource;
    }

    DxResource<ID2D1Effect> CreateEffect(REFCLSID effectId) {
      ID2D1Effect *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateEffect(effectId, &resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to create the effect.");
      }

      return resource;
    }

    DxResource<ID2D1BitmapRenderTarget> CreateCompatibleRenderTarget() {
      ID2D1BitmapRenderTarget *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateCompatibleRenderTarget(&resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to create the bitmap render target.");
      }

      return resource;
    }

  };
}