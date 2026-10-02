# Bounce Classic

A remake of **Bounce**, the classic Nokia phone game, written in **C** with the **[raylib](https://www.raylib.com)** library.

Roll and jump a red ball through three levels. Collect rings, avoid spikes and spiders, use springs to launch high, pump up to float through water, and reach the goal.

> **CSE 102 Term Project** · Department of Computer Science and Engineering, BUET

<img width="1366" height="768" alt="Screenshot (5444)" src="https://github.com/user-attachments/assets/41354602-2c39-4922-b0ca-50b377af53ef" />
<img width="1366" height="768" alt="Screenshot (5446)" src="https://github.com/user-attachments/assets/1ead361e-b474-4469-b3d9-e329f5ac6570" />
<img width="1366" height="768" alt="Screenshot (5447)" src="https://github.com/user-attachments/assets/4eff698a-cd68-489c-a98f-4d907597f6c8" />


---

## Features

- **Three levels** (Easy, Medium, Hard), each built from a text-based tile map
- **Ball physics**: gravity, friction, bouncing, a fall-speed limit and frame-rate independent movement
- **Springs** that launch the ball high into the air
- **Water and the pumper**: touch the pumper to become the big ball, which floats up through water
- **Moving spiders** that patrol up and down between walls
- **Rings** worth 500 points each
- **Leaderboard** saved to a file between sessions, with gold, silver and bronze medals for the top three
- **Result box** after every run, showing the score count-up, the high score and a *NEW HIGH SCORE!* badge
- **Animated main menu** with drifting clouds, scrolling hills and a bouncing ball
- **Screens**: Main Menu, Name Input, Level Select, How to Play, Leaderboard, Credits and Pause
- **Sound**: menu music, level music and sound effects for rings, popping and clearing a level, with an on/off toggle
- **Mouse and keyboard** control on every menu

---

## Controls

### In a level
| Key | Action |
|---|---|
| `←` / `→` | Roll left / right |
| `↑` (hold) | Jump |
| `P` / `Esc` | Pause / resume |
| `R` | Retry (in the pause menu or after a run) |
| `H` | Home (in the pause menu or after a run) |

### Menus
| Key | Action |
|---|---|
| `↑` `↓` / `W` `S` | Move between menu buttons |
| `←` `→` / `A` `D` | Choose a level card or a result-box button |
| `Enter` / `Space` | Select |
| `Esc` / `Backspace` | Back |
| Mouse | Hover and click any button |

---

## Building and running

### Requirements
- A C compiler (GCC or Clang)
- [raylib](https://github.com/raysan5/raylib) **5.0 or newer**

### Linux
```bash
sudo apt install libraylib-dev          # or build raylib from source
gcc main.c -o bounce -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
./bounce
```

### Windows (MinGW / w64devkit)
```bash
gcc main.c -o bounce.exe -lraylib -lopengl32 -lgdi32 -lwinmm
bounce.exe
```

### macOS
```bash
brew install raylib
clang main.c -o bounce $(pkg-config --cflags --libs raylib)
./bounce
```

> **Run the game from the project folder.** It loads its pictures and sounds from the `assets/` folder using relative paths.

---

## Project structure

```
.
├── main.c              # the whole game
├── highscore.txt       # leaderboard (created automatically)
└── assets/
    ├── images/         # ball, tiles, spider, buttons, logo
    └── audios/         # music and sound effects
```

<details>
<summary>Full list of asset files</summary>

| Folder | Files |
|---|---|
| `assets/images/` | `red-ball.png`, `ball_big@2x.png`, `pop-red-ball.png`, `brick.png`, `tile_spike.png`, `spring.png`, `tile_ring.png`, `tile_goal.png`, `pumper@2x.png`, `spider.png`, `title-logo.png`, `PauseButton.png`, `resume.png`, `retry-button.png`, `home.png`, `sound_on.png`, `sound_off.png`, `level-select-button.png` |
| `assets/audios/` | `stage1.wav` (menu music), `stage4.wav` (level music), `coins.mp3`, `pop.mp3`, `universfield-next-level-114480.mp3` |

</details>

---

## How it works

### Game states
The game is a **state machine**. A `Gamestate` variable says which screen is active, and every frame the main loop does two things:

1. **Update**: a `switch(state)` runs only the active screen's input and logic.
2. **Draw**: everything for that screen is drawn back to front.

```
MAIN_MENUE ─► NAME_INPUT ─► LEVEL_SELECT ─► PLAYING ⇄ PAUSED
     ▲                                          │
     │                                          ▼
     └──────────────── HOME ─────────── DEAD / WIN (result box)
```

### Level maps
Each level is an array of strings. Every character is one 32 × 32 pixel tile:

| Char | Tile | Behaviour |
|:---:|---|---|
| `0` | Empty | Nothing |
| `1` | Brick | Solid wall or floor |
| `2` | Spike | Pops the ball |
| `3` | Spring | Launches the ball upward |
| `4` | Ring | +500 points, then disappears |
| `5` | Goal | Clears the level |
| `6` | Water | The big ball floats in it |
| `7` | Pumper | Turns the ball into the big ball |
| `8` | Deflater | Turns the ball back into the small ball |

The original maps are `const`. The game plays on an editable copy, so collected rings disappear and come back when the level restarts.

### Physics
- The ball is a circle. For each nearby tile the game finds the **closest point** of the tile to the ball's centre. If that point is closer than the radius, the ball is pushed out and its speed into the tile is reversed and scaled by `BOUNCINESS`.
- Only the tiles **around** the ball are checked, not the whole map.
- Movement is multiplied by `dt` (the time since the last frame), so the game runs at the same speed on any computer.

### Leaderboard
Scores are saved to `highscore.txt` as `name score` lines, kept sorted from highest to lowest.

---

## Tuning

All the gameplay numbers are `#define`s at the top of `main.c`, for example:

| Constant | Meaning |
|---|---|
| `GRAVITY` | How strongly the ball falls |
| `JUMP` | Jump strength |
| `SPRING_JUMP` | Spring launch strength |
| `BOUNCINESS` | Speed kept after a bounce (0 to 1) |
| `MAX_SPEED` | Top rolling speed |
| `SPIDER_SPEED` | Spider speed |
| `WATER_BUOYANCY` | How strongly water pushes the big ball up |

---

## Credits

| | |
|---|---|
| **Game design & programming** | Ahanaf Tahamid & Arshad Akter Kalpo |
| **Supervised by** | Md. Mostofa Akbar Sir |
| **Built with** | C and [raylib](https://www.raylib.com) |
| **Inspired by** | *Bounce* by Nokia |

Course: **CSE 102**, Department of Computer Science and Engineering, Bangladesh University of Engineering and Technology (BUET).
