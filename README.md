# wiring-iot-skeleton
Arduino Framework based IoT Skeleton Embedded Application, powered by [PlatformIO](http://platformio.org "Cross-platform build system").

With this project you have a good starting point to create your own application that shall be based on the Arduino Framework. 

The project is suitable for the following controller boards:

* Wemos D1 Mini
* NodeMcu
* [Adafruit ESP8266 Huzzah](https://www.adafruit.com/product/2471 "Adafruit HUZZAH ESP8266 Breakout")
* DoIt ESP32 DEVKIT V1

## Purpose
This project builds up an Arduino Framework based IoT application skeleton and it comprises of several components helping with debugging and integrating embedded applications on several different contoller modules.

This skeleton application can help you to build up your own Arduino Framework based applications while working with [PlatformIO](http://platformio.org "Cross-platform build system").

This project demonstrates how to integrate the following **components**:

* ***SpinTimer***: Universal timer with 1 millisecond resolution, supporting OOP principles, providing recurring and non-recurring mode; helps to improve your application's architecture by encapsulating the timers into your components and thus make them active
* ***Ramutils***: helps to determine the free Ram that is currently available
* ***DbgTrace***: debug trace log environment with configurable log levels
* ***DbgCLI***: interactive console environment with command tree that can be built up decentralized (from any different location in your application code and within any component)
* ***App-Dbg***: boilerplate code setting up all the debug environment such as CLI and Tracing and free RAM info printer
* ***MqttClient***: Mqtt Client wrapping araound ***PubSubClient*** library, monitoring the LAN and Mqtt conection, able to automatically re-connect on connection loss, providing auto publish for selectable topics and auto subscribe for all registered topic subscriptions on re-connection, supports multiple subscritions also with wildcards in the topic path
* ***ArduinoJson***: 

The **command line interface** provides the following **functionality**:  

* set the trace level of the free heap monitoring printer
* configure the WiFi access point (SSID & Password) and show the WiFi connection status and the available hotspots around your device
* MQTT features:
  * connect to / disconnect from a broker
  * subscribe / unsubscribe to / from MQTT Topics
  * publish to MQTT Topics
* enable / disable LED Test Blink Publisher

This skeleton application demonstrates how to integrate libraries provided by the PlatformIO ecosystem and also how to use your own libraries and can help you to build up your own Arduino Framework based applications with focus on IoT. 

The following components are in focus:

* [SpinTimer](https://github.com/dniklaus/spin-timer)
* [Debug-Cli](https://github.com/ERNICommunity/debug-cli)
* [Dbg-Trace](https://github.com/ERNICommunity/dbg-trace)
* [Mqtt-Client](https://github.com/ERNICommunity/mqtt-client)
* [MQTT](https://github.com/256dpi/arduino-mqtt)

## Toolchain
[PlatformIO](http://platformio.org "Cross-platform build system") is the ideal foundation when developing IoT devices. It supports cross-platform development for many different controller boards widely used in the maker field and also for industrial applications.

### Installation
#### Python 3.7

**Windows**

* download Python 3.7.x or later from: https://www.python.org/downloads/
* select destination directory: keep default
* make sure python.exe is added to path

**Linux**

Install Python 3.7 or later using your package manager.

#### PlatformIO
Install PlatformIO using the Python Package Manager
(see also http://docs.platformio.org/en/latest/installation.html#python-package-manager):

* in a cmd shell enter: `pip install -U platformio`
* upgrade pip, in the cmd shell enter: `python -m pip install --upgrade pip`
* upgrade PlatformIO, in the cmd shell enter: `pio upgrade`

### Working with Visual Studio Code

#### How to build for VSCode

  1. Create a directory where your code project shall be cloned into. E.g. `C:\git\pio-prj`
  2. Clone this repository into the folder you created before: 
     `git clone git@github.com:ERNICommunity/wiring-iot-skeleton.git` (ssh) or
     `git clone https://github.com/ERNICommunity/wiring-iot-skeleton.git` (ssl)
  3. Open a command shell in the just cloned project folder, i.e. in `C:\git\pio-prj\wiring-iot-skeleton`
  4. Run the command `pio init --ide vscode`. 
     This prepares the project to be edited using Visual Studio Code.
  5. Run the command `pio run`, this starts the project build 

Note: Print a list of all available boards with the command: `pio boards`

#### Open project in VSCode

  1. Open VSCode
  2. In menu choose *File > Open Folder...* and choose the folder you cloned the project into before, i.e 
     `C:\git\pio-prj\wiring-iot-skeleton`

### Working with Eclipse CDT

#### How to build for Eclipse CDT

  1. Create a directory where your Eclipse Workspace will be stored and where this project shall be cloned into. E.g. `C:\git\pio-prj`
  2. Clone this repository into the folder you created before: 
     `git clone git@github.com:ERNICommunity/wiring-iot-skeleton.git` (ssh) or
     `git clone https://github.com/ERNICommunity/wiring-iot-skeleton.git` (ssl)
  3. Open a command shell in the just cloned project folder, i.e in `C:\git\pio-prj\wiring-iot-skeleton`
  4. Run the command `pio init --ide eclipse`. 
     This prepares the project to be edited using Eclipse CDT.
  5. Run the command `pio run`, this starts the project build 

Note: Print a list of all available boards with the command: `pio boards`

#### Open project in Eclipse CDT

  1. Open Eclipse CDT, choose the folder you created before as workspace, i.e `C:\git\pio-prj`
  2. Import the project with *File > Import > General > Existing Projects* into Workspace, choose the 
     `wiring-iot-skeleton` (i.e `C:\git\pio-prj\wiring-iot-skeleton`)

### Connect Terminal Emulation
In order to test and run the CLI commands, a terminal emulation program shall be used. The one giving you the best experience will be the [HTerm](http://www.der-hammer.info/terminal/). 

Load the _hterm-com10.cfg_ file (to be found in the project root directory) to configure HTerm properly. Alter the *COM10* accordingly to the one that has been selected on your computer.

## Debug Features
### Debug CLI Commands
#### Command Tree
     dbg                      Debug CLI root node
       tr                     Debug Trace Port config access
         heap                 Particular Trace Port (heap: see below in chapter Trace Port)
           out                Trace Output config access for the current trace port
             get              Show the assigned Trace Output
             set <outName>    Set a particular Trace Output name
             list             Show all available Trace Output names (& currently selected)
           lvl                Trace Level config access for the current trace port
             get              Show the current trace level
             set <level>      Set a particular trace level
             list             Show all available trace levels (& currenntly selected)
         conmon               Trace Port: LAN and MQTT connection monitor                
         mqttctrl             Trace Port: MQTT client control
         mqttrx               Trace Port: Received Messages from MQTT subscriptions
         mqttdflt             Trace Port: Default Mqtt Subscriber
         mqttled              Trace Port: Test LED Mqtt Subscriber
       wifi                   WiFi debug commands
         mac                  show WiFi MAC address
         nets                 list available WLAN networks
         stat                 Show WiFi status
         dis                  Disconnect WiFi
         con <SSID> <Pass>    Connect WiFi
       mqtt                   MQTT client control commands
         con                  Connect to broker
         dis                  Disconnect from broker
         sub <Topic>          Subscribe to MQTT Topic
         unsub <Topic>        Unsubscribe to MQTT Topic
         pub <Topic> <Value>  Publish Value to MQTT Topic
       ledpub                 LED Test Blink Publisher commands
         en                   Enable LED Test Blink Publisher
         dis                  Disable LED Test Blink Publisher

#### Example commands
* `dbg tr heap lvl set debug`
* `dbg ledpub en`


### Trace Port

|Trace Port|default level|functionality|
|-----------|-------------|:-------------------------------------------------------------------------------------|
|heap       |info         |if set to debug level: automatically print free heap memory [bytes], every 10 seconds |
|conmon     |info         |if set to debug level: show WLAN connect / disconnect status updates                  |
|mqttctrl   |info         |if set to debug: show MQTT Client connection status                                   |
|mqttrx     |info         |if set to degug level: show subscribed incoming messages                              |
|mqttled    |debug        |show incoming ledpublisher blink messages                                             |
|mqttdfltsub|debug        |show incoming messages for subscribed topics (subscribed by CLI: dbg mqtt sub <topic>)|


## Library Usage
This chapter lists all the libraries this project is using.

### PlatformIO Libraries

Name         |URL                                             |Description                                                                          |
|:------------|:-----------------------------------------------|:-----------------------------------------------------------------------------------|
|SerialCommand|https://github.com/kroimon/Arduino-SerialCommand|A Wiring/Arduino library to tokenize and parse commands received over a serial port.|
|spin-timer |https://github.com/dniklaus/spin-timer|Universal recurring or non-recurring Timer.|
|debug-cli|https://github.com/ERNICommunity/debug-cli|Debug CLI for Embedded Applications - Command Line  Interface for debugging and testing based on object oriented tree structure.|
|dbg-trace|https://github.com/ERNICommunity/dbg-trace|Debug Trace component for Embedded Applications - Debug and Trace Log message system based on trace ports with adjustable levels.|
|ThingSpeak   |https://github.com/mathworks/thingspeak-particle|"MathWorks": ThingSpeak Communication Library for Arduino & ESP8266                 |
|256dpi/MQTT |https://github.com/256dpi/arduino-mqtt                    |MQTT library for Arduino, bundling the [lwmqtt](https://github.com/256dpi/lwmqtt) MQTT 3.1.1 client and adds a thin wrapper to get an Arduino like API.                                                |
|ArduinoJson  |https://github.com/bblanchon/ArduinoJson        |An elegant and efficient JSON library for embedded systems                          |



### Homebrew Libraries

|Name       |URL                                             |Description                                                                                                                      |
|:----------|:-----------------------------------------------|:--------------------------------------------------------------------------------------------------------------------------------|
|Mqtt-Client|https://github.com/ERNICommunity/mqtt-client    |ERNI Community MQTT Client with pluggable publish and subscribe topic objects and connection status monitoring                   |
|RamUtils   |https://github.com/dniklaus/arduino-utils-mem   |Arduino Memory Utilities, helps to show free heap space                                                                           |
|App-Debug  |https://github.com/dniklaus/wiring-app-debug.git|Wiring application debug setup component                                                                                         |



## Create a new project based on this skeleton application

1. On **GitHub:** create new repository, i.e. *my-test*
2. Within a **Git Bash:**


```bash
  git clone --bare git@github.com:ERNICommunity/wiring-iot-skeleton.git
  cd ./wiring-iot-skeleton.git
  git remote rm origin
  git remote add origin git@github.com:your-name/my-test.git
  git push --mirror
```
