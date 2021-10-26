# esphome-components
A collection of additional components for ESPHome

## M18ST05B
Initial version of a UART-based display component for the Medion
MD18ST05B display. It was used in older computers from Medion -
see http://www.hit-karlsruhe.de/aol2mime/medion_md_8800_vfd.htm

## Features
- print text (including printf, row/column, text alignment)
- strftime function to show the current time (note: RTC of the
  display is not yet used)
- having "display pages" to show different pages
- derived from switch component so that the display can be turned
  off and on ("off" = no text shown)
- set the small 9x8 pixel image
- set the bar graph and some other special items of the display

## Usage
connect the display to +5V, GND and a Tx UART pin (I'm using the
second UART on a D1 mini (GPIO2/D4)).
Inside the esphome Yaml configuration file configure the UART:
```
  tx_pin: 2
  baud_rate: 9600
  id: uart_disp
```
And add the display component:
```
md18st05b:
  id: my_display
  name: $upper_devicename
  pages:
    - id: page_time
      lambda: |-
        m18st05b::M18ST05B& mit = (m18st05b::M18ST05B&)it;
        mit.strftime(0, 0, TextAlign::CENTER, "%H:%M:%S", id(homeassistant_time).now());
        mit.strftime(0, 1, TextAlign::CENTER, "%d.%m.%Y", id(homeassistant_time).now());
        mit.image("time");
    - id: page_pv
      lambda: |-
        m18st05b::M18ST05B& mit = (m18st05b::M18ST05B&)it;
        mit.printf(0, 0, "PV:  %.1f kWh", id(solaredge_production_power).state / 1000);
        mit.printf(0, 1, "akt: %.0f W", id(solaredge_current_power).state);
        mit.bar(0, 5000, id(solaredge_current_power).state);
        if (id(solaredge_current_power).state > 10) ((m18st05b::M18ST05B&)it).show_cd(3, 2);
        mit.image("sun");
    ...
```

