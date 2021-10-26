/*
from http://www.hit-karlsruhe.de/aol2mime/medion_md_8800_vfd.htm
VFD-Kommandos:

Jedes Kommando beginnt mit dem Escape-Zeichen (0x1Bhex / 27dec).

0x1B 0x00 M H T M J J - Uhr stellen, Werte als BCD (2 Ziffern/Byte)
0x1B 0x01 - 24h-EU-Datum/Zeit
0x1B 0x02 - AM/PM-US-Datum/Zeit
0x1B 0x03 - Uhr still
0x1B 0x04 - Uhr bewegt
0x1B 0x05 - Uhr anzeigen

Format: Esc "0" Element Data
0x1B 0x30 0 3 - Quelle: HDD (dimmbar mit Data-Wert)
0x1B 0x30 1 3 - Quelle: FW (dimmbar)
0x1B 0x30 2 3 - Quelle: CD (dimmbar)
0x1B 0x30 3 3 - Quelle: USB (dimmbar)
0x1B 0x30 4 3 - Media: Film (dimmbar)
0x1B 0x30 5 3 - Media: TV (dimmbar)
0x1B 0x30 6 3 - Media: Musik (dimmbar)
0x1B 0x30 7 3 - Media: Foto (dimmbar)
0x1B 0x30 8 3 - VCR: REC
0x1B 0x30 9 3 - Brief außen
0x1B 0x30 10 3 - Brief innen
0x1B 0x30 11 3 - Lautstärke 1
0x1B 0x30 12 3 - Lautstärke 2
0x1B 0x30 13 3 - Lautstärke 3
0x1B 0x30 14 3 - Lautstärke 4
0x1B 0x30 15 3 - Lautstärke 5
0x1B 0x30 16 3 - Lautstärke 6
0x1B 0x30 17 3 - Lautstärke 7
0x1B 0x30 18 3 - Lautstärke rote Linie
0x1B 0x30 19 3 - UnMute
0x1B 0x30 20 3 - Mute
0x1B 0x30 21 3 - Lautstärke 1 klein
0x1B 0x30 22 3 - Lautstärke 2 klein
0x1B 0x30 23 3 - Lautstärke 3 klein
0x1B 0x30 24 3 - Quelle Rahmen
0x1B 0x30 25 3 - Media Rahmen
0x1B 0x30 26 3 - VCR Rahmen
0x1B 0x30 27 3 - Briefrahmen

0x1B 0x31 9 Byte Gfx Data für VCR Symbol vrnl, oben LSB

0x1B 0x32 0..4 - Drehgeschw. CD
0x1B 0x33 0..4 - Blinkgeschw. VCR-Rec
0x1B 0x34 x - klein LS geht aus

0x1B 0x20 - Zeile ??
0x1B 0x21 - Zeile 1 wählen
0x1B 0x22 - Zeile 2 wählen

0x1B "P" - Zeile löschen
0x1B "Q" - CR "Wagenrücklauf" (setzt Schreibcursor in erste Spalte)
0x1B "R" - Zeile einschalten
0x1B "S" - Zeile ausschalten
*/

/*
	def bcd2(value):
		z, e = divmod(value, 10)
		return (z << 4) + e
	
	def set_clock(self):
		d = datetime.datetime.now()
		y1, y2 = divmod(d.year, 100)
		self.ser.write(bytes([0x1B,0x00, self.bcd2(d.minute), self.bcd2(d.hour),
		                      self.bcd2(d.day), self.bcd2(d.month),
							  self.bcd2(y1), self.bcd2(y2)]))
		self.ser.write(bytes([0x1B, 0x03]))	 # Uhr still
		self.ser.write(bytes([0x1B, 0x05]))	 # show clock
*/

#include <map>
#include "esphome/core/application.h"
#include "esphome/core/log.h"
#include "M18ST05B.h"

static const char *TAG = "m18st05b";

