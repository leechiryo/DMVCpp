﻿#pragma once

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
    float m_leftOffset = NAN;
    float m_topOffset = NAN;
    float m_rightOffset = NAN;
    float m_bottomOffset = NAN;
    char m_setWidth[20];
    char m_setHeight[20];

    LayoutInfo() {
      m_setWidth[0] = 0;
      m_setHeight[0] = 0;
    }
  };

  struct XmlSettings {
    map<string, wstring> settings;
  };

  class ViewBase {

    typedef SPView(*CreateSubViewFromXML)(SPView, string, const XmlSettings &);

    template<typename T>
    friend class View;
    friend class Window;

  public:
    static map<string, CreateSubViewFromXML> & GetXmlLoaders() {
      static map<string, CreateSubViewFromXML> s_xmlLoaders;
      return s_xmlLoaders;
    }

    static const int MSG_MOUSEENTER = WM_USER + 1;
    static const int MSG_MOUSELEFT = WM_USER + 2;

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

    bool isNumber(const char *str) {
      auto len = strlen(str);

      // check for empty string and period position.
      if (len == 0) return false;
      if (str[0] == '.' || str[len - 1] == '.') return false;

      int periodCnt = 0;

      for (size_t i = 0; i < len; i++) {
        if (str[i] == '.') {
          periodCnt++;
          if (periodCnt > 1) return false;
          continue;
        }

        bool isDigit = (str[i] >= '0') && (str[i] <= '9');

        if (!isDigit) {
          return false;
        }
      }
      return true;
    }

    bool isPercent(const char *str) {
      auto len = strlen(str);
      // check for empty and the only one character '%' 
      // should return false.
      if (len <= 1) return false;
      if (str[len - 1] != '%') return false;

      char buf[20];
      strncpy_s(buf, str, len - 1);

      return isNumber(buf);
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

    Window * m_parentWnd;
    Layout m_layout;
    Layout *m_parentLayout;
    ViewBase * m_parentView;

    int m_row;
    int m_col;
    int m_zOrder = 0;

    float m_leftOffset;
    float m_topOffset;
    float m_rightOffset;
    float m_bottomOffset;
    char m_setWidth[20];
    char m_setHeight[20];

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

    // 根据自身的高度设置，宽度设置，偏移量设置，以及所处的
    // Layout的cell的实际高度，宽度和位置，计算自身的实际高度，宽度和位置。
    void UpdatePositionAndSize() {
      const GridCell * cell = m_parentLayout->GetCell(m_row, m_col);

      // 计算View的宽度（按照下述1→2→3的优先顺序）
      // 1.如果左右偏移量都有定义，那么view的宽度就是所处的cell的宽度减去左右偏移量。
      // 2.否则，如果显式指定了view的宽度，则采用指定的宽度（可以是一个绝对值，或者一个百分比值）。
      // 如果是百分比值的话，则宽度是所处cell的宽度乘以该百分比。
      // 3.否则，使用该对象的缺省宽度（GetDefaultWidth）。
      if (!isnan(m_leftOffset) && !(isnan(m_rightOffset))) {
        m_calWidth = cell->width - m_leftOffset - m_rightOffset;
      }
      else if (strlen(m_setWidth) > 0) {
        if (isNumber(m_setWidth)) {
          sscanf_s(m_setWidth, "%f", &m_calWidth);
        }
        else if (isPercent(m_setWidth)) {
          char dg[20] = { 0 };
          strncpy_s(dg, m_setWidth, strlen(m_setWidth) - 1);
          float a = 0.0f;
          sscanf_s(dg, "%f", &a);
          m_calWidth = a * cell->width / 100.0f;
        }
      }
      else {
        m_calWidth = GetDefaultWidth();
      }

      // 计算View的高度（按照下述1→2→3的优先顺序）
      // 1.如果上下偏移量都有定义，那么view的高度就是所处的cell的高度减去上下偏移量。
      // 2.否则，如果显式指定了view的高度，则采用指定的高度（可以是一个绝对值，或者一个百分比值）。
      // 如果是百分比值的话，则高度是所处cell的高度乘以该百分比。
      // 3.否则，使用该对象的缺省高度（GetDefaultHeight）。
      if (!isnan(m_topOffset) && !(isnan(m_bottomOffset))) {
        m_calHeight = cell->height - m_topOffset - m_bottomOffset;
      }
      else if (strlen(m_setHeight) > 0) {
        if (isNumber(m_setHeight)) {
          sscanf_s(m_setHeight, "%f", &m_calHeight);
        }
        else if (isPercent(m_setHeight)) {
          char dg[20] = { 0 };
          strncpy_s(dg, m_setHeight, strlen(m_setHeight) - 1);
          float a = 0.0f;
          sscanf_s(dg, "%f", &a);
          m_calHeight = a * cell->height / 100.0f;
        }
      }
      else {
        m_calHeight = GetDefaultHeight();
      }

      // 计算View的四个边的坐标。
      // left：
      // 1.如果定义了左偏移量，则 左坐标=cell的左坐标+左偏移量
      // 2.否则，如果定义了右偏移量，则 左坐标=cell的右坐标-view的宽度-右偏移量
      // 3.否则，令view在cell里居中布置，左坐标=cell的左坐标+(cell宽度-view宽度)/2
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
      // 右坐标=左坐标+view宽度
      m_right = m_left + m_calWidth;

      // top
      // 1.如果定义了上偏移量，则 上坐标=cell的上坐标+上偏移量
      // 2.否则，如果定义了下偏移量，则 上坐标=cell的下坐标-view的高度-下偏移量
      // 3.否则，令view在cell里居中布置，上坐标=cell的上坐标+(cell高度-view高度)/2
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
      // 下坐标=上坐标+view高度
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

    // 清空view及其子view的特效context。
    void ClearEffectContext() {
      m_pBmpRT = nullptr;
      m_pEffectContext = nullptr;

      for (auto it = m_subViews.begin(); it != m_subViews.end(); ++it) {
        const auto &v = *it;
        auto ptr = v.lock();

        if (ptr) {
          ptr->ClearEffectContext();
        }
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

    // 设定view的缺省宽度。
    // 每个实际的view（Text，Rectangle，Label，TextBox等）会重载自己的缺省宽度算法。
    // 有些是固定值，有些则根据内部子元素的缺省宽度进行计算。
    virtual float GetDefaultWidth() {
      return 50;
    }

    // 设定view的缺省高度。
    // 每个实际的view（Text，Rectangle，Label，TextBox等）会重载自己的缺省高度算法。
    // 有些是固定值，有些则根据内部子元素的缺省高度进行计算。
    virtual float GetDefaultHeight() {
      return 50;
    }

    float GetCurrentWidth() {
      return m_calWidth;
    }

    float GetCurrentHeight() {
      return m_calHeight;
    }

    virtual void FocusChanged() {
    }

    virtual void FireEvent(int msg, WPARAM wParam, LPARAM lParam) = 0;

    Window* GetParentWnd() {
      return m_parentWnd;
    }

    WPView GetHitSubView(int pixelX, int pixelY) {
      // 查询所有的子view，看其是否处在鼠标位置。
      WPView retval;
      int maxZOrder = 0;

      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;
        if (spv->HitTest(pixelX, pixelY)) {
          // 如果鼠标事件发生时的坐标在子 View 的内部，
          // 则在该子view中进一步查询内部的子view。
          auto wphsv = spv->GetHitSubView(pixelX, pixelY);
          auto sphsv = wphsv.lock();
          if (sphsv && sphsv->m_zOrder > maxZOrder) {
            retval = wphsv;
            maxZOrder = sphsv->m_zOrder;
          }
          else if (spv->m_zOrder > maxZOrder) {
            retval = spv;
            maxZOrder = spv->m_zOrder;
          }
        }
      }

      return retval;
    }

    void UpdateMouseInState(const SPView & miView, int pixelX, int pixelY) {
      if (miView.get() != this && m_mouseIn) {
        m_mouseIn = 0;
        FireEvent(MSG_MOUSELEFT, pixelX, pixelY);
      }
      else if (miView.get() == this && !m_mouseIn) {
        m_mouseIn = 1;
        FireEvent(MSG_MOUSEENTER, pixelX, pixelY);
      }

      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (!spv) continue;
        spv->UpdateMouseInState(miView, pixelX, pixelY);
      }
    }

    void SetHidden(bool hidden) {
      m_hidden = hidden;

      if (!m_hidden) {
        m_layout.SetWidth(tof(m_right - m_left));
        m_layout.SetHeight(tof(m_bottom - m_top));
        for (auto subv : m_subViews) {
          auto v = subv.lock();
          if (v) {
            v->UpdatePositionAndSize();
          }
        }
      }
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
      strcpy_s(m_setWidth, 20, width);
      UpdatePositionAndSize();
    }

    void ClearWidth() {
      m_setHeight[0] = 0;
      UpdatePositionAndSize();
    }

    void SetHeight(const char *height) {
      strcpy_s(m_setHeight, 20, height);
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

    void ClearZOrder() {
      m_zOrder = 0;
      for (auto v : m_subViews) {
        auto spv = v.lock();
        if (spv) {
          spv->ClearZOrder();
        }
      }
    }

    void Draw(int resetZOrder = 0) {

      static int currentZOrder = 0;

      if (resetZOrder) {
        currentZOrder = 0;
      }

      if (m_hidden) {
        if (m_zOrder) ClearZOrder();
        return;
      }

      m_zOrder = currentZOrder++;
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

          if (ptr->m_effects.size() > 0 && ptr->m_showEffect) {
            // 如果已经设定了特效，则要先将view绘制到一个临时的bmp上，然后再对其
            // 施加指定的特效，然后再将最后的结果绘制到画面。
            if (ptr->m_pEffectContext.NotSet()) {
              // 如果特效用的context还未创建，则创建它
              ptr->m_pBmpRT = m_pContext.CreateCompatibleRenderTarget();
              ptr->m_pEffectContext = ptr->m_pBmpRT.Query<ID2D1DeviceContext>();
            }

            ptr->m_pContext = ptr->m_pEffectContext;
            ptr->m_pContext->SetTransform(TranslationMatrix(m_absLeft, m_absTop));

            ptr->m_pContext->BeginDraw();
            ptr->m_pContext->Clear(D2D1::ColorF(0xffffff, 0.0f));
            ptr->Draw();
            HRESULT hr = ptr->m_pContext->EndDraw();

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

    // 向view内部添加新的子view。
    // 该子view的Layout位置被设置为（0，0），即第一行第一列
    // 之后可以通过调用子view的SetGridPosition指定所在的行列以及偏移量。
    template <typename T, typename ...Args>
    shared_ptr<T> AppendSubView(Args ...args) {
      auto v = make_shared<T>(m_pContext, m_parentWnd, args...);
      v->m_wpThis = v;
      v->m_parentLayout = &m_layout;
      v->m_parentView = this;

      auto vb = static_pointer_cast<ViewBase>(v);
      vb->CreateD2DResource();
      m_subViews.push_back(v);
      App::RegisterSubView(v);

      return v;
    }

    void AddLayoutRow(const char * height) {
      m_layout.AddRow(height);
    }

    void AddLayoutCol(const char * width) {
      m_layout.AddCol(width);
    }

    template <typename T = Layer>
    shared_ptr<T> AppendLayer() {
      auto layer = AppendSubView<T>();
      layer->SetOffset(0, 0, 0, 0);
      return layer;
    }
  };
}
