# SolarSystemASCI


SolarSystemASCI is a terminal-based C application that visualizes the positions of the eight major planets in our solar system for any given date, using ASCII art. Enter a date, and the program fetches real planetary data, calculates their positions, and displays both the inner and outer solar system in a fun, retro ASCII style.

## Introduction

Two months before I started working on this project, I had just finished playing Outer Wilds and seeing the Three Body Problem on Netflix and reading Project Hail Mary by Andy Weir. I was in my space era, and, as I was approaching the personal project stage on Boot.dev, I wanted to code some space stuff. 
I reached out to a friend about a graphic representation of the solar system in C. He called me crazy, but then gave me the simplified idea:

"Why not something that takes a date and displays the position of the planets on that date as ASCII in the terminal?"  — Gabriel Tocha, 05-05-2025

Why in C? Because learning about space stuff while being a business major wasn't hard enough, so might as well add a programming language I had never done a project on.

## Dependencies / Requirements

- **C Compiler**: (e.g., gcc)
- **libcurl**: For HTTP requests to fetch planetary data
- **cJSON**: For parsing JSON responses
- **Internet Connection**: Required to fetch up-to-date planetary data from the API

### Installation on macOS

```bash
# Install Homebrew if you don't have it:
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install gcc (C compiler) and curl library
brew install gcc curl
# cJSON is included as source; no extra install needed
```
---

## How to Use

1. **Clone or Download the Repository**

2. **Compile the Program**

   Run the provided script:
   ```bash
   ./main.sh
   ```

3. **Input a Date**

   When prompted, enter a date in the format `dd/mm/yyyy` (e.g., `13/07/2025`).

4. **View the Output**

   The terminal will display ASCII diagrams of the inner and outer solar system, showing the positions of the planets for your chosen date.

---

## Space Theory / Assumptions

- **Orbital Calculations**: The program uses Keplerian elements and simple orbital mechanics to estimate planetary positions. Eccentricities and perihelion dates are set from reputable sources (NASA, JPL, Wikipedia).

- **Data Source**: Planetary data (mass, radius, period, etc.) is fetched in real-time from the [API Ninjas Planets API](https://api-ninjas.com/api/planets).

- **Simplifications**:
  - Orbits are assumed to be ellipses in a 2D plane.
  - Only the eight major planets are shown.
  - The Sun is at the center; orbits are not to scale but are visually separated for clarity.
  - The program does not account for orbital inclinations or perturbations.
  
- **ASCII Art**: Each planet is represented by a unique symbol. The Sun is marked with `*`, and orbital paths with `/`.

---

## Limitations/Next Steps

- **C Compiler Required**: You must have a C compiler (such as gcc) installed to build and run this project. Precompiled binaries are not provided.
- **Windows Compatibility**: The provided scripts and instructions are for Unix-like systems (macOS, Linux). On Windows, you will need to install a compatible C compiler (e.g., MinGW or MSVC), required libraries, and compile manually. The `main.sh` script will not run natively on Windows Command Prompt or PowerShell; use WSL, Git Bash, or adapt the build steps.
- **Terminal Output**: The ASCII art is designed for standard terminal sizes and may not display correctly in all terminal emulators or with non-monospace fonts.

---

---

Enjoy exploring the solar system from your terminal!
