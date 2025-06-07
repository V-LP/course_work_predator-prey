// graphwidget.h
#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QList>
#include <QPointF>
#include <QColor>

class GraphWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(QWidget *parent = nullptr);

public slots:
    void addDataPoint(int turn, int plants, int herbivores, int predators);
    void clearData();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void updateRanges();

    QList<QPointF> plantData;
    QList<QPointF> herbivoreData;
    QList<QPointF> predatorData;

    qreal maxTurn;
    qreal maxPopulation;
};

#endif // GRAPHWIDGET_H
