// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer> // Вже було, добре

// ВИПРАВЛЕНО: Додано необхідні заголовки для QPushButton та QLabel
#include <QPushButton>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class World;
class SimulationView;
class QGraphicsScene;
// class QLabel; // Тепер включено вище
// class QPushButton; // Тепер включено вище

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

private:
    Ui::MainWindow *ui; // If using .ui file, otherwise manage layouts manually

    SimulationView *simView;
    QGraphicsScene *scene;
    World *world;
    QTimer *simulationTimer;

    // UI elements for statistics (if not using .ui file)
    QLabel *turnLabel;
    QLabel *plantsLabel;
    QLabel *herbivoresLabel;
    QLabel *predatorsLabel;
    QPushButton *startButton;
    QPushButton *pauseButton; // Тепер тип QPushButton відомий
    QPushButton *resetButton;


    void setupUiManual(); // Call this if not using .ui file
};

#endif // MAINWINDOW_H
