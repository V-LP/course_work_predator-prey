// simulationview.h
#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QGraphicsView>
#include <QPoint>

class World; // Forward declaration
class Entity; // Forward declaration
enum class EntityType; // Forward declaration

class SimulationView : public QGraphicsView {
    Q_OBJECT
public:
    explicit SimulationView(QWidget *parent = nullptr);
    void setWorld(World* world);

protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void mousePressEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

signals:
    void userMadeChange();

private slots:
    void addEntityActionTriggered();
    void removeEntityActionTriggered();


private:
    World* currentWorld;
    QPoint lastRightClickScenePos; // To store grid position for context menu
};

#endif // SIMULATIONVIEW_H
