#include "StoredState.hpp"

using namespace ShuttersInternal;

StoredState::StoredState()
: _upCourseTime(0)
, _downCourseTime(0)
, _level(0)
, _rotationStepUp(0)
, _rotationStepDown(0)
, _tilt(0)
{
}

void StoredState::feed(uint64_t state) {
  const uint16_t upCourseTime = (uint16_t)(state >> 49); //2^15  1/10 ms (cs) s^-2 ( bits 64-50,  starting 1)
  const uint16_t downCourseTime = (uint16_t)((state << 15) >> 49); //2^15 1/10 ms (cs) s^-2 (bits 49 - 35, starting 1 )
  const uint16_t rawLevel = (uint16_t)((state << 30) >> 54); // 2^10 ( bits 34-25,  starting 1)
  const uint8_t rotationStepUp = (uint16_t)((state << 40) >> 57); // 2^7  ms ( bits 24-18,  starting 1)
  const uint8_t rotationStepDown = (uint16_t)((state << 47) >> 57); // 2^7  (bits 17 - 11)
  const uint16_t rawTilt =  (uint16_t)((state << 54) >> 54); // 2^10  ( bits 10 - 1 ,  starting 1)

  _upCourseTime = upCourseTime;
  _downCourseTime = downCourseTime;

  if (rawLevel < LEVEL_OFFSET) {
    _level = LEVEL_NONE;
  } else {
    _level = rawLevel - LEVEL_OFFSET;
  }

  if (rawTilt < LEVEL_OFFSET) {
    _tilt = LEVEL_NONE;
  } else {
    _tilt = rawTilt - LEVEL_OFFSET;
  }
  
 _rotationStepUp = rotationStepUp;
 _rotationStepDown = rotationStepDown;
}

bool StoredState::isValid() {
  bool upCourseTimeValid = _upCourseTime > 0;
  bool downCourseTimeValid = _downCourseTime > 0;
  bool levelValid = _level <= LEVELS;
  bool tiltValid = _tilt <= LEVELS;
  bool rotationStepUpValid = _rotationStepUp > 0 ;
  bool rotationStepDownValid = _rotationStepDown > 0 ;

  return upCourseTimeValid && downCourseTimeValid && levelValid && tiltValid && rotationStepUpValid && rotationStepDownValid ;
}

uint16_t StoredState::getLevel() {
  return _level;
}

void StoredState::setLevel(uint16_t level) {
  _level = level;
}

uint16_t StoredState::getTilt() {
  return _tilt;
}

void StoredState::setTilt(uint16_t tilt) {
  _tilt = tilt;
}
 
uint16_t StoredState::getUpCourseTime() {
  return _upCourseTime;
}

void StoredState::setUpCourseTime(uint16_t upCourseTime) {
  if (upCourseTime > ((uint16_t)1 << 15) - 1 || upCourseTime == 0) return; // max value for 16  1/10 ms (cs) s^-2

  _upCourseTime = upCourseTime;
}

uint16_t StoredState::getDownCourseTime() {
  return _downCourseTime;
}

void StoredState::setDownCourseTime(uint16_t downCourseTime) {
  if (downCourseTime > ((uint16_t)1 << 15) - 1 || downCourseTime == 0) return; // max value for 16  1/10 ms (cs) s^-2

  _downCourseTime = downCourseTime;
}

uint8_t StoredState::getRotationStepUp() {
  return _rotationStepUp;
}

void StoredState::setRotationStepUp(uint8_t rotationStep) {
  if (rotationStep > ((uint8_t)1 << 7) - 1 || rotationStep == 0) return; // max value for 12 bits -  ms (10^-3 s)

  _rotationStepUp = rotationStep;
}

uint8_t StoredState::getRotationStepDown() {
  return _rotationStepDown;
}

void StoredState::setRotationStepDown(uint8_t rotationStep) {
  if (rotationStep > ((uint8_t)1 << 7) - 1 || rotationStep == 0) return; // max value for 12 bits -  ms (10^-3 s)

  _rotationStepDown = rotationStep;
}

uint64_t StoredState::getState() {
  _state = _upCourseTime;
  _state <<= 15;
  _state |= _downCourseTime;
  _state <<= 10;
  _state |= (_level + LEVEL_OFFSET);
  _state <<= 7;
  _state |= _rotationStepUp;
  _state <<= 7;
  _state |= _rotationStepDown;
  _state <<= 10;
  _state |= (_tilt + LEVEL_OFFSET);
  return _state;
}

void StoredState::reset() {
  _upCourseTime = 0;
  _downCourseTime = 0;
  _level = 0;
  _rotationStepUp = 0 ;
  _rotationStepDown = 0;
  _tilt = 0;
}
