#pragma once
#include <Arduino.h>

class Screen {
protected:
    byte id;
    byte min_value;
    byte max_value;
    byte scale;
    byte type_id;
    const char *symbol;

public:
    Screen(byte id, byte type_id, byte min_value, byte max_value, byte scale, const char *symbol) {
        this->id = id;
        this->type_id = type_id;
        this->min_value = min_value;
        this->max_value = max_value;
        this->scale = scale;
        this->symbol = symbol;
    }

    byte getScale() { return scale; }
    byte getMaxValue() { return max_value; }
    byte getType() { return type_id; }


    virtual void addValue(uint16_t *value) = 0;

    virtual void fillScreenData(byte (&screenData)[20][4]) = 0;
};


