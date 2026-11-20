#include "datamanager.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QTextStream>

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    // Assure the existence of the data folder
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(folder);

    qDebug() << "Data folder:" << folder; // temporary: shows where the files are
}

// Helper function returns the path if needed (as QString)
QString DataManager::dietFilePath() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/diet.csv";
}

void DataManager::addMeal(const QString &food, int grams, int calories)
{
    QFile file(dietFilePath());
    if (!file.open(QIODevice::Text | QIODevice::Append)) {
        qWarning() << "Couldn't open file: " << file.fileName();
        return;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8); // To handle the Arabic language

    // Write the header names first if the file is newly created
    if (file.size() == 0)
        out << "Date,Time,Food,Grams,Calories\n";

    // Replace the comma by whitespace to not break the csv's columns
    QString safeFood = food;
    safeFood.replace(',', ' ');

    out << QDate::currentDate().toString(Qt::ISODate) << ','
        << QTime::currentTime().toString("HH:mm:ss") << ','
        << safeFood << ','
        << grams << ','
        << calories << '\n';

    file.close();
    emit dataChanged(); // A signal to the other tabs that we have a new record
}

QList<MealEntry> DataManager::mealsForDate(const QDate &date) const
{
    QList<MealEntry> result;

    QFile file(dietFilePath());
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return result; // No File -> No Meals (yet!)
    }

    QTextStream in(&file);
    // To handle the Arabic language when reading the file again
    in.setEncoding(QStringConverter::Utf8);
    in.readLine(); // Skip the header line in every first entry

    // Guard Clauses:- batgahel ay record msh monaseb
    while(!in.atEnd()) {
        // Clean the record (line / row) from white spaces
        const QString record = in.readLine().trimmed();

        if (record.isEmpty())
            continue;

        const QStringList fields = record.split(',');
        if (fields.size() != 5) continue;

        const QDate rowDate = QDate::fromString(fields[0], Qt::ISODate);
        if (rowDate != date) continue;

        MealEntry entry;
        entry.date = rowDate;
        entry.time = QTime::fromString(fields[1], "HH:mm:ss");
        entry.food = fields[2];
        entry.grams = fields[3].toInt();
        entry.calories = fields[4].toInt();

        result.append(entry);
    }

    return result;
}