#pragma once

#include <functional>
#include "Types.h"

namespace mvc {

  enum class AniStatus2 { Paused, Playing };
  enum class AniPlayMode2 { PlayAndPauseAtEnd, PlayAndPauseAtStart, PlayRepeatly };

  class AnimationBase
  {
  private:
    int m_frameIdx;
    int m_rotation;
    AniStatus2 m_status;
    AniPlayMode2 m_mode;

    // 每一个动画对象都要提供此方法。
    // 如果传入的frameIdx是动画中的最后一帧时，函数
    // 应返回True，否则应返回False.
    virtual bool UpdateFrameResource(int) = 0;

  public:
    // 可以设置动画完成时的回调函数
    std::function<void()> OnFinished;

    AnimationBase() {
      m_frameIdx = 0;
      m_rotation = 0;
      m_status = AniStatus2::Paused;
      OnFinished = nullptr;
    }

    void SetFrameIndex(int idx) {
      m_frameIdx = idx;
    }

    void Rotate(int angle) {
      m_rotation = angle;
    }

    void PlayAndPauseAtEnd() {
      m_status = AniStatus2::Playing;
      m_mode = AniPlayMode2::PlayAndPauseAtEnd;
    }

    void PlayAndPauseAtStart() {
      m_status = AniStatus2::Playing;
      m_mode = AniPlayMode2::PlayAndPauseAtStart;
    }

    void PlayRepeatly() {
      m_status = AniStatus2::Playing;
      m_mode = AniPlayMode2::PlayRepeatly;
    }

    void Pause() {
      m_status = AniStatus2::Paused;
    }

    void Stop(){
      m_frameIdx = 0;
      m_status = AniStatus2::Paused;
    }

    // 绘制动画
    void Update() {

      if (m_status == AniStatus2::Playing) {

        // 如果正处于播放状态，则在画面上绘制当前帧
        if (UpdateFrameResource(m_frameIdx)) {
          // 如果当前帧完成后动画播放完毕(没有下一帧了)，则将下一帧设为0（动画开始的位置），
          // 下次调用Show的时候将从头播放。
          if (m_mode == AniPlayMode2::PlayAndPauseAtStart) {
            m_frameIdx = 0;
            Pause();
          }
          else if (m_mode == AniPlayMode2::PlayAndPauseAtEnd) {
            Pause();
          }
          else if (m_mode == AniPlayMode2::PlayRepeatly) {
            m_frameIdx = 0;
          }

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
    }
  };


  template <typename T>
  class Animation2 : public AnimationBase{
  private:
    shared_ptr<T> m_pFrameResource;
    std::function<bool(T*, int)> UpdateFrame;

  protected:
    virtual bool UpdateFrameResource(int frameIdx){
      return UpdateFrame(m_pFrameResource.get(), frameIdx);
    }

  public:
    Animation2(shared_ptr<T> resource, decltype(UpdateFrame) update){
      m_pFrameResource = resource;
      UpdateFrame = update;
    }

  };
}
