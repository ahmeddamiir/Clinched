# 🥊 Clinched: Daily Athlete Training & Nutrition Tracker

**Clinched** is a simple single-user desktop app for fighters and athletes to log daily training and food intake, and compare calories against a personal daily target.

* **Stack:** Qt 6.11, **Qt Widgets**, C++17

* **Build system:** CMake (Qt Creator 20)

* **Storage:** no database. Two CSV logs + two JSON files.

## 📍 Project Status

* \[x\] Step 1: Project setup (empty window)

* \[x\] Step 2: Main window with tabs

* \[x\] Step 3: Diet tab with in-memory data

* \[ \] Step 4: `DataManager` + `diet.csv` *(done)* + `presets.json` *(in progress)*

* \[ \] Step 5: Workouts tab + `workouts.csv`

* \[ \] Step 6: Profile tab + calorie target

* \[ \] Step 7: Dashboard

* \[ \] Step 8: Polish (chart, dark theme)

## 🎯 Scope (keep it simple)

**In scope (v1):**

* Log meals (food + grams → calories)

* Log workouts (timed or strength)

* Profile tab → daily calorie target

* Dashboard with today's numbers

**Later (after the semester):**

* Line chart of history (Qt Charts)

* Searchable food list

* Edit/delete entries, unit tests, etc.

## 🔨 Build & Run

**Requirements:** Qt 6.x (developed on 6.11.2), Qt Creator, CMake.

1. Open `CMakeLists.txt` in Qt Creator.

2. Select a Desktop kit (e.g. MSVC 2022 64-bit).

3. Press **Ctrl+R**.

## 📂 Project Layout

```
Clinched/
├── CMakeLists.txt
├── main.cpp                     # Creates QApplication + MainWindow
├── mainwindow.h / .cpp          # Window with QTabWidget, owns DataManager
├── dashboardtab.h / .cpp        # (planned) Today's summary cards
├── diettab.h / .cpp             # Log meals, live calorie preview, today's meals table
├── workoutstab.h / .cpp         # (planned) Log workouts
├── profiletab.h / .cpp          # (planned) Body details + goal
├── datamanager.h / .cpp         # diet.csv persistence & date filtering (presets.json next)
├── profilecalculator.h / .cpp   # (planned) BMR, TDEE, calorie target
└── resources.qrc                # (planned) default presets.json

```

**Runtime files** (created automatically in the app data folder via `QStandardPaths::AppDataLocation`):

```
diet.csv            # every meal ever logged, one row per meal
workouts.csv        # every workout ever logged, one row per workout
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
 │ Dashboard │   │  DietTab  │   │ WorkoutsTab│   │ ProfileTab│
 │   Tab     │   │           │   │            │   │           │
 └─────▲─────┘   └─────┬─────┘   └──────┬─────┘   └─────┬─────┘
       │ dataChanged   │ addMeal()      │ addWorkout()  │ saveProfile()
       │ (signal)      ▼                ▼               ▼
       │        ┌─────────────────────────────────────────┐
       └────────│               DataManager               │
                │  - load presets.json                    │
                │  - addMeal() / addWorkout()             │
                │  - todayCalories() / todayMinutes()     │
                │  - load/save user_profile.json          │
                └───────┬─────────────────────┬───────────┘
                        ▼                     ▼
          diet.csv · workouts.csv      ProfileCalculator
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

The app doesn't "remember" the day. **Every row stores its own date** at the moment it is logged:

```
Add Meal clicked → date = QDate::currentDate(), time = QTime::currentTime() → append row

```

* Log lunch, close the app, reopen at night, log dinner → both rows have today's date automatically.

* **"Today"** = read the CSV and keep only rows where `Date == today`.

* **History** = group rows by `Date` and sum.

* Writing uses `QIODevice::Append`, so old rows are never overwritten.

## 🗄️ Data Formats

### 1. `diet.csv`

```
Date,Time,Food,Grams,Calories
2026-11-07,12:30:15,Chicken Breast,200,330
2026-11-07,21:10:04,White Rice,150,195

```

`Calories = Grams × cals_per_100g / 100`

### 2. `workouts.csv`

One fixed set of columns. Fields that don't apply to a workout are left empty.

```
Date,Time,Workout,Minutes,Sets,Reps,WeightKg,Intensity
2026-11-07,10:00:00,Wrestling / Grappling,60,,,,High
2026-11-07,17:30:00,Strength: Legs / Posterior Chain,,5,5,110,Moderate

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

Written by the Profile tab.

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

`QMainWindow` with a `QTabWidget`: **Dashboard · Diet · Workouts · Profile** *(Profile tab added in Step 6)*.

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

* **Add Meal** `QPushButton` → appends to `diet.csv`, emits `dataChanged()`

* `QTableWidget` of today's meals (Time, Food, Grams, Calories)

### Workouts (`WorkoutsTab`)

* Workout `QComboBox` (from `presets.json`)

* If `type == "timed"` → **Minutes** field

* If `type == "strength"` → **Sets / Reps / Weight (kg)** fields

* Intensity: Low / Moderate / High

* **Add Workout** button → appends to `workouts.csv`

* `QTableWidget` of today's workouts

### Profile (`ProfileTab`)

* Weight, height, age, sex, activity level, goal (`QFormLayout`)

* **Save** button → writes `user_profile.json`, recalculates the target

## 🛠️ Build Order

Each step works on its own before moving on.

| \# | Step | What you learn | 
| ----- | ----- | ----- | 
| 1 | Create project (Qt Widgets Application, CMake) and run an empty window | Project setup | 
| 2 | `MainWindow` with `QTabWidget` and tabs | Tabs, layouts | 
| 3 | `DietTab` with fake in-memory data (combo, spin box, live calories, table) | Widgets, signals & slots | 
| 4 | `DataManager`: save/load `diet.csv` + `presets.json` | File I/O, QJson, custom signals | 
| 5 | `WorkoutsTab` (timed vs. strength fields) + `workouts.csv` | Showing/hiding widgets | 
| 6 | `ProfileCalculator` + `ProfileTab` + `user_profile.json` | Logic + forms | 
| 7 | `DashboardTab` listening to `dataChanged()` | Keeping tabs in sync | 
| 8 | *(Later)* Qt Charts history chart, Fusion dark theme | Polish | 
