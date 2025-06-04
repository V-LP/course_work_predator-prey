// world.h
#ifndef WORLD_H
#define WORLD_H

#include <QObject>
#include <QList>
#include <QPoint>
#include <QHash> // For grid cache

class Entity; // Forward declaration
class QGraphicsScene;
enum class EntityType;

class World : public QObject {
    Q_OBJECT
public:
    explicit World(int size, QGraphicsScene* scene, QObject *parent = nullptr);
    ~World();

    void initializePopulation();
    void tick();

    bool isValidPosition(int x, int y) const;
    Entity* getEntityAt(int x, int y) const;
    QList<Entity*> getEntitiesInRadius(int centerX, int centerY, int radius) const;

    void addEntity(Entity* entity, bool addToScene = true);
    void removeEntity(Entity* entity);
    void removeEntityFromCellCache(int x, int y); // Specific for when an entity is eaten


    int getCurrentTurn() const;
    int getPlantCount() const;
    int getHerbivoreCount() const;
    int getPredatorCount() const;


signals:
    void statisticsUpdated(int turn, int plants, int herbivores, int predators);

private:
    void updateEntityGridCache();
    void updateCounts();
    QPoint findEmptyRandomPosition() const;


    int sizeN;
    QGraphicsScene* graphicsScene;
    QList<Entity*> allEntities;
    // Cache for faster lookups of entity at a specific grid cell
    // Using QHash for sparse grid, or QList<QList<Entity*>> for dense grid
    QHash<QPoint, Entity*> entityGridCache;


    int currentTurn;
    int plantCount;
    int herbivoreCount;
    int predatorCount;
};

#endif // WORLD_H
