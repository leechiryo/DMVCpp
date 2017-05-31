#pragma once

#include "Types.h"
#include "D2DContext.h"
#include "App.h"
#include "Layout.h"
#include <tuple>
#include "Animation.h"

using namespace std;

namespace mvc {

  class Layer;

  struct LayoutInfo {
    float m_leftOffset;
    float m_topOffset;
    float m_rightOffset;
    float m_bottomOffset;
    char m_setWidth[10];
    char m_setHeight[10];
  };

  class ViewBase {

    template<typename T>
    friend class View;

    friend class Window;

  private:
    bool m_hidden;

    float m_innerClipAreaLeftOffset;
    float m_innerClipAreaTopOffset;
    float m_innerClipAreaRightOffset;
    float m_innerClipAreaBottomOffset;

    D2DContext m_pEffectContext;
    DxResource<ID2D1BitmapRenderTarget> m_pBmpRT;
    bool m_showEffect = false;
    list<tuple<DxResource<ID2D1Effect>, int>> m_effects;

    Window * m_parentWnd;

    bool isNumber(const char *str) {
      int len = strlen(str);
      // check for empty string.
      if (len == 0) return false;
      bool retval = true;
      for (int i = 0; i < len; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }
      return retval;
    }

    bool isPercent(const char *str) {
      int len = strlen(str);
      // check for empty and the only one character '%' 
      // should return false.
      if (len <= 1) return false;
      bool retval = true;
      for (int i = 0; i < len - 1; i++) {
        retval = retval && (str[i] >= '0') && (str[i] <= '9');
      }

      retval = retval && (str[len - 1] == '%');
      return retval;
    }

  protected:
    WPView m_wpThis;
    WPViewList m_subViews;

    double m_absLeft;
    double m_absTop;

    double m_left;
    double m_top;
    double m_right;
    double m_bottom;

    Layout m_layout;

    Layout *m_parentLayout;
    int m_row;
    int m_col;

    float m_leftOffset;
    float m_topOffset;
    float m_rightOffset;
    float m_bottomOffset;
    char m_setWidth[10];
    char m_setHeight[10];

    float m_calWidth;
    float m_calHeight;
    float m_oldWidth;
    float m_oldHeight;

    bool m_mouseIn = 0;
    bool m_canBeFocused = true;
    bool m_focused = false;

    // 指向Window对象的D2DContext字段的指针。每个Window都有一个独立的D2DContext对象，
    // 其内部的所有subview将共享这一对象，并利用该对象进行绘制。
    D2DContext m_pContext;

    virtual void CreateD2DResource() = 0;
    virtual char HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT &result, WPView &eventView) = 0;
    virtual WPView GetClickedSubView(int pixelX, int pixelY) = 0;

    // 如果需要处理鼠标进入事件，可以重载此函数
    virtual void MouseEnter(double x, double y) {
    }

    // 如果需要处理鼠标离开事件，可以重载此函数
    virtual void MouseLeft(double x, double y) {
    }

    void SaveLayout(LayoutInfo *l) {
      l->m_leftOffset = m_leftOffset;
      l->m_topOffset = m_topOffset;
      l->m_rightOffset = m_rightOffset;
      l->m_bottomOffset = m_bottomOffset;
      strcpy_s(l->m_setWidth, m_setWidth);
      strcpy_s(l->m_setHeight, m_setHeight);
    }

    void RestoreLayout(const LayoutInfo &l) {
      m_leftOffset = l.m_leftOffset;
      m_topOffset = l.m_topOffset;
      m_rightOffset = l.m_rightOffset;
      m_bottomOffset = l.m_bottomOffset;
      strcpy_s(m_setWidth, l.m_setWidth);
      strcpy_s(m_setHeight, l.m_setHeight);
      UpdatePositionAndSize();
    }

    void RebuildD2DEnvironment() {
      CreateD2DResource();

      for (auto e : m_subViews) {
        auto spv = e.lock();
        if (spv) {
          spv->m_pContext = m_pContext;
          spv->RebuildD2DEnvironment();
        }
      }
    }

    float AbsPixelX2RelX(int absPixelX) {
      return tof((absPixelX - m_absLeft) / App::DPI_SCALE_X);
    }

    float AbsPixelY2RelY(int absPixelY) {
      return tof((absPixelY - m_absTop) / App::DPI_SCALE_Y);
    }

