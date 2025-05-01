#pragma once
#include <Arduino.h>
#include <EEPROM.h>

#define MAX_EEPROM_STRINGS 32
#define STRING_LENGTH 16
#define EEPROM_SIGNATURE_ADDR 0
#define EEPROM_SIGNATURE 0xBEEF
#define EEPROM_DATA_ADDR 2

struct EepromStringEntry {
    uint16_t id;
    char text[STRING_LENGTH];
};

class EEPROMDictionary {
private:
    EepromStringEntry entries[MAX_EEPROM_STRINGS];

    bool isEepromInitialized() {
        uint16_t sig;
        EEPROM.get(EEPROM_SIGNATURE_ADDR, sig);
        return sig == EEPROM_SIGNATURE;
    }

public:
    void load() {
        if (!isEepromInitialized()) {
            clear();
            save();
            EEPROM.put(EEPROM_SIGNATURE_ADDR, EEPROM_SIGNATURE);
        }
        EEPROM.get(EEPROM_DATA_ADDR, entries);
    }

    void save() {
        EEPROM.put(EEPROM_DATA_ADDR, entries);
    }

    bool remove(uint16_t id) {
        for (int i = 0; i < MAX_EEPROM_STRINGS; i++) {
            if (entries[i].id == id) {
                entries[i].id = 0xFFFF;
                entries[i].text[0] = '\0';
                save();
                return true;
            }
        }
        return false;
    }

    const char* get(uint16_t id) {
        for (int i = 0; i < MAX_EEPROM_STRINGS; i++) {
            if (entries[i].id == id && entries[i].text[0] != '\0') {
                return entries[i].text;
            }
        }
        return "UNK";
    }

    bool set(uint16_t id, const char* text) {
        // 1. Попытка найти и обновить существующую запись
        for (int i = 0; i < MAX_EEPROM_STRINGS; i++) {
            if (entries[i].id == id) {
                strncpy(entries[i].text, text, STRING_LENGTH - 1);
                entries[i].text[STRING_LENGTH - 1] = '\0';
                save();
                return true;
            }
        }

        // 2. Если не найдено — ищем первую свободную ячейку
        for (int i = 0; i < MAX_EEPROM_STRINGS; i++) {
            if (entries[i].text[0] == '\0') {
                entries[i].id = id;
                strncpy(entries[i].text, text, STRING_LENGTH - 1);
                entries[i].text[STRING_LENGTH - 1] = '\0';
                save();
                return true;
            }
        }

        return false;  // места нет
    }

    void clear() {
        for (int i = 0; i < MAX_EEPROM_STRINGS; i++) {
            entries[i].id = 0xFFFF;
            entries[i].text[0] = '\0';
        }
        save();
    }
};