#pragma once
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "../screen/Screen.h"
#define COLUMN_COUNT 16


class DisplayManager {
private:
    LiquidCrystal_I2C lcd;

    // Кастомные символы для графика (ступенчатая заливка)
    byte bar_levels[7][8] = {
        {B00000, B00000, B00000, B00000, B00000, B00000, B00000, B11111}, // 1 линия
        {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111}, // 2 линии
        {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111}, // 3 линии
        {B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111}, // 4 линии
        {B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111}, // 5 линий
        {B00000, B00000, B11111, B11111, B11111, B11111, B11111, B11111}, // 6 линий
        {B00000, B11111, B11111, B11111, B11111, B11111, B11111, B11111}  // 7 линий
    };


    uint16_t graph_values[COLUMN_COUNT];
    byte info_block[4][4];

    byte on_screen_data[20][4];
    byte previos_screen_data[20][4];
public:
    DisplayManager() : lcd(0x27, 20, 4) {
       for (int i = 0; i < COLUMN_COUNT; i++) graph_values[i] = 0xFFFF;
       for (int i = 0; i < 20; i++)
           for (int j = 0; j < 4; j++) {
               on_screen_data[i][j] = ' ';
               previos_screen_data[i][j] = 0;
           }
    } // Адрес 0x27, размер 20x4

    void begin() {
        delay(1000);
        lcd.init();
        lcd.backlight();

        delay(1000);
        lcd.clear();
        // Загружаем кастомные символы для графика в слоты 0..6
        for (int i = 0; i < 7; i++) {
            lcd.createChar(i, bar_levels[i]);
        }

        lcd.setCursor(0, 0);
        lcd.print("Init OK");
    }

    void text(const char* text) {
        lcd.setCursor(0, 0);
        lcd.print(text);
    }

    void render(Screen& screen) {
        screen.fillScreenData(on_screen_data);
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 4; j++) {
                if (previos_screen_data[i][j] != on_screen_data[i][j]) {
                    lcd.setCursor(i, j);
                    lcd.write(on_screen_data[i][j]);
                    previos_screen_data[i][j] = on_screen_data[i][j];
                }
            }
        }
    }

    void clear() {
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 4; j++) {
                lcd.setCursor(i, j);
                lcd.print(' ');
            }
        }
        memset(graph_values, 0, sizeof(graph_values));
        memset(info_block, 0, sizeof(info_block));
    }

    LiquidCrystal_I2C& getLcd() {
        return lcd;
    }
};
