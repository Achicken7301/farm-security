# Farm Security

**Ideal structure** 

<img src="./docs/sys_structure.png" alt="System Structure" width="600"/>

The rectangel with 2 antenas in the middle is called router which can access through the internet.

The vertical ESP32 can be manage by esp-mesh, but i still can not figure this out (WILL WORK ON THIS).

EACH esp32 connect to 8266 by TCP protocols (already implement this), so ESP32 wifi mode is softAp + STA where:
- softAP: 8266 connect to.
- STA: connect to router (parent node - if in esp-mesh).

Range expectation of this project:
- Each ESP32 should connect to each-orther for 200m at least. There is Long Range (LR) mode in ESP32, but i hope, i will not use this.
- I'm not expect much about 8266 but i think it's be easy.

# Hardware

This will cost ALOT.

In each ESP32, we expect should have:
- 3 CAMERAs + IR led light (4 directions "minus" the one we attach device on).
- 3 PIR sensors (HC-SR501 - cheap PIR, range 7m) should do the trick.
- Battery holder (idk how much mAh and voltage is enough) -> WILL FIGURE THIS OUT.

In each 8266 is much simpler:
- 1 PIR sensors (HC-SR501).
- Battery holder.
- I2S microphone (I HOPE I CAN REACH THIS TAGE LOL).

# Crazy Ideas
- LOST MODE: Switch to softAP mode with free wifi name "IM LOST, CALL ME <phone-number>" if anyone connect to this wifi, create a http server with 1 button which can ring a bell in the device. -> I SHOULD CREATE A DEVICE FOR SEARCHING THIS LOL.