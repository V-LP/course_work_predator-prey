// plant.h
#ifndef PLANT_H
#define PLANT_H

#include "entity.h"
// QList та QSet вже включені в entity.h

class Plant : public Entity {
public:
    Plant(int x, int y);

    // ВИПРАВЛЕНО: Змінено тип deadEntities
    void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) override;

private:
    int spreadCooldown;
};

#endif // PLANT_H
