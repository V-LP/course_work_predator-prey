// graphwidget.cpp
#include "graphwidget.h"
#include <QPainter>
#include <QPen>

GraphWidget::GraphWidget(QWidget *parent)
    : QWidget(parent), maxTurn(10), maxPopulation(10)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumHeight(150);
    // ВИДАЛЕНО: setBackgroundRole(QPalette::Base);
    // ВИДАЛЕНО: setAutoFillBackground(true);
}

void GraphWidget::addDataPoint(int turn, int plants, int herbivores, int predators)
{
    plantData.append(QPointF(turn, plants));
    herbivoreData.append(QPointF(turn, herbivores));
    predatorData.append(QPointF(turn, predators));
    updateRanges();
    update();
}

void GraphWidget::clearData()
{
    plantData.clear();
    herbivoreData.clear();
    predatorData.clear();
    maxTurn = 10;
    maxPopulation = 10;
    update();
}

void GraphWidget::updateRanges()
{
    maxTurn = 0;
    maxPopulation = 0;

    auto findMax = [&](const QList<QPointF>& data) {
        for (const auto& p : data) {
            if (p.x() > maxTurn) maxTurn = p.x();
            if (p.y() > maxPopulation) maxPopulation = p.y();
        }
    };

    findMax(plantData);
    findMax(herbivoreData);
    findMax(predatorData);

    if (maxTurn < 10) maxTurn = 10;
    if (maxPopulation < 10) maxPopulation = 10;
}

void GraphWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ВИПРАВЛЕНО: Явно замальовуємо фон білим кольором
    painter.fillRect(rect(), Qt::white);

    const int PADDING_LEFT = 50;
    const int PADDING_RIGHT = 20;
    const int PADDING_TOP = 20;
    const int PADDING_BOTTOM = 30;

    int graphWidth = width() - PADDING_LEFT - PADDING_RIGHT;
    int graphHeight = height() - PADDING_TOP - PADDING_BOTTOM;
    QPoint origin(PADDING_LEFT, height() - PADDING_BOTTOM);

    painter.setPen(Qt::black);
    painter.drawLine(origin, QPoint(width() - PADDING_RIGHT, origin.y()));
    painter.drawLine(origin, QPoint(origin.x(), PADDING_TOP));

    int numYTicks = 5;
    if (numYTicks > 0) {
        for (int i = 0; i <= numYTicks; ++i) {
            int y = origin.y() - (i * graphHeight / numYTicks);
            painter.drawLine(origin.x() - 5, y, origin.x(), y);
            double labelValue = i * maxPopulation / numYTicks;
            painter.drawText(QRect(0, y - 10, PADDING_LEFT - 7, 20), Qt::AlignRight, QString::number(labelValue, 'f', 0));
        }
    }

    int numXTicks = 5;
    if (numXTicks > 0 && maxTurn > 0) {
        for (int i = 0; i <= numXTicks; ++i) {
            int x = origin.x() + (i * graphWidth / numXTicks);
            painter.drawLine(x, origin.y(), x, origin.y() + 5);
            double labelValue = i * maxTurn / numXTicks;
            painter.drawText(QRect(x - 20, origin.y() + 7, 40, 20), Qt::AlignCenter, QString::number(labelValue, 'f', 0));
        }
    }

    painter.translate(origin);
    qreal scaleX = (maxTurn == 0) ? 1.0 : graphWidth / maxTurn;
    qreal scaleY = (maxPopulation == 0) ? -1.0 : -graphHeight / maxPopulation;

    auto transformPoints = [&](const QList<QPointF>& data) {
        QPolygonF polyline;
        for (const auto& point : data) {
            polyline << QPointF(point.x() * scaleX, point.y() * scaleY);
        }
        return polyline;
    };

    QPen pen;
    pen.setWidth(2);

    pen.setColor(QColor(34, 139, 34));
    painter.setPen(pen);
    if (plantData.size() > 1) painter.drawPolyline(transformPoints(plantData));

    pen.setColor(QColor(0, 0, 205));
    painter.setPen(pen);
    if (herbivoreData.size() > 1) painter.drawPolyline(transformPoints(herbivoreData));

    pen.setColor(QColor(220, 20, 60));
    painter.setPen(pen);
    if (predatorData.size() > 1) painter.drawPolyline(transformPoints(predatorData));

    painter.resetTransform();
    int legendX = width() - 120;
    int legendY = PADDING_TOP + 10;
    painter.setPen(Qt::black);

    painter.fillRect(legendX, legendY, 15, 10, QColor(34, 139, 34));
    painter.drawText(legendX + 20, legendY + 10, "Plants");

    painter.fillRect(legendX, legendY + 20, 15, 10, QColor(0, 0, 205));
    painter.drawText(legendX + 20, legendY + 30, "Herbivores");

    painter.fillRect(legendX, legendY + 40, 15, 10, QColor(220, 20, 60));
    painter.drawText(legendX + 20, legendY + 50, "Predators");
}
