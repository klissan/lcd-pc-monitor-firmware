#pragma once
#include "ConstantsManager.h"
#include "Screen.h"
#include "../constants/EEPROMDictionary.h"


class TextScreen : public Screen {
protected:
    byte string_count;
    char line_buffers[4][21]; // максимум 20 символов на строку
    const char *lines[4]; // указатели на строки
    EEPROMDictionary &dictionary;

    const char *getLine(byte i) const {
        return (i < string_count) ? lines[i] : "";
    }

    byte getLineCount() const {
        return string_count;
    }

public:
    TextScreen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol,
                byte string_count, EEPROMDictionary& dictionary)
        : Screen(id, type_id, min_value, max_value, scale, symbol), dictionary(dictionary) {
        this->string_count = string_count;
    }


    void fillScreenData(byte (&screenData)[20][4]) override {
        for (int i = 0; i < getLineCount(); i++) {
            const char* line = getLine(i);
            for (int j = 0; j < 20; j++) {
                screenData[j][i] = line[j];
            }
        }
        if (getLineCount() < 4) {
            for (int i = getLineCount(); i < 4; i++) {
                for (int j = 0; j < 20; j++) {
                    screenData[j][i] = ' ';
                }
            }
        }
    }
};

//-------------------------------------------------------------------------------
class LinesFormattedScreen : public TextScreen {

public:
    LinesFormattedScreen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol,
                byte string_count, EEPROMDictionary& dictionary)
        : TextScreen(id, type_id, min_value, max_value, scale, symbol, string_count / 3, dictionary) {
    }

    void addValue(uint16_t *values) override {
        for (int i = 0; i < string_count; i++) {
            uint16_t key_id = values[i * 3 + 0];
            uint16_t value = values[i * 3 + 1];
            uint16_t unit_id = values[i * 3 + 2];

            uint16_t int_part = value;
            uint16_t frac_part = 0;

            if (scale > 0) {
                uint16_t divider = 1;
                for (byte s = 0; s < scale; s++) divider *= 10;
                int_part = value / divider;
                frac_part = value % divider;
            }

            const char *key = dictionary.get(key_id);
            const char *unit = dictionary.get(unit_id);

            snprintf(line_buffers[i], sizeof(line_buffers[i]), "%s%3d.%d %s", key, int_part, frac_part, unit);


            size_t len = strlen(line_buffers[i]);
            for (size_t j = len; j < sizeof(line_buffers[i]) - 1; j++) {
                line_buffers[i][j] = ' ';
            }
            line_buffers[i][sizeof(line_buffers[i]) - 1] = '\0';
            lines[i] = line_buffers[i];
        }
    }

};
