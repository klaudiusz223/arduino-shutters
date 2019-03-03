#pragma once

#include "Arduino.h"

namespace ShuttersInternal {
  const uint8_t LEVEL_OFFSET = 23;
  const uint16_t LEVEL_NONE = 65535; // level must be between 0 and 1000, so np
  const uint16_t LEVELS = 1000;
  class StoredState {
  private:
    // 2^26 
    uint32_t _upCourseTime;
    // 2^26 
    uint32_t _downCourseTime;
    // 2^10
    uint16_t _level;
    // this leaves 2 bits for another feature for 64 bits

    uint64_t _state;

  public:
    StoredState();
    // void feed(const char* state);
    void feed(uint64_t state);
    bool isValid();
    uint16_t getLevel();
    void setLevel(uint16_t level);
    uint32_t getUpCourseTime();
    void setUpCourseTime(uint32_t upCourseTime);
    uint32_t getDownCourseTime();
    void setDownCourseTime(uint32_t downCourseTime);
    // const char* getState();
    uint64_t getState();
    void reset();
  };
}
