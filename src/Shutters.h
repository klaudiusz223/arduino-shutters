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
  // const uint16_t LEVELS = 1000;

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
  typedef void (*LevelReachedCallback)(::Shutters* s, uint16_t level);
}

class Shutters {
private:
  uint32_t _upCourseTime;
  uint32_t _downCourseTime;
  float _calibrationRatio;
  uint32_t _upStepTime;
  uint32_t _downStepTime;
  uint32_t _upCalibrationTime;
  uint32_t _downCalibrationTime;

  uint16_t _rotationTimeUp;
  uint16_t _rotationTimeDown;
  uint8_t _rotationStepUp;
  uint8_t _rotationStepDown;

  ShuttersInternal::State _state;
  uint32_t _stateTime;
  ShuttersInternal::Direction _direction;

  ShuttersInternal::StoredState _storedState;

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
  uint16_t getRotationTimeUp();
  uint16_t getRotationTimeDown();
  Shutters& setOperationHandler(ShuttersInternal::OperationHandler handler);
  Shutters& restoreState(uint64_t state); 
  Shutters& setWriteStateHandler(ShuttersInternal::WriteStateHandler handler);
  Shutters& setCourseTime(uint32_t upCourseTime, uint32_t downCourseTime = 0);
  Shutters& setRotationTime(uint16_t rotationTimeUp,uint16_t rotationTimeDown);
  Shutters& setCourseTime();
  float getCalibrationRatio();
  Shutters& setCalibrationRatio(float calibrationRatio);
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
