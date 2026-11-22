#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QDate>
#include <QList>
#include <QObject>
#include <QString>
#include <QTime>

class QJsonDocument;

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

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    void addMeal(const QString &food, int grams, int calories);
    QList<Meal> mealsForDate(const QDate &date) const;
    int caloriesForDate(const QDate &date) const;
    QList<Food> foods() const;

signals:
    void dataChanged(); // emitted after anything is saved

private:
    QString filePath(const QString &fileName) const;
    QJsonDocument readDocument(const QString &fileName) const;
    bool writeDocument(const QString &fileName, const QJsonDocument &doc) const;
    void ensurePresetsFile() const;
};

#endif // DATAMANAGER_H