namespace esphome {
namespace m18st05b {

const char ESC = 0x1B;
const uint8_t cls[] = { ESC, 'P' };    // clear selected line
const uint8_t cr[] = { ESC, 'Q' };     // carriage return
const uint8_t enable_line[] = { ESC, 'R' };
const uint8_t disable_line[] = { ESC, 'S' };

const uint8_t lineall[] = { ESC, 0x20 };   // select both lines
const uint8_t line1[] = { ESC, 0x21 };  // select line 1
const uint8_t line2[] = { ESC, 0x22 };  // select line 2

// icons
const std::map<const char *, std::array<uint8_t, 11>> icons = {
  { "empty", std::array<uint8_t, 11>{0x1B, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  { "car",   std::array<uint8_t, 11>{0x1B, 0x31, 0x00, 0x7E, 0x29, 0x39, 0x39, 0x39, 0x29, 0x7E, 0x00}},
  { "sun",   std::array<uint8_t, 11>{0x1B, 0x31, 0x00, 0x00, 0x01, 0x11, 0x08, 0x03, 0x07, 0x6F, 0x0F}},
  { "time",  std::array<uint8_t, 11>{0x1B, 0x31, 0x00, 0x1C, 0x22, 0x49, 0x49, 0x45, 0x22, 0x1C, 0x00}},
  { "temp",  std::array<uint8_t, 11>{0x1B, 0x31, 0x22, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x07, 0x05, 0x07}},
};

void M18ST05B::setup() {
  PollingComponent::setup();
  write_state(get_initial_state().value_or(true));
}

void M18ST05B::update() {
  static char old_text[DISP_HEIGHT][DISP_WIDTH+1];
  static DisplayPage *old_page;
  static bool old_state = false;

  if (state) {
    for (int i=0; i<DISP_HEIGHT; i++)
      memset(_text[i], ' ', DISP_WIDTH);

    if (old_page != page_) {  // new page - clear everything
      clear();
      old_page = page_;
    } else {  // page did not change - only update text
      write_array(lineall, 2);
      write_array(cr, 2);
    }

    this->do_update_();

    // clear screen only, if text was changed to avoid flickering
    if (0 != memcmp(old_text, _text, sizeof(_text))) 
    {
      //ESP_LOGD(TAG, "%s (%d) - %s (%d)", _text[0], strlen(_text[0]), _text[1], strlen(_text[1]));
      write_str(_text[0]);
      write_array(line2, 2);
      write_str(_text[1]);

      // remember current text
      memcpy(old_text, _text, sizeof(_text));
    }
  }
}

void M18ST05B::clear() 
{
    //ESP_LOGD(TAG, "clear");
    for (int i=0; i<= 27; i++) write_service(i, 0);
    image("empty");
    write_array(lineall, 2);
    write_array(cls, 2);
}

void M18ST05B::print(int x, int y, TextAlign align, const char *text) {
    if ((y < 0) || (y >= DISP_HEIGHT) || (x < 0) || (x >= DISP_WIDTH)) {
      ESP_LOGE(TAG, "Text out of range: x=%d, y=%d", x, y);
    }
    memset(&_text[y][0], ' ', DISP_WIDTH);
    int len = strlen(text);
    if (static_cast<int>(align) & static_cast<int>(TextAlign::RIGHT)) x = DISP_WIDTH-x-len;
    else if (static_cast<int>(align) & static_cast<int>(TextAlign::CENTER)) x = (DISP_WIDTH-x-len) / 2;
    if ((len + x) > DISP_WIDTH) len = DISP_WIDTH - x;
    strncpy(&_text[y][x], text, len);

    if (strlen(text) > DISP_WIDTH) print(x, y+1, align, text+len);  // continue on next line
    //ESP_LOGD(TAG, "%p ** (%d) %s", this, y, _text[y]);
}

void M18ST05B::print(int y, const char *text)
{
    print(0, y, TextAlign::LEFT, text);
}

void M18ST05B::printf(int x, int y, TextAlign align, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, align, format, arg);
  va_end(arg);
}

void M18ST05B::printf(int x, int y, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  this->vprintf_(x, y, TextAlign::LEFT, format, arg);
  va_end(arg);
}

void M18ST05B::vprintf_(int x, int y, TextAlign align, const char *format, va_list arg) {
  char buffer[256];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  if (ret > 0)
    this->print(x, y, align, buffer);
}

void M18ST05B::strftime(int x, int y, TextAlign align, const char *format, time::ESPTime time)
{
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    this->print(x, y, align, buffer);
}

void M18ST05B::image(const char *image)
{
  if (state) {
    auto ic = icons.find(image);
    if (ic != icons.end()) {
      write_array(ic->second.data(), 11);
    } else {
      write_array(icons.at("empty").data(), 11);
    }
  }
}

void M18ST05B::bar(float minval, float maxval, float value)
{
  if (state) {
    if (std::isnan(value)) value = 0;
    for (int i = 0; i < 7; i++)
    {
      if ((value - minval) < (maxval * (i+1) / 8))
      {
          write_service(11+i, 0);
      } else {
          write_service(11+i, 3);
      }
    }
  }
}

void M18ST05B::show_cd(uint8_t dim, uint8_t turn)
{
  if (state) {
    static uint8_t old_dim = 0;
    static uint8_t old_turn = 0;

    if (dim != old_dim) {
        write_service(2, dim);
        old_dim = dim;
    }

    if (turn != old_turn) {
        write_byte(ESC);
        write_byte(0x32);
        write_byte(turn);
        old_turn = turn;
    }
  }
}

void M18ST05B::write_service(const uint8_t id, const uint8_t data)
{
  if (state) {
    this->write_byte(ESC);
    this->write_byte(0x30);
    this->write_byte(id);
    this->write_byte(data);
  }
}

void M18ST05B::write_state(bool newState)
{
  ESP_LOGD(TAG, "Switch state from %d to %d", state, newState);
  if (false == newState) {
    clear();
    write_array(line1, 2);
    write_array(disable_line, 2);
    write_array(line2, 2);
    write_array(disable_line, 2);
  } else {
    write_array(line1, 2);
    write_array(enable_line, 2);
    write_array(line2, 2);
    write_array(enable_line, 2);
  }
  publish_state(newState);
  update();
}


}  // namespace m18st05b
}  // namespace esphome