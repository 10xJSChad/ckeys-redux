#define CKEYS_X11
#include "ckeys.hpp"


void OnF5Pressed(void) {
    printf("F5 pressed\n");
}


void OnF6Pressed(void) {
    printf("F6 pressed\n");
}


int main(void) {
    CKeys_Init();
    Key_OnDown(XK_F5, OnF5Pressed);
    Key_OnDown(XK_F6, OnF6Pressed);

    while (true) {
        CKeys_Tick();
        SLEEP_MS(10);
    }

    return 0;
}
