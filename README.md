# GameBoy MK3 Build
Scripts collection for the GB MK3 project

![GBMK3](/images/gbmk3.jpg)

**ProMicro_GB3.ino** handles the keypad, battery meter and brightness control using direct port manipulation and debouncing.

**BoostConverter** drives the screen backlight through an Attiny85-driven boost converter.

Discussion here: https://www.sudomod.com/forum/viewtopic.php?f=43&t=4745

My second build is based on a raspberry pi 3 and a DPI screen. I have already built a GBZero with a standard composite screen I have never been satisfied of, so while designing this mk3 (yes, it's just for the raspi3) a dpi screen was a must-have feature.
With the idea of building all around the screen I had to sacrifice some feature from the previous version such as the GPIO buttons, I2C and serial.

Here's a list of feature I implemented in this build:

- A powerful RPi3
- DPI 18bit
- Adjustable backlight brightness (via a self-made boost-converter!)
- Arduino PRO micro keyboard/peripherals managment
- Still include RTC support via I2C
- Custom OSD for battery/wifi/temp meter (https://github.com/vascofazza/Retropie-open-OSD)
- Automatic HDMI switching (https://github.com/vascofazza/Retropie-ES-Scripts)
- Internal cooling fan
- Fancy status led(s)


Here it is:

![1.jpg](https://www.sudomod.com/forum/download/file.php?id=9342&sid=78253282ed044cebc24026680518f067)

other pics

![hdmi.jpg](https://www.sudomod.com/forum/download/file.php?id=9337&sid=78253282ed044cebc24026680518f067)

Completed with adapded/modified OSD, key shortcuts and automatic/hot-plug HDMI clone script (automatically detects when cabe is inserted).

**guide for display mirroring** here:
https://sudomod.com/forum/viewtopic.php?f=20&t=4912

**SUPER free OSD** here:

https://sudomod.com/forum/viewtopic.php?f=20&t=4927

![test.png](https://www.sudomod.com/forum/download/file.php?id=9473&sid=78253282ed044cebc24026680518f067)

![test1.png](https://www.sudomod.com/forum/download/file.php?id=9474&sid=78253282ed044cebc24026680518f067)