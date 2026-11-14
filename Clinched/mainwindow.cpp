#include "mainwindow.h"
#include <QAction>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QTabWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Clinched");
    resize(800, 600);

    createTabs();
    createMenus();
}

void MainWindow::createTabs()
{
    m_tabs = new QTabWidget(this);

    // Temporary pages: each is just a centered label for now
    auto *dashboardPage = new QLabel("Dashboard page");
    auto *dietPage = new QLabel("Diet page");
    auto *workoutsPage = new QLabel("Workouts page");
    dashboardPage->setAlignment(Qt::AlignCenter);
    dietPage->setAlignment(Qt::AlignCenter);
    workoutsPage->setAlignment(Qt::AlignCenter);

    m_tabs->addTab(dashboardPage, "Dashboard");
    m_tabs->addTab(dietPage, "Diet");
    m_tabs->addTab(workoutsPage, "Workouts");

    setCentralWidget(m_tabs);
}

void MainWindow::createMenus()
{
    // One QAction = one command (text + shortcut + what it does)
    auto *profileAction = new QAction("&Profile...", this);
    profileAction->setShortcut(QKeySequence("Ctrl+P"));
    connect(profileAction, &QAction::triggered, this, &MainWindow::showProfile);

    auto *quitAction = new QAction("&Quit", this);
    quitAction->setShortcut(QKeySequence("Ctrl+Q"));
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(profileAction);
    fileMenu->addSeparator();
    fileMenu->addAction(quitAction);
}

void MainWindow::showProfile()
{
    QMessageBox::information(this, "Profile", "Profile dialog comes in step 6.");
}