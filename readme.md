# Pixfight

This is an opensource crossplatform clone of [WeeWar](https://web.archive.org/web/20150315005124/http://weewar.com/) pixelart turn based strategy with AI. Done in C++14 with OpenGL 2.0 ES / 3.2 Core. With native applications for each platform (iOS, macOS, Linux, Windows, Android) 

## System Requirements:

Basically this should run on modern sandwich toasters: 

* 32Bit operating system 
* Single core CPU running at about 800Mhz (dual core 1Ghz recommended)
* 128MB RAM (256 recommended)
* GPU able to run GL2.0 ES or 3.2Core Profile with about 128MB (256 recommended)
* About 20MB disk space (depends on platform and build type)
* Music card (PC) 
* Mouse (PC)
* Monitor capable of running in 16bit colour mode with at least 1024x768 resolution (PC)

## Controls

Desktop:

* RMB move map
* LMB select
* Scroll wheel to zoom

Mobile:

* pinch to scale 
* use two fingers to move
* tap to select unit 
  
## How to play:

Your main task is to capture all bases on map, to do that you need infantry units. Capturing the base take 2 turns. You can repair damaged unit (not infantry unit) if you move back into your own base. Repair cost 1 turn.
If unit stays on base it gets extra defence bonus.

* Click / tap on base to open build menu

* After selecting unit it will show its area of movement, tap within grey area to select path and confirm it by tapping again by tapping / clicking on destination point.
* To attack tap/klick on your unit and select unit to attack (if you are close enough / artillery use range of movement)

### Units:

![Infantry](../master/CORE/Resources/textures/icons/icon_infantry1.png) - Infantry unit, cost 75. Can capture bases. Strong against other infantry units, weak against Jeep.  

![Bazooka](../master/CORE/Resources/textures/icons/icon_bazooka1.png) - Bazooka unit, cost 150. Can capture bases. Strong against mobile units, weak against Jeep.  
 
![Jeep](../master/CORE/Resources/textures/icons/icon_jeep1.png) - Jeep unit, cost 200. Strong against infantry units, weak against Tank or Artillery.  

![Tank](../master/CORE/Resources/textures/icons/icon_tank1.png) - Tank unit, cost 300. Strong against mobile and infantry units, weak against Bazooka unit. 

![Artillery](../master/CORE/Resources/textures/icons/icon_artillery1.png) - Artillery unit, cost 200. Strong against any unit. May shoot at distance. If you move it first it cannot shoot. Weak agains Tank and Bazooka unit.


## Download

Latest stable versions:

[Android](https://github.com/inFullMobile/pixfight/releases)  
[Windows](https://github.com/inFullMobile/pixfight/releases)  
[Linux](https://github.com/inFullMobile/pixfight/releases) - Please follow building instructions to run it  
[macOS](https://github.com/inFullMobile/pixfight/releases)  
[iOS](https://developer.apple.com/xcode/) - Please download the source code and build it using Xcode


## FAQ:

Q: Can you provide a *.ipa for me?  
A: No, if you want to run this game on iPhone/iPad please download the Xcode and build it for yourself.

Q: Can I make a spinoff / modify it?   
A: Of course the whole idea of this project is to motivate people to make this even better.

Q: Do you consider support for other platforms like Switch, PS4, HTML5?  
A: Yes, but I don't have access to the hardware required to build it or test it. Most likely I will try to build HMTL5 version using Web-assembly.

Q: Can I contribute ?  
A: Yes! If you think you are able to help adding another platform drop me a line or fork this repo and make pull request.

Q: Why did you not make it a true cross-platform (Raw OpenGL + C++) ?  
A: I wanted to learn new languages and libraries and understand how to connect C++ code to native application.


## Gallery

![iphone](../master/images/iphone.png)
![Android](../master/images/pixandroid.png)  
![Linux](../master/images/pixlinux.png)
![macOS](../master/images/pixmacos.png)   
![Windows](../master/images/pixwin32.png)  

## Contents

[1. Idea](#1-idea)  
[2. Credits](#2-credits)  
[3. 3rd party libraries](#3-3rd-party-libraries)  
[4. Building](#4-building)  
[5. Next steps](#5-next-steps)   
[6. License](#6-license)

# 1. Idea

The root idea behind this project is to show how to integrate C++ code with some 3rd party libraries into your native application, done in other platforms as a "business logic" Core game logic is written in C++14 and use FMOD, OpenGL and freetype2 as dependencies. Each platform uses this Core to render game in their native environment. (e.g: Android in Java, iOS in objC etc.)

AI in this game is based on the following article from [gamasutra](https://www.gamasutra.com/view/feature/129959/designing_ai_algorithms_for_.php)

Inspire for the game comes from [WeeWar](https://web.archive.org/web/20150315005124/http://weewar.com/) (no more exist)

# 2. Credits

Big thanks to [infullmobile](https://www.infullmobile.com) for providing textures for this project.  

# 3. 3rd party libraries

This project use the following 3rd party libraries:

2D Helper library - [FurionGL](http://rezoner.net/)  
Graphics library - [OpenGL](https://www.khronos.org/opengl)  
Sound library - [FMOD](https://www.fmod.com)  
Font loading - [freetype2](https://www.freetype.org)  
Image loading - [stb_image](https://github.com/nothings/stb)  
UI components on Linux and Windows - [nuclear](https://github.com/vurtun/nuklear)  
Window creation and input on Linux and Windows [GLFW](http://www.glfw.org)  
OpenGL extensions [GLEW](http://glew.sourceforge.net)

Music - [dl-sounds](https://www.dl-sounds.com/)  
Sound - [freesound](https://freesound.org/)

# 4. Building  

### iOS

iOS freetype2 library taken from [cdave1](https://github.com/cdave1/freetype2-ios)

* Navigate to pixfight/PLATFORM/iOS
* Open `PixFight.xcodeproj` 
* Set target to Simulator or use your appleid to enable codesign for device
* Connect device (if needed)
* Press `cmd+b` or press play to build.

### macOS

To enable freetype on macOS you need to download it by using eg `brew install freetype` from terminal

* Navigate to pixfight/PLATFORM/macOS
* Open `PixFight.xcodeproj` 
* Press `cmd+b` or press play to build.

### Android

Android freetype2 library taken from [cdave1](https://github.com/cdave1/freetype2-android)  

To compile the project you need to download:  

`SDK Platforms` API21, 22 (tested also on 27)  
`SDK Tools` CMake, LLDB, NDK

* Open Android Studio
* Select open and navigate to pixfight/PLATFORM/Android and open project  
* Open emulator or connect your Android device
* Press `crtl+r` or press play button to build. 

### Linux

You need to install [codeblocks](http://www.codeblocks.org) to compile this project.

Before compiling and running you need to download additional libraries:
```
sudo apt-get install libfreetype6-dev
sudo apt-get install libglfw3-dev 
sudo apt-get install libglew-dev
```

Also run codeblocks from terminal with sudo command to let it copy FMOD libs to `usr/lib/x86_64-linux-gnu`

or copy them manually and remove following lines from pre-build steps in project:

```
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmod.so" "/usr/lib/x86_64-linux-gnu"
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmodL.so" "/usr/lib/x86_64-linux-gnu"
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmod.so.10" "/usr/lib/x86_64-linux-gnu"
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmodL.so.10" "/usr/lib/x86_64-linux-gnu"
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmod.so.10.3" "/usr/lib/x86_64-linux-gnu"
cp  "$(PROJECT_DIR)/FMOD/lib/x86_64/libfmodL.so.10.3" "/usr/lib/x86_64-linux-gnu"
```

* Navigate to pixfight/PLATFORM/Linux
* Open pixfight.cbp project file
* Press F9 or play button to build and run.

### Windows

This project require at least `Visual Studio 2017 community version`  	 

* Navigate to pixfight/PLATFORM/Windows/pixfight
* Open pixfight.sln
* Press F5 or play button to build and run.

### Mapeditor

Editor uses `Makefile`, you need to install `cmake` to compile and build.

* Open bash console (on Windows you can use mingw)
* Navigate to pixfight/MAPEDITOR/
* Type `make` and tap enter
* Binaries with additional catalogues will be placed in `bin` directory

To run on macOS / Linux navigate to `bin` directory and type `./editor`. Windows will generate appropriate `*.exe` file

To build it on Windows you will need to install [MinGW](http://www.mingw.org/) with `gcc` compiler as dependency. Also you need to download [glew](http://glew.sourceforge.net/) and [glfw](http://www.glfw.org/) and copy content of `lib` and `include` directories to appropriate folders in `C:\MinGW\` (Please choose mingw32 libraries)

If you have problems running editor on macOS (mostly High Sierra) you will need to manually download [glfw source code](https://github.com/glfw/glfw) and follow this steps: (then rebuild)  

* Download and extract the GLFW source code  
* Open the Terminal  
* `cd` to the extracted directory  
* Type in `cmake .`, hit return  
* A Makefile will be created for you  
* Type in `make`, hit return  
* After the compilation process, type in `sudo make install`  
* The libraries will be copied to `/usr/local/lib/`, the header files to `/usr/local/include/`  

# 5. Next steps  
  
  
* Add Undo / Redo move
* Multiplayer    

I will cooperate with [infullmobile](https://www.infullmobile.com) to create a fully functional iOS/Android version of this game.

I highly encourage everyone to fork / download and play with this prototype all suggestions and fixes will be appreciated as I'm not an expert on all listed platforms.

# 6. License

```
MIT

Copyright 2018 Marcin Małysz

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
