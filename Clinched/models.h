#ifndef MODELS_H
#define MODELS_H

#include <QDate>
#include <QString>
#include <QTime>

// Plain data holders: no logic, no widgets, no file access.

// One record of diet.json
struct Meal
{
    QDate date;
    QTime time;
    QString food;
    int grams = 0;
    int calories = 0;
};

// One food from presets.json
struct Food {
    QString name;
    int calsPer100g = 0;
};

// How a workout is measured
enum class WorkoutType
{
    Timed,      // measured in minutes (wrestling, sparring, roadwork...)
    Strength    // measured in sets x reps @ weight (lifting)
};

// One workout from presets.json
struct WorkoutPreset
{
    QString name;
    WorkoutType type = WorkoutType::Timed;
};

// One record of workouts.json
struct Workout
{
    QDate date;
    QTime time;
    QString name;
    WorkoutType type = WorkoutType::Timed;
    int minutes = 0;        // used by Timed workouts
    int sets = 0;           // used by Strength workouts
    int reps = 0;
    double weightKg = 0;
    QString intensity;      // "Low", "Moderate" or "High"
};

#endif // MODELS_H
