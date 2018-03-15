#pragma once

#include <map>
#include <set>
#include <string>
#include <memory>
#include <wincodec.h>
#include "Types.h"
#include "ConstructorProxy.h"

using namespace std;

namespace mvc {
  class Window;

  class App {

    friend class Window;

  private:
    static ComLibrary s_comLib;
    static map<string, SPView> s_viewsWithId;
    static map<string, SPModel> s_models;
    static list<SPView> s_allViews;

  public:

    static DxResource<ID2D1Factory1> s_pDirect2dFactory;
    static DxResource<IDWriteFactory1> s_pDWriteFactory;
    static DxResource<IWICImagingFactory> s_pImagingFactory;

    static double DPI_SCALE_X;
    static double DPI_SCALE_Y;

    static ViewBase* mainWnd;

    static void Initialize()
    {
      // create the d2d factory.
      D2D1_FACTORY_OPTIONS options;
      ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));
      HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        s_pDirect2dFactory.GetGUID(),
        &options,
        reinterpret_cast<void**>(&s_pDirect2dFactory));

      if (!SUCCEEDED(hr)) {
        CoUninitialize();
        throw std::system_error(EINTR, std::system_category(), "Direct2D is not initialized successfully.");
      }

      // create the dwrite factory.
      hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        s_pDWriteFactory.GetGUID(),
        reinterpret_cast<IUnknown**>(&s_pDWriteFactory));

      if (!SUCCEEDED(hr)) {
        CoUninitialize();
        throw std::system_error(EINTR, std::system_category(), "DirectWrite is not initialized successfully.");
      }

      float dpiX, dpiY;
      s_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      DPI_SCALE_X = dpiX / 96.0f;
      DPI_SCALE_Y = dpiY / 96.0f;

      hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory,
        reinterpret_cast<void**>(&s_pImagingFactory));

      if (!SUCCEEDED(hr)) {
        CoUninitialize();
        throw std::system_error(EINTR, std::system_category(), "Windows Imaging Component is not initialized successfully.");
      }
    }

    static DxResource<IDWriteTextFormat> CreateTextFormat(const WCHAR* fontName, float fontSize,
      DWRITE_FONT_WEIGHT fontWeight = DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE fontStyle = DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH fontStretch = DWRITE_FONT_STRETCH_NORMAL,
      const WCHAR *localeName = L"ja-JP"){
      return App::s_pDWriteFactory.GetResource<IDWriteTextFormat>(&IDWriteFactory::CreateTextFormat,
        fontName, nullptr, fontWeight, fontStyle, fontStretch, fontSize, localeName);
    }

    static DxResource<IDWriteTextLayout> CreateTextLayout(const WCHAR* text, unsigned length,
      IDWriteTextFormat *textFormat, float maxWidth, float maxHeight){
      return App::s_pDWriteFactory.GetResource<IDWriteTextLayout>(&IDWriteFactory::CreateTextLayout,
        text, length, textFormat, maxWidth, maxHeight);
    }

    template<typename ...Args>
    static shared_ptr<Window> CreateMainWindow(string id, Args ...args){
      auto w = make_shared<Window>(args...);
      w->m_wpThis = w;
      RegisterSubView(id, w);
      return w;
    }

    static void RegisterSubView(string id, const SPView &v){
      if (s_viewsWithId.find(id) != s_viewsWithId.end()) {
        throw std::runtime_error("The id exists already." + id);
      }
      s_viewsWithId.insert({ id, v });
    }

    static void RegisterSubView(const SPView &v){
      s_allViews.push_back(v);
    }

    template <typename T>
    static ModelSafePtr<T> CreateModel(string id, const ConstructorProxy<Model<T>> &cp) {
      if (s_models.find(id) != s_models.end()) {
        throw std::runtime_error("The id exists already." + id);
      }

      shared_ptr<Model<T>> ptr = cp.GetSP();
      s_models.insert({ id, ptr });

      // save the weak pointer of T to the object itself.
      ptr->m_wpThis = ptr;

      return ptr->get_safeptr();
    }

    static void RemoveView(string id)
    {
      auto it = s_viewsWithId.find(id);
      if (it != s_viewsWithId.end()) {
        if (it->second) {
          it->second.reset();
        }
        s_viewsWithId.erase(it);
      }
    }

    static void RemoveModel(string id)
    {
      auto it = s_models.find(id);
      if (it != s_models.end()) {
        if (it->second) {
          it->second.reset();
        }
        s_models.erase(it);
      }
    }

    template <typename T>
    static shared_ptr<T> GetView(string id) {
      if (s_viewsWithId.find(id) == s_viewsWithId.end()) {
        throw std::runtime_error("Can not find the view with id: " + id + ".");
      }
      auto ptr = dynamic_pointer_cast<T, ViewBase>(s_viewsWithId[id]);
      return ptr;
    }

    template <typename T>
    static ModelSafePtr<T> GetModel(string id) {
      if (s_models.find(id) == s_models.end()) {
        throw std::runtime_error("Can not find the model with id: " + id + ".");
      }
      auto ptrm = static_pointer_cast<Model<T>, ModelBase>(s_models[id]);
      return ptrm->get_safeptr();
    }
  };
}
