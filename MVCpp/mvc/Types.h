#pragma once

#include <memory>
#include <set>
#include <d2d1_1.h>
#include <dwrite_1.h>
#include <d3d11_1.h>

namespace mvc {

  class ViewBase;
  class ModelBase;

  template<typename T>
  class View;

  template<typename T>
  class Model;

  template<typename T>
  class ModelRef;

  template<typename T>
  class ModelSafePtr;

  template <typename T>
  struct WeakPtrComparer {
    bool operator()(const std::weak_ptr<T> &x, const std::weak_ptr<T> &y) const {
      auto px = x.lock();
      auto py = y.lock();
      return px < py;
    }
  };

  typedef std::shared_ptr<ViewBase> SPView;
  typedef std::shared_ptr<ModelBase> SPModel;
  typedef std::weak_ptr<ViewBase> WPView;
  typedef std::weak_ptr<ModelBase> WPModel;

  typedef std::set<WPView, WeakPtrComparer<ViewBase>> WPViewSet;


  template<class Interface>
  inline void SafeRelease(Interface *pT) {
    if (pT != nullptr) {
      pT->Release();
      pT = nullptr;
    }
  }

  template<class T>
  inline D2D1_RECT_F RectD(T l, T t, T r, T b) {
    return D2D1::RectF(
      static_cast<float>(l),
      static_cast<float>(t),
      static_cast<float>(r),
      static_cast<float>(b));
  }

  template<class T>
  inline D2D1_POINT_2F Point2D(T x, T y) {
    return D2D1::Point2F(
      static_cast<float>(x),
      static_cast<float>(y));
  }

  template<class T>
  inline float tof(T x){
    return static_cast<float>(x);
  }

  // DirectX 资源管理
  template <typename T>
  class DxResource{
  private:
    T* m_pResource;
    std::shared_ptr<int> sp;

  public:
    DxResource(){
      m_pResource = nullptr;
      sp = std::make_shared<int>();
    }

    DxResource(T* pResource){
      m_pResource = pResource;
      sp = std::make_shared<int>();
    }

    DxResource(const DxResource& dxR) = delete;

    DxResource& operator=(const DxResource& dxR){
      m_pResource = dxR.m_pResource;
      sp = dxR.sp;
      return *this;
    }

    DxResource(DxResource&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
    }

    DxResource& operator=(DxResource&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
      std::swap(sp, dxR.sp);
      return *this;
    }

    T** operator&(){
      return &m_pResource;
    }

    void Clear(){
      if (sp.unique()){
        SafeRelease(m_pResource);
      }
    }

    ~DxResource(){
      if (sp.unique()){
        SafeRelease(m_pResource);
      }
    }

    inline REFIID GetGUID(){
      return __uuidof(T);
    }

    T* operator->(){
      return m_pResource;
    }

    T* ptr(){
      return m_pResource;
    }

    template<typename R>
    DxResource<R> Query(){
      R* pS;
      HRESULT hr = S_OK;
      hr = m_pResource->QueryInterface(__uuidof(R), (void **)&pS);

      if (hr != S_OK){
        throw std::runtime_error("Failed to query the interface.");
      }

      return pS;
    }

    template<typename R, typename OP, typename... Args>
    DxResource<R> Query(OP op, Args... args){
      R* pS;
      HRESULT hr = S_OK;
      hr = (m_pResource->*op)(args..., __uuidof(R), (void **)&pS);

      if (hr != S_OK){
        throw std::runtime_error("Failed to query the interface.");
      }

      return pS;
    }

    template<typename R>
    DxResource<R> GetParent(){
      R* pS;
      HRESULT hr = S_OK;
      hr = m_pResource->GetParent(__uuidof(R), (void **)&pS);

      if (hr != S_OK){
        throw std::runtime_error("Failed to get the parent of the interface.");
      }

      return pS;
    }

    template<typename R>
    DxResource<R> GetResource(HRESULT(__stdcall T::*op)(R**)) {
      R *resource;
      HRESULT hr = S_OK;

      hr = (m_pResource->*op)(&resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to get the directx resource.");
      }

      return resource;
    }

    template<typename R, typename OP, typename... Args>
    DxResource<R> GetResource(OP op, Args... args) {
      R *resource;
      HRESULT hr = S_OK;

      hr = (m_pResource->*op)(args..., &resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to get the directx resource.");
      }

      return resource;
    }
  };

#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#ifndef HINST_THISCOMPONENT
  EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif
}
