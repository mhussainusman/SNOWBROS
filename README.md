# ❄️ Snow Bros

A feature-rich remake of the classic **Snow Bros** arcade game, built from scratch in C++ using SFML.

## Gameplay Overview

Snow Bros is a challenging and engaging platformer where players throw snowballs to freeze enemies, then roll them into other enemies to defeat them. The game features multiple enemy types, boss fights, a gem economy system, and power-ups - making it highly replayable and fun.

## Features

### Core Gameplay
- ❄️ Classic Snow Bros mechanics — freeze enemies, roll them, chain kills
- 👾 Multiple enemy types with different traits and attack patterns
- 🔄 Enemy variants — same enemy type with different behaviors and difficulty
- 💎 Gem collection system — collect gems to earn currency
- ⚡ Power-ups — enhance player abilities mid-game
- 🛒 Shop system — spend gems to buy power-ups between levels
- 🏆 Score tracking system

### Boss Fights
- **Boss 1** — Spawns small baby enemies continuously to overwhelm the player
- **Boss 2** — Fires rockets across the entire screen in unpredictable patterns — extremely difficult to dodge
- Pre-boss levels feature special collectibles to prepare the player

### Screens & UI
- 🏠 Start Screen
- 🔐 Login / Authentication System
- 📋 Main Menu
- 🎭 Character Selection Screen
- ⏸️ Pause Menu
- 🛒 Shop Screen
- 💀 Game Over Screen

## Tech Stack

- **Language:** C++
- **Graphics Library:** SFML (Simple and Fast Multimedia Library)
- **IDE:** Visual Studio
- **Paradigm:** Object-Oriented Programming (OOP)

## Developers

| Developer | GitHub |
|-----------|--------|
| Muhammad Hussain Usman | [@mhussainusman](https://github.com/mhussainusman) |
| Abdullah Moazzam | [@Abdullah106210](https://github.com/Abdullah106210) |

## 📁 Project Structure

```
Snow-Bros/
├── src/
│   ├── main.cpp
│   ├── Game.cpp / Game.h
│   ├── Player.cpp / Player.h
│   ├── Enemy.cpp / Enemy.h
│   ├── Boss.cpp / Boss.h
│   ├── Shop.cpp / Shop.h
│   ├── Auth.cpp / Auth.h
│   └── ...
├── assets/
│   ├── sprites/
│   ├── sounds/
│   └── fonts/
└── include/
```

## How to Run

1. Clone the repository:
```bash
git clone https://github.com/mhussainusman/Snow-Bros.git
```
2. Open the solution file in Visual Studio
3. Ensure SFML is properly linked
4. Build and run (**Ctrl + F5**)

## Requirements

- Visual Studio 2019 or later
- SFML 2.x
- Windows OS

---

*Built as a semester project — BS Software Engineering, FAST-NUCES Islamabad*
