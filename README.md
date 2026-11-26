# 🥊 Clinched: Daily Athlete Training & Nutrition Tracker

**Clinched** is a simple single-user desktop app for fighters and athletes to log daily training and food intake, and compare calories against a personal daily target.

* **Stack:** Qt 6.11, **Qt Widgets**, C++17

* **Build system:** CMake (Qt Creator 20)

* **Storage:** no database. Everything is stored in plain **JSON** files.

## 📍 Project Status

* \[x\] Step 1: Project setup (empty window)

* \[x\] Step 2: Main window with tabs

* \[x\] Step 3: Diet tab with in-memory data

* \[x\] Step 4: `DataManager`

  * \[x\] 4a: Meals saved and loaded from a file

  * \[x\] 4b: Meals stored as JSON (`diet.json`)

  * \[x\] 4c: Foods loaded from `presets.json`

* \[x\] Step 5: Workouts tab + `workouts.json`

* \[ \] Step 6: Profile dialog + calorie target

* \[ \] Step 7: Dashboard

* \[ \] Step 8: Polish (chart, dark theme)

## 🎯 Scope (keep it simple)

**In scope (v1):**

* Log meals (food + grams → calories)

* Log workouts (timed or strength)

* Profile dialog → daily calorie target

* Dashboard with today's numbers

**Later (after the semester):**

* Line chart of history (Qt Charts)

* Searchable food list

* Edit/delete entries, unit tests, etc.

## 🔨 Build & Run

**Requirements:** Qt 6.x (developed on 6.11.2), Qt Creator, CMake.

1. Open `Clinched/CMakeLists.txt` in Qt Creator.

2. Select a Desktop kit (e.g. MSVC 2022 64-bit).

3. Press **Ctrl+R**.

## 📂 Project Layout

```
Clinched/                          # repo root
├── README.md
└── Clinched/                      # Qt project folder
├── CMakeLists.txt
├── main.cpp                   # Creates QApplication + MainWindow
├── core/                      # Non-UI data models and persistence
│   ├── models.h               # Plain structs & enums (Meal, Food, Workout)
│   ├── datamanager.h / .cpp   # JSON read/write, date filtering, dataChanged() signal
│   └── profilecalculator.h / .cpp # (planned) BMR, TDEE, calorie target
├── ui/                        # Qt Widgets screens and dialogs
│   ├── mainwindow.h / .cpp    # Window with QTabWidget, owns DataManager
│   ├── dashboardtab.h / .cpp  # (planned) Today's summary cards
│   ├── diettab.h / .cpp       # Log meals, live calorie preview, today's meals table
│   ├── workoutstab.h / .cpp   # Log workouts (inputs change by workout type)
│   └── profiledialog.h / .cpp # (planned) Body details + goal
└── resources/
└── presets.json           # Default foods/workouts (bundled via CMake)

```

**Runtime files** (created automatically in the app data folder via `QStandardPaths::AppDataLocation`):

```
diet.json           # every meal ever logged
workouts.json       # every workout ever logged
presets.json        # foods + workouts list (editable, no recompiling)
user_profile.json   # weight, height, age, sex, activity, goal

```

A default `presets.json` is bundled in the app (Qt resource) and copied to the data folder on first run.

## 🧩 Architecture

UI classes only show things and react to clicks. **All file work and math live in two non-UI classes.**

```
                    ┌──────────────────────┐
                    │      MainWindow      │
                    │   (QTabWidget, owns  │
                    │     DataManager)     │
                    └──────────┬───────────┘
       ┌───────────────┬───────┴───────┬───────────────┐
       ▼               ▼               ▼               ▼
 ┌───────────┐   ┌───────────┐   ┌────────────┐   ┌───────────┐
 │ Dashboard │   │  DietTab  │   │ WorkoutsTab│   │  Profile  │
 │   Tab     │   │           │   │            │   │  Dialog   │
 └─────▲─────┘   └─────┬─────┘   └──────┬─────┘   └─────┬─────┘
       │ dataChanged   │ addMeal()      │ addWorkout()  │ saveProfile()
       │ (signal)      ▼                ▼               ▼
       │        ┌─────────────────────────────────────────┐
       └────────│               DataManager               │
                │  - load presets.json                    │
                │  - addMeal() / addWorkout()             │
                │  - mealsForDate() / caloriesForDate()   │
                │  - load/save user_profile.json          │
                └───────┬─────────────────────┬───────────┘
                        ▼                     ▼
          diet.json · workouts.json    ProfileCalculator
          presets.json · user_profile  (BMR, TDEE, target;
                                        pure math)

```

**How tabs stay in sync (signals & slots):**
`DataManager` emits `dataChanged()` whenever a meal/workout/profile is saved. `DashboardTab` connects to that signal and refreshes its labels. Tabs never talk to each other directly.

**Roles:**

* **Tabs (QWidget subclasses)** = layouts, inputs, buttons.

* **DataManager (QObject)** = reads/writes files, filters by date, sums totals, emits signals.

* **ProfileCalculator** = pure math, no file access.

## 🔄 How Dates Work

The app doesn't "remember" the day. **Every record stores its own date** at the moment it is logged:

```
Add Meal clicked → date = QDate::currentDate(), time = QTime::currentTime() → saved with the meal

```

* Log lunch, close the app, reopen at night, log dinner → both records have today's date automatically.

* **"Today"** = load the file and keep only records where `date == today`.

