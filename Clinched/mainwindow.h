#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showProfile(); // runs when the Profile action is triggered

private:
    void createTabs();
    void createMenus();

    QTabWidget *m_tabs;
};

#endif