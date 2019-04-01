#include "Shutters.h"

using namespace ShuttersInternal;

Shutters::Shutters()
: _upCourseTime(0)
, _downCourseTime(0)
, _calibrationRatio(100)
, _state(STATE_IDLE)
, _stateTime(0)
, _direction(DIRECTION_UP)
, _currentLevel(LEVEL_NONE)
, _targetLevel(LEVEL_NONE)
, _currentTilt(LEVEL_NONE)
, _targetTilt(LEVEL_NONE)
, _safetyDelay(false)
, _safetyDelayTime(0)
, _safetyDelayDirection(DIRECTION_DOWN)
, _reset(true)
, _operationHandler(nullptr)
, _writeStateHandler(nullptr)
, _levelReachedCallback(nullptr)
, _tiltReachedCallback(nullptr)
{
}

void Shutters::_up() {
  DPRINTLN(F("Shutters: going up"));
  _operationHandler(this, ShuttersOperation::UP);
  _safetyDelay = false;
}

void Shutters::_down() {
  DPRINTLN(F("Shutters: going down"));
  _operationHandler(this, ShuttersOperation::DOWN);
  _safetyDelay = false;
}

void Shutters::_halt() {
  DPRINTLN(F("Shutters: halting"));
  _operationHandler(this, ShuttersOperation::HALT);
  _setSafetyDelay();
}

void Shutters::_setSafetyDelay() {
  _safetyDelayTime = millis();
  _safetyDelay = true;
}

void Shutters::_notifyLevel() {
  DPRINT(F("Shutters: notifying level "));
  DPRINTLN(_currentLevel);
  if (_levelReachedCallback) _levelReachedCallback(this, _currentLevel);
}

void Shutters::_notifyTilt() { 
  DPRINT(F("Shutters: notifying tilt "));
  DPRINTLN(_currentTilt);
  if (_tiltReachedCallback) _tiltReachedCallback(this, _currentTilt);
}

uint32_t Shutters::getUpCourseTime() {
  return _upCourseTime;
}

uint32_t Shutters::getDownCourseTime() {
  return _downCourseTime;
}


Shutters& Shutters::setOperationHandler(ShuttersInternal::OperationHandler handler) {
  _operationHandler = handler;

  return *this;
}

Shutters& Shutters::setWriteStateHandler(ShuttersInternal::WriteStateHandler handler) {
  _writeStateHandler = handler;

  return *this;
}

Shutters& Shutters::setReadStateHandler(ShuttersInternal::ReadStateHandler handler) {
  _readStateHandler = handler;

  return *this;
}

Shutters& Shutters::setCourseTime(uint32_t upCourseTime, uint32_t downCourseTime) {
  _upCourseTime = upCourseTime/10 * 10 ;
  _downCourseTime = downCourseTime/10 * 10;

  _rotationStepUp = _upCourseTime / 1500;
  _rotationStepDown = _downCourseTime / 1500;  

  return *this;
}

Shutters& Shutters::setRotationTime(uint16_t rotationTimeUp,uint16_t rotationTimeDown) {
  if (rotationTimeUp == 0 || rotationTimeDown == 0)  { 
    rotationTimeUp = 1500;
    rotationTimeDown = 1500;
  }

  _rotationStepUp = _upCourseTime/rotationTimeUp;
  _rotationStepDown = _downCourseTime/rotationTimeDown;

    if (_rotationStepUp > ((uint16_t)1 << 7) - 1)  { 
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this; 
  }

  if (_rotationStepDown > ((uint16_t)1 << 7) - 1) { 
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this; // max value for 12 bits
  }

  return *this;
}

