#ifndef DIETTAB_H
#define DIETTAB_H

#include <QList>
#include <QString>
#include <QWidget>

class QComboBox;
class QLabel;
class QPushButton;
class QSpinBox;
class QTableWidget;

class DietTab : public QWidget
{
    Q_OBJECT

public:
    explicit DietTab(QWidget *parent = nullptr);

private slots:
    void updatePreview(); // recalculates the live calories label
    void addMeal(); // adds the current selection to the table

private:
    struct Food {
        QString name;
        int calsPer100g;
    };

void loadFoods();
void buildUi();
void refreshTotal();
int caloriesFor(int foodIndex, int grams) const;

QList<Food> m_foods;
int m_totalCalories = 0;

QComboBox *m_foodCombo;
QSpinBox *m_gramsSpin;
QLabel *m_previewLabel;
QPushButton *m_addButton;
QLabel *m_totalLabel;
QTableWidget *m_table;

signals:

};

#endif // DIETTAB_H