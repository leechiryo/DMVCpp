﻿#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  enum class AniStatus { Stoped, Paused, Playing };

  typedef void(*AnimationCallBack)();

  class Animation : public View<Animation>
  {
  private:
    int m_frameIdx;
    int m_rotation;
    AniStatus m_status;

  public:

    // 每一个动画对象都要实现此方法。
    virtual bool DrawFrame(int frameIdx) = 0;

    // 可以设置动画完成时的回调函数
    AnimationCallBack OnFinished;

    Animation() {
      m_frameIdx = 0;
      m_rotation = 0;
      m_status = AniStatus::Stoped;
      OnFinished = nullptr;
    }

    void SetFrameIndex(int idx) {
      m_frameIdx = idx;
    }

    void Rotate(int angle) {
      m_rotation = angle;
    }

    void Play() {
      m_status = AniStatus::Playing;
    }

    void Stop() {
      m_frameIdx = 0;
      m_status = AniStatus::Stoped;
    }

    void Pause() {
      m_status = AniStatus::Paused;
    }

    // 绘制动画
    virtual void DrawSelf() {

      if (m_status == AniStatus::Playing) {

        // 如果正处于播放状态，则在画面上绘制当前帧
        if (DrawFrame(m_frameIdx)) {

          // 如果当前帧完成后动画播放完毕(没有下一帧了)，则将下一帧设为0（动画开始的位置），
          // 下次调用Show的时候将从头播放。
          m_frameIdx = 0;

          if (OnFinished) {
            // 如果定义了播放完成后的回调函数，则调用它。
            OnFinished();
          }
        }
        else {

          // 如果当前帧完成后动画并没有播放完毕，
          // 则将当前帧的索引加1，下次调用Show时将绘制下一帧。
          m_frameIdx++;
        }
      }
      else if (m_status == AniStatus::Paused) {
        DrawFrame(m_frameIdx);
      }
    }

  };
}