Shutters& Shutters::setCourseTime() {
  if (!_reset) {
    _upCourseTime = 0;
    _downCourseTime = 0;
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this;
  }

  if ( !_readStateHandler || !_writeStateHandler) { //
    _upCourseTime = 0;
    _downCourseTime = 0;
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this;
  }

  if (_upCourseTime > (((uint32_t)1 << 15) - 1) * 10 || _upCourseTime == 0) {
    _upCourseTime = 0;
    _downCourseTime = 0;
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this;
  }  // max value for 16 bits
  // if down course time is not set, consider it's the same as up
  if (_downCourseTime == 0) _downCourseTime = _upCourseTime;
  if (_downCourseTime > (((uint32_t)1 << 15) - 1) * 10) {
    _upCourseTime = 0;
    _downCourseTime = 0;
    _rotationStepUp = 1;
    _rotationStepDown = 1;
    return *this; // max value for 16 bits
  }

   uint64_t state ;

  _readStateHandler(this,state);

  ShuttersInternal::StoredState storedState;
  storedState.feed(state);

  if (storedState.isValid()) {
    DPRINTLN(F("Shutters: Stored state is valid"));
    _currentLevel = storedState.getLevel();
    _currentTilt = storedState.getTilt();
    _notifyLevel();
    _notifyTilt();
  } else {
    DPRINTLN(F("Stored state is invalid"));
  }

  if (_upCourseTime != storedState.getUpCourseTime() * 10  || _downCourseTime != storedState.getDownCourseTime() * 10 || _rotationStepUp != storedState.getRotationStepUp() || _rotationStepDown != storedState.getRotationStepDown()) {
    DPRINTLN(F("Shutters: course time is not the same, invalidating stored state"));
    storedState.setLevel(LEVEL_NONE);
    storedState.setTilt(LEVEL_NONE);
    _currentLevel = LEVEL_NONE;
    _currentTilt = LEVEL_NONE;
  }

  _upStepTime = _upCourseTime / LEVELS;
  storedState.setUpCourseTime(_upCourseTime / 10);

  _downStepTime = _downCourseTime / LEVELS;
  storedState.setDownCourseTime(_downCourseTime / 10 );

  storedState.setRotationStepUp(_rotationStepUp);
  storedState.setRotationStepDown(_rotationStepDown);
  
  _writeStateHandler(this, storedState.getState());

  return *this;
}

uint8_t Shutters::getCalibrationRatio() {
  return _calibrationRatio;
}

Shutters& Shutters::setCalibrationRatio(uint8_t calibrationRatio) {
  if (calibrationRatio == 0 ) calibrationRatio = 100;
  _calibrationRatio = calibrationRatio;

  return *this;
}

Shutters& Shutters::onLevelReached(ShuttersInternal::LevelReachedCallback callback) {
  _levelReachedCallback = callback;

  return *this;
}

Shutters& Shutters::onTiltReached(ShuttersInternal::LevelReachedCallback callback) {
  _tiltReachedCallback = callback;

  return *this;
}

Shutters& Shutters::begin() {
  if (_upCourseTime == 0 || _downCourseTime == 0) return *this;

  _reset = false;

  return *this;
}

Shutters& Shutters::setLevel(uint16_t level) {
  if (_reset) {
    return *this;
  }

  if (level > LEVELS) {
    return *this;
  }

  if (_state == STATE_IDLE && level == _currentLevel) return *this;
  if ((_state == STATE_TARGETING || _state == STATE_NORMALIZING) && level == _targetLevel) return *this; // normalizing check useless, but avoid following lines overhead

  _targetLevel = level;
  Direction direction = (_targetLevel > _currentLevel) ? DIRECTION_DOWN : DIRECTION_UP;
  if (_state == STATE_TARGETING && _direction != direction) {
    _state = STATE_NORMALIZING;
  }

  return *this;
}

Shutters& Shutters::setTilt(uint16_t position) {
  
  if (_reset) {
    return *this;
  }

  if (position > (uint16_t)LEVELS) {
    return *this;
  }

  if (_state == STATE_IDLE && position == _currentTilt) return *this;

  _targetTilt = position;

  return *this;
}

