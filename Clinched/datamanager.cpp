#include "datamanager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    // Assure the existence of the data folder
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(folder);

    qDebug() << "Data folder:" << folder; // temporary: shows where the files are
}

// Helper function returns the path if needed (as QString)
QString DataManager::filePath(const QString &fileName) const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + fileName;
}

void DataManager::addMeal(const QString &food, int grams, int calories)
{
    // Build the new record as a JSON object
    QJsonObject meal;
    meal["date"] = QDate::currentDate().toString(Qt::ISODate);
    meal["time"] = QTime::currentTime().toString("HH:mm:ss");
    meal["food"] = food;
    meal["grams"] = grams;
    meal["calories"] = calories;

    // Load all old meals and add the new one
    QJsonArray meals = readDocument("diet.json").array();
    meals.append(meal);

    // Save/Write everything back
    if (writeDocument("diet.json", QJsonDocument(meals)))
        emit dataChanged(); // A signal to the other tabs that we have a new record
}

QList<Meal> DataManager::mealsForDate(const QDate &date) const
{
    QList<Meal> result;

    const auto meals = readDocument("diet.json").array();
    for (const auto &value : meals) {
        const auto obj = value.toObject();

        const auto rowDate = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        if (rowDate != date)
                continue; // skip if it's a different day

        Meal meal;
        meal.date = rowDate;
        meal.time = QTime::fromString(obj["time"].toString(), "HH:mm:ss");
        meal.food = obj["food"].toString();
        meal.grams = obj["grams"].toInt();
        meal.calories = obj["calories"].toInt();
        result.append(meal);
    }

    return result;
}

int DataManager::caloriesForDate(const QDate &date) const
{
    const auto meals = mealsForDate(date);
    auto totalCalories = 0;
    for (const auto &meal : meals) {
        totalCalories+=meal.calories;
    }

    return totalCalories;
}

QJsonDocument DataManager::readDocument(const QString &fileName) const
{
    QFile file(filePath(fileName));
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonDocument(); // No file yet = empty document
    }

    return QJsonDocument::fromJson(file.readAll()); // null document if damaged
}


bool DataManager::writeDocument(const QString &fileName, const QJsonDocument &doc) const
{
    QSaveFile file(filePath(fileName));
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open file: " << file.fileName();
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    return file.commit();
}
