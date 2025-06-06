// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QLabel>

// Forward declarations
class World;
class SimulationView;
class QGraphicsScene;
class SetupDialog;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_pauseButton_clicked();
    void on_resetButton_clicked();

    void updateSimulation();
    void updateStatisticsDisplay(int turn, int plants, int herbivores, int predators);
    void onUserInteraction();

private:
    void setupUiManual();
    void setupNewSimulation(); // Нова функція для налаштування симуляції
    void clearSimulation(); // Функція для очищення

    Ui::MainWindow *ui;

    SimulationView *simView;
    QGraphicsScene *scene;
    World *world;
    QTimer *simulationTimer;

    // Зберігаємо параметри симуляції для скидання
    int m_gridSize;
    int m_initialPlants;
    int m_initialHerbivores;
    int m_initialPredators;

    // UI elements
    QLabel *turnLabel;
    QLabel *plantsLabel;
    QLabel *herbivoresLabel;
    QLabel *predatorsLabel;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *resetButton;
};

#endif // MAINWINDOW_H
