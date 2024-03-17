# salt-level-meter

A water softener uses salt to decscale the hard water. The salt water is pumped to the resin core to get rid of the calcium and is drained to the sewage.

https://en.wikipedia.org/wiki/Water_softening

Sooner or later all the salt in the vessel is dissolved so you'll need to take a peek now and then to see if a refill is needed.

This project uses an ESP32 device with a HC-SR04 ultrasonic sensor to  measure a distance. The ESP32 is connected to the wireless network and delivers messages with the measured distance to a MQTT server.

### TO DO

Find or create a design for a casing that easily fits in the salt vessel.