# esphome-components
A collection of additional components for ESPHome

## M18ST05B
Initial version of a UART-based display component for the Medion
MD18ST05B display. It was used in older computers from Medion -
see http://www.hit-karlsruhe.de/aol2mime/medion_md_8800_vfd.htm

### Features
- print text (including printf, row/column, text alignment)
- strftime function to show the current time (note: RTC of the
  display is not yet used)
- having "display pages" to show different pages
- derived from switch component so that the display can be turned
  off and on ("off" = no text shown)
- set the small 9x8 pixel image
- draw on thae pixel image (line, circle, etc.)
- set the bar graph and some other special items of the display

### Usage
connect the display to +5V, GND and a Tx UART pin (I'm using the
second UART on a D1 mini (GPIO2/D4)).
Inside the esphome Yaml configuration file include the md18st05b
component and configure the UART:
```
external_components:
  source: github://JanPeter1/esphome-components
  components: [ md18st05b ]

uart:
  tx_pin: 2
  baud_rate: 9600
  id: uart_disp
```
Now add the display component:
```
md18st05b:
  id: my_display
  name: "My Display"
  pages:
    - id: page_time
      lambda: |-
        m18st05b::M18ST05B& mit = (m18st05b::M18ST05B&)it;
        mit.strftime(0, 0, TextAlign::CENTER, "%H:%M:%S", id(homeassistant_time).now());
        mit.strftime(0, 1, TextAlign::CENTER, "%d.%m.%Y", id(homeassistant_time).now());
        mit.line(1, 0, 6, 6)
        mit.circle(4, 3, 3)
    - id: page_pv
      lambda: |-
        m18st05b::M18ST05B& mit = (m18st05b::M18ST05B&)it;
        mit.printf(0, 0, "PV:  %.1f kWh", id(solaredge_production_power).state / 1000);
        mit.printf(0, 1, "akt: %.0f W", id(solaredge_current_power).state);
        mit.bar(0, 5000, id(solaredge_current_power).state);
        if (id(solaredge_current_power).state > 10) mit.show_cd(3, 2);
    ...
```

## Image9x7
Allows the definition of small 9x7 black-and-white images for the usage in the small
pixel display of the MD18ST05B.

```
image_9x7:
  - id: image_empty
    data: [0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00]
  - id: image_temp
    data: [0x22, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x07, 0x05, 0x07]
```
Usage:
```
  id(my_display)->image(0, 0, id(image_temp));
```

### Using icons from TTF font
The small 9x7 display can also be used to show single characters from a TTF front.
Here is an example using MDI icons:
```
substitutions:
  devicename: "My Display"
  mdi_car: "\U000F010B"
  mdi_clock_outline: "\U000F0150"

font:
  - file: "fonts/materialdesignicons-webfont.ttf"
    id: mat_font
    size: 9
    glyphs: [ 
      $mdi_car,
      $mdi_clock_outline,
    ]

m18st05b:
  id: my_display
  name: $devicename
  pages:
    - id: page_time
      lambda: |-
        m18st05b::M18ST05B& mit = (m18st05b::M18ST05B&)it;
        mit.strftime(0, 0, TextAlign::CENTER, "%H:%M:%S", id(homeassistant_time).now());
        mit.strftime(0, 1, TextAlign::CENTER, "%d.%m.%Y", id(homeassistant_time).now());
        it.print(0, -1, id(mat_font), "$mdi_clock_outline");
     ...
```

## WiFi_CSI
This component implements a motion detection sensor using the WiFi RSSI signal without the need
for any additional hardware.
Currently it calculates the average of several RSSI signal readings and reports a motion, if
this average deviates from the current RSSI reading by a certain amount.

Without an antenna is seems to work on my 8266 D1 mini board, but the accuracy is not very
good (ie. you have to be quite close to detect a motion). More tests are required...

### Usage
```
external_components:
  source: github://JanPeter1/esphome-components
  components: [ wifi_csi ]
  
wifi_csi:
  id: csi_motion
  name: Motion detected
#  timing: 101ms
#  buffer_size: 100
#  hysteresis: 2
```

timing is an optional parameter and gives the cycle time in which the RSSI value is read (default value is 100ms).
buffer_size is the length of the buffer (ie. the amount of RSSI readings over which the average value gets calculated, default value is 100)
hysteresis is the minimal deviation from the average value that leads to a motion detection (default value is 2)
