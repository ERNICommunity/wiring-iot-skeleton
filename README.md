# wiring-iot-skeleton

Arduino Framework based IoT Skeleton Embedded Application, powered by [PlatformIO](http://platformio.org "Cross-platform build system").

With this project you have a good starting point to create your own application that shall be based on the Arduino Framework.

The project is suitable for the following controller boards:

* [LOLIN D32](https://www.wemos.cc/en/latest/d32/d32.html#technical-specs)
* DoIt ESP32 DEVKIT V1
* Wemos D1 Mini
* NodeMcu
* [Adafruit ESP8266 Huzzah](https://www.adafruit.com/product/2471 "Adafruit HUZZAH ESP8266 Breakout")

## Purpose

This project builds up an Arduino Framework based IoT device application skeleton and it comprises of several components helping with debugging and integrating embedded applications on several different contoller modules.

This skeleton application can help you to build up your own Arduino Framework based applications while working with [PlatformIO](http://platformio.org "Cross-platform build system").

This project demonstrates how to integrate the following **components**:

* ***SpinTimer***: Universal timer with 1 millisecond resolution, supporting OOP principles, providing recurring and non-recurring mode; helps to improve your application's architecture by encapsulating the timers into your components and thus make them active
* ***Ramutils***: helps to determine the free Ram that is currently available
* ***DbgTrace***: debug trace log environment with configurable log levels
* ***DbgCLI***: interactive console environment with command tree that can be built up decentralized (from any different location in your application code and within any component)
* ***App-Dbg***: boilerplate code setting up all the debug environment such as CLI and Tracing and free RAM info printer
* ***ArduinoJson***: C++ JSON library for Arduino and IoT (Internet Of Things)

The **command line interface** provides the following **functionality**:  

* set the trace level of the free heap monitoring printer
* configure the WiFi access point (SSID & Password) and show the WiFi connection status and the available hotspots around your device

This skeleton application demonstrates how to integrate libraries provided by the PlatformIO ecosystem and also how to use your own libraries and can help you to build up your own Arduino Framework based applications with focus on IoT.

The following components are in focus:

* [SpinTimer](https://github.com/dniklaus/spin-timer)
* [Debug-Cli](https://github.com/ERNICommunity/debug-cli)
* [Dbg-Trace](https://github.com/ERNICommunity/dbg-trace)

## Toolchain

[PlatformIO](http://platformio.org "Cross-platform build system") is the ideal foundation when developing IoT devices. It supports cross-platform development for many different controller boards widely used in the maker field and also for industrial applications.

### Installation

#### Python 3.7

**Windows**

* download Python 3.7.x or later from: <https://www.python.org/downloads/>
* select destination directory: keep default
* make sure python.exe is added to path

**Linux**

Install Python 3.7 or later using your package manager.

#### PlatformIO

Install PlatformIO using the Python Package Manager
(see also <http://docs.platformio.org/en/latest/installation.html#python-package-manager>):

* in a cmd shell enter: `pip install -U platformio`
* upgrade pip, in the cmd shell enter: `python -m pip install --upgrade pip`
* upgrade PlatformIO, in the cmd shell enter: `pio upgrade`

### Working with Visual Studio Code

#### Getting started

  1. Create a directory where your code project shall be cloned into. E.g. `C:\git\pio-prj`
  2. Clone this repository into the folder you created before:
     `git clone git@github.com:ERNICommunity/wiring-iot-skeleton.git` (ssh) or
     `git clone https://github.com/ERNICommunity/wiring-iot-skeleton.git` (ssl)
  3. Open a command shell in the just cloned project folder, i.e. in `C:\git\pio-prj\wiring-iot-skeleton`
  4. Run the command `pio init --ide vscode`.
     This prepares the project to be edited using Visual Studio Code.

#### Open project in VSCode

  1. Open VSCode
  2. In menu choose *File > Open Folder...* and choose the folder you cloned the project into before, i.e
     `C:\git\pio-prj\wiring-iot-skeleton`

#### How to select your target platform

  1. Press <kbd>F1</kbd> to open the command menu
  2. Enter "platformIO: Switch Project Environment"
  3. Select the desired target platform. If your platform is not present, it may not be supported.

#### How to build the project

  1. Press <kbd>F1</kbd> to open the command menu
  2. Enter "platformIO: Build"

  &emsp;&nbsp; or:

  1. On windows, press: <kbd>ctrl</kbd> + <kbd>alt</kbd> + <kbd>B</kbd>

#### How to upload/flash the target platform

  1. Press <kbd>F1</kbd> to open the command menu
  2. Enter "platformIO: Upload"

  &emsp;&nbsp; or:

  1. On windows, press: <kbd>ctrl</kbd> + <kbd>alt</kbd> + <kbd>U</kbd>

#### How to build and upload the file system

  1. Click on the PlatformIO on the left bar menu
  2. In the PROJECT TASKS menu select your target platform name and open the Platform menu
  3. Click on "Build Filesystem Image"
  4. Click on "Upload Filesystem Image"

  ![build and upload file system img](doc/img/FileSystem.PNG)

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

Load the *hterm-com10.cfg* file (to be found in the project root directory) to configure HTerm properly. Alter the *COM10* accordingly to the one that has been selected on your computer.

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
       wifi                   WiFi debug commands
         mac                  show WiFi MAC address
         nets                 list available WLAN networks
         stat                 Show WiFi status
         dis                  Disconnect WiFi
         con <SSID> <Pass>    Connect WiFi

#### Example commands
* `dbg tr heap lvl set debug`

### Trace Port

|Trace Port|default level|functionality|
|-----------|-------------|:-------------------------------------------------------------------------------------|
|heap       |info         |if set to debug level: automatically print free heap memory [bytes], every 10 seconds |

## Library Content

This chapter lists and explains all the libraries in this project. You can find the libraries under the 'lib' folder.

Libraries are divided in two application libraries and interface libraries.

### Application Libraries

Application libraries are meant to be agnostic from the platform they are deployed to. No code that bounds these libraries to a platform should be added.

* AppHandler
  * Offers the full functionality of the repo by wrapping all the other application libraries, meaning only this library must be included. It provides an Arduino like interface to setup and loop. It provides wrapper functions to expose required functionality from the application libraries.

* AzureHAndler
  * Offers functionality to register devices to an IoT Hub through a DPS, send telemetry, device twin changes and receive messages from the IoT Hub.

* ConfigHandler
  * Offer functionality to configure the board and other application libraries. Configuration of the WiFi, oft access point, device location, azure credentials and landing page are available.

* ConfigTypes
  * Holds together all the common types shared by all the application libraries.

* LandingPageHandler
  * Offers functionality for a web server that exposes the device's configuration, accessible through the IP address of the device. The landing page can be accessed by either connecting to the device soft access point or to the same WiFi network the device is connected to. Both, the device's soft access point IP or the WiFi IP (if connected) are printed in the log at boot time.

### Interface Libraries

Interface libraries are meant to connect the application libraries to any specific target platform. This way, if the need to support a new platform arises, only these libraries need to be updated.

* FileHandler - Interface for File System handling
* WifiHandler - Interface for board WiFi handling

TODO: Create interface libraries for JSON, MQTT, SNTP and HTTP. Move the direct code used in application libraries to the interface libraries.

## Library Usage

This chapter lists all the libraries this project is using.

### PlatformIO Libraries

Name         |URL                                             |Description                                                                          |
|:------------|:-----------------------------------------------|:-----------------------------------------------------------------------------------|
|SerialCommand|<https://github.com/kroimon/Arduino-SerialCommand>|A Wiring/Arduino library to tokenize and parse commands received over a serial port.|
|spin-timer |<https://github.com/dniklaus/spin-timer>|Universal recurring or non-recurring Timer.|
|debug-cli|<https://github.com/ERNICommunity/debug-cli>|Debug CLI for Embedded Applications - Command Line  Interface for debugging and testing based on object oriented tree structure.|
|dbg-trace|<https://github.com/ERNICommunity/dbg-trace>|Debug Trace component for Embedded Applications - Debug and Trace Log message system based on trace ports with adjustable levels.|
|ArduinoJson  |<https://github.com/bblanchon/ArduinoJson>        |An elegant and efficient JSON library for embedded systems                          |

### Homebrew Libraries

|Name       |URL                                             |Description                                                                                                                      |
|:----------|:-----------------------------------------------|:--------------------------------------------------------------------------------------------------------------------------------|
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
