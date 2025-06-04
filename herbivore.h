// herbivore.h
#ifndef HERBIVORE_H
#define HERBIVORE_H

#include "livingbeing.h" // livingbeing.h -> entity.h (включає QList та QSet)

class Herbivore : public LivingBeing {
public:
    Herbivore(int x, int y);

    // ВИПРАВЛЕНО: Змінено тип deadEntities
    void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) override;

protected:
    Entity* createOffspring(int x, int y) override;

private:
    // ВИПРАВЛЕНО: Змінено тип deadEntities для внутрішнього методу eat
    void move(World& world);
    void eat(World& world, QSet<Entity*>& deadEntities);

    int speed;
    int visionRange;
};

#endif // HERBIVORE_H
