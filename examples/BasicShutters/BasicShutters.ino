#include <Shutters.h>
#include <EEPROM.h>

const uint8_t eepromOffset = 0;
const uint64_t upCourseTime = 30 * 1000;
const uint64_t downCourseTime = 45 * 1000;
const float calibrationRatio = 0.1;

void shuttersUp(Shutters* shutters) {
  Serial.println("Shutters going up.");
  // TODO: Implement the code for the shutters to go up
}

void shuttersDown(Shutters* shutters) {
  Serial.println("Shutters going down.");
  // TODO: Implement the code for the shutters to go down
}

void shuttersHalt(Shutters* shutters) {
  Serial.println("Shutters halted.");
  // TODO: Implement the code for the shutters to halt
}

char* shuttersGetState(Shutters* shutters, uint8_t length) {
  char state[length + 1];
  for (uint8_t i = 0; i < length; i++) {
    state[i] = EEPROM.read(eepromOffset + i);
  }
  state[length] = '\0';

  return strdup(state);
}

void shuttersSetState(Shutters* shutters, const char* state, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    EEPROM.write(eepromOffset + i, state[i]);
    #ifdef ESP8266
    EEPROM.commit();
    #endif
  }
}

void onShuttersLevelReached(Shutters* shutters, uint8_t level) {
  Serial.print("Shutters at ");
  Serial.print(level);
  Serial.println("%");
}

Shutters shutters(shuttersUp, shuttersDown, shuttersHalt, shuttersGetState, shuttersSetState);

void setup() {
  Serial.begin(9600);
  delay(100);
  #ifdef ESP8266
  EEPROM.begin(512);
  #endif
  Serial.println("Starting");
  shutters.setCourseTime(upCourseTime, downCourseTime);
  shutters.begin();

  shutters.setLevel(50); // Go to 50%
}

void loop() {
  shutters.loop();
}
