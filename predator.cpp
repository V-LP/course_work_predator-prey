// predator.cpp
#include "predator.h"
#include "world.h"
#include "herbivore.h" // Потрібен для логіки руху та поїдання
#include "config.h"
#include <QRandomGenerator>
#include <QtMath> // For qAbs

Predator::Predator(int x, int y)
    : LivingBeing(EntityType::PREDATOR, x, y, Qt::red, PREDATOR_INITIAL_HUNGER),
    speed(PREDATOR_SPEED), visionRange(PREDATOR_VISION_RANGE) {
}

void Predator::tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) {
    if (isMarkedForDeath()) return;

    decreaseHunger(HUNGER_DECREASE_PER_TURN);
    if (isStarving()) {
        deadEntities.insert(this);
        markForDeath();
        return;
    }

    move(world);
    eat(world, deadEntities);
    if (isMarkedForDeath()) return;

    tryReproduce(world, newEntities);
}

void Predator::move(World& world) {
    Herbivore* nearestHerbivore = nullptr;
    int minDistToHerbivore = visionRange + 1;

    for (Entity* entity : world.getEntitiesInRadius(gridX, gridY, visionRange)) {
        if (entity == this || entity->isMarkedForDeath()) continue;
        if (auto herbivore = dynamic_cast<Herbivore*>(entity)) {
            if (!herbivore->isMarkedForDeath()){
                int dist = qAbs(herbivore->getGridX() - gridX) + qAbs(herbivore->getGridY() - gridY);
                if (dist < minDistToHerbivore) {
                    minDistToHerbivore = dist;
                    nearestHerbivore = herbivore;
                }
            }
        }
    }

    int targetX = gridX;
    int targetY = gridY;

    if (nearestHerbivore) {
        if (nearestHerbivore->getGridX() < gridX) targetX -= speed;
        else if (nearestHerbivore->getGridX() > gridX) targetX += speed;
        if (nearestHerbivore->getGridY() < gridY) targetY -= speed;
        else if (nearestHerbivore->getGridY() > gridY) targetY += speed;
    } else {
        if (QRandomGenerator::global()->bounded(2) == 0) {
            targetX += (QRandomGenerator::global()->bounded(3) - 1) * speed;
        } else {
            targetY += (QRandomGenerator::global()->bounded(3) - 1) * speed;
        }
    }

    targetX = qBound(0, targetX, GRID_SIZE_N - 1);
    targetY = qBound(0, targetY, GRID_SIZE_N - 1);

    if (world.isValidPosition(targetX, targetY)) {
        Entity* entityAtTarget = world.getEntityAt(targetX, targetY);
        if (entityAtTarget == nullptr || dynamic_cast<Herbivore*>(entityAtTarget) || entityAtTarget == this) {
            setGridPos(targetX, targetY);
        } else {
            int randX = gridX + (QRandomGenerator::global()->bounded(3) - 1) * speed;
            int randY = gridY + (QRandomGenerator::global()->bounded(3) - 1) * speed;
            randX = qBound(0, randX, GRID_SIZE_N - 1);
            randY = qBound(0, randY, GRID_SIZE_N - 1);
            if (world.isValidPosition(randX, randY) && (world.getEntityAt(randX, randY) == nullptr || dynamic_cast<Herbivore*>(world.getEntityAt(randX, randY)) )) {
                setGridPos(randX, randY);
            }
        }
    }
}

void Predator::eat(World& world, QSet<Entity*>& deadEntities) {
    Entity* entityAtPos = world.getEntityAt(gridX, gridY);
    if (auto herbivore = dynamic_cast<Herbivore*>(entityAtPos)) {
        // ВИПРАВЛЕНО: Видалено 'herbivore != this'
        // dynamic_cast вже гарантує, що 'herbivore' не є 'this' (Predator),
        // оскільки Predator не може бути успішно перетворений на Herbivore.
        if (!herbivore->isMarkedForDeath()) {
            increaseHunger(PREDATOR_EAT_HERBIVORE_HUNGER_GAIN);
            foodEaten++;
            deadEntities.insert(herbivore);
            herbivore->markForDeath();
            world.removeEntityFromCellCache(herbivore->getGridX(), herbivore->getGridY());
        }
    }
}

Entity* Predator::createOffspring(int x, int y) {
    return new Predator(x, y);
}
