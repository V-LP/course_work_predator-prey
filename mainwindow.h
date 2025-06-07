// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QVector>

class World;
class SimulationView;
class QGraphicsScene;
class SetupDialog;
class GraphWidget;

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
    void on_saveButton_clicked();
    void on_loadButton_clicked(); // Новий слот

    void updateSimulation();
    void updateStatisticsDisplay(int turn, int plants, int herbivores, int predators);
    void onUserInteraction();

private:
    void setupUiManual();
    void setupNewSimulation();
    void clearSimulation();

    Ui::MainWindow *ui;
    SimulationView *simView;
    QGraphicsScene *scene;
    World *world;
    QTimer *simulationTimer;

    int m_gridSize;
    int m_initialPlants;
    int m_initialHerbivores;
    int m_initialPredators;

    QLabel *turnLabel;
    QLabel *plantsLabel;
    QLabel *herbivoresLabel;
    QLabel *predatorsLabel;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *resetButton;
    QPushButton *saveButton;
    QPushButton *loadButton; // Нова кнопка

    GraphWidget *graphWidget;
    QList<QVector<int>> m_statsHistory;
};

#endif // MAINWINDOW_H
