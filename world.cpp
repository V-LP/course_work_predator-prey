// world.cpp
#include "world.h"
#include "entity.h"
#include "plant.h"
#include "herbivore.h"
#include "predator.h"
#include "config.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QSet> // For efficient removal tracking
#include <QtMath> // for qAbs

// Helper for QHash<QPoint, Entity*>
inline uint qHash(const QPoint &key, uint seed = 0) {
    return qHash(QPair<int, int>(key.x(), key.y()), seed);
}


World::World(int size, QGraphicsScene* scene, QObject *parent)
    : QObject(parent), sizeN(size), graphicsScene(scene), currentTurn(0),
    plantCount(0), herbivoreCount(0), predatorCount(0) {
}

World::~World() {
    qDeleteAll(allEntities);
    allEntities.clear();
}

void World::initializePopulation() {
    allEntities.clear();
    entityGridCache.clear(); // Clear cache
    currentTurn = 0;

    for (int i = 0; i < INITIAL_PLANTS; ++i) {
        QPoint pos = findEmptyRandomPosition();
        addEntity(new Plant(pos.x(), pos.y()));
    }
    for (int i = 0; i < INITIAL_HERBIVORES; ++i) {
        QPoint pos = findEmptyRandomPosition();
        addEntity(new Herbivore(pos.x(), pos.y()));
    }
    for (int i = 0; i < INITIAL_PREDATORS; ++i) {
        QPoint pos = findEmptyRandomPosition();
        addEntity(new Predator(pos.x(), pos.y()));
    }
    updateCounts();
    emit statisticsUpdated(currentTurn, plantCount, herbivoreCount, predatorCount);
}

QPoint World::findEmptyRandomPosition() const {
    int x, y;
    do {
        x = QRandomGenerator::global()->bounded(sizeN);
        y = QRandomGenerator::global()->bounded(sizeN);
    } while (getEntityAt(x, y) != nullptr);
    return QPoint(x, y);
}

void World::tick() {
    currentTurn++;
    QList<Entity*> newEntitiesThisTurn;
    QSet<Entity*> entitiesToRemoveThisTurn;

    updateEntityGridCache();

    QList<Predator*> predators_list; // Змінено ім'я змінної
    QList<Herbivore*> herbivores_list; // Змінено ім'я змінної
    QList<Plant*> plants_list; // Змінено ім'я змінної

    for(Entity* e : allEntities) {
        if(e->isMarkedForDeath()) continue;

        if(auto pred = dynamic_cast<Predator*>(e)) predators_list.append(pred); // Змінено 'p' на 'pred'
        else if(auto herb = dynamic_cast<Herbivore*>(e)) herbivores_list.append(herb); // Змінено 'h' на 'herb'
        else if(auto plant_ptr = dynamic_cast<Plant*>(e)) plants_list.append(plant_ptr); // Змінено 'p' на 'plant_ptr'
    }

    // --- PREDATOR PHASE ---
    for (Predator* predator : predators_list) {
        if (predator->isMarkedForDeath() || entitiesToRemoveThisTurn.contains(predator)) continue;
        // ВИПРАВЛЕНО: Передаємо QSet напряму
        predator->tick(*this, newEntitiesThisTurn, entitiesToRemoveThisTurn);
    }

    // --- HERBIVORE PHASE ---
    for (Herbivore* herbivore : herbivores_list) {
        if (herbivore->isMarkedForDeath() || entitiesToRemoveThisTurn.contains(herbivore)) continue;
        // ВИПРАВЛЕНО: Передаємо QSet напряму
        herbivore->tick(*this, newEntitiesThisTurn, entitiesToRemoveThisTurn);
    }

    // --- PLANT PHASE ---
    for (Plant* plant : plants_list) {
        if (plant->isMarkedForDeath() || entitiesToRemoveThisTurn.contains(plant)) continue;
        // ВИПРАВЛЕНО: Передаємо QSet напряму
        plant->tick(*this, newEntitiesThisTurn, entitiesToRemoveThisTurn);
    }

    // --- RESOLUTION PHASE ---
    QMutableListIterator<Entity*> it(allEntities);
    bool changed = false; // Ця змінна не використовувалася, можна видалити або використати для entityGridCache
    while (it.hasNext()) {
        Entity* entity = it.next();
        if (entity->isMarkedForDeath() || entitiesToRemoveThisTurn.contains(entity)) {
            if (graphicsScene) {
                graphicsScene->removeItem(entity);
            }
            it.remove();
            delete entity;
            // changed = true; // Якщо потрібно для кешу
        }
    }

    bool entitiesAddedOrRemoved = !newEntitiesThisTurn.isEmpty() || !entitiesToRemoveThisTurn.isEmpty();


    for (Entity* entity : newEntitiesThisTurn) {
        allEntities.append(entity);
        if (graphicsScene) {
            graphicsScene->addItem(entity);
        }
        // changed = true; // Якщо потрібно для кешу
    }

    if(entitiesAddedOrRemoved) { // Оновлюємо кеш, тільки якщо були зміни
        updateEntityGridCache();
    }


    updateCounts();
    emit statisticsUpdated(currentTurn, plantCount, herbivoreCount, predatorCount);
}

bool World::isValidPosition(int x, int y) const {
    return x >= 0 && x < sizeN && y >= 0 && y < sizeN;
}

Entity* World::getEntityAt(int x, int y) const {
    return entityGridCache.value(QPoint(x,y), nullptr);
}

// Call this when an entity is consumed mid-tick, before full cache rebuild
void World::removeEntityFromCellCache(int x, int y) {
    entityGridCache.remove(QPoint(x,y));
}


QList<Entity*> World::getEntitiesInRadius(int centerX, int centerY, int radius) const {
    QList<Entity*> foundEntities;
    for (int i = centerX - radius; i <= centerX + radius; ++i) {
        for (int j = centerY - radius; j <= centerY + radius; ++j) {
            if (i == centerX && j == centerY) continue; // Skip self cell
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

void World::addEntity(Entity* entity, bool addToScene) {
    if (!entity) return;
    allEntities.append(entity);
    entityGridCache.insert(QPoint(entity->getGridX(), entity->getGridY()), entity);
    if (addToScene && graphicsScene) {
        graphicsScene->addItem(entity);
    }
    updateCounts(); // Could be more efficient by incrementing specific count
}

void World::removeEntity(Entity* entity) {
    if (!entity) return;
    entityGridCache.remove(QPoint(entity->getGridX(), entity->getGridY()));
    if (graphicsScene) {
        graphicsScene->removeItem(entity);
    }
    allEntities.removeAll(entity);
    delete entity;
    updateCounts(); // Could be more efficient by decrementing specific count
}


void World::updateEntityGridCache() {
    entityGridCache.clear();
    for (Entity* entity : allEntities) {
        if (!entity->isMarkedForDeath()) { // Only cache active entities
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
