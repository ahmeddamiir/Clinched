#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QDate>
#include <QList>
#include <QObject>
#include <QString>
#include <QTime>

// One row of diet.csv
struct Meal
{
    QDate date;
    QTime time;
    QString food;
    int grams = 0;
    int calories = 0;
};

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);

    void addMeal(const QString &food, int grams, int calories);
    QList<Meal> mealsForDate(const QDate &date) const;

signals:
    void dataChanged(); // emitted after anything is saved

private:
    QString dietFilePath() const;
};

#endif // DATAMANAGER_H
