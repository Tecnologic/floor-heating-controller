#include "esphome.h"
#include <esp32-hal-timer.h>


static const char *const TAG = "current_cover";
static hw_timer_t *dimmer_timer = nullptr;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/// Run timer interrupt code and return in how many µs the next event is expected
void IRAM_ATTR HOT timer_interrupt() {
  // run at least with 1kHz
  uint32_t now = micros();
  ESP_LOGD(TAG, "INTERRUPT Seconds since start: %.6fs", (float)now * 1e-6f);
}

class CurrentCover : public Component, public Cover {
 public:
  void setup() override {
    // 80 Divider -> 1 count=1µs
    dimmer_timer = timerBegin(0, 80, true);
    timerAttachInterrupt(dimmer_timer, &timer_interrupt, true);
    // For ESP32, we can't use dynamic interval calculation because the timerX functions
    // are not callable from ISR (placed in flash storage).
    // Here we just use an interrupt firing every 100 µs.
    timerAlarmWrite(dimmer_timer, 1000, true);
    timerAlarmEnable(dimmer_timer);
  }
  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(false);
    traits.set_supports_position(true);
    traits.set_supports_tilt(false);
    return traits;
  }
  void control(const CoverCall &call) override {
    static float position = 0.0f;
    static bool dir = true;
    // This will be called every time the user requests a state change.
    if (call.get_position().has_value()) {
      float pos = *call.get_position();
      // Write pos (range 0-1) to cover
      // ...
      pos = position;
      
      if(dir)
      {
        position += 0.001;
        if(position >= 1.0)
        {
          position = 1.0f;
          dir = false;
        }
      }
      else
      {
        position -= 0.001;
        if(position <= 0.0)
        {
          position = 0.0f;
          dir = false;
        }
      }
      // Publish new state
      this->position = pos;
      this->publish_state();
    }
    if (call.get_stop()) {
      // User requested cover stop
    }
  }

  void loop (void) override {
    static uint32_t last = 0;
    uint32_t now = micros();
    static uint32_t cnt = 0;

    cnt++; 
    if (now - last > 1000000)
    {
      last = now;
      ESP_LOGD(TAG, "LOOP counts: %d, at %dus", cnt, now) ;
    }
    
  }
};