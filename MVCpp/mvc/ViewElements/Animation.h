#pragma once

#include "..\Types.h"
#include "..\View.h"
#include "..\ModelRef.h"

namespace mvc {

  enum class AniStatus{Stoped, Paused, Playing};

  class Animation : public View<Animation>
  {
  private:
    int m_x;
    int m_y;
    int m_frameIdx;
    int m_rotation;
    AniStatus m_status;

  public:
    virtual void DrawFrame(int frameIdx) = 0;
    virtual void OnFinished() { }

    Animation() {
      m_x = 0;
      m_y = 0;
      m_frameIdx = 0;
      m_rotation = 0;
      m_status = AniStatus::Stoped;
    }

    void SetPosition(int x, int y) {
      m_x = x;
      m_y = y;
    }

    void SetFrameIndex(int idx) {
      m_frameIdx = idx;
    }

    void Rotate(int angle) {
      m_rotation = angle;
    }

    void Show() {

    }

    void Play() {
      m_status = AniStatus::Playing;
    }

    void Stop() {
      m_status = AniStatus::Stoped;
    }
    void Pause() {
      m_status = AniStatus::Paused;
    }

  };
}