void Shutters::_rotate() {
  if (_targetTilt != LEVEL_NONE) {
    Direction direction = (_targetTilt > _currentTilt) ? DIRECTION_DOWN : DIRECTION_UP;

    uint8_t _rotateStep = direction ==  DIRECTION_UP ? _rotationStepUp : _rotationStepDown;
    int8_t tiltCalibration =  _targetTilt == 0 ? -1 : ( _targetTilt == 1000 ? 1 : 0 ) ; // add one step more on extreme postions
    int16_t targetLevel = _currentLevel + (( (int16_t)_targetTilt - (int16_t)_currentTilt ) / _rotateStep) + tiltCalibration;

    targetLevel = VALIDATE(targetLevel,0,LEVELS);
    setLevel((uint16_t)targetLevel);
    _targetTilt = LEVEL_NONE;
  }
}

Shutters& Shutters::stop() {
  if (_reset) return *this;

  _targetLevel = LEVEL_NONE;
  if (_state == STATE_IDLE) return *this;
 
  if (_state == STATE_TARGETING) {
    _state = STATE_NORMALIZING;
  }

  return *this;
}

Shutters& Shutters::loop() {
  if (_reset) return *this;

  if (_safetyDelay) {
    if (millis() - _safetyDelayTime >= SAFETY_DELAY) {
      DPRINTLN(F("Shutters: end of safety delay"));
      _safetyDelay = false;
    }

    if (_targetLevel != LEVEL_NONE ) {
      _direction = (_targetLevel > _currentLevel) ? DIRECTION_DOWN : DIRECTION_UP;

      if (_direction == _safetyDelayDirection) 
          return *this;          
    }
  }

  // here, we're safe for relays

  ShuttersInternal::StoredState storedState;

  if (_currentLevel == LEVEL_NONE) {
    if (_state != STATE_RESETTING) {
      DPRINTLN(F("Shutters: level not known, resetting"));
      _up();
      _state = STATE_RESETTING;
      _stateTime = millis();
    } else if (millis() - _stateTime >= _upCourseTime + _upCourseTime / _calibrationRatio) {
      DPRINTLN(F("Shutters: level now known"));
      _halt();
      _state = STATE_IDLE;
      _currentLevel = 0;
      _currentTilt = 0 ;

      storedState.setLevel(_currentLevel);
      storedState.setTilt(_currentTilt);

      storedState.setUpCourseTime(_upCourseTime / 10);
      storedState.setDownCourseTime(_downCourseTime / 10 );

      storedState.setRotationStepUp(_rotationStepUp);
      storedState.setRotationStepDown(_rotationStepDown);
    
      _writeStateHandler(this, storedState.getState());
      _notifyLevel();
      _notifyTilt();
      // _rotate(); //?
    }
    return *this;
  }

  // here, level is known
  if (_state == STATE_IDLE && _targetTilt != LEVEL_NONE) {
    _rotate();  
  };

  if (_state == STATE_IDLE && _targetLevel == LEVEL_NONE) return *this; // nothing to do

  if (_state == STATE_CALIBRATING) {
    const uint32_t calibrationTime = (_direction == DIRECTION_UP) ? (_upCourseTime / _calibrationRatio) : (_downCourseTime / _calibrationRatio);
    if (millis() - _stateTime >= calibrationTime) {
      DPRINTLN(F("Shutters: calibration is done"));
      _currentTilt =  (_direction == DIRECTION_UP) ? 0 : 1000 ;
      _halt();
      _state = STATE_IDLE;
      _notifyLevel();
      _notifyTilt();

      storedState.setLevel(_currentLevel);
      storedState.setTilt(_currentTilt);

      storedState.setUpCourseTime(_upCourseTime / 10);
      storedState.setDownCourseTime(_downCourseTime / 10 );

      storedState.setRotationStepUp(_rotationStepUp);
      storedState.setRotationStepDown(_rotationStepDown);

      _writeStateHandler(this, storedState.getState());
      _rotate();
    }

    return *this;
  }

  // here, level is known and calibrated, and we need to do something

  if (_state == STATE_IDLE) {
    DPRINTLN(F("Shutters: starting move"));
    _direction = (_targetLevel > _currentLevel) ? DIRECTION_DOWN : DIRECTION_UP;

    _safetyDelayDirection =  _direction == DIRECTION_DOWN ? DIRECTION_UP : DIRECTION_DOWN  ;  

    storedState.setLevel(LEVEL_NONE);
    storedState.setTilt(LEVEL_NONE);

    storedState.setUpCourseTime(_upCourseTime / 10);
    storedState.setDownCourseTime(_downCourseTime / 10 );

    storedState.setRotationStepUp(_rotationStepUp);
    storedState.setRotationStepDown(_rotationStepDown);

    _writeStateHandler(this, storedState.getState());
    (_direction == DIRECTION_UP) ? _up() : _down();
    _state = STATE_TARGETING;
    _stateTime = millis();

    return *this;
  }

  // here, we have to handle targeting and normalizing

  const uint32_t stepTime = (_direction == DIRECTION_UP) ? _upStepTime : _downStepTime;

  if (millis() - _stateTime < stepTime) return *this;

  _currentLevel += (_direction == DIRECTION_UP) ? -1 : 1;

  _currentTilt = (uint16_t)VALIDATE( (int16_t)_currentTilt + ( (_direction == DIRECTION_UP) ? - _rotationStepUp : _rotationStepDown ),0,LEVELS );
  _stateTime = millis();

  if (_currentLevel == 0 || _currentLevel == LEVELS) { // we need to calibrate
    DPRINTLN(F("Shutters: starting calibration"));
    _state = STATE_CALIBRATING;
    if (_currentLevel == _targetLevel) _targetLevel = LEVEL_NONE;

    return *this;
  }

  if (_state == STATE_NORMALIZING) { // we've finished normalizing
    DPRINTLN(F("Shutters: finished normalizing"));
    _halt();
    _state = STATE_IDLE;
    _notifyLevel();
    _notifyTilt();
    if (_targetLevel == LEVEL_NONE) 
    {
      storedState.setLevel(_currentLevel);
      storedState.setTilt(_currentTilt);

      storedState.setUpCourseTime(_upCourseTime / 10);
      storedState.setDownCourseTime(_downCourseTime / 10 );

      storedState.setRotationStepUp(_rotationStepUp);
      storedState.setRotationStepDown(_rotationStepDown);
      _writeStateHandler(this, storedState.getState());
    }
    _rotate();
    return *this;
  }

  if (_state == STATE_TARGETING && _currentLevel == _targetLevel) { // we've reached out target
    DPRINTLN(F("Shutters: reached target"));
    _halt();
    _state = STATE_IDLE;
    _targetLevel = LEVEL_NONE;
    _notifyLevel();
    _notifyTilt();

    storedState.setLevel(_currentLevel);
    storedState.setTilt(_currentTilt);

    storedState.setUpCourseTime(_upCourseTime / 10);
    storedState.setDownCourseTime(_downCourseTime / 10 );

    storedState.setRotationStepUp(_rotationStepUp);
    storedState.setRotationStepDown(_rotationStepDown);

    _writeStateHandler(this, storedState.getState());

    _rotate();
    return *this;
  }

  // we've reached an intermediary level

  _notifyLevel();
  _notifyTilt();

  return *this;
}

bool Shutters::isIdle() {
  return _state == STATE_IDLE;
}

uint16_t Shutters::getCurrentLevel() {
  return _currentLevel;
}

uint16_t Shutters::getCurrentTilt() {
  return _currentTilt;
}

Shutters& Shutters::reset() {
  DPRINTLN(" Reset shutter");
  _halt();
  ShuttersInternal::StoredState storedState;
  storedState.reset();
  _writeStateHandler(this, storedState.getState());
  _reset = true;

  return *this;
}

bool Shutters::isReset() {
  return _reset;
}
