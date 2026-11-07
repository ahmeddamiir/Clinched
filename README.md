# 🥊 Clinched: Daily Athlete Training & Nutrition Tracker (Qt / C++ / QML)

**Clinched** is a local desktop application designed for fighters and athletes to track daily training volume and caloric intake against target body goals. It replaces complex relational database engines with a robust, date-indexed **CSV log structure** and an external **JSON preset dictionary**, ensuring zero runtime overhead and clear data persistence.

---

## 📂 Repository Directory Tree Layout

```text
Clinched/
├── Clinched.pro                 # Core Qt Project Build Script
├── presets.json                 # Reference library for foods & workout types
├── user_profile.json            # Target body weight, baseline BMR, and calculated budget
├── data/                        # Historical & active daily logs
│   ├── 2026-11-05_workouts.csv
│   ├── 2026-11-05_diet.csv
│   ├── 2026-11-06_workouts.csv
│   ├── 2026-11-06_diet.csv
│   ├── 2026-11-07_workouts.csv  # Active daily session
│   └── 2026-11-07_diet.csv      # Active daily session
├── src/
│   ├── main.cpp                 # Bootstrapper & QML engine registration
│   ├── datamanager.h            # CSV append engine, day-file scanner & chart parser
│   ├── datamanager.cpp
│   ├── profilecalculator.h     # BMR, TDEE, & Calorie budget algorithms
│   └── profilecalculator.cpp
└── qml/
    ├── main.qml                 # Main window with navigation bar
    ├── DashboardView.qml        # Daily summary cards & historical QtCharts LineSeries
    ├── WorkoutsView.qml         # Training logging tab with search & session list
    ├── DietView.qml             # Caloric intake logger with live math & batch review
    └── components/
        ├── MetricCard.qml       # Reusable UI card for metrics
        └── SearchableCombo.qml  # Filterable editable ComboBox
```

---

## 🔄 End-to-End Data Flow Architecture

The data architecture guarantees session recovery (closing and reopening the application maintains daily persistence) using standard file I/O operations in **Append Mode (`QIODevice::Append`)**.

