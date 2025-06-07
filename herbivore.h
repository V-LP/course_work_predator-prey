// herbivore.h
#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "livingbeing.h"

class Herbivore : public LivingBeing {
public:
    Herbivore(int x, int y);

    void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) override;

protected:
    Entity* createOffspring(int x, int y) override;

private:
    void move(World& world);
    void eat(World& world, QSet<Entity*>& deadEntities);

    int speed;
    int visionRange;
};

#endif // HERBIVORE_H
