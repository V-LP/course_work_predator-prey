// livingbeing.h
#ifndef LIVINGBEING_H
#define LIVINGBEING_H

#include "entity.h" // entity.h вже включає QList та QSet

class LivingBeing : public Entity {
public:
    LivingBeing(EntityType type, int x, int y, QColor color, int initialHunger);

    // ВИПРАВЛЕНО: Змінено тип deadEntities
    virtual void tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) override = 0;

    int getHunger() const;
    void decreaseHunger(int amount);
    void increaseHunger(int amount);
    bool isStarving() const;

protected:
    void tryReproduce(World& world, QList<Entity*>& newEntities);
    virtual Entity* createOffspring(int x, int y) = 0;

    int hunger;
    int maxHunger;
    int foodEaten;
    int reproductionCooldown;
};

#endif // LIVINGBEING_H
