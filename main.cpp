// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include <QRandomGenerator> // Включаємо для генератора

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
