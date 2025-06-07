// mainwindow.cpp
#include "mainwindow.h"
#include "simulationview.h"
#include "world.h"
#include "config.h"
#include "setupdialog.h"
#include "graphwidget.h"

#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSizePolicy>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(nullptr),
    scene(nullptr),
    world(nullptr),
    m_gridSize(0),
    graphWidget(nullptr)
{
    setupUiManual();

    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &MainWindow::updateSimulation);

    connect(simView, &SimulationView::userMadeChange, this, &MainWindow::onUserInteraction);

    // Початковий стан кнопок
    startButton->setText("New Simulation");
    pauseButton->setEnabled(false);
    resetButton->setEnabled(false);
    saveButton->setEnabled(false);
    loadButton->setEnabled(true);
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

    if (graphWidget) {
        graphWidget->clearData();
    }
    m_statsHistory.clear();
}


void MainWindow::setupUiManual() {
    QWidget *centralWidget = new QWidget(this);
    this->setCentralWidget(centralWidget);
    this->setWindowTitle("Predators and Prey Simulation");

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    simView = new SimulationView(this);
    simView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    leftLayout->addWidget(simView);

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
    saveButton = new QPushButton("Save Log", this);
    loadButton = new QPushButton("Load Log", this); // Створюємо кнопку
    buttonsLayout->addWidget(startButton);
    buttonsLayout->addWidget(pauseButton);
    buttonsLayout->addWidget(resetButton);
    buttonsLayout->addWidget(saveButton);
    buttonsLayout->addWidget(loadButton); // Додаємо на лейаут
    controlsLayout->addLayout(buttonsLayout);

    leftLayout->addWidget(controlsGroup);
    mainLayout->addLayout(leftLayout, 2);

    graphWidget = new GraphWidget(this);
    mainLayout->addWidget(graphWidget, 1);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::on_pauseButton_clicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::on_resetButton_clicked);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::on_saveButton_clicked);
    connect(loadButton, &QPushButton::clicked, this, &MainWindow::on_loadButton_clicked);
}

void MainWindow::setupNewSimulation() {
    clearSimulation();
    SetupDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        m_gridSize = dialog.getGridSize();
        m_initialPlants = dialog.getInitialPlants();
        m_initialHerbivores = dialog.getInitialHerbivores();
        m_initialPredators = dialog.getInitialPredators();
        scene = new QGraphicsScene(this);
        scene->setSceneRect(0, 0, m_gridSize * CELL_SIZE_PX, m_gridSize * CELL_SIZE_PX);
        simView->setMinimumSize(m_gridSize * CELL_SIZE_PX + 2, m_gridSize * CELL_SIZE_PX + 2);
        world = new World(m_gridSize, scene, this);
        simView->setScene(scene);
        simView->setWorld(world);
        connect(world, &World::statisticsUpdated, this, &MainWindow::updateStatisticsDisplay);
        world->initializePopulation(m_initialPlants, m_initialHerbivores, m_initialPredators);
        updateStatisticsDisplay(0, m_initialPlants, m_initialHerbivores, m_initialPredators);
        startButton->setText("Start");
        startButton->setEnabled(true);
        pauseButton->setEnabled(false);
        resetButton->setEnabled(false);
        saveButton->setEnabled(true);
        loadButton->setEnabled(true);
    }
}

void MainWindow::on_startButton_clicked() {
    if (!world) {
        setupNewSimulation();
        return;
    }
    simulationTimer->start(SIMULATION_TIMER_MS);
    startButton->setEnabled(false);
    pauseButton->setEnabled(true);
    resetButton->setEnabled(false);
    loadButton->setEnabled(false); // Не можна завантажувати під час симуляції
}

void MainWindow::on_pauseButton_clicked() {
    simulationTimer->stop();
    startButton->setEnabled(true);
    pauseButton->setEnabled(false);
    resetButton->setEnabled(true);
    loadButton->setEnabled(true); // Можна завантажити, коли на паузі
}

void MainWindow::on_resetButton_clicked() {
    simulationTimer->stop();
    setupNewSimulation();
}

void MainWindow::on_saveButton_clicked()
{
    if (m_statsHistory.isEmpty()) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, "Save Simulation Log", "", "CSV Files (*.csv);;Text Files (*.txt)");
    if (fileName.isEmpty()) {
        return;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);
    out << "Turn,Plants,Herbivores,Predators\n";
    for (const auto& statLine : m_statsHistory) {
        out << statLine[0] << "," << statLine[1] << "," << statLine[2] << "," << statLine[3] << "\n";
    }
    file.close();
}

// Новий метод для завантаження
void MainWindow::on_loadButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Load Simulation Log", "", "CSV Files (*.csv);;Text Files (*.txt)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open the file for reading.");
        return;
    }

    clearSimulation(); // Очищуємо поточний стан

    QTextStream in(&file);
    // Пропускаємо заголовок
    if (!in.atEnd()) {
        in.readLine();
    }

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList parts = line.split(',');
        if (parts.size() == 4) {
            bool ok1, ok2, ok3, ok4;
            int turn = parts[0].toInt(&ok1);
            int plants = parts[1].toInt(&ok2);
            int herbivores = parts[2].toInt(&ok3);
            int predators = parts[3].toInt(&ok4);
            if (ok1 && ok2 && ok3 && ok4) {
                // Додаємо дані до історії та на графік
                m_statsHistory.append({turn, plants, herbivores, predators});
                graphWidget->addDataPoint(turn, plants, herbivores, predators);
            }
        }
    }
    file.close();

    // Оновлюємо UI, щоб показати стан останнього запису
    if (!m_statsHistory.isEmpty()) {
        const auto& lastStat = m_statsHistory.last();
        updateStatisticsDisplay(lastStat[0], lastStat[1], lastStat[2], lastStat[3]);
        QMessageBox::information(this, "Success", "Log file loaded successfully.");
    }

    // Блокуємо кнопки симуляції, оскільки ми в режимі перегляду логу
    startButton->setText("New Simulation");
    startButton->setEnabled(true); // Залишаємо можливість почати нову симуляцію
    pauseButton->setEnabled(false);
    resetButton->setEnabled(false);
    saveButton->setEnabled(true); // Дозволяємо перезберегти лог
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

    if (world) {
        if (m_statsHistory.isEmpty() || m_statsHistory.last().first() != turn) {
            m_statsHistory.append({turn, plants, herbivores, predators});
            if (graphWidget) {
                graphWidget->addDataPoint(turn, plants, herbivores, predators);
            }
        }
    }
}

void MainWindow::onUserInteraction() {
    if (world && !simulationTimer->isActive()) {
        world->tick();
    }
}
