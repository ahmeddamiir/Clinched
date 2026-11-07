# 🥊 Clinched: Daily Athlete Training & Nutrition Tracker

**Clinched** is a simple single-user desktop app for fighters and athletes to log daily training and food intake, and compare calories against a personal daily target.

* **Stack:** Qt 6.11 (Qt Quick / QML for the UI, C++ for the logic)
* **Build system:** CMake (Qt Creator 20)
* **Storage:** no database. Two CSV logs + two JSON files.

---

## 🎯 Scope (keep it simple)

**In scope (v1):**
* Log meals (food + grams → calories)
* Log workouts (timed or strength)
* Profile screen → daily calorie target
* Dashboard with today's numbers

**Later (after the semester):**
* Line chart of history (QtCharts)
* Searchable ComboBox
* Edit/delete entries, unit tests, etc.

---

## 📂 Project Layout

```text
Clinched/
├── CMakeLists.txt
├── main.cpp                  # Starts the QML engine, registers DataManager
├── datamanager.h / .cpp      # CSV read/write, presets, daily totals
├── profilecalculator.h / .cpp# BMR, TDEE, calorie target
├── Main.qml                  # Window + tab bar (4 tabs)
├── DashboardView.qml         # Today's cards (+ chart later)
├── DietView.qml              # Log meals
├── WorkoutsView.qml          # Log workouts
├── ProfileView.qml           # Edit body details + goal
└── components/
    └── MetricCard.qml        # Reusable card
```

**Runtime files** (created automatically in the app data folder via `QStandardPaths::AppDataLocation`):

```text
diet.csv            # every meal ever logged, one row per meal
workouts.csv        # every workout ever logged, one row per workout
presets.json        # foods + workouts list (editable, no recompiling)
user_profile.json   # weight, height, age, sex, activity, goal
```

---

## 🧩 Architecture

Two C++ classes, exposed to QML. QML never touches files directly.

```text
┌──────────────────────── QML (UI) ────────────────────────┐
│  Dashboard   │   Diet   │   Workouts   │   Profile        │
└──────┬───────────┬────────────┬──────────────┬───────────┘
       │           │            │              │
       ▼           ▼            ▼              ▼
┌──────────────────────────┐   ┌─────────────────────────┐
│       DataManager        │   │    ProfileCalculator    │
│  - load presets.json     │   │  - BMR (Mifflin-St Jeor)│
│  - addMeal()             │   │  - TDEE                 │
│  - addWorkout()          │   │  - daily calorie target │
│  - todayCalories()       │   └────────────┬────────────┘
│  - todayMinutes()        │                │
│  - load/save profile     │◄───────────────┘
└────────────┬─────────────┘
             ▼
   diet.csv · workouts.csv · presets.json · user_profile.json
```

**Roles:**
* **QML** = what the user sees and clicks.
* **DataManager** = reads/writes files, filters by date, sums totals.
* **ProfileCalculator** = pure math, no file access.

---

## 🔄 How Dates Work

The app doesn't "remember" the day. **Every row stores its own date** at the moment it is logged:

```text
Add meal clicked → date = QDate::currentDate(), time = QTime::currentTime() → append row
```

* Log lunch, close the app, reopen at night, log dinner → both rows have today's date automatically.
* **"Today"** = read the CSV and keep only rows where `Date == today`.
* **History** = group rows by `Date` and sum.
* Writing uses `QIODevice::Append`, so old rows are never overwritten.

---

## 🗄️ Data Formats

### 1. `diet.csv`
```text
Date,Time,Food,Grams,Calories
2026-11-07,12:30:15,Chicken Breast (Cooked),200,330
2026-11-07,21:10:04,White Rice (Cooked),150,195
```
`Calories = Grams × cals_per_100g / 100`

### 2. `workouts.csv`
One fixed set of columns. Fields that don't apply to a workout are left empty.
```text
Date,Time,Workout,Minutes,Sets,Reps,WeightKg,Intensity
2026-11-07,10:00:00,Wrestling / Grappling,60,,,,High
2026-11-07,17:30:00,Strength: Legs / Posterior Chain,,5,5,110,Moderate
```

### 3. `presets.json`
Each workout has a `type` that decides which input fields the UI shows.
```json
{
  "foods": [
    {"name": "Chicken Breast (Cooked)", "cals_per_100g": 165},
    {"name": "White Rice (Cooked)",     "cals_per_100g": 130},
    {"name": "Rolled Oats",             "cals_per_100g": 389},
    {"name": "Whole Eggs",              "cals_per_100g": 143},
    {"name": "Peanut Butter",           "cals_per_100g": 588}
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
```json
{
  "weightKg": 80,
  "heightCm": 178,
  "age": 24,
  "sex": "male",
  "activityFactor": 1.55,
  "goal": "maintain"
}
```

---

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

---

## 🖥️ Screens

### Main window
`TabBar` + `StackLayout` with 4 tabs: **Dashboard · Diet · Workouts · Profile**.

### Dashboard
Four `MetricCard`s:
* **Daily Target** (from profile)
* **Consumed Today** (sum of today's calories)
* **Remaining** (Target − Consumed)
* **Training Today** (total minutes + number of strength sessions)

*Later:* line chart of daily calories vs. a target line.

### Diet
* Food `ComboBox` (from `presets.json`)
* Grams field
* Live label: `Calories = grams × cals_per_100g / 100`
* **Add Meal** button → appends to `diet.csv`, updates Dashboard
* List of today's meals

### Workouts
* Workout `ComboBox` (from `presets.json`)
* If `type == "timed"` → **Minutes** field
* If `type == "strength"` → **Sets / Reps / Weight (kg)** fields
* Intensity: Low / Moderate / High
* **Add Workout** button → appends to `workouts.csv`
* List of today's workouts

### Profile
* Weight, height, age, sex, activity level, goal
* **Save** button → writes `user_profile.json`, recalculates the target

---

## 🛠️ Build Order

Each step works on its own before moving on.

| # | Step | What you learn |
|---|------|----------------|
| 1 | Create project (Qt Quick Application, Qt 6.11) and run an empty window | Project setup |
| 2 | `Main.qml` with 4 empty tabs | TabBar + StackLayout |
| 3 | `DietView.qml` with fake in-memory data (ComboBox, grams, live calories, list) | Core QML |
| 4 | `DataManager` in C++, connect to QML, save/load `diet.csv` + `presets.json` | **C++ ↔ QML** (main learning goal) |
| 5 | `WorkoutsView` (timed vs. strength fields) + `workouts.csv` | Conditional UI |
| 6 | `ProfileCalculator` + `ProfileView` + `user_profile.json` | Logic + forms |
| 7 | `DashboardView` + `MetricCard` | Binding to live data |
| 8 | *(Later)* QtCharts history line chart, searchable combo | Polish |
