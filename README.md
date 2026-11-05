# 🥊 Clinched: MMA Training & Nutrition Tracker (Qt 5 / C++ / QML)

**Clinched** is a high-performance desktop application architecture engineered using **Qt 5 (C++)** and **QML Quick Components** following strict Model-View-Controller (MVC) architectural patterns. It provides single-user local synchronization for training metrics, weight cuts, and diet macros.

---

## 📂 Repository Directory Tree Layout

```text
Clinched/
├── Clinched.pro                 # Core QMake Project Configuration
├── src/                         # Backend C++ Engine (Logic & Persistence)
│   ├── main.cpp                 # Main Native Bootstrapper
│   ├── database/
│   │   ├── databasemanager.cpp
│   │   └── databasemanager.h    # Embedded SQLite CRUD Controller
│   ├── models/
│   │   ├── workoutmodel.cpp     # QAbstractListModel Bridge for Workouts
│   │   ├── workoutmodel.h
│   │   ├── dietmodel.cpp        # QAbstractListModel Bridge for Diet Logs
│   │   └── dietmodel.h
│   └── controllers/
│       ├── profilecontroller.cpp# Target Weight & Macro Calculation Module
│       └── profilecontroller.h
└── qml/                         # Presentation Layer (Custom Dark Theme UI)
    ├── main.qml                 # Main Viewport Container & State Routing
    ├── qml.qrc                  # Embedded Qt Resource Configuration
    ├── components/              # Decoupled UI Element Primitives
    │   ├── CustomButton.qml
    │   ├── CustomTextField.qml
    │   └── Style.qml            # Global Centralized Palette Configuration
    └── views/                   # Specialized Screen Layouts
        ├── DashboardView.qml    # High-Impact QtCharts & Progress Metrics
        ├── WorkoutsLogView.qml  # Split Session Insertion Interface
        └── DietLogView.qml      # Micro-nutrient Tracking Spreadsheet
```

## 📐 Structural Architecture Blueprint

```text
 ┌────────────────────────────────────────────────────────┐
 │                      VIEW (QML)                        │
 │  DashboardView  │  WorkoutsLogView  │  DietLogView     │
 └───────────────────────────┬────────────────────────────┘
                             │
                             ▼ [Signals / Invokables]
 ┌────────────────────────────────────────────────────────┐
 │                    CONTROLLER (C++)                    │
 │               [ ProfileController Class ]              │
 └───────────────────────────┬────────────────────────────┘
                             │
                             ▼ [Updates Models]
 ┌────────────────────────────────────────────────────────┐
 │                     MODEL (C++)                        │
 │        [ WorkoutModel ]      │       [ DietModel ]     │
 └───────────────────────────┬────────────────────────────┘
                             │
                             ▼ [Prepared Transactions]
 ┌────────────────────────────────────────────────────────┐
 │                 PERSISTENCE (SQLite)                   │
 │                  [ DatabaseManager ]                   │
 └────────────────────────────────────────────────────────┘
```

---

## 🗄️ Database Tables Schema

User Profile Table
Stores the user's physical attributes, athletic target goals, and automated calculation constraints.
* `name` (TEXT): The fighter's profile name.
* `current_weight` (REAL): Present baseline body weight in kilograms.
* `target_weight` (REAL): Tournament weight class milestone goal.
* `daily_calorie_budget` (INTEGER): Daily automated calorie budget target.

---

Workouts Table
Appends sequential discrete logs tracking diverse athletic session blocks.
* `id` (INTEGER - PK AUTOINCREMENT): Unique sequence transaction identifier.
* `datetime` (TEXT): ISO timeline stamp (e.g., `"2026-11-05 10:00 PM"`).
* `type` (TEXT): Structural split category (`"MMA Drills"` / `"Strength & Weights"`).
* `details` (TEXT): Content logging text (e.g., `"Wrestling Takedowns"` / `"Bench Press"`).
* `duration_or_weight` (TEXT): Volume tracker (e.g., `"45 mins"` / `"4 sets x 10 reps @ 70kg"`).

---

Diet Table
Captures food consumption events mapped against structural macro bounds.
* `id` (INTEGER - PK AUTOINCREMENT): Unique caloric record tracking index.
* `datetime` (TEXT): Consumption execution timestamp (e.g., `"2026-11-05 01:00 PM"`).
* `food_name` (TEXT): Item label tracking name (e.g., `"Chicken Breast & Rice"`).
* `weight_grams` (REAL): Raw mass dimension metric (e.g., `200.0`).
* `calories` (INTEGER): Total computed energetic value in kilocalories (e.g., `550`).

---

## ⚡ Functional Specifications
- **Dedicated Athletic Partitions:** Distinct tracking models isolating cardiovascular performance/sparring rounds from macro-nutrient intake indices.
- **Dynamic Interface Synthesis:** Direct mapping of custom C++ models into responsive QML listing elements optimizing host runtime RAM footprint.
- **Unified Styled Tokens:** Centralized color configurations preventing palette drifting and facilitating immediate asset rendering.
