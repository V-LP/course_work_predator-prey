// config.h
#ifndef CONFIG_H
#define CONFIG_H

#include <QColor>

// Розміри світу та клітинок
const int GRID_SIZE_N = 30;       // Кількість клітинок по одній осі (поле N x N)
const int CELL_SIZE_PX = 20;      // Розмір однієї клітинки в пікселях

// Початкова кількість істот
const int INITIAL_PLANTS = 60;
const int INITIAL_HERBIVORES = 20;
const int INITIAL_PREDATORS = 7;

// Параметри вигляду
const QColor PLANT_COLOR = Qt::darkGreen;
const QColor HERBIVORE_COLOR = QColor(0, 100, 255);
const QColor PREDATOR_COLOR = QColor(220, 20, 60);

// Параметри рослин
const int PLANT_SPREAD_COOLDOWN_TURNS = 8;  // Кількість ходів до наступної спроби розповсюдження

// Загальні параметри живих істот
const int MAX_HUNGER = 100;                 // Максимальний рівень голоду
const int HUNGER_DECREASE_PER_TURN = 2;     // Зменшення голоду за хід
const int FOOD_TO_REPRODUCE = 2;            // Кількість з'їденої їжі для розмноження
const int REPRODUCTION_COOLDOWN_TURNS = 15; // Мінімальний інтервал між розмноженнями

// Параметри травоїдних
const int HERBIVORE_INITIAL_HUNGER = 70;
const int HERBIVORE_SPEED = 1;              // Клітин за хід
const int HERBIVORE_VISION_RANGE = 5;       // Радіус зору
const int HERBIVORE_EAT_PLANT_HUNGER_GAIN = 35; // Скільки голоду відновлює, з'ївши рослину

// Параметри хижаків
const int PREDATOR_INITIAL_HUNGER = 80;
const int PREDATOR_SPEED = 1;               // Клітин за хід
const int PREDATOR_VISION_RANGE = 7;        // Радіус зору
const int PREDATOR_EAT_HERBIVORE_HUNGER_GAIN = 60; // Скільки голоду відновлює, з'ївши травоїдну

// Швидкість симуляції
const int SIMULATION_TIMER_MS = 200; // мілісекунд на хід (чим менше, тим швидше)

#endif // CONFIG_H
