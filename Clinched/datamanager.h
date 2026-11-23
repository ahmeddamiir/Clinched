#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QList>
#include <QObject>
#include <QString>

#include "models.h"

class QJsonDocument;
class QJsonObject;

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

    // Shared by diet and workouts
    QJsonObject newRecord() const;
    bool appendRecord(const QString &fileName, const QJsonObject &record) const;
    QList<QJsonObject> recordsForDate(const QString &fileName, const QDate &date) const;
};

#endif // DATAMANAGER_H
