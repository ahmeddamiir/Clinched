#ifndef WORKOUTSTAB_H
#define WORKOUTSTAB_H

#include "models.h"

#include <QWidget>
#include <QList>

class DataManager;
class QComboBox;
class QDoubleSpinBox;
class QFormLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableWidget;

class WorkoutsTab : public QWidget
{
    Q_OBJECT

public:
    explicit WorkoutsTab(DataManager *dataManager, QWidget *parent = nullptr);

private slots:
    void updateForm();          // shows the right inputs + enables/disables the button
    void addWorkout();          // sends the current inputs to the DataManager
    void refreshWorkouts();     // reloads today's workouts into the table

private:
    void buildUi();
    void appendWorkoutRecord(const Workout &workout);
    QString detailsText(const Workout &workout) const;
    WorkoutType currentType() const;

    DataManager *m_data;
    QList<WorkoutPreset> m_presets;

    QFormLayout *m_form;
    QComboBox *m_workoutCombo;
    QSpinBox *m_minutesSpin;
    QSpinBox *m_setsSpin;
    QSpinBox *m_repsSpin;
    QDoubleSpinBox *m_weightSpin;
    QComboBox *m_intensityCombo;
    QPushButton *m_addButton;
    QLabel *m_summaryLabel;
    QTableWidget *m_table;

signals:

};

#endif // WORKOUTSTAB_H
