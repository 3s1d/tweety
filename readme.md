# Skytraxx Tweety
A low-cost audio-only variometer which is fully open hardware/source and Arduino compatible.
It uses a [MS5637](http://www.mouser.com/new/measurement-specialties/measurement-specialties-ms5637/) pressure sensor in combination with [Linear Regression](https://en.wikipedia.org/wiki/Linear_regression) to estimate the current climb rate. All the computations are done on an Atmel [Attiny88](http://www.atmel.com/devices/ATTINY88.aspx). The CR2450 battery should last a whole season...

![Tweety](https://github.com/3s1d/tweety/raw/master/doc/tweety.png)

You can build it on your own or buy it in our shop and modify it as you like.

## Operation

### Turn on
Press the button until you here a short beep. Release the button within in 3 seconds and Tweety turns on. You will here the welcome sound, followed by 0.5 sec pause and the battery state:
- 2 beeps: Battery is good
- 1 beep: Battery is okay
- no beep: You should consider changing the battery

### Turn off
Press the button until you hear the bye sound. Release the button.

### Configuration
The sink tone can be switched on or off.
Remove the battery. Press and hold the button while re-inserting the coin cell. Release the button. 
- 2 beeps: Sink tone on
- 1 beep: Sink tone off

A short press changes the settings. Use long press (4 sec.) to store the setting and to enter the normal operation mode again.

## Settings
See `climb.h`
- Climb threshold: > 0.5m/s
- Sink threshold: < -2.5m/s
- Averaging interval: 1.5 sec

If you want to change those settings or change the vario tone profile (`piezo.c`), you need to recompile and upload the sources.

