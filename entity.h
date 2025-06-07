// entity.h
#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QGraphicsRectItem>
#include "entitytype.h"
#include <QList>
#include <QSet>

class World; // Forward declaration

class Entity : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    Entity(EntityType type, int x, int y, QColor color, QGraphicsItem *parent = nullptr);
    virtual ~Entity() = default;

    virtual void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) = 0;

    EntityType getEntityType() const;
    int getGridX() const;
    int getGridY() const;
    void setGridPos(int x, int y);

    bool isMarkedForDeath() const;
    void markForDeath();


protected:
    EntityType entityType;
    int gridX;
    int gridY;
    bool markedForDeath;
};

#endif // ENTITY_H
