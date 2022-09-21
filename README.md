# ESP32 Temp API Datalogger

Example code of using an esp32-s2 with an onboard bme280 to log information to a api endpoint

## Description

I used the [Adafruit ESP32-S2 Feather with BME280 Sensor](https://www.adafruit.com/product/5303) for this project. It takes readings from the onboard Bosch BME280 temp sensor and the LiPoly battery monitor LC709203 and logs that data to an API endpoint. The esp32 deeps sleeps between readings and take a reading every 10 minutes.  The first reading is not taken until after a deep sleep cycle.  I attempted to put so WiFi error handling logic in it

### Issues

The onboard sensor gives incorrect temp data when the battery is charging.  This could be as much as 30&deg;F.  I used this board off a fully charged battery to avoid this problem

## Deployment

I used the Arduino IDE for this sketch.  Please be sure to setup the correct board
