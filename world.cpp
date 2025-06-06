// world.cpp
#include "world.h"
#include "entity.h"
#include "plant.h"
#include "herbivore.h"
#include "predator.h"
#include "config.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QMutableListIterator>
#include <QtMath> // For qAbs

// Helper для QHash<QPoint, Entity*>
inline uint qHash(const QPoint &key, uint seed = 0) {
    return qHash(QPair<int, int>(key.x(), key.y()), seed);
}

World::World(int size, QGraphicsScene* scene, QObject *parent)
    : QObject(parent), sizeN(size), graphicsScene(scene), currentTurn(0),
    plantCount(0), herbivoreCount(0), predatorCount(0) {
}

World::~World() {
    // Елементи будуть видалені через qDeleteAll, сцена не повинна ними володіти
    qDeleteAll(allEntities);
    allEntities.clear();
}

void World::initializePopulation(int numPlants, int numHerbivores, int numPredators) {
    // ВИПРАВЛЕННЯ 1: Правильне очищення перед перезапуском
    // Замість graphicsScene->clear(), що призводило до подвійного видалення,
    // ми тепер вручну видаляємо об'єкти, а сцена просто забуває про них.
    qDeleteAll(allEntities);
    allEntities.clear();
    entityGridCache.clear();
    scheduledAdditions.clear();
    scheduledForRemoval.clear();
    if(graphicsScene) {
        // Очищаємо сцену від вказівників, не видаляючи об'єкти (їх ми вже видалили)
        graphicsScene->clear();
    }
    currentTurn = 0;

    for (int i = 0; i < numPlants; ++i) {
        addEntityImmediately(new Plant(findEmptyRandomPosition().x(), findEmptyRandomPosition().y()));
    }
    for (int i = 0; i < numHerbivores; ++i) {
        addEntityImmediately(new Herbivore(findEmptyRandomPosition().x(), findEmptyRandomPosition().y()));
    }
    for (int i = 0; i < numPredators; ++i) {
        addEntityImmediately(new Predator(findEmptyRandomPosition().x(), findEmptyRandomPosition().y()));
    }
    updateCounts();
    emit statisticsUpdated(currentTurn, plantCount, herbivoreCount, predatorCount);
}

void World::scheduleAddEntity(EntityType type, int x, int y) {
    scheduledAdditions.append({type, QPoint(x, y)});
}

void World::scheduleRemoveEntity(Entity* entity) {
    if (entity && !entity->isMarkedForDeath()) {
        entity->markForDeath();
        scheduledForRemoval.insert(entity);
    }
}

void World::tick() {
    currentTurn++;
    QList<Entity*> newEntitiesThisTurn;
    QSet<Entity*> deadEntitiesThisTurn;

    // Обробка запланованих дій (додавання/видалення з UI)
    processScheduledActions(deadEntitiesThisTurn, newEntitiesThisTurn);

    updateEntityGridCache();

    // ... (логіка тіку для хижаків, травоїдних, рослин) ...
    QList<Predator*> predators_list;
    QList<Herbivore*> herbivores_list;
    QList<Plant*> plants_list;

    for(Entity* e : allEntities) {
        if(e->isMarkedForDeath()) continue;
        if(auto pred = dynamic_cast<Predator*>(e)) predators_list.append(pred);
        else if(auto herb = dynamic_cast<Herbivore*>(e)) herbivores_list.append(herb);
        else if(auto plant_ptr = dynamic_cast<Plant*>(e)) plants_list.append(plant_ptr);
    }
    for (Predator* predator : predators_list) {
        if (predator->isMarkedForDeath() || deadEntitiesThisTurn.contains(predator)) continue;
        predator->tick(*this, newEntitiesThisTurn, deadEntitiesThisTurn);
    }
    for (Herbivore* herbivore : herbivores_list) {
        if (herbivore->isMarkedForDeath() || deadEntitiesThisTurn.contains(herbivore)) continue;
        herbivore->tick(*this, newEntitiesThisTurn, deadEntitiesThisTurn);
    }
    for (Plant* plant : plants_list) {
        if (plant->isMarkedForDeath() || deadEntitiesThisTurn.contains(plant)) continue;
        plant->tick(*this, newEntitiesThisTurn, deadEntitiesThisTurn);
    }


    if (!deadEntitiesThisTurn.isEmpty()) {
        QMutableListIterator<Entity*> it(allEntities);
        while (it.hasNext()) {
            Entity* entity = it.next();
            if (deadEntitiesThisTurn.contains(entity)) {
                it.remove();
                removeEntityImmediately(entity);
            }
        }
    }

    for (Entity* entity : newEntitiesThisTurn) {
        if (getEntityAt(entity->getGridX(), entity->getGridY()) == nullptr) {
            addEntityImmediately(entity);
        } else {
            delete entity;
        }
    }

    updateCounts();
    emit statisticsUpdated(currentTurn, plantCount, herbivoreCount, predatorCount);
}