    virtual bool HitTest(double dipX, double dipY) {
      return dipX >= 0 && dipX <= (m_right - m_left)
        && dipY >= 0 && dipY <= (m_bottom - m_top);
    }

    bool HitTest(int pixelsX, int pixelsY) {
      double dipX = AbsPixelX2RelX(pixelsX);
      double dipY = AbsPixelY2RelY(pixelsY);

      return HitTest(dipX, dipY);
    }

    void UpdatePositionAndSize() {
      const GridCell * cell = m_parentLayout->GetCell(m_row, m_col);

      // 计算View的宽度
      if (!isnan(m_leftOffset) && !(isnan(m_rightOffset))) {
        m_calWidth = cell->width - m_leftOffset - m_rightOffset;
      }
      else if (strlen(m_setWidth) > 0) {
        if (isNumber(m_setWidth)) {
          m_calWidth = (float)atoi(m_setWidth);
        }
        else if (isPercent(m_setWidth)) {
          char dg[10] = { 0 };
          strcpy_s(dg, strlen(m_setWidth) + 1, m_setWidth);
          dg[strlen(m_setWidth) - 1] = 0;
          int a = atoi(dg);
          m_calWidth = a * cell->width / 100.0f;
        }
      }
      else {
        m_calWidth = GetDefaultWidth();
      }

      // 计算View的高度
      if (!isnan(m_topOffset) && !(isnan(m_bottomOffset))) {
        m_calHeight = cell->height - m_topOffset - m_bottomOffset;
      }
      else if (strlen(m_setHeight) > 0) {
        if (isNumber(m_setHeight)) {
          m_calHeight = (float)atoi(m_setHeight);
        }
        else if (isPercent(m_setHeight)) {
          char dg[10] = { 0 };
          strcpy_s(dg, strlen(m_setHeight) + 1, m_setHeight);
          dg[strlen(m_setHeight) - 1] = 0;
          int a = atoi(dg);
          m_calHeight = a * cell->height / 100.0f;
        }
      }
      else {
        m_calHeight = GetDefaultHeight();
      }

      // 计算View的四个顶点横坐标。
      // left
      if (!isnan(m_leftOffset)) {
        m_left = cell->left + m_leftOffset;
      }
      else if (!isnan(m_rightOffset)) {
        m_left = cell->right - m_rightOffset - m_calWidth;
      }
      else {
        float offset = (cell->width - m_calWidth) / 2.0f;
        m_left = cell->left + offset;
      }

      // right
      m_right = m_left + m_calWidth;

      // top
      if (!isnan(m_topOffset)) {
        m_top = cell->top + m_topOffset;
      }
      else if (!isnan(m_bottomOffset)) {
        m_top = cell->bottom - m_bottomOffset - m_calHeight;
      }
      else {
        float offset = (cell->height - m_calHeight) / 2.0f;
        m_top = cell->top + offset;
      }

      // bottom
      m_bottom = m_top + m_calHeight;

      // 检查calWidth和calHeight的值，如果他们发生了改变，
      // 则递归更新子View的位置和大小。
      if (m_calWidth != m_oldWidth || m_calHeight != m_oldHeight) {
        m_layout.SetWidth(m_calWidth);
        m_layout.SetHeight(m_calHeight);
        for (auto subv : m_subViews) {
          auto v = subv.lock();
          if (v) {
            v->UpdatePositionAndSize();
          }
        }
        m_oldWidth = m_calWidth;
        m_oldHeight = m_calHeight;
      }
    }

  public:

    ViewBase(const D2DContext & context, Window *parentWnd) {
      m_hidden = false;
      m_pContext = context;
      m_parentWnd = parentWnd;
      m_innerClipAreaLeftOffset = NAN;
      m_innerClipAreaTopOffset = NAN;
      m_innerClipAreaRightOffset = NAN;
      m_innerClipAreaBottomOffset = NAN;

      m_row = 0;
      m_col = 0;
      m_leftOffset = NAN;
      m_rightOffset = NAN;
      m_topOffset = NAN;
      m_bottomOffset = NAN;
      m_setWidth[0] = 0;
      m_setHeight[0] = 0;
      m_calWidth = NAN;
      m_calHeight = NAN;
      m_oldWidth = NAN;
      m_oldHeight = NAN;
    }

    virtual ~ViewBase() { }

