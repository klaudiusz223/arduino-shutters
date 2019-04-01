#include "StoredState.hpp"

using namespace ShuttersInternal;

StoredState::StoredState()
: _state(0)
{
}

void StoredState::feed(uint64_t state) {
  _state = state;
  const uint16_t rawLevel = (uint16_t)((state << 30) >> 54);
  const uint16_t rawTilt =  (uint16_t)((state << 54) >> 54);

  if (rawLevel < LEVEL_OFFSET) 
      setLevel(LEVEL_NONE);
    
  if (rawTilt < LEVEL_OFFSET) 
      setTilt(LEVEL_NONE);
}

bool StoredState::isValid() {
  bool upCourseTimeValid = getUpCourseTime() > 0;
  bool downCourseTimeValid = getDownCourseTime() > 0;
  bool levelValid = getLevel() <= LEVELS;
  bool tiltValid = getTilt() <= LEVELS;
  bool rotationStepUpValid = getRotationStepUp() > 0 ;
  bool rotationStepDownValid = getRotationStepDown() > 0 ;

  return upCourseTimeValid && downCourseTimeValid && levelValid && tiltValid && rotationStepUpValid && rotationStepDownValid ;
}

uint16_t StoredState::getLevel() {
  return (uint16_t) (((_state << 30) >> 54) - LEVEL_OFFSET);

}

void StoredState::setLevel(uint16_t level) {  
  _state = _state | (((uint64_t)level + LEVEL_OFFSET ) << 24 );
}

uint16_t StoredState::getTilt() {
  return (uint16_t) (((_state << 54) >> 54) - LEVEL_OFFSET);
}

void StoredState::setTilt(uint16_t tilt) {
  _state = _state | ((uint64_t) tilt  + LEVEL_OFFSET);
}
 
uint16_t StoredState::getUpCourseTime() {
  return (uint16_t)(_state >> 49);
}

void StoredState::setUpCourseTime(uint16_t upCourseTime) {
  if (upCourseTime > ((uint16_t)1 << 15) - 1 || upCourseTime == 0) return; // max value for 16  1/10 ms (cs) s^-2

  _state = _state | ((uint64_t) upCourseTime << 49) ;
}

uint16_t StoredState::getDownCourseTime() {
  return(uint16_t)((_state << 15) >> 49);
}

void StoredState::setDownCourseTime(uint16_t downCourseTime) {
  if (downCourseTime > ((uint16_t)1 << 15) - 1 || downCourseTime == 0) return; // max value for 16  1/10 ms (cs) s^-2

    _state = _state | ((uint64_t) downCourseTime << 34) ;
}

uint8_t StoredState::getRotationStepUp() {
  return (uint16_t)((_state << 40) >> 57);
}

void StoredState::setRotationStepUp(uint8_t rotationStep) {
  if (rotationStep > ((uint8_t)1 << 7) - 1 || rotationStep == 0) return; // max value for 12 bits -  ms (10^-3 s)

   _state = _state | ((uint64_t)rotationStep << 17) ;

}

uint8_t StoredState::getRotationStepDown() {
  return (uint16_t)((_state << 47) >> 57);
}

void StoredState::setRotationStepDown(uint8_t rotationStep) {
  if (rotationStep > ((uint8_t)1 << 7) - 1 || rotationStep == 0) return; // max value for 12 bits -  ms (10^-3 s)

     _state = _state | ((uint64_t) rotationStep << 10) ;
}

uint64_t StoredState::getState() {
  return _state;
}

void StoredState::reset() {
  _state = 0;
  setLevel(LEVEL_NONE);
  setTilt(LEVEL_NONE);
}
