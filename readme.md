#Pixfight

This is an opensource crossplatform pixelart turn based strategy with AI done in C++14 with OpenGL 2.0 ES / 3.2 Core. With native applications for each platform (iOS, macOS, Linux, Windows, Android)

[1. Idea](#1.-Idea)  
[2. Credits](#2.-Credits)  
[3. 3rd party libraries](#3.-3rd-party-libraries)  
[4. Building](#4.-Building)  
[5. Next steps](#5.-Next-steps)   
[6. License](#6.-License)

#1. Idea

The root idea behind this project is to show how to integrate C++ code with some 3rd party libraries into your native application done in other platforms as a "business logic" Core game logic is written in C++14 and use FMOD, OpenGL and freetype2 as dependencies. Each platform uses this Core to render game in their native environment. (Eg Android in Java, iOS in objC etc.)

AI in this game is based on the following article from [gamasutra](https://www.gamasutra.com/view/feature/129959/designing_ai_algorithms_for_.php)

#2. Credits

Big thanks to [infullmobile](https://www.infullmobile.com) for providing textures for this project.  

#3. 3rd party libraries

This project use the following 3rd party libraries:

Gaphics library - [OpenGL](https://www.khronos.org/opengl)  
Sound library - [FMOD](https://www.fmod.com)  
Fount loading - [freetype2](https://www.freetype.org)  
Image loading - [stb_image](https://github.com/nothings/stb)  
UI components on Linux and Windows - [nuclear](https://github.com/vurtun/nuklear)  
Window creation and input on Linux and Windows [GLFW](http://www.glfw.org)  
OpenGL extensions [GLEW](http://glew.sourceforge.net)

All resources have been downloaded for free from the internet (music & sound)

#4. Building  

###iOS

iOS freetype2 library taken from [cdave1](https://github.com/cdave1/freetype2-ios)

* Navigate to pixfight/PLATFORM/iOS
* Open `PixFight.xcodeproj` 
* Set target to Simulator or use your appleid to enable codesign for device
* Connect device (if needed)
* Press `cmd+b` or press play to build.

###macOS

To enable freetype on macOS you need to download it by using eg `brew install freetype` from terminal

* Navigate to pixfight/PLATFORM/macOS
* Open `PixFight.xcodeproj` 
* Press `cmd+b` or press play to build.

###Android

Android freetype2 library taken from [cdave1](https://github.com/cdave1/freetype2-android)  

To compile the project you need to download:  

`SDK Platforms` API21, 22 (tested also on 27)  
`SDK Tools` CMake, LLDB, NDK

* Open Android Studio
* Select open and navigate to pixfight/PLATFORM/Android and open project  
* Open emulator or connect your Android device
* Press `crtl+r` or press play button to build. 

###Linux

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

###Windows

This project require at least `Visual Studio 2017 community version`  	 

* Navigate to pixfight/PLATFORM/Windows/pixfight
* Open pixfight.sln
* Press F5 or play button to build and run.

#5. Next steps  
  
  
* Game need a lot of refactoring mostly in C++ code.
* Bugfixes  
* I'm planning to add hot seats mode like in Heroes game
* Multiplayer    

I will cooperate with [infullmobile](https://www.infullmobile.com) to create a fully functional iOS/Android version of this game.

I highly encourage everyone to fork / download and play with this prototype all suggestions and fixes will be appreciated as I'm not an expert on all listed plaforms.

#6. License

```
MIT

Copyright 2018 Marcin Małysz

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