    virtual void DrawSelf() = 0;

    virtual float GetDefaultWidth() {
      return 50;
    }

    virtual float GetDefaultHeight() {
      return 50;
    }

    virtual void FocusChanged() {
    }

    void SetHidden(bool hidden) {
      m_hidden = hidden;
    }

    void SetLeftOffset(float offset) {
      m_leftOffset = offset;
      UpdatePositionAndSize();
    }

    void ClearLeftOffset() {
      m_leftOffset = NAN;
      UpdatePositionAndSize();
    }

    void SetRightOffset(float offset) {
      m_rightOffset = offset;
      UpdatePositionAndSize();
    }

    void ClearRightOffset() {
      m_rightOffset = NAN;
      UpdatePositionAndSize();
    }

    void SetTopOffset(float offset) {
      m_topOffset = offset;
      UpdatePositionAndSize();
    }

    void ClearTopOffset() {
      m_topOffset = NAN;
      UpdatePositionAndSize();
    }

    void SetBottomOffset(float offset) {
      m_bottomOffset = offset;
      UpdatePositionAndSize();
    }

    void ClearBottomOffset() {
      m_bottomOffset = NAN;
      UpdatePositionAndSize();
    }

    void SetOffset(float left, float top) {
      m_leftOffset = left;
      m_topOffset = top;
      UpdatePositionAndSize();
    }

    void SetOffset(float left, float top, float right, float bottom) {
      m_leftOffset = left;
      m_topOffset = top;
      m_rightOffset = right;
      m_bottomOffset = bottom;
      UpdatePositionAndSize();
    }

    void SetWidth(const char *width) {
      strcpy_s(m_setWidth, 10, width);
      UpdatePositionAndSize();
    }

    void ClearWidth() {
      m_setHeight[0] = 0;
      UpdatePositionAndSize();
    }

    void SetHeight(const char *height) {
      strcpy_s(m_setHeight, 10, height);
      UpdatePositionAndSize();
    }

    void ClearHeight() {
      m_setHeight[0] = 0;
      UpdatePositionAndSize();
    }

    void SetGridPosition(int row, int col) {
      m_row = row;
      m_col = col;
      UpdatePositionAndSize();
    }

    void SetGridPosition(int row, int col, float leftOffset, float topOffset, float rightOffset, float bottomOffset) {
      m_row = row;
      m_col = col;
      m_leftOffset = leftOffset;
      m_topOffset = topOffset;
      m_rightOffset = rightOffset;
      m_bottomOffset = bottomOffset;
      UpdatePositionAndSize();
    }

    template<typename T>
    void SetInnerClipAreaOffset(T leftOffset, T topOffset, T rightOffset, T bottomOffset) {
      m_innerClipAreaLeftOffset = static_cast<float>(leftOffset);
      m_innerClipAreaTopOffset = static_cast<float>(topOffset);
      m_innerClipAreaRightOffset = static_cast<float>(rightOffset);
      m_innerClipAreaBottomOffset = static_cast<float>(bottomOffset);
    }

    void ClearInnerClipArea() {
      m_innerClipAreaLeftOffset = NAN;
      m_innerClipAreaTopOffset = NAN;
      m_innerClipAreaRightOffset = NAN;
      m_innerClipAreaBottomOffset = NAN;
    }

    bool DefinedInnerClipArea() {
      return !isnan(m_innerClipAreaLeftOffset)
        && !isnan(m_innerClipAreaTopOffset)
        && !isnan(m_innerClipAreaRightOffset)
        && !isnan(m_innerClipAreaBottomOffset);
    }

    // 为view生成一个特效对象。inputIdx是该view将输入到特效对象时位置索引
    // 如果不需要将该view输入到特效对象，则可调用另一版本
    DxResource<ID2D1Effect> CreateEffect(const IID & effectId, int inputIdx) {
      auto retval = m_pContext.CreateEffect(effectId);
      m_effects.push_back(make_tuple(retval, inputIdx));
      return retval;
    }

    // 为view生成一个特效对象。该特效对象不接受view作为输入
    DxResource<ID2D1Effect> CreateEffect(const IID & effectId) {
      return CreateEffect(effectId, -1);
    }

    bool IsEffectOn() {
      return m_showEffect;
    }

    void EffectOn() {
      m_showEffect = true;
    }

