# Alice in Wonderland Game 🧙‍♀️🎮

<p align="center">
  <img src="https://img.shields.io/badge/C++-SFML-blue.svg" alt="C++ SFML">
  <img src="https://img.shields.io/badge/Version-1.0-green.svg" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License">
</p>

## 📖 The Story

Welcome to **Alice in Wonderland** - a classic maze chase game inspired by Lewis Carroll's beloved story "Alice's Adventures in Wonderland."

In this game, you play as **Alice**, who finds herself trapped in a whimsical yet dangerous maze within Wonderland. The maze is filled with magical pellets that Alice must collect to navigate through this fantastical world.

### The Characters

- **Alice (You)** 🧒 - The curious young girl navigating through Wonderland, collecting pellets to survive
- **The Red Queen** 🔴 - Aggressive and always chasing directly
- **The Pink Flamingo** 🩷 - Uses cunning tactics to cut off Alice's path
- **The Blue Caterpillar** 🔵 - Strikes from unexpected angles
- **The Orange Hare** 🟠 - Patrols the corners with unique movement patterns

### The Objective

Navigate through the mysterious maze of Wonderland, collect all the magical pellets while avoiding the quirky characters chasing you. Use the special **mushroom power-ups** (energizer pellets) to turn the tables on your pursuers and send them running back to their homes!

---

## 🎮 Game Controls

| Key | Action |
|-----|--------|
| **Arrow Keys** | Move Alice (Up, Down, Left, Right) |
| **Enter** | Start Game / Select Menu Option |
| **Escape** | Exit Game |

---

## 🛠️ Installation & Dependencies

### Prerequisites

Before building the game, you need to install the following:

### Windows (Visual Studio)

1. **Install Visual Studio** (2019 or later recommended)
   - Download from: https://visualstudio.microsoft.com/

2. **Download SFML SDK** (Version 2.5.1 or compatible)
   - Download from: https://www.sfml-dev.org/download.php
   - Choose the Visual C++ version

3. **Setup SFML in Visual Studio**:
   - Extract the SFML folder to a convenient location (e.g., `C:\SFML`)
   - Open the project: `alice.vcxproj`
   - Go to **Project → Properties**
   - Add SFML include directory: `C:\SFML\include`
   - Add SFML library directory: `C:\SFML\lib`
   - Link the following libraries:
     - `sfml-graphics.lib`
     - `sfml-window.lib`
     - `sfml-system.lib`
     - `sfml-audio.lib`
     - `sfml-network.lib`

### Linux (Ubuntu/Debian)

```
bash
# Install SFML development libraries
sudo apt-get update
sudo apt-get install libsfml-dev

# Install build tools
sudo apt-get install g++ make
```

### macOS

```
bash
# Using Homebrew
brew install sfml

# Or using MacPorts
sudo port install sfml
```

---

## 🏗️ Building the Game

### Windows (Visual Studio)

1. Open `alice.sln` or `alice.vcxproj` in Visual Studio
2. Select **Build → Build Solution** (or press `Ctrl+Shift+B`)
3. The executable will be generated in the `bin` folder

### Windows (Command Line)

```
batch
# Using MSBuild
MSBuild alice.sln /p:Configuration=Release
```

### Linux/macOS

```
bash
# Create a build directory
mkdir build && cd build

# Compile
g++ -std=c++17 ../src/*.cpp -o alice -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Run
./alice
```

---

## ▶️ Running the Game

### Windows
After building, run the executable from the `bin` folder:
```
batch
bin\alice.exe
```

### Linux/macOS
```
bash
./alice
```

**Note:** Ensure the game is run from the project root directory so it can find the `assets`, `audio`, and other required files.

---

## 📁 Project Structure

```
AliceInWonderland/
├── assets/              # Game assets
│   ├── Map.txt         # Maze layout
│   └── textures/       # Game textures (sprites, dots, fonts)
├── audio/              # Sound effects
│   ├── alice_intro.wav
│   ├── cheshire_intro.wav
│   ├── death_1.WAV
│   ├── eat_fruit.wav
│   ├── eat_monster.wav
│   ├── game_start.WAV
│   ├── munch_1.WAV
│   ├── munch_2.WAV
│   ├── power_pellet.WAV
│   ├── retreating.WAV
│   ├── siren.WAV
│   └── text_sound.WAV
├── bin/                # Compiled binaries
├── include/            # Header files
│   ├── alice.h        # Main game definitions
│   ├── Animate.h      # Animation definitions
│   ├── Gameloop.h     # Game loop definitions
│   ├── monsters.h    # Monster definitions
│   ├── Player.h       # Player definitions
│   ├── Render.h       # Rendering definitions
│   └── Sound.h        # Sound definitions
├── src/                # Source code
│   ├── Main.cpp       # Entry point
│   ├── Gameloop.cpp   # Game loop logic
│   ├── alice.cpp      # Core game functions
│   ├── Animate.cpp    # Animation logic
│   ├── monsters.cpp   # Monster AI
│   ├── Player.cpp     # Player movement
│   ├── Render.cpp     # Rendering logic
│   └── Sound.cpp      # Sound management
├── textures/          # Additional textures
├── alice.vcxproj      # Visual Studio project file
├── highscore.txt      # High score storage
└── README.md          # This file
```

---

## 🎯 Game Features

### Core Gameplay
- ✅ Classic maze chase gameplay
- ✅ 4 unique AI monsters with different behaviors
- ✅ Power pellets for turning the tables
- ✅ Score system with bonus points
- ✅ High score persistence
- ✅ Multiple lives system

### Game States
- **Menu** - Title screen with game start option
- **Game Start** - Initial countdown
- **Main Loop** - Active gameplay
- **Win** - Level completion
- **Lose** - Player caught by monster
- **Game Over** - All lives lost

### AI Behaviors
- **Chase Mode** - Monsters actively pursue Alice
- **Scatter Mode** - Monsters retreat to their corners
- **Frightened Mode** - Monsters flee after eating power pellet
- **Return Home** - Eaten monsters return to the monster house

---

## 🔧 Troubleshooting

### Game crashes on startup
- Ensure all SFML DLLs are in the same directory as the executable
- Verify the game is running from the project root directory

### No sound
- Check that audio files exist in the `audio/` folder
- Ensure your system audio is not muted

### Window not displaying correctly
- The game runs at a fixed resolution. Try restarting or check your display settings.

### Build errors
- Make sure SFML is properly installed and linked
- Verify you have the correct Visual Studio version

---

## 📝 License

This project is licensed under the MIT License.

---

## 🙏 Acknowledgments

- **SFML** - Simple and Fast Multimedia Library
- **Lewis Carroll** - Author of "Alice's Adventures in Wonderland"
- Original Pac-Man game concept by Namco

---

## 👤 Authors

**Yassmine Feki**
**Khadija Bouchhima**
**Nessryne Chouchene**

---

<p align="center">
  <sub>Made with ❤️ for Wonderland</sub>
</p>
