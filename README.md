# ubidot-votebox

This project is a IOT customer satisfaction voting box using the (free) Ubidots library and service. The intention is to collect a simple -1/0/+1 metric with timestamps on multiple devices without creating extra datasource in Ubidots.

# Hardware:

* NodeMCU ESP8266 board
* PL9823 LED (wired with ~100R in series on data line) - multi-colour feedback for clear signal of device status
* 3 buttons (wired to ground)

# Method of Operation:

1. Poll box connects to Ubidots service
2. Unit ready to recieve votes, checks 3 buttons (bad, neutral, good) in a non-blocking manner. Colour indicates vote (red, yellow, green)
3. State machine waits for button release, then transmits vote
4. Non-blocking delay before returning to (2) to prevent vote spam.

# Future Development

* Intention of eventually adding Wifimanager support for field configuration of Wifi SSID/PASS
