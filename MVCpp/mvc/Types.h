#pragma once

#include <memory>
#include <list>
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

  typedef std::list<WPView> WPViewList;


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
  inline D2D1::Matrix3x2F TranslationMatrix(T x, T y) {
    return D2D1::Matrix3x2F::Translation(
      static_cast<float>(x),
      static_cast<float>(y));
  }

  template<class T>
  inline float tof(T x){
    return static_cast<float>(x);
  }

  class ScopeGuard{
  public:
    explicit ScopeGuard(std::function<void()> onExitScope)
      : onExitScope_(onExitScope), dismissed_(false)
    { }

    ~ScopeGuard()
    {
      if (!dismissed_)
      {
        onExitScope_();
      }
    }

    // 如果调用此函数，则会取消析构函数的执行。
    void Dismiss()
    {
      dismissed_ = true;
    }

  private:
    std::function<void()> onExitScope_;
    bool dismissed_;

  private: // noncopyable
    ScopeGuard(ScopeGuard const&);
    ScopeGuard& operator=(ScopeGuard const&);
  };

  // Com 的初始化和销毁管理
  class ComLibrary{
  public:
    ComLibrary(){
      HRESULT hr = CoInitialize(NULL);
      if (!SUCCEEDED(hr)) {
        throw std::system_error(EINTR, std::system_category(), "COM environment is not initialized successfully.");
      }
    }

    ~ComLibrary(){
      CoUninitialize();
    }
  };

  // DirectX 资源管理
  template <typename T>
  class DxResource{
    friend class D2DContext;
  private:
    std::shared_ptr<T> m_pResource;

  public:
    DxResource(){
    }

    DxResource(T* pResource){
      m_pResource.reset(pResource, [](T *p){
        SafeRelease(p);
      });
    }

    DxResource(const DxResource& dxR){
      m_pResource = dxR.m_pResource;
    }

    DxResource& operator=(const DxResource& dxR){
      m_pResource = dxR.m_pResource;
      return *this;
    }

    DxResource(DxResource&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
    }

    DxResource& operator=(DxResource&& dxR){
      std::swap(m_pResource, dxR.m_pResource);
      return *this;
    }

    bool NotSet(){
      return m_pResource == nullptr;
    }

    ~DxResource(){
      m_pResource.reset();
    }

    inline REFIID GetGUID(){
      return __uuidof(T);
    }

    T* operator->(){
      return m_pResource.get();
    }

    T* ptr(){
      return m_pResource.get();
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
      hr = (m_pResource.get()->*op)(args..., __uuidof(R), (void **)&pS);

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

      hr = (m_pResource.get()->*op)(&resource);

      if (hr != S_OK){
        throw std::runtime_error("Failed to get the directx resource.");
      }

      return resource;
    }

    template<typename R, typename OP, typename... Args>
    DxResource<R> GetResource(OP op, Args... args) {
      R *resource;
      HRESULT hr = S_OK;

      hr = (m_pResource.get()->*op)(args..., &resource);

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
