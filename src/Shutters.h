#ifndef Shutters_h
#define Shutters_h

#include "Arduino.h"

#include "ShuttersOperation.hpp"

#include "Shutters/StoredState.hpp"

#ifndef _SAFETY_DELAY_
  #define _SAFETY_DELAY_ 1 * 1000
#endif

#define VALIDATE(a,b,c) ( (a) > (int16_t)(c) ? (c) : ( (a) < (b) ?  (b) : (a) ) )

// #define DEBUG
#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif

class Shutters;

namespace ShuttersInternal {
  const uint16_t SAFETY_DELAY = _SAFETY_DELAY_;
  
  enum State : uint8_t {
    STATE_IDLE, // not moving
    STATE_RESETTING, // when state not known, goes to 0
    STATE_TARGETING, // when going to target
    STATE_NORMALIZING, // when target changed, goes to next known level
    STATE_CALIBRATING // when 0 or 100, to ensure actually at the end
  };
  enum Direction : bool { DIRECTION_DOWN, DIRECTION_UP };

  typedef void (*OperationHandler)(::Shutters* s, ::ShuttersOperation operation);
  typedef void (*WriteStateHandler)(::Shutters* s, uint64_t state);
  typedef void (*ReadStateHandler)(::Shutters* s, uint64_t &state);  
  typedef void (*LevelReachedCallback)(::Shutters* s, uint16_t level);
}

class Shutters {
private:
  uint32_t _upCourseTime;
  uint32_t _downCourseTime;
  uint8_t _calibrationRatio;
  uint32_t _upStepTime;
  uint32_t _downStepTime;
  uint8_t _rotationStepUp;
  uint8_t _rotationStepDown;

  ShuttersInternal::State _state;
  uint32_t _stateTime;
  ShuttersInternal::Direction _direction;

  uint16_t _currentLevel;
  uint16_t _targetLevel;

  uint16_t _currentTilt;
  uint16_t _targetTilt;
  // int8_t _tiltCorrection;

  bool _safetyDelay;
  uint32_t _safetyDelayTime;
  ShuttersInternal::Direction _safetyDelayDirection; 

  bool _init;
  bool _reset;

  ShuttersInternal::OperationHandler _operationHandler;
  ShuttersInternal::WriteStateHandler _writeStateHandler;
  ShuttersInternal::ReadStateHandler _readStateHandler;

  ShuttersInternal::LevelReachedCallback _levelReachedCallback;
  ShuttersInternal::LevelReachedCallback _tiltReachedCallback;

  void _up();
  void _down();
  void _halt();
  void _rotate();
  void _setSafetyDelay();
  void _notifyLevel();
  void _notifyTilt();
public:
  Shutters();
  uint32_t getUpCourseTime();
  uint32_t getDownCourseTime();
  Shutters& setOperationHandler(ShuttersInternal::OperationHandler handler);
  Shutters& setWriteStateHandler(ShuttersInternal::WriteStateHandler handler);
  Shutters& setReadStateHandler(ShuttersInternal::ReadStateHandler handler);
  Shutters& setCourseTime(uint32_t upCourseTime, uint32_t downCourseTime = 0);
  Shutters& setRotationTime(uint16_t rotationTimeUp,uint16_t rotationTimeDown);
  Shutters& setCourseTime();
  uint8_t getCalibrationRatio();
  Shutters& setCalibrationRatio(uint8_t calibrationRatio);
  Shutters& onLevelReached(ShuttersInternal::LevelReachedCallback callback);
  Shutters& onTiltReached(ShuttersInternal::LevelReachedCallback callback);
  Shutters& begin();
  Shutters& setLevel(uint16_t level);
  Shutters& setTilt(uint16_t level);
  Shutters& stop();
  Shutters& loop();
  bool isIdle();
  uint16_t getCurrentLevel();
  uint16_t getCurrentTilt();
  Shutters& reset();
  bool isReset();
};

#endif
