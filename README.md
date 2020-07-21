<p align="center"><img alt="MudPi Smart Garden" title="MudPi Smart Garden" src="https://mudpi.app/img/mudPI_LOGO_small_grad.png" width="200px"></p>

# MudPi Passive Node
> ESP32 firmware to monitor battery voltage and take sensor readings. The unit is typically powered off a battery with solar charging and spends most of the time in deep sleep.


## Installation
Download the repository and place it inside your `Arduino` folder. 

Open the Arduino IDE and select `passive-node`. 

Navigate to the `config.h` file and make your desired configuration changes such as adding your WiFi credentials. 

Flash firmware to the device (ESP32)

## How It Works
The node will wake up from deep sleep and cycle through the sensors you set in your configuration. It will take a reading of each of the sensors and then attempt to send this data over WiFi as JSON to the `database_endpoint` that you set in the `config.h` file. 
The data will arrive in a JSON format like the one below:
```
{
  "source": 4 // Device ID 
  "boots": 17 // The current boot count
  "value": [ // array of sensor readings (see below)
    {
      "value": 1300, // raw value of the sensor read
      "sensor": "soil", // sensor that took the reading
      "parsed": 70 // value turned into percent
      "boots": 17 // boot count the reading was taken on
    }
  ] 
}
```

## Database Endpoint
You are expected to provide your own endpoint or use the MudPi one (once it becomes available) in order to store the readings sent by the node. The endpoint can be local or a remote address. The node will send the data to the endpoint and wait for a `200` response. The endpoint you use should be able to recieve the JSON object in the format shown above. 

## Fail Protection
The node will attempt to send up to 10 previous failed readings if a failure occurs during sending. The failed readings will attempt to be sent BEFORE the new reading from the current boot cycle. The readings are stored in RTC memory and will persist over deep sleep. Once the device wakes up it checks for failed readings and will handle those first. Keep in mind if the device crashes or restarts otherwise these readings will be lost as they are in memory.

## Passive vs Active Nodes
A passive node is one that is designed to handle its own logic and configuration. This node will perform its operation independently and then send it to the main controller. This allows for the passive node to be in deep sleep or perform other complex operations and then notify MudPi of the results. This is different from an active node which awaits for its instruction and commands from the main controller. This requires an 'always on' connection so that it does not miss a command. An always on connection can consume more power and is harder to use on battery. 

The other main consideration when choosing passive vs active nodes is the role the node fulfills. A active node fits better in mission critical components that requires a more consistent communication between the controller and node. Where a passive node is better suited to run components that it may be ok to miss a reading here and there. For example its ok if I miss one soil reading out of 30 sent every hour. Where its not ok if I miss a signal to turn off a pump ever. Systems can be comprised of both passive and active nodes to solve a variety of setup needs.

## Early Version
Any contributions you can make will be greatly appreciated. This is early version firmware and not guaranteed to work for every device or sitatuon. I am only a solo developer balancing many projects so updates come as time and funding permits. 


## Versioning
Breaking.Major.Minor


## Authors
* Eric Davisson  - [Website](http://ericdavisson.com)
* [Twitter.com/theDavisson](https://twitter.com/theDavisson)

## Community
* Discord  - [Join](https://discord.gg/daWg2YH)
* [Twitter.com/MudpiApp](https://twitter.com/mudpiapp)


## Documentation
*Please note this firmware is in early stages and full documentation is not available.*

For general documentation visit [mudpi.app](https://mudpi.app/docs)


## Hardware Tested On
* ESP32
* ESP8266 (swap the wifi library in `NetworkController`)

Let me know if you are able to confirm tests on any other devices

## Sensors Supported
- [x] Soil

- [x] Voltage Sensor (3.7v Battery)

- [ ] DHT 11 / 22

- [ ] Pressure / Barometer

- [ ] Light

- [ ] Onewire Temperature

- [ ] Float 

- [ ] PH Meter

- [ ] EC Meter


## License
If you use this project share it online or send me a postcard! :)


<img alt="MudPi Smart Garden" title="MudPi Smart Garden" src="https://mudpi.app/img/mudPI_LOGO_small_flat.png" width="50px">

