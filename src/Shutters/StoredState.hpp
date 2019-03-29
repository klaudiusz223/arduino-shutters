#pragma once

#include "Arduino.h"

namespace ShuttersInternal {
  const uint8_t LEVEL_OFFSET = 23;
  const uint16_t LEVEL_NONE = 65535; // level must be between 0 and 1000, so np
  const uint16_t LEVELS = 1000;
  const uint16_t TILT_NONE = 65535;
  class StoredState {
  private:
    // 2^15 
    uint16_t _upCourseTime;
    // 2^15 
    uint16_t _downCourseTime;
    // 2^10
    uint16_t _level;
    // 2^7
    uint8_t _rotationStepUp; // _upCourseTime/_rotationTimeUp
    // 2^7
    uint8_t _rotationStepDown;
    //2^10
    uint16_t _tilt;

    uint64_t _state;

  public:
    StoredState();
    // void feed(const char* state);
    void feed(uint64_t state);
    bool isValid();
    uint16_t getLevel();
    void setLevel(uint16_t level);
    uint16_t getTilt();
    void setTilt(uint16_t level);
    uint16_t getUpCourseTime();
    void setUpCourseTime(uint16_t upCourseTime);
    uint16_t getDownCourseTime();
    void setDownCourseTime(uint16_t downCourseTime);
    uint8_t getRotationStepUp();
    void setRotationStepUp(uint8_t rotationStep);
    uint8_t getRotationStepDown();
    void setRotationStepDown(uint8_t rotationStep);
    uint64_t getState();
    void reset();
  };
}
