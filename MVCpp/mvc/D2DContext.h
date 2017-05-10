#pragma once

#include "Types.h"

namespace mvc {
  class D2DContext : public DxResource<ID2D1DeviceContext> {
  public:
    D2DContext& operator=(const D2DContext& dxR) {
      m_pResource = dxR.m_pResource;
      sp = dxR.sp;
      return *this;
    }

    D2DContext& operator=(D2DContext&& dxR) {
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
      return *this;
    }

    D2DContext& operator=(DxResource<ID2D1DeviceContext>&& dxR) {
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
      return *this;
    }

    template <typename... Args>
    DxResource<ID2D1SolidColorBrush> CreateSolidColorBrush(Args... args) {
      ID2D1SolidColorBrush *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateSolidColorBrush(args..., &resource);

      if (hr != S_OK) {
        throw std::runtime_error("Failed to create the solid color brush");
      }

      return resource;
    }

    DxResource<ID2D1Bitmap1> CreateBitmap(IDXGISurface* surface, const D2D1_BITMAP_PROPERTIES1& prop)
    {
      ID2D1Bitmap1 *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateBitmapFromDxgiSurface(surface, prop, &resource);

      if (hr != S_OK) {
        throw std::runtime_error("Failed to create the bitmap from DXGI surface.");
      }

      return resource;
    }

    DxResource<ID2D1Bitmap1> LoadImageFile(wstring filePath) {
      DxResource<IWICBitmapDecoder> m_pDecoder;
      DxResource<IWICBitmapFrameDecode> m_pSource;
      DxResource<IWICFormatConverter> m_pConverter;
      m_pDecoder = App::s_pImagingFactory.GetResource<IWICBitmapDecoder>(&IWICImagingFactory::CreateDecoderFromFilename, filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad);
      m_pSource = m_pDecoder.GetResource<IWICBitmapFrameDecode>(&IWICBitmapDecoder::GetFrame, 0);
      m_pConverter = App::s_pImagingFactory.GetResource<IWICFormatConverter>(&IWICImagingFactory::CreateFormatConverter);
      HRESULT hr = m_pConverter->Initialize(m_pSource.ptr(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0.0f, WICBitmapPaletteTypeMedianCut);
      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to initialize image format converter.");
      }

      ID2D1Bitmap1 *pmap;
      hr = m_pResource->CreateBitmapFromWicBitmap(m_pConverter.ptr(), nullptr, &pmap);
      if (!SUCCEEDED(hr)) {
        throw std::runtime_error("Failed to create bitmap from wic bitmap.");
      }

      return pmap;
    }

    DxResource<ID2D1Effect> CreateEffect(REFCLSID effectId) {
      ID2D1Effect *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateEffect(effectId, &resource);

      if (hr != S_OK) {
        throw std::runtime_error("Failed to create the effect.");
      }

      return resource;
    }


    DxResource<ID2D1BitmapRenderTarget> CreateCompatibleRenderTarget() {
      ID2D1BitmapRenderTarget *resource;
      HRESULT hr = S_OK;

      hr = m_pResource->CreateCompatibleRenderTarget(&resource);

      if (hr != S_OK) {
        throw std::runtime_error("Failed to create the bitmap render target.");
      }

      return resource;
    }

    DxResource<IDWriteTextLayout> DrawText(const wstring *text, IDWriteTextFormat *textFormat,
      const D2D1_RECT_F &rect, ID2D1SolidColorBrush *brush) {
      return DrawText(text, text->length(), textFormat, rect, brush);
    }

    DxResource<IDWriteTextLayout> DrawText(const wstring *text, size_t charCnt, IDWriteTextFormat *textFormat,
      const D2D1_RECT_F &rect, ID2D1SolidColorBrush *brush) {
      auto layout = App::s_pDWriteFactory.GetResource<IDWriteTextLayout>(&IDWriteFactory::CreateTextLayout,
        text->c_str(), charCnt, textFormat, rect.right - rect.left, rect.bottom - rect.top);
      layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
      D2D1_POINT_2F o;
      o.x = rect.left;
      o.y = rect.top;
      m_pResource->DrawTextLayout(o, layout.ptr(), brush);
      return layout;
    }

    DxResource<IDWriteTextLayout> GetTextLayout(const wstring *text, size_t charCnt, IDWriteTextFormat *textFormat,
      const D2D1_RECT_F &rect, ID2D1SolidColorBrush *brush) {
      auto layout = App::s_pDWriteFactory.GetResource<IDWriteTextLayout>(&IDWriteFactory::CreateTextLayout,
        text->c_str(), charCnt, textFormat, rect.right - rect.left, rect.bottom - rect.top);
      layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
      return layout;
    }
  };
}