    void EffectOff() {
      m_showEffect = false;
    }

    void Draw() {

      if (m_hidden) return;

      DrawSelf();

      // 如果定义了内部剪切区域,则设置之
      if (DefinedInnerClipArea()) {
        D2D1_RECT_F innerClipArea = RectD(m_left + m_innerClipAreaLeftOffset,
          m_top + m_innerClipAreaTopOffset,
          m_right - m_innerClipAreaRightOffset,
          m_bottom - m_innerClipAreaBottomOffset);
        m_pContext->PushAxisAlignedClip(innerClipArea, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
      }

      // 開始繪製内部元素，首先移動坐標原點到父元素的左上角
      D2D1_MATRIX_3X2_F oldMatrix;
      m_pContext->GetTransform(&oldMatrix);
      m_pContext->SetTransform(TranslationMatrix(m_absLeft, m_absTop));

      for (auto it = m_subViews.begin(); it != m_subViews.end(); ++it) {
        const auto &v = *it;
        auto ptr = v.lock();

        if (ptr) {
          ptr->m_absLeft = m_absLeft + ptr->m_left;
          ptr->m_absTop = m_absTop + ptr->m_top;

          ptr->m_pEffectContext = m_pEffectContext;
          ptr->m_pBmpRT = m_pBmpRT;

          if (ptr->m_effects.size() > 0 && ptr->m_showEffect) {
            // 如果已经设定了特效，则要先将view绘制到一个临时的bmp上，然后再对其
            // 施加指定的特效，然后再将最后的结果绘制到画面。
            ptr->m_pContext = ptr->m_pEffectContext;
            ptr->m_pContext->SetTransform(TranslationMatrix(m_absLeft, m_absTop));

            ptr->m_pContext->BeginDraw();
            ptr->m_pContext->Clear(D2D1::ColorF(0xffffff, 0.0f));
            ptr->Draw();
            ptr->m_pContext->EndDraw();

            auto bmp = ptr->m_pBmpRT.GetResource<ID2D1Bitmap>(&ID2D1BitmapRenderTarget::GetBitmap);

            // 遍历所有的特效设定，将view的绘制结果设为其输入
            for (auto pe = ptr->m_effects.begin(); pe != ptr->m_effects.end(); pe++) {
              auto &e = get<0>(*pe);
              auto inputIdx = get<1>(*pe);
              if (inputIdx >= 0) {
                e->SetInput(inputIdx, bmp.ptr());
              }
            }

            auto &lastEffect = get<0>(*(--(ptr->m_effects.end())));
            m_pContext->SetTransform(D2D1::Matrix3x2F::Identity());
            m_pContext->DrawImage(lastEffect.ptr());
            m_pContext->SetTransform(TranslationMatrix(m_absLeft, m_absTop));
          }
          else {
            // 如果没有设置特效，则直接将view绘制到画面
            ptr->m_pContext = m_pContext;
            ptr->Draw();
          }
        }
        else if (v.expired()) {
          m_subViews.erase(it);
        }
      }

      // 内部元素繪製完成，將坐標移回
      m_pContext->SetTransform(oldMatrix);

      if (DefinedInnerClipArea()) {
        m_pContext->PopAxisAlignedClip();
      }
    }

    template <typename T, typename ...Args>
    shared_ptr<T> AppendSubView(Args ...args) {
      auto v = make_shared<T>(m_pContext, m_parentWnd, args...);
      v->m_wpThis = v;
      v->m_parentLayout = &m_layout;

      auto vb = static_pointer_cast<ViewBase>(v);
      vb->CreateD2DResource();
      m_subViews.push_back(v);
      return v;
    }

    void AddLayoutRow(const char * height) {
      m_layout.AddRow(height);
    }

    void AddLayoutCol(const char * width) {
      m_layout.AddCol(width);
    }

    template <typename T>
    shared_ptr<Animation<T>> AddAnimation(std::function<bool(T*, int)> updateFunc) {
      if (m_parentWnd) {
        T *downcast = dynamic_cast<T*>(this);
        return m_parentWnd->CreateAnimation(downcast, updateFunc);
      }
      return nullptr;
    }

    template <typename T = Layer>
    shared_ptr<T> AppendLayer() {
      auto layer = AppendSubView<T>();
      layer->SetOffset(0, 0, 0, 0);
      return layer;
    }
  };
}
