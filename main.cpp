// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QRandomGenerator> // For seeding
#include <QTime>      // For seeding

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Seed the global random number generator
    QRandomGenerator::global()->seed(static_cast<quint32>(QTime::currentTime().msec()));

    MainWindow w;
    w.show();
    return a.exec();
}
