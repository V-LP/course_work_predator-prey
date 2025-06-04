// entity.cpp
#include "entity.h"
#include "config.h"
#include <QBrush> // Додано для QBrush

Entity::Entity(EntityType type, int x, int y, QColor color, QGraphicsItem *parent)
    : QGraphicsRectItem(0, 0, CELL_SIZE_PX, CELL_SIZE_PX, parent),
    entityType(type), gridX(x), gridY(y), markedForDeath(false) {
    setPos(gridX * CELL_SIZE_PX, gridY * CELL_SIZE_PX);
    setBrush(QBrush(color)); // ВИПРАВЛЕНО: Явне створення QBrush з QColor
}

EntityType Entity::getEntityType() const {
    return entityType;
}

int Entity::getGridX() const {
    return gridX;
}

int Entity::getGridY() const {
    return gridY;
}

void Entity::setGridPos(int x, int y) {
    gridX = x;
    gridY = y;
    setPos(gridX * CELL_SIZE_PX, gridY * CELL_SIZE_PX);
}

bool Entity::isMarkedForDeath() const {
    return markedForDeath;
}

void Entity::markForDeath() {
    markedForDeath = true;
}
