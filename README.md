# Facens-Rockets
This is the codes that we use in our model rocket, it is still in development so it may not be working properly yet.

## Our project

### Inside the rocket
We are producing the avionics of a model rocket that has to achieve 1km of apogee and then fire the drogue, after this, when the rocket are at +-500m it has to fire the secondary parachute.

We are capturing the altitude to make the rocket work, and the acceleration to store it and use that information to help the development of the rocket

After the landing the Esp32 will start it's WiFi in AP mode so that we can search and find it faster.

### Connections
We are using other 3 Esp32 to communicate with the rocket, we want to fire the rocket at a 500m distance from the rocket. To do that we use the EspNow protocol together with external antennas, we will press a button and the Esp that are with us will send a message to other Esp32, this Esp32 will send it to the Esp32 controlling the ignition, that will then fire the rocket.

We are aware that a LoRa would make it better and easier, but we already had the Esp32 and antennas. 

## We are using the following itens:

### Inside the rocket
- Esp32
- Bmp 180
- MPU 6050
- SD adapter
- External battery

### Used for connections
- 3x Esp32
- 3x Wifi antenna
- 3x External battery
