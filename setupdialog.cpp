#include "setupdialog.h"
#include "config.h" // Для значень за замовчуванням
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QLabel>

SetupDialog::SetupDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    setWindowTitle("Simulation Setup");

    // З'єднуємо зміну розміру сітки з оновленням максимальної кількості істот
    connect(gridSizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, &SetupDialog::updateMaxCreatureCount);
}

void SetupDialog::setupUi()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // Створення полів вводу
    gridSizeSpinBox = new QSpinBox(this);
    gridSizeSpinBox->setRange(10, 500); // Мінімальний та максимальний розмір поля
    gridSizeSpinBox->setValue(GRID_SIZE_N);
    gridSizeSpinBox->setSuffix(" x " + QString::number(gridSizeSpinBox->value()));
    connect(gridSizeSpinBox, &QSpinBox::valueChanged, [this](int value){
        gridSizeSpinBox->setSuffix(" x " + QString::number(value));
    });


    plantsSpinBox = new QSpinBox(this);
    plantsSpinBox->setRange(0, 10000);
    plantsSpinBox->setValue(INITIAL_PLANTS);

    herbivoresSpinBox = new QSpinBox(this);
    herbivoresSpinBox->setRange(0, 10000);
    herbivoresSpinBox->setValue(INITIAL_HERBIVORES);

    predatorsSpinBox = new QSpinBox(this);
    predatorsSpinBox->setRange(0, 10000);
    predatorsSpinBox->setValue(INITIAL_PREDATORS);

    // Додавання віджетів до форми
    formLayout->addRow("Grid Size (N x N):", gridSizeSpinBox);
    formLayout->addRow(new QLabel("Initial Population:", this));
    formLayout->addRow("Plants:", plantsSpinBox);
    formLayout->addRow("Herbivores:", herbivoresSpinBox);
    formLayout->addRow("Predators:", predatorsSpinBox);

    mainLayout->addLayout(formLayout);

    // Кнопки OK та Cancel
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);

    // Оновлюємо обмеження при створенні
    updateMaxCreatureCount();
}

void SetupDialog::updateMaxCreatureCount()
{
    int size = gridSizeSpinBox->value();
    int maxCreatures = size * size;
    plantsSpinBox->setMaximum(maxCreatures);
    herbivoresSpinBox->setMaximum(maxCreatures);
    predatorsSpinBox->setMaximum(maxCreatures);
}

// Реалізація функцій для отримання значень
int SetupDialog::getGridSize() const {
    return gridSizeSpinBox->value();
}

int SetupDialog::getInitialPlants() const {
    return plantsSpinBox->value();
}

int SetupDialog::getInitialHerbivores() const {
    return herbivoresSpinBox->value();
}

int SetupDialog::getInitialPredators() const {
    return predatorsSpinBox->value();
}
