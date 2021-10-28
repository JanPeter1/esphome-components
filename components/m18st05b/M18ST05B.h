#pragma once

#include "esphome/core/color.h"
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/uart/uart.h"

using namespace esphome::display;

namespace esphome {
namespace m18st05b {

class M18ST05B 
: public PollingComponent
, public display::DisplayBuffer
, public uart::UARTDevice
, public switch_::Switch {
public:
  static const int DISP_HEIGHT = 2;
  static const int DISP_WIDTH = 16;
  static const int BUFFER_SIZE = 9;

  M18ST05B();
  ~M18ST05B();

  void setup() override;
  void dump_config() override;

  virtual void update() override;
  virtual void clear();

  virtual void print(int x, int y, TextAlign align, const char *text);
  virtual void print(int y, const char *text);
  void printf(int x, int y, TextAlign align, const char *format, ...)
      __attribute__((format(printf, 5, 6)));
  void printf(int x, int y, const char *format, ...)
      __attribute__((format(printf, 4, 5)));
  void strftime(int x, int y, TextAlign align, const char *format, time::ESPTime time)
      __attribute__((format(strftime, 5, 0)));
  void bar(float minval, float maxval, float value);
  void show_cd(uint8_t dim, uint8_t turn);

  virtual void write_service(const uint8_t id, const uint8_t data);
  virtual void write_state(bool newState);

protected:
  virtual void draw_absolute_pixel_internal(int x, int y, Color color) override;
  virtual int get_height_internal() override { return 8; };
  virtual int get_width_internal() override { return 9; };
  void vprintf_(int x, int y, TextAlign align, const char *format, va_list arg);

  char _text[DISP_HEIGHT][DISP_WIDTH+1];
};


}  // namespace m18st05b
}  // namespace esphome
