// herbivore.cpp
#include "herbivore.h"
#include "world.h"
#include "plant.h"
#include "predator.h" // Потрібен для логіки руху
#include "config.h"
#include <QRandomGenerator>
#include <QtMath> // For qAbs

Herbivore::Herbivore(int x, int y)
    : LivingBeing(EntityType::HERBIVORE, x, y, Qt::blue, HERBIVORE_INITIAL_HUNGER),
    speed(HERBIVORE_SPEED), visionRange(HERBIVORE_VISION_RANGE) {
}

// ВИПРАВЛЕНО: Сигнатура і використання deadEntities.insert
void Herbivore::tick(World& world, QList<Entity*>& newEntities, QSet<Entity*>& deadEntities) {
    if (isMarkedForDeath()) return;

    decreaseHunger(HUNGER_DECREASE_PER_TURN);
    if (isStarving()) {
        deadEntities.insert(this); // ВИПРАВЛЕНО: insert замість append
        markForDeath();
        return;
    }

    move(world);
    eat(world, deadEntities); // Тепер передає QSet
    if (isMarkedForDeath()) return;

    tryReproduce(world, newEntities);
}

void Herbivore::move(World& world) {
    Predator* nearestPredator = nullptr;
    Plant* nearestPlant = nullptr;
    int minDistToPredator = visionRange + 1;
    int minDistToPlant = visionRange + 1;

    for (Entity* entity : world.getEntitiesInRadius(gridX, gridY, visionRange)) {
        if (entity == this || entity->isMarkedForDeath()) continue;

        if (auto predator = dynamic_cast<Predator*>(entity)) {
            int dist = qAbs(predator->getGridX() - gridX) + qAbs(predator->getGridY() - gridY);
            if (dist < minDistToPredator) {
                minDistToPredator = dist;
                nearestPredator = predator;
            }
        } else if (auto plant = dynamic_cast<Plant*>(entity)) {
            int dist = qAbs(plant->getGridX() - gridX) + qAbs(plant->getGridY() - gridY);
            if (dist < minDistToPlant) {
                minDistToPlant = dist;
                nearestPlant = plant;
            }
        }
    }

    int targetX = gridX;
    int targetY = gridY;

    if (nearestPredator) {
        if (nearestPredator->getGridX() < gridX) targetX += speed;
        else if (nearestPredator->getGridX() > gridX) targetX -= speed;
        if (nearestPredator->getGridY() < gridY) targetY += speed;
        else if (nearestPredator->getGridY() > gridY) targetY -= speed;
    } else if (nearestPlant) {
        if (nearestPlant->getGridX() < gridX) targetX -= speed;
        else if (nearestPlant->getGridX() > gridX) targetX += speed;
        if (nearestPlant->getGridY() < gridY) targetY -= speed;
        else if (nearestPlant->getGridY() > gridY) targetY += speed;
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
        if (entityAtTarget == nullptr || dynamic_cast<Plant*>(entityAtTarget) || entityAtTarget == this) {
            setGridPos(targetX, targetY);
        } else {
            int randX = gridX + (QRandomGenerator::global()->bounded(3) - 1) * speed;
            int randY = gridY + (QRandomGenerator::global()->bounded(3) - 1) * speed;
            randX = qBound(0, randX, GRID_SIZE_N - 1);
            randY = qBound(0, randY, GRID_SIZE_N - 1);
            if (world.isValidPosition(randX, randY) && (world.getEntityAt(randX, randY) == nullptr || dynamic_cast<Plant*>(world.getEntityAt(randX, randY)))) {
                setGridPos(randX, randY);
            }
        }
    }
}

// ВИПРАВЛЕНО: Сигнатура і використання deadEntities.insert
void Herbivore::eat(World& world, QSet<Entity*>& deadEntities) {
    Entity* entityAtPos = world.getEntityAt(gridX, gridY);
    if (auto plant = dynamic_cast<Plant*>(entityAtPos)) {
        if (!plant->isMarkedForDeath()) {
            increaseHunger(HERBIVORE_EAT_PLANT_HUNGER_GAIN);
            foodEaten++;
            deadEntities.insert(plant); // ВИПРАВЛЕНО: insert
            plant->markForDeath();
            world.removeEntityFromCellCache(plant->getGridX(), plant->getGridY());
        }
    }
}

Entity* Herbivore::createOffspring(int x, int y) {
    return new Herbivore(x, y);
}