* **History** = group records by `date` and sum.

* Saving = load the whole array, append the new object, write the file back with `QSaveFile` (atomic: the old file is replaced only if the write fully succeeds).

## 🗄️ Data Formats

Why JSON and not CSV? A comma or a new line inside a text field breaks CSV columns and has to be cleaned by hand. JSON escapes everything automatically, keeps numbers as numbers, and lets a record simply omit fields that don't apply.

### 1. `diet.json`

An array of meal objects.

```
[
  {
    "date": "2026-11-07",
    "time": "12:30:15",
    "food": "Chicken Breast",
    "grams": 200,
    "calories": 330
  }
]

```

`calories = grams × cals_per_100g / 100`

### 2. `workouts.json`

Fields that don't apply to a workout are simply left out.

```
[
  {"date": "2026-11-07", "time": "10:00:00", "workout": "Wrestling / Grappling",
   "type": "timed", "minutes": 60, "intensity": "High"},
  {"date": "2026-11-07", "time": "17:30:00", "workout": "Strength: Legs / Posterior Chain",
   "type": "strength", "sets": 5, "reps": 5, "weightKg": 110, "intensity": "Moderate"}
]

```

### 3. `presets.json`

Each workout has a `type` that decides which inputs the UI shows.

```
{
  "foods": [
    {"name": "Chicken Breast", "cals_per_100g": 165},
    {"name": "White Rice",     "cals_per_100g": 130},
    {"name": "Rolled Oats",    "cals_per_100g": 389},
    {"name": "Whole Eggs",     "cals_per_100g": 143},
    {"name": "Peanut Butter",  "cals_per_100g": 588}
  ],
  "workouts": [
    {"name": "Wrestling / Grappling",            "type": "timed"},
    {"name": "MMA Sparring",                     "type": "timed"},
    {"name": "Muay Thai / Striking",             "type": "timed"},
    {"name": "Roadwork / Conditioning",          "type": "timed"},
    {"name": "Strength: Push Focus",             "type": "strength"},
    {"name": "Strength: Pull Focus",             "type": "strength"},
    {"name": "Strength: Legs / Posterior Chain", "type": "strength"}
  ]
}

```

### 4. `user_profile.json`

Written by the Profile dialog.

```
{
  "weightKg": 80,
  "heightCm": 178,
  "age": 24,
  "sex": "male",
  "activityFactor": 1.55,
  "goal": "maintain"
}

```

## 🧮 Calorie Target

1. **BMR (Mifflin-St Jeor):**
   `BMR = 10 × weight(kg) + 6.25 × height(cm) − 5 × age + s`
   (`s = +5` male, `−161` female)

2. **TDEE:** `TDEE = BMR × activityFactor`
   Typical factors: 1.2 (sedentary), 1.375 (light), 1.55 (moderate), 1.725 (very active).

3. **Target by goal:**

   * Bulk: `TDEE + 400`

   * Cut: `TDEE − 500`

   * Maintain: `TDEE`

## 🖥️ Screens

### Main window

`QMainWindow` with a `QTabWidget`: **Dashboard · Diet · Workouts**.
The profile is a `QAction` in the **File** menu (`Ctrl+P`) that opens a dialog *(added in Step 6)*.

### Dashboard (`DashboardTab`)

Four summary cards (`QGroupBox` + `QLabel`):

* **Daily Target** (from profile)

* **Consumed Today** (sum of today's calories)

* **Remaining** (Target − Consumed)

* **Training Today** (total minutes + number of strength sessions)

*Later:* line chart of daily calories vs. a target line (`QChartView`).

### Diet (`DietTab`)

* Food `QComboBox` (from `presets.json`)

* Grams `QSpinBox`

* Live `QLabel`: `Calories = grams × cals_per_100g / 100`

* **Add Meal** `QPushButton` → saves to `diet.json`, emits `dataChanged()`

* `QTableWidget` of today's meals (Time, Food, Grams, Calories), newest on top

### Workouts (`WorkoutsTab`)

* Workout `QComboBox` (from `presets.json`)

* If `type == "timed"` → **Minutes** field

* If `type == "strength"` → **Sets / Reps / Weight (kg)** fields

* Intensity: Low / Moderate / High

* **Add Workout** button → saves to `workouts.json`

* `QTableWidget` of today's workouts

### Profile dialog (`QDialog`)

* Weight, height, age, sex, activity level, goal (`QFormLayout`)

* **Save** button → writes `user_profile.json`, recalculates the target

## 🛠️ Build Order

Each step works on its own before moving on.

| \# | Step | What you learn | 
| ----- | ----- | ----- | 
| 1 | Create project (Qt Widgets Application, CMake) and run an empty window | Project setup | 
| 2 | `MainWindow` with `QTabWidget` and tabs | Tabs, layouts, QAction | 
| 3 | `DietTab` with fake in-memory data (combo, spin box, live calories, table) | Widgets, signals & slots | 
| 4 | `DataManager`: save/load `diet.json` + `presets.json` | File I/O, QJson, custom signals | 
| 5 | `WorkoutsTab` (timed vs. strength fields) + `workouts.json` | Showing/hiding widgets | 
| 6 | `ProfileCalculator` + Profile dialog + `user_profile.json` | Logic + QDialog + forms | 
| 7 | `DashboardTab` listening to `dataChanged()` | Keeping tabs in sync | 
| 8 | *(Later)* Qt Charts history chart, Fusion dark theme | Polish | 
