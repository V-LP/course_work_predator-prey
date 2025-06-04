// livingbeing.cpp
#include "livingbeing.h"
#include "world.h"
#include "config.h"
#include <QRandomGenerator>

LivingBeing::LivingBeing(EntityType type, int x, int y, QColor color, int initialHunger)
    : Entity(type, x, y, color), hunger(initialHunger), maxHunger(MAX_HUNGER), foodEaten(0), reproductionCooldown(REPRODUCTION_COOLDOWN_TURNS) {
}

int LivingBeing::getHunger() const {
    return hunger;
}

void LivingBeing::decreaseHunger(int amount) {
    hunger -= amount;
    if (hunger < 0) hunger = 0;
}

void LivingBeing::increaseHunger(int amount) {
    hunger += amount;
    if (hunger > maxHunger) hunger = maxHunger;
}

bool LivingBeing::isStarving() const {
    return hunger <= 0;
}

void LivingBeing::tryReproduce(World& world, QList<Entity*>& newEntities) {
    if (reproductionCooldown > 0) {
        reproductionCooldown--;
    }

    if (foodEaten >= FOOD_TO_REPRODUCE && reproductionCooldown <= 0) {
        QList<QPoint> emptyNeighbors;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;
                int newX = gridX + dx;
                int newY = gridY + dy;
                if (world.isValidPosition(newX, newY) && world.getEntityAt(newX, newY) == nullptr) {
                    emptyNeighbors.append(QPoint(newX, newY));
                }
            }
        }

        if (!emptyNeighbors.isEmpty()) {
            QPoint birthPos = emptyNeighbors.at(QRandomGenerator::global()->bounded(emptyNeighbors.size()));
            Entity* offspring = createOffspring(birthPos.x(), birthPos.y()); // Implemented by derived classes
            if (offspring) {
                newEntities.append(offspring);
                foodEaten = 0; // Reset food count for parent
                reproductionCooldown = REPRODUCTION_COOLDOWN_TURNS; // Reset cooldown
            }
        }
    }
}
