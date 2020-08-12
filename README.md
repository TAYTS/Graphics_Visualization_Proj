# Project

To simulate object collision behaviours in the physical world through modelling the interaction between the maze wall and the ball. The project aims to utilise computer graphics techniques to model the effects of gravitational forces acted on an object such as acceleration and bouncing off effect after collision. And discover methods for efficient collision detection computation.

# Application View

![](assets/maze_gameplay.gif)

# Build & Compiling the application

## For Mac

Download and install CMake for Mac OS: https://cmake.org/download/

Input the following commands on the Terminal:

1. Create the folder `./build`
2. cd to the folder: `./build`
3. `cmake ..`
4. `make`
5. `./MAZE`

## For Windows

Download and install CMake for Windows: https://cmake.org/download/
Download and install Visual Studio: https://visualstudio.microsoft.com/

1. Copy `win\glut32.dll` to `C:\WINDOWS\SysWOW64\`
2. Input the following commands on the CMD Terminal: 
   - cd to the folder: `build`
   - `cmake ..` (if you have more than one VS version, using -G to specify one, e.g., cmake .. -G "Visual Studio 14 2015")
3. Open the `Maze.sln` file in `build` folder with your Visual Studio
4. Build and Run the Visual Studio Project
5. Copy files from data folder to where the executable is.

# Running the application

## For Mac

1. Go to `./release/mac`
2. Run the application: `./MAZE`
3. In case `./MAZE` is not runnable, run `chmod a+x ./MAZE`

## For Windows

To run executable:

1. Go to `./release/windows`
2. Run the application by double click `MAZE.exe`
