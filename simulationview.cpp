// simulationview.cpp
#include "simulationview.h"
#include "world.h"
#include "config.h"
#include "entity.h" // For EntityType and dynamic_cast
#include "plant.h"
#include "herbivore.h"
#include "predator.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QGraphicsScene>


SimulationView::SimulationView(QWidget *parent) : QGraphicsView(parent), currentWorld(nullptr) {
    setRenderHint(QPainter::Antialiasing, false); // For sharper grid lines
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing, true);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setBackgroundBrush(Qt::lightGray); // Background for area outside scene
}

void SimulationView::setWorld(World* world) {
    currentWorld = world;
}

void SimulationView::drawBackground(QPainter *painter, const QRectF &rect) {
    QGraphicsView::drawBackground(painter, rect); // Draw default background first

    // Draw grid lines
    painter->setPen(QPen(Qt::gray, 0)); // Thin gray lines
    qreal left = int(rect.left()) - (int(rect.left()) % CELL_SIZE_PX);
    qreal top = int(rect.top()) - (int(rect.top()) % CELL_SIZE_PX);

    for (qreal x = left; x < rect.right(); x += CELL_SIZE_PX) {
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    }
    for (qreal y = top; y < rect.bottom(); y += CELL_SIZE_PX) {
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
    }
}

void SimulationView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        // Store the position for the context menu
        // mapToScene gives coordinates relative to the scene's origin
        QPointF scenePoint = mapToScene(event->pos());
        int gridX = static_cast<int>(scenePoint.x() / CELL_SIZE_PX);
        int gridY = static_cast<int>(scenePoint.y() / CELL_SIZE_PX);

        // Clamp to grid boundaries
        gridX = qBound(0, gridX, GRID_SIZE_N - 1);
        gridY = qBound(0, gridY, GRID_SIZE_N - 1);
        lastRightClickScenePos = QPoint(gridX, gridY);

        // Let contextMenuEvent handle showing the menu
        QGraphicsView::mousePressEvent(event); // Pass event for other uses
        return; // Don't call contextMenuEvent directly
    }
    QGraphicsView::mousePressEvent(event);
}


void SimulationView::contextMenuEvent(QContextMenuEvent *event) {
    if (!currentWorld) return;

    QMenu menu(this);
    Entity* entityAtPos = currentWorld->getEntityAt(lastRightClickScenePos.x(), lastRightClickScenePos.y());

    if (entityAtPos) {
        QAction *removeAction = menu.addAction(QString("Remove %1").arg(
            entityAtPos->getEntityType() == EntityType::PLANT ? "Plant" :
                entityAtPos->getEntityType() == EntityType::HERBIVORE ? "Herbivore" : "Predator"
            ));
        connect(removeAction, &QAction::triggered, this, &SimulationView::removeEntityActionTriggered);
    } else {
        QAction *addPlantAction = menu.addAction("Add Plant");
        addPlantAction->setData(static_cast<int>(EntityType::PLANT));
        connect(addPlantAction, &QAction::triggered, this, &SimulationView::addEntityActionTriggered);

        QAction *addHerbivoreAction = menu.addAction("Add Herbivore");
        addHerbivoreAction->setData(static_cast<int>(EntityType::HERBIVORE));
        connect(addHerbivoreAction, &QAction::triggered, this, &SimulationView::addEntityActionTriggered);

        QAction *addPredatorAction = menu.addAction("Add Predator");
        addPredatorAction->setData(static_cast<int>(EntityType::PREDATOR));
        connect(addPredatorAction, &QAction::triggered, this, &SimulationView::addEntityActionTriggered);
    }
    menu.exec(event->globalPos());
}


void SimulationView::addEntityActionTriggered() {
    if (!currentWorld) return;

    QAction *action = qobject_cast<QAction*>(sender());
    if (!action) return;

    EntityType type = static_cast<EntityType>(action->data().toInt());
    int x = lastRightClickScenePos.x();
    int y = lastRightClickScenePos.y();

    currentWorld->scheduleAddEntity(type, x, y);
    emit userMadeChange();
}

void SimulationView::removeEntityActionTriggered() {
    if (!currentWorld) return;
    int x = lastRightClickScenePos.x();
    int y = lastRightClickScenePos.y();
    Entity* entityToRemove = currentWorld->getEntityAt(x, y);

    if (entityToRemove) {
        currentWorld->scheduleRemoveEntity(entityToRemove);
        emit userMadeChange();
    }
}
