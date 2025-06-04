// mainwindow.cpp
#include "mainwindow.h"
#include "simulationview.h"
#include "world.h"
#include "config.h" // For GRID_SIZE_N, CELL_SIZE_PX, SIMULATION_TIMER_MS
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUiManual(); // simView створюється тут

    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, GRID_SIZE_N * CELL_SIZE_PX, GRID_SIZE_N * CELL_SIZE_PX);
    simView->setScene(scene); // Встановлюємо сцену для simView

    // ВИПРАВЛЕНО: Спочатку створюємо world, потім передаємо його в simView
    world = new World(GRID_SIZE_N, scene, this);
    simView->setWorld(world); // Тепер передаємо ініціалізований world

    connect(world, &World::statisticsUpdated, this, &MainWindow::updateStatisticsDisplay);

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::updateSimulation);

    world->initializePopulation(); // Ініціалізація популяції

    // Початковий стан кнопок
    pauseButton->setEnabled(false);
    startButton->setEnabled(true);
}

MainWindow::~MainWindow() {
    // world та scene будуть видалені автоматично, якщо MainWindow є їхнім батьком
    // (що так і є, оскільки 'this' передається як батьківський об'єкт).
    // simulationTimer також.
}

void MainWindow::setupUiManual() {
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    this->setWindowTitle("Predators and Prey Simulation");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Simulation View
    simView = new SimulationView(this); // simView створюється тут
    simView->setMinimumSize(GRID_SIZE_N * CELL_SIZE_PX + 2, GRID_SIZE_N * CELL_SIZE_PX + 2); // +2 for border
    simView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(simView);

    // Controls and Statistics GroupBox
    QGroupBox *controlsGroup = new QGroupBox("Controls & Statistics", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);

    // Statistics Labels
    QHBoxLayout *statsLayout = new QHBoxLayout();
    turnLabel = new QLabel("Turn: 0", this);
    plantsLabel = new QLabel("Plants: 0", this);
    herbivoresLabel = new QLabel("Herbivores: 0", this);
    predatorsLabel = new QLabel("Predators: 0", this);
    statsLayout->addWidget(turnLabel);
    statsLayout->addWidget(plantsLabel);
    statsLayout->addWidget(herbivoresLabel);
    statsLayout->addWidget(predatorsLabel);
    controlsLayout->addLayout(statsLayout);

    // Control Buttons
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    startButton = new QPushButton("Start", this);
    pauseButton = new QPushButton("Pause", this);
    resetButton = new QPushButton("Reset", this);
    buttonsLayout->addWidget(startButton);
    buttonsLayout->addWidget(pauseButton);
    buttonsLayout->addWidget(resetButton);
    controlsLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(controlsGroup);

    // Connections for manually created buttons
    connect(startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::on_pauseButton_clicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
}


void MainWindow::on_startButton_clicked() {
    simulationTimer->start(SIMULATION_TIMER_MS);
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    resetButton->setEnabled(false);
}

void MainWindow::on_pauseButton_clicked() {
    simulationTimer->stop();
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(true);
}

void MainWindow::on_resetButton_clicked() {
    simulationTimer->stop();
    if (world) { // Перевірка, чи world існує
        world->initializePopulation();
    }
    if (scene) { // Перевірка, чи scene існує
        scene->update();
    }
    if (simView && simView->viewport()) { // Перевірка simView та viewport
        simView->viewport()->update();
    }


    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(true);
    if (world) { // Оновлення статистики, якщо world існує
        updateStatisticsDisplay(0, world->getPlantCount(), world->getHerbivoreCount(), world->getPredatorCount());
    } else { // Якщо world не існує, показуємо нулі
        updateStatisticsDisplay(0,0,0,0);
    }
}

void MainWindow::updateSimulation() {
    if (world) {
        world->tick();
        if (scene) { // Додаткова перевірка
            scene->update();
        }
    }
}

void MainWindow::updateStatisticsDisplay(int turn, int plants, int herbivores, int predators) {
    if (turnLabel) turnLabel->setText(QString("Turn: %1").arg(turn));
    if (plantsLabel) plantsLabel->setText(QString("Plants: %1").arg(plants));
    if (herbivoresLabel) herbivoresLabel->setText(QString("Herbivores: %1").arg(herbivores));
    if (predatorsLabel) predatorsLabel->setText(QString("Predators: %1").arg(predators));
}
