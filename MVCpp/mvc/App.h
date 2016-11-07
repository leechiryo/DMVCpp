#pragma once

#include <d2d1.h>
#include <dwrite.h>
#include <map>
#include <set>
#include <string>
#include <memory>
#include "Types.h"
#include "ConstructorProxy.h"

using namespace std;

namespace mvc {
  class App {

  private:
    static map<string, SPView> s_views;
    static map<string, SPModel> s_models;

  public:

    static ID2D1Factory* s_pDirect2dFactory;
    static IDWriteFactory* s_pDWriteFactory;

    static double DPI_SCALE_X;
    static double DPI_SCALE_Y;

    static ViewBase* mainWnd;

    static void Initialize()
    {
      HRESULT hr = CoInitialize(NULL);
      if (!SUCCEEDED(hr)) {
        throw std::system_error(EINTR, std::system_category(), "COM environment is not initialized successfully.");
      }
      // create the d2d factory.
      hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        &s_pDirect2dFactory);

      if (!SUCCEEDED(hr)) {
        CoUninitialize();
        throw std::system_error(EINTR, std::system_category(), "Direct2D is not initialized successfully.");
      }

      // create the dwrite factory.
      hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&s_pDWriteFactory));

      if (!SUCCEEDED(hr)) {
        SafeRelease(s_pDirect2dFactory);
        CoUninitialize();
        throw std::system_error(EINTR, std::system_category(), "DirectWrite is not initialized successfully.");
      }

      float dpiX, dpiY;
      s_pDirect2dFactory->GetDesktopDpi(&dpiX, &dpiY);

      DPI_SCALE_X = dpiX / 96.0f;
      DPI_SCALE_Y = dpiY / 96.0f;
    }

    static void Uninitialize() {
      SafeRelease(s_pDWriteFactory);
      SafeRelease(s_pDirect2dFactory);
      CoUninitialize();
    }

    static void UpdateViews();

    template <typename T>
    static shared_ptr<T> CreateView(string id, const ConstructorProxy<T> &cp) {
      if (s_views.find(id) != s_views.end()) {
        throw std::runtime_error("The id exists already." + id);
      }

      shared_ptr<T> ptr = cp.GetSP();
      s_views.insert({ id, ptr });

      // save the weak pointer of T to the object itself.
      ptr->m_wpThis = ptr;

      return ptr;
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
      auto it = s_views.find(id);
      if (it != s_views.end()) {
        if (it->second) {
          it->second.reset();
        }
        s_views.erase(it);
      }
    }

    static void RemoveModel(string id)
    {
      auto it = s_models.find(id);
      if (it != s_models.end()) {
        if (it->second) {
          int cnt = it->second.use_count();
          it->second.reset();
        }
        s_models.erase(it);
      }
    }

    template <typename T>
    static shared_ptr<T> GetView(string id) {
      if (s_views.find(id) == s_views.end()) {
        throw std::runtime_error("Can not find the view.");
      }
      auto ptr = dynamic_pointer_cast<T, ViewBase>(s_views[id]);
      return ptr;
    }

    template <typename T>
    static ModelSafePtr<T> GetModel(string id) {
      if (s_models.find(id) == s_models.end()) {
        throw std::runtime_error("Can not find the model.");
      }
      auto ptrm = dynamic_pointer_cast<Model<T>, ModelBase>(s_models[id]);
      return ptrm->get_safeptr();
    }
  };
}
