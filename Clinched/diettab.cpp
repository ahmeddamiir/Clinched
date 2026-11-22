#include "diettab.h"
#include "datamanager.h"

#include <QDate>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>

DietTab::DietTab(DataManager *dataManager, QWidget *parent)
    : QWidget{parent}, m_data(dataManager)
{
    loadFoods();
    buildUi();
}

void DietTab::loadFoods()
{
    // Temporary hard-coded data. In step 4 this comes from presets.json
    m_foods = {
        {"Chicken Breast", 165},
        {"White Rice",     130},
        {"Rolled Oats",    389},
        {"Whole Eggs",     143},
        {"Peanut Butter",  588},
    };
}

void DietTab::buildUi()
{
    // ---- Create the widgets ----
    auto *title = new QLabel("Insert a meal");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);

    m_foodCombo = new QComboBox;
    for (const Food &food : m_foods)
        m_foodCombo->addItem(food.name);

    m_gramsSpin = new QSpinBox;
    m_gramsSpin->setRange(0, 5000);
    m_gramsSpin->setSuffix(" g");

    m_previewLabel = new QLabel;
    m_addButton = new QPushButton("Add Meal");
    m_totalLabel = new QLabel;

    m_table = new QTableWidget(0, 4);
    m_table->setHorizontalHeaderLabels({"Time", "Food", "Grams", "Calories"});
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->setVisible(false);

    // ---- Arrange them in the layouts ----
    auto *inputRow = new QHBoxLayout;
    inputRow->addWidget(m_foodCombo, 1); // stretch = 1: takes the extra width
    inputRow->addWidget(m_gramsSpin);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(title);
    mainLayout->addLayout(inputRow);
    mainLayout->addWidget(m_previewLabel);
    mainLayout->addWidget(m_addButton, 0, Qt::AlignLeft);
    mainLayout->addWidget(m_totalLabel);
    mainLayout->addWidget(m_table);

    // ---- Connect Signals to slots ----
    connect(m_foodCombo, &QComboBox::currentIndexChanged, this, &DietTab::updatePreview);
    connect(m_gramsSpin, &QSpinBox::valueChanged, this, &DietTab::updatePreview);
    connect(m_addButton, &QPushButton::clicked, this, &DietTab::addMeal);
    connect(m_data, &DataManager::dataChanged, this, &DietTab::refreshMeals);

    // ---- Initial state ----
    updatePreview();
    refreshMeals();
}

int DietTab::caloriesFor(int foodIndex, int grams) const
{
    const Food &food = m_foods.at(foodIndex);
    return qRound(grams * food.calsPer100g / 100.0);
}

void DietTab::updatePreview()
{
    const int grams = m_gramsSpin->value();
    const int cals = caloriesFor(m_foodCombo->currentIndex(), grams);

    m_previewLabel->setText(QString("Calories: %1 kcal").arg(cals));
    m_addButton->setEnabled(grams > 0);
}

void DietTab::addMeal()
{
    const QString name = m_foodCombo->currentText();
    const int grams = m_gramsSpin->value();
    const int cals = caloriesFor(m_foodCombo->currentIndex(), grams);

    m_data->addMeal(name, grams, cals);
    m_gramsSpin->setValue(0);
}

void DietTab::refreshMeals()
{
    const QList<Meal> meals = m_data->mealsForDate(QDate::currentDate());

    m_table->setRowCount(0);
    int total = 0;

    // Newest meals always on top
    const int numberOfMeals = meals.count();
    for (int i = numberOfMeals-1; i >= 0; i--)
    {
        appendMealRecord(meals[i]);
        total+=meals[i].calories;
    }

    m_totalLabel->setText(QString("Total today: %1 kcal").arg(total));
}

void DietTab::appendMealRecord(const Meal &meal)
{
    const int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(meal.time.toString("HH:mm")));
    m_table->setItem(row, 1, new QTableWidgetItem(meal.food));
    m_table->setItem(row, 2, new QTableWidgetItem(QString::number(meal.grams)));
    m_table->setItem(row, 3, new QTableWidgetItem(QString::number(meal.calories)));
}