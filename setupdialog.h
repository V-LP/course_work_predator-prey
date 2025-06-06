#ifndef SETUPDIALOG_H
#define SETUPDIALOG_H

#include <QDialog>

// Forward declarations
class QSpinBox;
class QDialogButtonBox;

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog(QWidget *parent = nullptr);

    // Функції для отримання значень, введених користувачем
    int getGridSize() const;
    int getInitialPlants() const;
    int getInitialHerbivores() const;
    int getInitialPredators() const;

private:
    void setupUi();
    void updateMaxCreatureCount();

    QSpinBox *gridSizeSpinBox;
    QSpinBox *plantsSpinBox;
    QSpinBox *herbivoresSpinBox;
    QSpinBox *predatorsSpinBox;
    QDialogButtonBox *buttonBox;
};

#endif // SETUPDIALOG_H
