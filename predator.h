// predator.h
#ifndef PREDATOR_H
#define PREDATOR_H

#include "livingbeing.h"

class Predator : public LivingBeing {
public:
    Predator(int x, int y);

    void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) override;

protected:
    Entity* createOffspring(int x, int y) override;

private:
    void move(World& world);
    void eat(World& world, QSet<Entity*>& deadEntities);

    int speed;
    int visionRange;
};

#endif // PREDATOR_H
