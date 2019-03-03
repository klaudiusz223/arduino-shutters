#include "StoredState.hpp"

using namespace ShuttersInternal;

StoredState::StoredState()
: _upCourseTime(0)
, _downCourseTime(0)
, _level(0)
{
}

void StoredState::feed(uint64_t state) {
  const uint32_t upCourseTime = (uint32_t)(state >> 38);
  const uint32_t downCourseTime = (uint32_t)((state << 26) >> 38);
  const uint16_t rawLevel = (uint16_t)((state << 52) >> 54);

  _upCourseTime = upCourseTime;
  _downCourseTime = downCourseTime;

  if (rawLevel < LEVEL_OFFSET) {
    _level = LEVEL_NONE;
  } else {
    _level = rawLevel - LEVEL_OFFSET;
  }
}

bool StoredState::isValid() {
  bool upCourseTimeValid = _upCourseTime > 0;
  bool downCourseTimeValid = _downCourseTime > 0;
  bool levelValid = _level <= LEVELS;

  return upCourseTimeValid && downCourseTimeValid && levelValid;
}

uint16_t StoredState::getLevel() {
  return _level;
}

void StoredState::setLevel(uint16_t level) {
  _level = level;
}

uint32_t StoredState::getUpCourseTime() {
  return _upCourseTime;
}

void StoredState::setUpCourseTime(uint32_t upCourseTime) {
  if (upCourseTime > 67108864UL || upCourseTime == 0) return; // max value for 26 bits

  _upCourseTime = upCourseTime;
}

uint32_t StoredState::getDownCourseTime() {
  return _downCourseTime;
}

void StoredState::setDownCourseTime(uint32_t downCourseTime) {
  if (downCourseTime > 67108864UL || downCourseTime == 0) return; // max value for 26 bits

  _downCourseTime = downCourseTime;
}

uint64_t StoredState::getState() {
  _state = _upCourseTime;
  _state <<= 26;
  _state |= _downCourseTime;
  _state <<= 10;
  _state |= (_level + LEVEL_OFFSET);
  _state <<= 2;
  return _state;
}

void StoredState::reset() {
  _upCourseTime = 0;
  _downCourseTime = 0;
  _level = 0;
}
