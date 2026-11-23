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

#endif // MODELS_H
