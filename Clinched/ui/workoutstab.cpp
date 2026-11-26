#include "workoutstab.h"
#include "datamanager.h"

#include <QComboBox>
#include <QDate>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

WorkoutsTab::WorkoutsTab(DataManager *dataManager, QWidget *parent)
    : QWidget{parent}, m_data(dataManager)
{
    m_presets = m_data->workoutPresets();
    buildUi();
}

void WorkoutsTab::buildUi()
{
    // ---- Create the widgets ----
    auto *title = new QLabel("Log a workout");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);

    m_workoutCombo = new QComboBox;
    for (const WorkoutPreset &preset : m_presets)
        m_workoutCombo->addItem(preset.name);

    m_minutesSpin = new QSpinBox;
    m_minutesSpin->setRange(0, 600);
    m_minutesSpin->setSuffix(" min");

    m_setsSpin = new QSpinBox;
    m_setsSpin->setRange(0, 50);

    m_repsSpin = new QSpinBox;
    m_repsSpin->setRange(0, 200);

    m_weightSpin = new QDoubleSpinBox;
    m_weightSpin->setRange(0, 500);
    m_weightSpin->setDecimals(1);
    m_weightSpin->setSingleStep(2.5);
    m_weightSpin->setSuffix(" kg");

    m_intensityCombo = new QComboBox;
    m_intensityCombo->addItems({"Low", "Moderate", "High"});
    m_intensityCombo->setCurrentIndex(1); // Moderate by default

    m_addButton = new QPushButton("Add Workout");
    m_summaryLabel = new QLabel;

    m_table = new QTableWidget(0, 4);
    m_table->setHorizontalHeaderLabels({"Time", "Workout", "Details", "Intensity"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);

    // ---- Arrange them in the layouts ----
    m_form = new QFormLayout;
    m_form->addRow("Workout:", m_workoutCombo);
    m_form->addRow("Minutes:", m_minutesSpin);
    m_form->addRow("Sets:", m_setsSpin);
    m_form->addRow("Reps:", m_repsSpin);
    m_form->addRow("Weight:", m_weightSpin);
    m_form->addRow("Intensity:", m_intensityCombo);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addLayout(m_form);
    mainLayout->addWidget(m_addButton, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_summaryLabel);
    mainLayout->addWidget(m_table);

    // ---- Connect Signals to slots ----
    connect(m_workoutCombo, &QComboBox::currentIndexChanged, this, &WorkoutsTab::updateForm);
    connect(m_minutesSpin, &QSpinBox::valueChanged, this, &WorkoutsTab::updateForm);
    connect(m_setsSpin, &QSpinBox::valueChanged, this, &WorkoutsTab::updateForm);
    connect(m_repsSpin, &QSpinBox::valueChanged, this, &WorkoutsTab::updateForm);
    connect(m_addButton, &QPushButton::clicked, this, &WorkoutsTab::addWorkout);
    connect(m_data, &DataManager::dataChanged, this, &WorkoutsTab::refreshWorkouts);

    // ---- Initial state ----
    updateForm();
    refreshWorkouts();
}

WorkoutType WorkoutsTab::currentType() const
{
    // value() returns a default WorkoutPreset if the index is out of range (e.g. -1)
    return m_presets.value(m_workoutCombo->currentIndex()).type;
}

void WorkoutsTab::updateForm()
{
    const bool timed = (currentType() == WorkoutType::Timed);

    // Show only the inputs that make sense for this type of workout
    m_form->setRowVisible(m_minutesSpin, timed);
    m_form->setRowVisible(m_setsSpin, !timed);
    m_form->setRowVisible(m_repsSpin, !timed);
    m_form->setRowVisible(m_weightSpin, !timed);

    // Enable the button only when the needed inputs are filled
    bool ready = false;
    if (timed)
        ready = m_minutesSpin->value() > 0;
    else
        ready = m_setsSpin->value() > 0 && m_repsSpin->value() > 0;

    m_addButton->setEnabled(ready && !m_presets.isEmpty());
}

void WorkoutsTab::addWorkout()
{
    const WorkoutPreset preset = m_presets.value(m_workoutCombo->currentIndex());

    Workout workout;
    workout.name = preset.name;
    workout.type = preset.type;
    workout.intensity = m_intensityCombo->currentText();

    if (preset.type == WorkoutType::Timed) {
        workout.minutes = m_minutesSpin->value();
    } else {
        workout.sets = m_setsSpin->value();
        workout.reps = m_repsSpin->value();
        workout.weightKg = m_weightSpin->value();
    }

    m_data->addWorkout(workout);

    // Reset the inputs
    m_minutesSpin->setValue(0);
    m_setsSpin->setValue(0);
    m_repsSpin->setValue(0);
    m_weightSpin->setValue(0);
}

void WorkoutsTab::refreshWorkouts()
{
    const QList<Workout> workouts = m_data->workoutsForDate(QDate::currentDate());

    m_table->setRowCount(0);
    int totalMinutes = 0;

    // Newest workouts always on top
    for (int i = workouts.count() - 1; i >= 0; --i) {
        appendWorkoutRecord(workouts[i]);
        totalMinutes += workouts[i].minutes;
    }

    m_summaryLabel->setText(QString("Today: %1 workouts, %2 min")
                                .arg(workouts.count())
                                .arg(totalMinutes));
}

void WorkoutsTab::appendWorkoutRecord(const Workout &workout)
{
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(workout.time.toString("HH:mm")));
    m_table->setItem(row, 1, new QTableWidgetItem(workout.name));
    m_table->setItem(row, 2, new QTableWidgetItem(detailsText(workout)));
    m_table->setItem(row, 3, new QTableWidgetItem(workout.intensity));
}

QString WorkoutsTab::detailsText(const Workout &workout) const
{
    if (workout.type == WorkoutType::Timed)
        return QString("%1 min").arg(workout.minutes);

    return QString("%1 x %2 @ %3 kg")
        .arg(workout.sets)
        .arg(workout.reps)
        .arg(workout.weightKg);
}
