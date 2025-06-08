# Zeph Programming Language <img src="./zeph.svg" width=50px>

<div style="display: flex; align-items: center">
<img src="./zeph.svg" width=50px> A Simple Programming Language made for Game Development.
</div>

## What is the zeph programming language ?

**Zeph** is a modern programming language written in **C++**, designed to be powerful yet beginner-friendly. The main purpose of the zeph programming language is to operate as a game development scripting language offering seamless integration with my custom game engine. Zeph is an interpreted language as well as a compiled one. As of now just the interpreter (posea) is implemented and the compiler (hades) will be implemented in the future.

## 🟨 Key Features to Develop

### 🟡 Seamless Game Development

Zeph will support integration with **a custom C++ game engine**, making it easy to:

- Create windows and GUI elements  
- Develop 2D/3D games  
- Handle input, rendering, and physics  
- Script game logic quickly and efficiently  

This opens up possibilities for rapid game prototyping or building complex interactive systems using Zeph’s concise syntax.

### 🟢 Features implemented
- Variable declarations (let x = 1, const x, const y = "string")
- Variable assignments (x = 1)
- User defined functions (def main(a, b) { #code })
- Builtin functions (print, typeof)
- Control Flow (break, continue)
- Conditionals (if, else)
- Logical Expressions (and, or)
- Loops (while)

### 🟡 Features planned for the near future
- Loops (for)
- Objects (javascript like)

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