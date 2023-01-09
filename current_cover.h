#include "esphome.h"

class CurrentCover : public Component, public Cover {
 public:
  void setup() override {
    // This will be called by App.setup()
    pinMode(5, INPUT);
  }
  CoverTraits get_traits() override {
    auto traits = CoverTraits();
    traits.set_is_assumed_state(false);
    traits.set_supports_position(true);
    traits.set_supports_tilt(false);
    return traits;
  }
  void control(const CoverCall &call) override {
    // This will be called every time the user requests a state change.
    if (call.get_position().has_value()) {
      float pos = *call.get_position();
      // Write pos (range 0-1) to cover
      // ...

      // Publish new state
      this->position = pos;
      this->publish_state();
    }
    if (call.get_stop()) {
      // User requested cover stop
    }
  }
};