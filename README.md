# Zeph Programming Language

A simple programmig language made in c++

## What is the zeph programming language ?

**Zeph** is a modern programming language written in **C++**, designed to be powerful yet beginner-friendly. The main purpose of the zeph programming language is to operate as a game development scripting language offering seamless integration with a custom custom game engine developd by me in order for fast and "write and play" game development. Zeph is an interpreted language as well as a compiled one. As of now just the interpreter (posea) is implemented and the compiler (hades) will be implemented in the future.

## 🟨 Key Features to Develop

### 🟡 Seamless Game Development

Zeph will support integration with **a custom C++ game engine**, making it easy to:

- Create windows and GUI elements  
- Develop 2D/3D games  
- Handle input, rendering, and physics  
- Script game logic quickly and efficiently  

This opens up possibilities for rapid game prototyping or building complex interactive systems using Zeph

### 🟢 Zeph features

As of now in the initial stages of development, zeph is maturing and gaining quality of life features such as:  

- Variable declarations
- Variable assignments
- User defined functions
- Builtin functions
- Control flow
- Conditionals
- Logical expressions
- While loops

### 🟡 Features planned for the near future
- For loops
- Objects (javascript like objects and JSON support)

## ▼ Instalation

As of now the zeph programming language has no install script. So the source code must be downloaded and the interpreter compiled from source

Download this git repository and navigate inside it
```
git clone https://github.com/LuanTSP/Zeph.git
cd Zeph
```

Then make a build folder, navigate inside it and build it with a c++ compiler using cmake
```
mkdir build
cd build
cmake ..
make
```

Now the interpreter 'posea' is compiled. Just point it to a filepath for a .zeph file and it will run the code
```
posea myZephProgram.zeph
```