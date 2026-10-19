# 💰 Milionerzy – Win32 API Desktop Quiz Game
A comprehensive desktop quiz application modeled after the popular TV show "Who Wants to Be a Millionaire?". Developed in C++ using pure, low-level **Windows API (Win32)**, the game combines advanced game logic with asynchronous multimedia processing and dynamic UI rendering.

## Key Features

### Player & Question Management
- **Dynamic Game Session:** The database is divided into 12 difficulty tiers (from 500 PLN to 1,000,000 PLN). Each new game generates a unique set of questions via pseudo-random selection.
- **Prize Ladder Pro:** A sidebar displays the reward hierarchy, tracks current progress, and handles guaranteed thresholds (**1,000 PLN** at tier 2 and **40,000 PLN** at tier 7).

### Lifeline Algorithms
- **50/50 Lifeline:** Preserves the correct answer while randomly eliminating two incorrect options with permanent interaction locks.
- **Phone a Friend:** Simulation module with a 70% probability for the correct answer and a 30% chance for a random option.
- **Ask the Audience (GDI Charts):** A dynamic module generating balanced percentage vote bars rendered directly via GDI graphics.

### Animation & Multitasking System
- **Multiphase Feedback:** Triggers an asynchronous yellow flashing sequence (8 state changes controlled by a system timer) upon selecting an answer.
- **Data Consistency & Security:** Locks mouse input during validation to prevent premature clicks.
- **Audio Engine:** Background tension music powered by an asynchronous system loop.

## 🛠 Tech Stack
- **Language:** C++ (C++17 / C++20)
- **GUI:** Windows API (Win32 User & GDI)
- **Audio:** Waveform Audio (`winmm.lib` / `mmsystem.h`)
- **IDE:** Visual Studio (MSVC Compiler)

## Installation & Running

### ⚠️ Important: Multimedia Assets Configuration
Ensure resource files are located in the executable output directory so the application can load them properly:
1. Copy `milionerzy.bmp` and `audio.wav` from the root project directory.
2. Paste them directly into:
   - `[Project_Directory]/Debug/`
   - `[Project_Directory]/x64/Debug/` (or `Release` depending on your build configuration).

### Steps in Visual Studio:
1. Open `Milionerzy.sln` in Microsoft Visual Studio.
2. Verify that the multimedia library is linked (`#pragma comment(lib, "winmm.lib")`).
3. Compile and run using **Local Windows Debugger** (F5).

## Gameplay Overview
1. **Start:** The main window initializes with a question, four choice buttons, a prize panel, and lifelines.
2. **Answering:** Clicking a choice starts the validation animation (yellow flash), followed by green (correct) or red (incorrect) highlighting.
3. **End & Restart:** Game over states trigger a native `MessageBox` showing winnings based on guaranteed thresholds, automatically resetting the session upon acknowledgment.
