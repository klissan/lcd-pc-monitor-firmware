#pragma once
#include "../screen/Screen.h"
#include "../screen/GraphicScreen.h"
#include "../screen/TextScreen.h"
#define MAX_SCREENS 20
# define MAX_DATA 120

struct ScreenEntry {
    byte id;
    Screen* screen;
};

extern int __heap_start, *__brkval;
int freeMemory() {
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}


class ScreenManager {
private:
    ScreenEntry screens[MAX_SCREENS];
    Screen* activeScreen = nullptr;
    uint16_t values[MAX_DATA];
    EEPROMDictionary &dictionary;
    char text[STRING_LENGTH];
    unsigned long last_data_received = 0;

public:
    ScreenManager(EEPROMDictionary& dictionary): dictionary(dictionary) {
    }

    void tick() {
        if (Serial.available() >= 2 && Serial.peek() == 0xFF) {
            Serial.read(); // съедаем 0xFF
            byte id = Serial.read();

            if (Serial.available() < 1) return;
            byte type = Serial.read();

            if (type == 0xEF) {
                // → фиксированный путь только для констант
                while (Serial.available() < 4 + 1); // символ + длина
                char symbol[5];
                for (int i = 0; i < 4; i++) symbol[i] = Serial.read();
                symbol[4] = '\0';

                byte string_length = Serial.read();
                while (Serial.available() < string_length); // дождись строки

                for (int i = 0; i < min(string_length, STRING_LENGTH - 1); i++) {
                    text[i] = Serial.read();
                }
                text[min(string_length, STRING_LENGTH - 1)] = '\0';

                dictionary.set(id, text);
                last_data_received = millis();
                Serial.write(0xAA);
            } else {
                // остальной разбор — только если это НЕ 0xEF
                if (Serial.available() < 5 + 1 + 1) return;
                byte min_value = Serial.read();
                byte max_value = Serial.read();
                byte scale = Serial.read();
                char symbol[5];
                for (int i = 0; i < 4; i++) symbol[i] = Serial.read();
                symbol[4] = '\0';

                byte count = Serial.read();
                while (Serial.available() < count * 2); // все значения
                for (int i = 0; i < count; i++) {
                    values[i] = Serial.read() | (Serial.read() << 8);
                }

                registerOrUpdateScreen(id, type, min_value, max_value, scale, symbol, count);
                last_data_received = millis();
                Serial.write(0xAA);
            }
        }
    }




    // void tick() {
    //     if (Serial.available() >= 1 && Serial.peek() == 0xFF) {
    //         if (Serial.available() >= 8) {
    //             Serial.read();  // съедаем 0xFF
    //             // Serial.println(F("=== NEW TICK ==="));
    //             // Serial.print(F("Free RAM: "));
    //             // Serial.println(freeMemory());
    //
    //             parsePacket();
    //         }
    //     }
    //
    // }

    void parsePacket() {
        // Читать заголовок
        byte id = Serial.read();
        byte type = Serial.read();
        byte min_value = Serial.read();
        byte max_value = Serial.read();
        byte scale = Serial.read();
        char symbol[5];
        for (int i = 0; i < 4; i++) {
            symbol[i] = Serial.read();
        }

        if (type == 1 || type == 2) { // SingleScreen
            byte count = Serial.read();

            for (int i = 0; i < count; i++) {
                values[i] = Serial.read() | (Serial.read() << 8); // 2 байта little endian
            }

            registerOrUpdateScreen(id, type, min_value, max_value, scale, symbol, count);
            last_data_received = millis();
            Serial.write(0xAA);
        } else if (type == 0xEF) {
            int string_length = Serial.read();
            for (int i = 0; i < min(string_length, STRING_LENGTH - 1); i++) {
                text[i] = Serial.read();
            }
            text[min(string_length, STRING_LENGTH - 1)] = '\0';

            dictionary.set(id, text);
            last_data_received = millis();
            Serial.write(0xAA);
        }
    }

    void registerOrUpdateScreen(byte id, byte type, byte min_value, byte max_value, byte scale, const char* symbol, byte count) {
        Screen* screen = findScreen(id);
        //TODO: shitcode
        if (!screen) {
            if (type == 1) {
                if (count == 1) {
                    screen = new SingleScreen(id,1, min_value, max_value, scale, symbol);
                } else {
                    screen = new ListScreen(id, 1, min_value, max_value, scale, symbol, count);
                }
                addScreen(id, screen);
            } else if (type == 2) {
                screen = new LinesFormattedScreen(id, 2, min_value, max_value, scale, symbol, count, dictionary);
                addScreen(id, screen);
            }
        }
        if (screen) {
            screen->addValue(this->values);
            activeScreen = screen;
        }
    }

    Screen* findScreenDebug(byte id) {
        for (int i = 0; i < MAX_SCREENS; i++) {
            if (screens[i].screen) {
                Serial.print(F("🧪 Slot ")); Serial.print(i);
                Serial.print(F(" → id=")); Serial.print(screens[i].id);
                Serial.print(F(", want=")); Serial.println(id);

                if (screens[i].id == id) {
                    Serial.println(F("✅ Found screen!"));
                    return screens[i].screen;
                }
            }
        }
        Serial.println(F("❌ Not found! Will create new."));
        return nullptr;
    }

    Screen* findScreen(byte id) {
        for (int i = 0; i < MAX_SCREENS; i++) {
            if (screens[i].screen && screens[i].id == id) {
                return screens[i].screen;
            }
        }
        return nullptr;
    }

    void addScreen(byte id, Screen* screen) {
        for (int i = 0; i < MAX_SCREENS; i++) {
            if (screens[i].screen == nullptr) {
                screens[i].id = id;
                screens[i].screen = screen;
                return;
            }
        }
    }

    Screen* getActiveScreen() {
        return activeScreen;
    }

    unsigned long get_last_data_received() {
        return last_data_received;
    };
};
