#pragma once
#include "Screen.h"
#include "Utils.h"
#define COLUMN_COUNT 16


class GraphicScreen : public Screen {
protected:
    uint16_t buffer[COLUMN_COUNT];
    int count = 0;
    byte infoBlock[4][4] = {{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '},{' ',' ',' ',' '}};

    uint16_t getValue(int position) {
        return buffer[position];
    }

    byte (*getInfoBlock())[4] {
        if (count > 0) {
            encodeToDigits(buffer[count - 1], scale, infoBlock[2]);
        } else {
            for (int i = 0; i < 4; i++) infoBlock[2][i] = ' ';
        }
        return infoBlock;
    }

public:
    GraphicScreen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol)
        : Screen(id, type_id, min_value, max_value, scale, symbol) {
        infoBlock[0][0] = symbol[0];
        infoBlock[0][1] = symbol[1];
        infoBlock[0][2] = symbol[2];
        infoBlock[0][3] = symbol[3];
    }

    void fillScreenData(byte (&screenData)[20][4]) override {
        for (int i = 0; i < COLUMN_COUNT; i++) {
            uint16_t current_byte = getValue(COLUMN_COUNT - 1 - i);

            int value = safeMap(current_byte, 0, max_value, 0, 32, scale);
            if (value > 31) value = 31;

            int full_rows = value / 8;
            int remainder = value % 8;

            for (int row = 0; row < 4; row++) {
                if (row < full_rows) {
                    screenData[COLUMN_COUNT - 1 - i][3 - row] = byte(6); // полная заливка
                } else if (row == full_rows && remainder > 0) {
                    byte partial = remainder - 1;
                    if (partial > 6) partial = 6;
                    screenData[COLUMN_COUNT - 1 - i][3 - row] = partial;
                } else {
                    screenData[COLUMN_COUNT - 1 - i][3 - row] = ' ';
                }
            }
        }

        auto info_block = getInfoBlock();
        for (int row = 0; row < 4; row++) {
            for (int col = 0; col < 4; col++) {
                screenData[20 - 4 + col][row] = info_block[row][col];
            }
        }
    };

    static void encodeToDigits(uint16_t value, byte scale, byte output[4]) {
        uint16_t int_part = value;
        uint16_t frac_part = 0;

        if (scale > 0) {
            uint16_t divider = 1;
            for (byte s = 0; s < scale; s++) divider *= 10;
            int_part = value / divider;
            frac_part = value % divider;
        }

        output[0] = '0' + (int_part / 10);
        output[1] = '0' + (int_part % 10);
        output[2] = '.';
        output[3] = '0' + frac_part;
    }
};

//----------------------------------------------------------------------------------------------------------------
class SingleScreen : public GraphicScreen {
public:
    SingleScreen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol)
        : GraphicScreen(id, type_id, min_value, max_value, scale, symbol) {
        for (int i = 0; i < COLUMN_COUNT; i++) {
            buffer[i] = ' ';
        }
    }

    void addValue(uint16_t *value) {
        if (count < COLUMN_COUNT) {
            buffer[count++] = value[0];
        } else {
            memmove(buffer, buffer + 1, (COLUMN_COUNT - 1) * sizeof(uint16_t));
            buffer[COLUMN_COUNT - 1] = value[0];
        }
    }
};


//----------------------------------------------------------------------------------------------------------------
class ListScreen : public GraphicScreen {
private:
    byte size = 0;

public:
    ListScreen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol, byte size)
        : GraphicScreen(id, type_id, min_value, max_value, scale, symbol) {
        this->size = size;
        for (int i = 0; i < COLUMN_COUNT; i++) buffer[i] = 0;
    }

    void addValue(uint16_t *value) {
        for (int i = 0; i < size; i++) {
            buffer[i] = value[i];
        }
    }

};