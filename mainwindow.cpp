// mainwindow.cpp
#include "mainwindow.h"
#include "simulationview.h"
#include "world.h"
#include "config.h"
#include "setupdialog.h" // Включаємо наш новий діалог
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(nullptr),
    scene(nullptr),
    world(nullptr),
    m_gridSize(0)
{
    setupUiManual();

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::updateSimulation);

    // ВИПРАВЛЕННЯ 2: З'єднуємо сигнал з видом (view) зі слотом у вікні
    connect(simView, &SimulationView::userMadeChange, this, &MainWindow::onUserInteraction);

    startButton->setText("New Simulation");
    pauseButton->setEnabled(false);
    resetButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    clearSimulation();
    delete ui;
}

void MainWindow::clearSimulation()
{
    if(simulationTimer->isActive()) {
        simulationTimer->stop();
    }
    if (simView) {
        simView->setScene(nullptr);
    }
    delete world;
    world = nullptr;
    delete scene;
    scene = nullptr;
}


void MainWindow::setupUiManual() {
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    this->setWindowTitle("Predators and Prey Simulation");

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    simView = new SimulationView(this);
    simView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(simView);

    QGroupBox *controlsGroup = new QGroupBox("Controls & Statistics", this);
    QVBoxLayout *controlsLayout = new QVBoxLayout(controlsGroup);

    QHBoxLayout *statsLayout = new QHBoxLayout();
    turnLabel = new QLabel("Turn: -", this);
    plantsLabel = new QLabel("Plants: -", this);
    herbivoresLabel = new QLabel("Herbivores: -", this);
    predatorsLabel = new QLabel("Predators: -", this);
    statsLayout->addWidget(turnLabel);
    statsLayout->addWidget(plantsLabel);
    statsLayout->addWidget(herbivoresLabel);
    statsLayout->addWidget(predatorsLabel);
    controlsLayout->addLayout(statsLayout);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    startButton = new QPushButton("Start", this);
    pauseButton = new QPushButton("Pause", this);
    resetButton = new QPushButton("Reset", this);
    buttonsLayout->addWidget(startButton);
    buttonsLayout->addWidget(pauseButton);
    buttonsLayout->addWidget(resetButton);
    controlsLayout->addLayout(buttonsLayout);

    mainLayout->addWidget(controlsGroup);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::on_pauseButton_clicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
}

void MainWindow::setupNewSimulation() {
    clearSimulation(); // Очищаємо попередню симуляцію, якщо вона була

    // Створюємо та показуємо діалог налаштувань
    SetupDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        // Отримуємо параметри від користувача
        m_gridSize = dialog.getGridSize();
        m_initialPlants = dialog.getInitialPlants();
        m_initialHerbivores = dialog.getInitialHerbivores();
        m_initialPredators = dialog.getInitialPredators();

        // Створюємо нову сцену та світ
        scene = new QGraphicsScene(this);
        scene->setSceneRect(0, 0, m_gridSize * CELL_SIZE_PX, m_gridSize * CELL_SIZE_PX);
        simView->setMinimumSize(m_gridSize * CELL_SIZE_PX + 2, m_gridSize * CELL_SIZE_PX + 2);

        world = new World(m_gridSize, scene, this);

        simView->setScene(scene);
        simView->setWorld(world);

        connect(world, &World::statisticsUpdated, this, &MainWindow::updateStatisticsDisplay);

        world->initializePopulation(m_initialPlants, m_initialHerbivores, m_initialPredators);

        // Готово до старту
        startButton->setText("Start");
        startButton->setEnabled(true);
        pauseButton->setEnabled(false);
        resetButton->setEnabled(false); // Скидати можна буде після паузи
    }
}


void MainWindow::on_startButton_clicked() {
    if (!world) { // Якщо симуляція ще не створена
        setupNewSimulation();
        // Не запускаємо таймер одразу, даємо користувачу натиснути "Start" ще раз
        return;
    }

    simulationTimer->start(SIMULATION_TIMER_MS);
    startButton->setText("Start");
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
    if (world) {
        // Використовуємо збережені параметри для перезапуску
        world->initializePopulation(m_initialPlants, m_initialHerbivores, m_initialPredators);
    }
    if(scene) {
        scene->update();
    }
    if (simView && simView->viewport()) {
        simView->viewport()->update();
    }

    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(false);
    if (world) {
        updateStatisticsDisplay(0, world->getPlantCount(), world->getHerbivoreCount(), world->getPredatorCount());
    } else {
        updateStatisticsDisplay(0, 0, 0, 0);
    }
}

void MainWindow::updateSimulation() {
    if (world) {
        world->tick();
    }
}

void MainWindow::updateStatisticsDisplay(int turn, int plants, int herbivores, int predators) {
    turnLabel->setText(QString("Turn: %1").arg(turn));
    plantsLabel->setText(QString("Plants: %1").arg(plants));
    herbivoresLabel->setText(QString("Herbivores: %1").arg(herbivores));
    predatorsLabel->setText(QString("Predators: %1").arg(predators));
}

void MainWindow::onUserInteraction() {
    // Цей слот викликається, коли користувач додає/видаляє істоту.
    // Якщо симуляція на паузі, ми вручну викликаємо tick(), щоб обробити
    // зміни, і відразу зменшуємо лічильник ходів, щоб це не рахувалося за хід.
    if (world && !simulationTimer->isActive()) {
        world->tick();
        updateStatisticsDisplay(world->getCurrentTurn(), world->getPlantCount(), world->getHerbivoreCount(), world->getPredatorCount());
    }
}
