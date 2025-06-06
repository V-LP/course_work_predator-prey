// plant.cpp
#include "plant.h"
#include "world.h"
#include "config.h"
#include <QRandomGenerator>

Plant::Plant(int x, int y)
    : Entity(EntityType::PLANT, x, y, PLANT_COLOR), spreadCooldown(PLANT_SPREAD_COOLDOWN_TURNS) {
}

// ВИПРАВЛЕНО: Сигнатура відповідає базовому класу
void Plant::tick(World& world, QList<Entity*>& newEntities, [[maybe_unused]] QSet<Entity*>& deadEntities) {
    if (isMarkedForDeath()) return;

    spreadCooldown--;
    if (spreadCooldown <= 0) {
        spreadCooldown = PLANT_SPREAD_COOLDOWN_TURNS;

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
            QPoint spreadPos = emptyNeighbors.at(QRandomGenerator::global()->bounded(emptyNeighbors.size()));
            Plant* newPlant = new Plant(spreadPos.x(), spreadPos.y());
            newEntities.append(newPlant);
        }
    }
}
