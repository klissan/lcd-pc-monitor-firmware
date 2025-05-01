//
// Created by klissan on 29/04/2025.
//


#include <Arduino.h>
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "../lib/screen/TextScreen.h"
#include "../lib/constants/EEPROMDictionary.h"

DisplayManager displayManager;
EEPROMDictionary dict;
Screen* test_screen = new SingleScreen(1, 1, 0, 100, 1, "CPU");
Screen* test_screen2 = new ListScreen(2, 1, 0, 100, 1, "CORE", 5);
Screen* test_screen3 = new LinesFormattedScreen(3, 2, 0, 100, 1, "CORE", 6, dict);
ScreenManager screenManager = ScreenManager(dict);


void setup() {
    Serial.begin(115200);
    dict.load();
    displayManager.begin();
    randomSeed(analogRead(A0));
    displayManager.clear();
}

uint16_t val[12];
void loop() {

    // val[0] = 1;
    // val[1] = random(0, 1000);
    // val[2] = 202;
    // val[3] = 2;
    // val[4] = random(0, 1000);
    // val[5] = 202;
    // val[6] = 3;
    // val[7] = random(0, 1000);
    // val[8] = 200;
    // val[9] = 4;
    // val[10] = random(0, 1000);
    // val[11] = 201;
    //
    //
    // test_screen3->addValue(val);
    // displayManager.render(*test_screen3);
    //delay(500);
    screenManager.tick();
    auto active_screen = screenManager.getActiveScreen();
    if (active_screen) {
        displayManager.render(*active_screen);
    }
    delay(2);
}
