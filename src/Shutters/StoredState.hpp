#pragma once

#include "Arduino.h"

namespace ShuttersInternal {
  const uint8_t LEVEL_OFFSET = 23;
  const uint16_t LEVEL_NONE = 65535; // level must be between 0 and 1000, so np
  const uint8_t STATE_LENGTH = 20; // max representation of uint64
  const uint16_t LEVELS = 1000;
  class StoredState {
  private:
    // 2^26 but 64 for bitwise (otherwise bitwise shift does not work well)
    uint64_t _upCourseTime;
    // 2^26 but 64 for bitwise
    uint64_t _downCourseTime;
    // 2^10
    uint64_t _level;
    // this leaves 2 bits for another feature for 64 bits

    char _state[STATE_LENGTH + 1];

  public:
    StoredState();
    void feed(const char* state);
    bool isValid();
    uint16_t getLevel();
    void setLevel(uint16_t level);
    uint32_t getUpCourseTime();
    void setUpCourseTime(uint32_t upCourseTime);
    uint32_t getDownCourseTime();
    void setDownCourseTime(uint32_t downCourseTime);
    const char* getState();
    void reset();
  };
}
