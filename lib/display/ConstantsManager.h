#pragma once
struct IdString {
    uint16_t id;
    const char* text;
};

const IdString idStrings[] = {
    //Network
    {1, "Upload:  "},
    {2, "Download:"},
    {3, "U/Speed: "},
    {4, "D/Speed: "},

    //measurement
    {200, "KB/s"},
    {201, "MB/s"},
    { 202, "GB"}

};

const int idStringsCount = sizeof(idStrings) / sizeof(idStrings[0]);

const char* findStringById(uint16_t id) {
    for (int i = 0; i < idStringsCount; i++) {
        if (idStrings[i].id == id) {
            return idStrings[i].text;
        }
    }
    return "UNKNOWN";
}