void World::processScheduledActions(QSet<Entity*>& deadEntities, QList<Entity*>& newEntities) {
    deadEntities.unite(scheduledForRemoval);
    scheduledForRemoval.clear();

    for (const auto& addition : scheduledAdditions) {
        if (isValidPosition(addition.second.x(), addition.second.y()) && getEntityAt(addition.second.x(), addition.second.y()) == nullptr) {
            Entity* newEntity = nullptr;
            switch (addition.first) {
            case EntityType::PLANT:     newEntity = new Plant(addition.second.x(), addition.second.y()); break;
            case EntityType::HERBIVORE: newEntity = new Herbivore(addition.second.x(), addition.second.y()); break;
            case EntityType::PREDATOR:  newEntity = new Predator(addition.second.x(), addition.second.y()); break;
            }
            if (newEntity) {
                newEntities.append(newEntity);
            }
        }
    }
    scheduledAdditions.clear();
}

void World::addEntityImmediately(Entity* entity, bool addToScene) {
    if (!entity) return;
    allEntities.append(entity);
    entityGridCache.insert(QPoint(entity->getGridX(), entity->getGridY()), entity);
    if (addToScene && graphicsScene) {
        graphicsScene->addItem(entity);
    }
}

void World::removeEntityImmediately(Entity* entity) {
    if (!entity) return;
    entityGridCache.remove(QPoint(entity->getGridX(), entity->getGridY()));
    if (graphicsScene) {
        graphicsScene->removeItem(entity);
    }
    delete entity;
}

void World::removeEntityFromCellCache(int x, int y) {
    entityGridCache.remove(QPoint(x,y));
}

bool World::isValidPosition(int x, int y) const {
    return x >= 0 && x < sizeN && y >= 0 && y < sizeN;
}

Entity* World::getEntityAt(int x, int y) const {
    return entityGridCache.value(QPoint(x,y), nullptr);
}

QList<Entity*> World::getEntitiesInRadius(int centerX, int centerY, int radius) const {
    QList<Entity*> foundEntities;
    for (int i = centerX - radius; i <= centerX + radius; ++i) {
        for (int j = centerY - radius; j <= centerY + radius; ++j) {
            if (i == centerX && j == centerY) continue;
            if (isValidPosition(i, j)) {
                Entity* entity = getEntityAt(i, j);
                if (entity) {
                    foundEntities.append(entity);
                }
            }
        }
    }
    return foundEntities;
}

QPoint World::findEmptyRandomPosition() const {
    if (allEntities.size() >= sizeN * sizeN) {
        return QPoint(-1, -1);
    }
    int x, y;
    int attempts = 0;
    const int maxAttempts = sizeN * sizeN * 2; // Запобіжник від нескінченного циклу
    do {
        x = QRandomGenerator::global()->bounded(sizeN);
        y = QRandomGenerator::global()->bounded(sizeN);
        if(attempts++ > maxAttempts) return QPoint(-1,-1); // Поле занадто заповнене
    } while (getEntityAt(x, y) != nullptr);
    return QPoint(x, y);
}

void World::updateEntityGridCache() {
    entityGridCache.clear();
    for (Entity* entity : allEntities) {
        if (!entity->isMarkedForDeath()) {
            entityGridCache.insert(QPoint(entity->getGridX(), entity->getGridY()), entity);
        }
    }
}

void World::updateCounts() {
    plantCount = 0;
    herbivoreCount = 0;
    predatorCount = 0;
    for (const Entity* entity : allEntities) {
        if (entity->isMarkedForDeath()) continue;
        switch (entity->getEntityType()) {
        case EntityType::PLANT: plantCount++; break;
        case EntityType::HERBIVORE: herbivoreCount++; break;
        case EntityType::PREDATOR: predatorCount++; break;
        }
    }
}

int World::getCurrentTurn() const { return currentTurn; }
int World::getPlantCount() const { return plantCount; }
int World::getHerbivoreCount() const { return herbivoreCount; }
int World::getPredatorCount() const { return predatorCount; }
