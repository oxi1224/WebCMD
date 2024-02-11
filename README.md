# Windows WebCMD

## Table of Contents
1. [Description](#description)
2. [Requirements](#requirements)
3. [Setup](#setup)
4. [Usage](#usage)
5. [License](#license)
6. [Authors](#authors)

## Description
WebCMD uses a custom HTTP server, which allows you to execute terminal commands via a website.
The interface is secured with a login system and also allows for opening of multiple terminals at a time.

## Requirements
- Any modern Windows version (must support `WinSock2.h`)
- (Recommended) Local network connection

## Setup

##### Using Release (Recommended)
- Download `WebCMD.zip` from the latest release at [releases](https://github.com/oxi1224/ESP32-Audio-Controller/releases)
- Unpack the `WebCMD.zip` file
- Change the credentials in `.env` (**ONLY MODIFY WHAT IS WITHIN QUOTES**)
- Run `WebCMD.exe` (read [usage](#usage))
- Go to (by default) `http://localhost:4000/` in your browser

##### Compiling
- Download [CMake](https://cmake.org/)
- I recommend to compile via [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with the Windows APIs installed.

## Usage
The executable file has the following arguments which allow for customization:
- `--help` - displays information about all arguments
- `--dev` - runs the server in development mode (live reloading)
- `--staticPath [PATH]` - changes the default path to the folder containing the frontend (default: `./static`)
- `--envPath [PATH]` - chabged the default path to the `.env` file (default: `./.env`)
- `--addr [ADDRESS]` - changes the address the server will try to run on (default: `0.0.0.0` aka `localhost`)
- `--port [PORT]` - changes the port the server will try to listen on (default: `4000`)

## License
[Attribution-NonCommercial 4.0 International License](LICENSE.md)

## Authors
- [Oxi1224](https://github.com/oxi1224)
