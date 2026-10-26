# Movies Manager

A dual-interface C++20 application for managing a personal movie catalog and watchlist. Provides both a command-line interface and a full Qt 6 GUI, backed by a choice of four persistence backends.

---

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Running](#running)
- [Usage — GUI](#usage--gui)
- [Usage — Console](#usage--console)
- [Repository Backends](#repository-backends)
- [Validation Rules](#validation-rules)
- [Project Structure](#project-structure)

---

## Features

### Movie Catalog (Admin)
- **Add** a movie with title, genre, release year, likes count, and a trailer URL
- **Update** any field of an existing movie (with confirmation prompt)
- **Remove** a movie from the catalog (with confirmation prompt)
- **Undo / Redo** any add, update, or remove operation via the Command pattern
- **Generate random movies** — seeds from `movies_seed.txt`, skipping duplicates already in the catalog

### Watchlist (User)
- **Browse** the full catalog in a sortable table
- **Filter** movies by genre, release year, and direction (before / after / any)
- **Add to watchlist** and **remove from watchlist**
- **Open watchlist file** — launches the CSV or HTML file in the system's default application
- Watchlist persists across sessions (CSV or HTML backend)

### General
- **Dual UI** — Qt 6 desktop GUI and an interactive console menu, sharing the same service layer
- **Switchable role** — flip between Admin and User view without restarting
- **Four storage backends** — plain text, SQLite, CSV, and HTML (see [Repository Backends](#repository-backends))
- **Input validation** — strict rules on every field, with aggregated error messages shown in a single dialog
- **27 IMDB genres** with case-insensitive entry in the console UI
- **Test suite** — assert-based unit tests covering domain, validation, and service logic

---

## Architecture

```
MoviesManager/
├── headers/                  # Public interface headers
│   ├── domain/
│   │   ├── domain.h          # Movie entity
│   │   ├── MovieValidator.h  # Validation logic
│   │   └── action.h          # Command pattern (ActionAdd/Remove/Update)
│   ├── repository/
│   │   ├── IRepository.h     # Pure-abstract repository interface
│   │   ├── repository.h      # In-memory base implementation
│   │   ├── file_repository.h
│   │   ├── csv_repository.h
│   │   ├── html_repository.h
│   │   ├── db_repository.h
│   │   └── repository_factory.h
│   ├── service/
│   │   └── service.h         # Business logic, undo/redo stacks
│   ├── ui/
│   │   └── ui.h              # Console UI
│   └── exceptions/
│       └── exceptions.h      # MovieException, RepoException
├── src/                      # Implementations
├── gui/                      # Qt 6 GUI (MoviesGUI target)
│   ├── gui_main.cpp          # Entry point + startup dialog
│   ├── mainwindow.h/.cpp     # Main window (Admin + User tabs)
│   └── mainwindow.ui         # Qt Designer layout
├── movies_seed.txt           # Seed data for random generation
└── CMakeLists.txt
```

### Dependency flow

```
IRepository (interface)
    └── Repository (in-memory)
            ├── FileRepository   (.txt)
            ├── CSVRepository    (.csv)
            ├── HtmlRepository   (.html  + companion .csv)
            └── DBRepository     (SQLite .db)

Service ──uses──▶ IRepository& (catalog)
        ──uses──▶ IRepository& (watchlist)
        ──owns──▶ undo stack / redo stack (Action*)

GUI / Console ──uses──▶ Service
```

`Service` depends only on `IRepository`, not on any concrete class — new backends can be added without touching business logic.

---

## Prerequisites

| Requirement | Minimum version |
|---|---|
| C++ compiler | GCC 13 / MSVC 2022 / Clang 16 (C++20 required) |
| CMake | 3.30 |
| Qt | 6.9 (Core, Gui, Widgets) |
| Ninja or Make | any recent version |

> **Windows (recommended)**: install [Qt Online Installer](https://www.qt.io/download-qt-installer), selecting the **Qt 6 MinGW 64-bit** kit. CMake and Ninja are bundled under `C:\Qt\Tools\`.

---

## Building

### Qt Creator (easiest)

1. Open `CMakeLists.txt` with **Qt Creator → File → Open File or Project**
2. Select the **Desktop Qt 6 MinGW 64-bit Debug** (or Release) kit
3. Click **Build All** (`Ctrl+B`)

Two executables are produced:
- `MoviesGUI.exe` — Qt desktop application
- `MoviesManager.exe` — console application

### Command line (Windows, Qt MinGW)

```bat
set PATH=C:\Qt\Tools\CMake_64\bin;C:\Qt\Tools\Ninja;C:\Qt\Tools\mingw1310_64\bin;C:\Qt\6.12.0\mingw_64\bin;%PATH%

cmake -S . -B build -G Ninja ^
      -DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64 ^
      -DCMAKE_BUILD_TYPE=Debug

cmake --build build --target MoviesGUI MoviesManager
```

### Command line (Linux / macOS)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target MoviesGUI MoviesManager
```

---

## Running

### Qt GUI

```
build/MoviesGUI.exe          # Windows
build/MoviesGUI              # Linux / macOS
```

A startup dialog appears first to choose storage formats (see below), then the main window opens.

### Console

```
build/MoviesManager.exe      # Windows
build/MoviesManager          # Linux / macOS
```

The console app prompts for repository types before entering the main menu.

### Data files

All data files are written relative to the **working directory** at runtime:

| File | Purpose |
|---|---|
| `movies.txt` | Catalog — text backend |
| `movies.db` | Catalog — SQLite backend |
| `watchlist.csv` | Watchlist — CSV backend |
| `watchlist.html` | Watchlist — HTML backend (also writes `watchlist.csv`) |
| `movies_seed.txt` | Seed pool for "Generate Random" (read-only) |

---

## Usage — GUI

### Startup dialog

On launch, select:
- **Movies** — `Text file (.txt)` or `SQLite database (.db)`
- **Watchlist** — `CSV file (.csv)` or `HTML file (.html)`

Both choices persist between sessions via the selected file format.

### Admin tab

| Control | Purpose |
|---|---|
| Movie table | Click any row to auto-fill the form below |
| Title / Trailer URL | Free-text inputs |
| Genre | Dropdown of all 27 IMDB genres |
| Year / Likes | Spin boxes (year range 1888–2100) |
| **Add** | Validate and insert the movie in the form |
| **Update** | Apply form changes to the selected row (confirmation required) |
| **Remove** | Delete the selected row (confirmation required) |
| **Undo / Redo** | Step through add / update / remove history |
| Spin + **Generate Random** | Add *N* movies from the seed file, skipping duplicates |
| **Switch to User** | Jump to the User tab |

### User tab

| Control | Purpose |
|---|---|
| Genre dropdown | Filter by genre, or pick "Any genre" |
| Year spin (0 = Any) | Filter by release year |
| Direction combo | Any / Before / After the chosen year |
| **Filter** | Apply the current filter criteria |
| **Clear** | Reset filter and show all movies |
| **Add to Watchlist** | Add the selected catalog movie to the watchlist |
| **Remove from Watchlist** | Remove the selected watchlist entry (confirmation required) |
| **Open Watchlist File** | Open the watchlist file in the system's default app |
| **Switch to Admin** | Jump to the Admin tab |

The status bar always shows the live catalog and watchlist counts.

---

## Usage — Console

The console menu is role-based. On startup you choose repository backends, then land on the main menu.

### Admin menu

```
1. Add movie
2. Remove movie          (confirmation prompt)
3. Update movie          (confirmation prompt)
4. List all movies
5. Undo
6. Redo
7. Generate N random movies
8. Switch to User mode
0. Back
```

### User menu

```
1. Show all movies
2. Filter movies         (by genre and/or year)
3. Add to watchlist
4. Show watchlist
5. Remove from watchlist
6. Switch to Admin mode
0. Back
```

**Genre entry** in the console is case-insensitive. Typing `sci-fi`, `SCI-FI`, or `Sci-Fi` all resolve to the canonical `Sci-Fi`.

---

## Repository Backends

### Catalog

| Backend | File | Notes |
|---|---|---|
| `FileRepository` | `movies.txt` | One movie per line, pipe-delimited |
| `DBRepository` | `movies.db` | Embedded SQLite (no external dependency) |

### Watchlist

| Backend | File | Notes |
|---|---|---|
| `CSVRepository` | `watchlist.csv` | Standard CSV, one movie per line |
| `HtmlRepository` | `watchlist.html` | Rendered HTML table; **also writes `watchlist.csv`** as a companion file so the two backends are interchangeable across sessions |

Because both watchlist backends share `watchlist.csv` as their persistence format, you can switch between CSV and HTML on different runs without losing data.

---

## Validation Rules

Every movie is validated before being accepted. All errors are collected and reported together.

| Field | Rules |
|---|---|
| **Title** | At least 2 characters |
| **Genre** | Must be one of the 27 IMDB genres (case-sensitive in the backend; case-insensitive in the console UI) |
| **Year** | ≥ 1888 (first film ever made) and ≤ current year |
| **Likes** | ≥ 0 |
| **Trailer** | Must be a well-formed URL matching the standard URL regex |

### Accepted genres

`Action` · `Adventure` · `Animation` · `Biography` · `Comedy` · `Crime` · `Documentary` · `Drama` · `Family` · `Fantasy` · `Film-Noir` · `Game-Show` · `History` · `Horror` · `Music` · `Musical` · `Mystery` · `News` · `Reality-TV` · `Romance` · `Sci-Fi` · `Short` · `Sport` · `Talk-Show` · `Thriller` · `War` · `Western`

---

## Project Structure

```
MoviesManager/
├── CMakeLists.txt
├── movies_seed.txt          # Seed pool (Title|Genre|Year|TrailerURL)
│
├── headers/                 # All public .h files
│   ├── domain/
│   │   ├── domain.h
│   │   ├── MovieValidator.h
│   │   └── action.h
│   ├── repository/
│   │   ├── IRepository.h
│   │   ├── repository.h
│   │   ├── file_repository.h
│   │   ├── csv_repository.h
│   │   ├── html_repository.h
│   │   ├── db_repository.h
│   │   └── repository_factory.h
│   ├── service/
│   │   └── service.h
│   ├── ui/
│   │   └── ui.h
│   ├── tests/
│   │   └── tests.h
│   └── exceptions/
│       └── exceptions.h
│
├── src/                     # Implementations
│   ├── domain/
│   │   ├── domain.cpp
│   │   └── MovieValidator.cpp
│   ├── repository/
│   │   ├── repository.cpp
│   │   ├── file_repository.cpp
│   │   ├── csv_repository.cpp
│   │   ├── html_repository.cpp
│   │   ├── db_repository.cpp
│   │   ├── repository_factory.cpp
│   │   └── sqlite3.c        # Embedded SQLite amalgamation
│   ├── service/
│   │   └── service.cpp
│   ├── ui/
│   │   └── ui.cpp
│   ├── tests/
│   │   └── tests.cpp
│   └── utils.cpp
│
├── gui/                     # Qt 6 desktop application
│   ├── gui_main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   └── mainwindow.ui
│
├── main.cpp                 # Console app entry point
├── StartUp.h
└── StartUp.cpp
```
