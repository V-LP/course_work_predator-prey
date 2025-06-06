// world.h
#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QList>
#include <QPoint>
#include <QHash>
#include <QSet>
#include <utility> // for std::pair
#include "entitytype.h"

class Entity;
class QGraphicsScene;

class World : public QObject {
    Q_OBJECT
public:
    explicit World(int size, QGraphicsScene* scene, QObject *parent = nullptr);
    ~World();

    // Змінена функція для ініціалізації з параметрами
    void initializePopulation(int numPlants, int numHerbivores, int numPredators);
    void tick();

    bool isValidPosition(int x, int y) const;
    Entity* getEntityAt(int x, int y) const;
    QList<Entity*> getEntitiesInRadius(int centerX, int centerY, int radius) const;

    void scheduleAddEntity(EntityType type, int x, int y);
    void scheduleRemoveEntity(Entity* entity);

    void removeEntityImmediately(Entity* entity);
    void removeEntityFromCellCache(int x, int y);

    int getCurrentTurn() const;
    int getPlantCount() const;
    int getHerbivoreCount() const;
    int getPredatorCount() const;


signals:
    void statisticsUpdated(int turn, int plants, int herbivores, int predators);

private:
    void processScheduledActions(QSet<Entity*>& deadEntities, QList<Entity*>& newEntities);

    void addEntityImmediately(Entity* entity, bool addToScene = true);
    void updateEntityGridCache();
    void updateCounts();
    QPoint findEmptyRandomPosition() const;

    int sizeN;
    QGraphicsScene* graphicsScene;
    QList<Entity*> allEntities;
    QHash<QPoint, Entity*> entityGridCache;

    QList<std::pair<EntityType, QPoint>> scheduledAdditions;
    QSet<Entity*> scheduledForRemoval;

    int currentTurn;
    int plantCount;
    int herbivoreCount;
    int predatorCount;
};

#endif // WORLD_H
