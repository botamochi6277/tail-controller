#ifndef MISC_UTILS_CPP
#define MISC_UTILS_CPP
#include <Arduino.h>
namespace botalab {

#if defined(LEDR)
void init_xiao_nrf52840_builtin_led() {
  // build-in leds
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  // turn off the all build-in leds
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);
}
#endif

}  // namespace botalab
#endif