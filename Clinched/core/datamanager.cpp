#include "datamanager.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <algorithm>

namespace {

// The ONLY place that knows how a workout type is written in the files
WorkoutType typeFromString(const QString &text)
{
    return text == "strength" ? WorkoutType::Strength : WorkoutType::Timed;
}

QString typeToString(WorkoutType type)
{
    return type == WorkoutType::Strength ? "strength" : "timed";
}

} // namespace

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    // Assure the existence of the data folder
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(folder);

    qDebug() << "Data folder:" << folder; // temporary: shows where the files are
    ensurePresetsFile();
}

// بتاخد بيانات الوجبة و تحولها لجيسون اوبجكت و تضيف عليه التاريخ و الساعة
// بتقرأ مصفوفة الوجبات القديمة من ملف الدايت و تضيف عليها الجديدة و تحفظهم و تبعت إشارة
// عشان التاب المهتم يحدث نفسه
void DataManager::addMeal(const QString &food, int grams, int calories)
{
    // Build the new record as a JSON object
    auto meal = newRecord();
    meal["food"] = food;
    meal["grams"] = grams;
    meal["calories"] = calories;

    // Save/Write everything back
    if (appendRecord("diet.json", meal))
        emit dataChanged(); // A signal to the other tabs that we have a new record
}

// بتقرأ كل الوجبات, تفلترهم, و ترجع قائمة بالوجبات اللي تاريخها يطابق اليوم المطلوب بس
QList<Meal> DataManager::mealsForDate(const QDate &date) const
{
    QList<Meal> result;

    const auto records = recordsForDate("diet.json", date);
    for (const auto &obj : records) {
        Meal meal;
        meal.date = date;
        meal.time = QTime::fromString(obj["time"].toString(), "HH:mm:ss");
        meal.food = obj["food"].toString();
        meal.grams = obj["grams"].toInt();
        meal.calories = obj["calories"].toInt();
        result.append(meal);
    }

    return result;
}

// بتجيب وجبات اليوم و تجمع سعراتهم و ترجع الاجمالي كرقم صحيح
int DataManager::caloriesForDate(const QDate &date) const
{
    const auto meals = mealsForDate(date);
    auto totalCalories = 0;
    for (const auto &meal : meals) {
        totalCalories+=meal.calories;
    }

    return totalCalories;
}

// بتقرأ ملف البريستس و تدخل على مصفوفة فوودز, تحول كل عنصر فيها لستراكت (الاسم و السعرات)
// و ترجعهم في قائمة عشان تابة الدايت تعرضهم
QList<Food> DataManager::foods() const
{
    QList<Food> result;

    const QJsonObject root = readDocument("presets.json").object();
    const QJsonArray foodArray = root["foods"].toArray();

    for (const QJsonValue &value : foodArray) {
        const auto obj = value.toObject();

        Food food;
        food.name = obj["name"].toString();
        food.calsPer100g = obj["cals_per_100g"].toInt();

        if (!food.name.isEmpty() && food.calsPer100g > 0)
            result.append(food);
    }

    // ترتيب الأكلات أبجدياً
    std::sort(result.begin(), result.end(), [] (const Food &a, const Food &b) {
        return a.name < b.name;
    });

    return result;
}

// The date and time inside `workout` are ignored: DataManager stamps the current ones
void DataManager::addWorkout(const Workout &workout)
{
    QJsonObject record = newRecord();
    record["workout"] = workout.name;
    record["type"] = typeToString(workout.type);
    record["intensity"] = workout.intensity;

    // Only write the fields that make sense for this type of workout
    if (workout.type == WorkoutType::Timed) {
            record["minutes"] = workout.minutes;
    } else {
        record["sets"] = workout.sets;
        record["reps"] = workout.reps;
        record["weightKg"] = workout.weightKg;
    }

    if (appendRecord("workouts.json", record))
        emit dataChanged();
}

QList<Workout> DataManager::workoutsForDate(const QDate &date) const
{
    QList<Workout> result;

    const auto records = recordsForDate("workouts.json", date);
    for (const auto &obj : records) {
        Workout workout;
        workout.date = date;
        workout.time = QTime::fromString(obj["time"].toString(), "HH:mm:ss");
        workout.name = obj["workout"].toString();
        workout.type = typeFromString(obj["type"].toString());
        workout.minutes = obj["minutes"].toInt();
        workout.sets = obj["sets"].toInt();
        workout.reps = obj["reps"].toInt();
        workout.weightKg = obj["weightKg"].toDouble();
        workout.intensity = obj["intensity"].toString();
        result.append(workout);
    }

    return result;
}

QList<WorkoutPreset> DataManager::workoutPresets() const
{
    QList<WorkoutPreset> result;

    const QJsonObject root = readDocument("presets.json").object();
    const QJsonArray workoutArray = root["workouts"].toArray();

    for (const QJsonValue &value : workoutArray) {
        const auto obj = value.toObject();

        WorkoutPreset preset;
        preset.name = obj["name"].toString();
        preset.type = typeFromString(obj["type"].toString());

        if (!preset.name.isEmpty())
            result.append(preset);
    }

    return result;
}

// دالة مساعدة بترجع مسار أي ملف جوه فولدر بيانات التطبيق
QString DataManager::filePath(const QString &fileName) const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + fileName;
}

// بتفتح أي ملف جيسون و تقرأ محتواه و ترجعه كدوكمنت و لو مفيش ملف ترجع مستند فاضي
QJsonDocument DataManager::readDocument(const QString &fileName) const
{
    QFile file(filePath(fileName));
    if (!file.open(QIODevice::ReadOnly)) {
        return QJsonDocument(); // No file yet = empty document
    }

    return QJsonDocument::fromJson(file.readAll()); // null document if damaged
}

// بتكتب أي دوكمنت في الملف باستخدام كيو سيف فايل
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

// A new record that already has today's date and the current time
QJsonObject DataManager::newRecord() const
{
    QJsonObject record;
    record["date"] = QDate::currentDate().toString(Qt::ISODate);
    record["time"] = QTime::currentTime().toString("HH:mm:ss");
    return record;
}

// Load the whole array, add the record at the end, save everything back
bool DataManager::appendRecord(const QString &fileName, const QJsonObject &record) const
{
    QJsonArray records = readDocument(fileName).array();
    records.append(record);
    return writeDocument(fileName, QJsonDocument(records));
}

// All the records of one day (the date filter lives in ONE place now)
QList<QJsonObject> DataManager::recordsForDate(const QString &fileName, const QDate &date) const
{
    QList<QJsonObject> result;

    const QJsonArray records = readDocument(fileName).array();
    for (const QJsonValue &value : records) {
        const QJsonObject obj = value.toObject();

        const QDate rowDate = QDate::fromString(obj["date"].toString(), Qt::ISODate);
        if (rowDate == date)
            result.append(obj);
    }
    return result;
}

// بتشيك لو ملف البريسيتس مش موجود, بتنسخه و بتفك عنه وضع الفراءة فقط عشان أعدل فيه
void DataManager::ensurePresetsFile() const
{
    const QString target = filePath("presets.json");
    if (QFile::exists(target)) return; // keep the user's own copy

    if (!QFile::copy(":/resources/presets.json", target)) {
        qWarning() << "Couldn't create" << target;
        return;
    }

    // Files copied out of a Qt resources are read-only, so make it writable
    QFile::setPermissions(target, QFile::ReadOwner | QFile::WriteOwner);
}