```text
 ┌────────────────────────────────────────────────────────────────────────┐
 │                         INITIALIZATION PHASE                           │
 └────────────────────────────────────────────────────────────────────────┘
        │
        ├──> [ presets.json ] ───> Loads searchable food items & workout lists
        │
        └──> [ user_profile.json ] ──> Computes Target Calories via Mifflin-St Jeor:
                                        Target = TDEE (+/- Surplus/Deficit)

 ┌────────────────────────────────────────────────────────────────────────┐
 │                         ACTIVE LOGGING PHASE                           │
 └────────────────────────────────────────────────────────────────────────┘

     [ WorkoutsView ]                            [ DietView ]
    (Select type + Duration/Vol)                (Select food + Weight in Grams)
              │                                               │
              │                                               ▼
              │                                Live preview calculation:
              │                                Cals = (Grams * Cal/100g) / 100
              │                                               │
              ▼                                               ▼
   Append to daily log:                           Append to daily log:
   `YYYY-MM-DD_workouts.csv`                      `YYYY-MM-DD_diet.csv`
              │                                               │
              └───────────────────────┬───────────────────────┘
                                      ▼
                        [ System Data Directory ]
                      (Existing rows preserved;
                       new rows appended safely)

 ┌────────────────────────────────────────────────────────────────────────┐
 │                      AGGREGATION & DASHBOARD PHASE                     │
 └────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
                             [ DataManager (C++) ]
                                      │
    ┌─────────────────────────────────┴─────────────────────────────────┐
    ▼                                                                   ▼
[ Current Day Aggregate ]                                    [ Historical Timeline ]
 • Reads `today_diet.csv`                                     • Scans `data/*.csv` files
 • Sums daily consumed calories                               • Extracts daily total calories
 • Compares with Target Budget                                • Builds `QLineSeries` coordinates
 • Calculates remaining allowance                             • Renders trend vs. target line
    │                                                                   │
    └─────────────────────────────────┬─────────────────────────────────┘
                                      ▼
                             [ DashboardView.qml ]
                      (Live metric cards + Progress chart)
```

---

## 🗄️ Flat-File Storage Specifications

### 1. Daily Diet Logs (`YYYY-MM-DD_diet.csv`)
Appends individual food consumption entries. Closing and reopening the app checks for today's file and loads existing rows.
* **Format:** `Timestamp,FoodName,Grams,ComputedCalories`
* **Example:**
  ```text
  12:30:15,Chicken Breast,200,330
  15:10:04,White Rice,150,195
  19:45:22,Peanut Butter,30,176
  ```

### 2. Daily Workout Logs (`YYYY-MM-DD_workouts.csv`)
Appends discrete physical training bouts focusing on training volume instead of inaccurate calorie estimation.
* **Format:** `Timestamp,WorkoutType,DurationOrVolume,IntensityRating`
* **Example:**
  ```text
  10:00:00,Wrestling / Sparring,60 mins,High
  17:30:00,Strength: Lower Body,5 sets x 5 reps @ 110kg,Moderate
  ```

### 3. Presets Dictionary (`presets.json`)
Read-only reference file editable without recompilation.
```json
{
  "foods": [
    {"name": "Chicken Breast (Cooked)", "cals_per_100g": 165},
    {"name": "White Rice (Cooked)", "cals_per_100g": 130},
    {"name": "Rolled Oats", "cals_per_100g": 389},
    {"name": "Whole Eggs", "cals_per_100g": 143},
    {"name": "Peanut Butter", "cals_per_100g": 588}
  ],
  "workouts": [
    "Wrestling / Grappling",
    "MMA Sparring",
    "Muay Thai / Striking",
    "Strength: Push Focus",
    "Strength: Pull Focus",
    "Strength: Legs / Posterior Chain",
    "Roadwork / Conditioning"
  ]
}
```

---

## 🧮 Target Calculation Model

The target daily intake accounts for user goals (weight loss, maintenance, or weight gain):

1. **Basal Metabolic Rate (BMR) [Mifflin-St Jeor]:**
   $$BMR = 10 \times \text{weight (kg)} + 6.25 \times \text{height (cm)} - 5 \times \text{age} + s$$
   *(where $s = +5$ for males, $-161$ for females)*

2. **Total Daily Energy Expenditure (TDEE):**
   $$TDEE = BMR \times \text{Activity Factor}$$

3. **Goal-Adjusted Caloric Target:**
   * **Weight Gain (Bulking):** $\text{Target} = TDEE + 350 \text{ to } 500 \text{ kcal}$
   * **Weight Loss (Cutting):** $\text{Target} = TDEE - 500 \text{ kcal}$
   * **Maintenance:** $\text{Target} = TDEE$

---

## 🖥️ Screen Layout Specifications

### 1. Dashboard View (`DashboardView.qml`)
* **Metric Cards:**
  * **Daily Target:** Static calculated calorie baseline.
  * **Consumed Today:** Sum of all `ComputedCalories` in today's CSV.
  * **Remaining Balance:** $\text{Target} - \text{Consumed}$.
  * **Today's Training Volume:** Total sessions or minutes logged today.
* **Progress Line Chart (`QtCharts`):**
  * Plots daily consumed calories across all dates present in the `data/` folder.
  * Displays a horizontal reference line for the daily target budget to monitor compliance over time.

### 2. Workouts Tab (`WorkoutsView.qml`)
* Searchable `ComboBox` populated from `presets.json`.
* Input field for duration (e.g., minutes) or resistance volume (e.g., sets/reps/weight).
* Intensity indicator (Low, Moderate, High).
* Multi-entry capability: clicking **"Add Workout"** appends to today's CSV and updates the in-memory session list without overwriting prior logs.

### 3. Diet Tab (`DietView.qml`)
* Searchable `ComboBox` populated from `presets.json`.
* Portion weight input in grams.
* Live preview label showing calculated calories prior to logging:
  $$\text{Calories} = \frac{\text{Grams} \times \text{cals\_per\_100g}}{100}$$
* Multi-entry capability: clicking **"Add Meal"** appends to today's CSV and instantly increments the daily dashboard totals.