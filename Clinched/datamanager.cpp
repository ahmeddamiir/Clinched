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
    ensurePresetsFile();
}

// دالة مساعدة بترجع مسار أي ملف جوه فولدر بيانات التطبيق
QString DataManager::filePath(const QString &fileName) const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + '/' + fileName;
}

// بتاخد بيانات الوجبة و تحولها لجيسون اوبجكت و تضيف عليه التاريخ و الساعة
// بتقرأ مصفوفة الوجبات القديمة من ملف الدايت و تضيف عليها الجديدة و تحفظهم و تبعت إشارة
// عشان التاب المهتم يحدث نفسه
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

// بتقرأ كل الوجبات, تفلترهم, و ترجع قائمة بالوجبات اللي تاريخها يطابق اليوم المطلوب بس
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

        if (!food.name.isEmpty())
            result.append(food);
    }

    return result;
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

// بتشيك لو ملف البريسيتس مش موجود, بتنسخه و بتفك عنه وضع الفراءة فقط عشان أعدل فيه
void DataManager::ensurePresetsFile() const
{
    const QString target = filePath("presets.json");
    if (QFile::exists(target)) return; // keep the user's own copy

    if (!QFile::copy(":/presets.json", target)) {
        qWarning() << "Couldn't create" << target;
        return;
    }

    // Files copied out of a Qt resources are read-only, so make it writable
    QFile::setPermissions(target, QFile::ReadOwner | QFile::WriteOwner);
}