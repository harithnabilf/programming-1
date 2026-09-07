# Photron

A 2-player local multiplayer arcade game built in C++ using the Simple and Fast Multimedia Library (SFML). Developed as a programming project during the Foundation in Physical Science program at Universiti Malaya.

The game is inspired by classic Tron light cycle mechanics. Two players navigate a 2D grid arena, each leaving behind an impassable neon light trail. The objective is to outmaneuver the opposing player without crashing into any trail or the boundary walls.

---

## Game Controls

| Action | Player 1 (Neon Pink) | Player 2 (Neon Blue) |
| :--- | :--- | :--- |
| **Move Up** | `W` | `Up Arrow` |
| **Move Down** | `S` | `Down Arrow` |
| **Move Left** | `A` | `Left Arrow` |
| **Move Right** | `D` | `Right Arrow` |

- **Menu Interaction:** Left Mouse Click on "Play Again" or "Exit" buttons.
- **Quick Exit:** Press `Escape` key at any time.

---

## Technical Features

- **Object-Oriented Architecture:** Modular class structure separating game state management (`Game`), menu/post-match UI (`GameOver`), and player data representations (`Player`, `Position`).
- **Memory & Pointer Manipulation:** Explicit pointer references (`int* ptrX`, `int* ptrY`) utilized for calculating coordinate step progressions and memory addressing.
- **Discrete Grid & Tick Engine:** A 40x40 logical grid synchronized with a fixed tick rate timer (0.08s intervals) decoupled from screen refresh rates.
- **Collision Detection Pipeline:** Evaluates coordinate overlap between player heads and stored trail arrays on each tick, supporting single-winner resolution and simultaneous head-on draw states.
- **Dynamic Resolution Scaling:** Automatically queries desktop resolution and performs aspect-ratio-preserving letterboxing/pillarboxing in fullscreen mode.
- **Input Direction Validation:** Rejects immediate 180-degree reverse inputs using mapped directional opposites to prevent self-collision on the same axis.

---

## Tech Stack

- **Language:** C++
- **Graphics & Windowing:** SFML (Simple and Fast Multimedia Library)
- **Audio System:** OpenAL (bundled with SFML audio)
- **Assets:** Custom pixel font (`pixel_font.ttf`)

---

## Project Structure

```text
programming-1/
├── main.cpp              # Core game loop, logic, and SFML rendering code
├── main.exe              # Pre-compiled executable for Windows
├── pixel_font.ttf        # Retro pixel font asset for in-game UI
├── openal32.dll          # OpenAL runtime library
├── sfml-*.dll            # SFML release and debug shared libraries
└── README.md             # Project documentation
```

---

## Running the Game

### Option 1: Run Pre-Compiled Binary (Windows)

The repository includes pre-compiled binaries and required SFML runtime DLLs. You can launch the game directly:

1. Clone the repository:
   ```bash
   git clone https://github.com/harithnabilf/programming-1.git
   cd programming-1
   ```

2. Double-click `main.exe` or execute it from the terminal:
   ```cmd
   .\main.exe
   ```

### Option 2: Build from Source

Ensure you have a C++ compiler (such as MinGW-w64 / GCC) and SFML 2.x installed:

```bash
g++ -c main.cpp -I<path_to_sfml>/include
g++ main.o -o main.exe -L<path_to_sfml>/lib -lsfml-graphics -lsfml-window -lsfml-system
```
