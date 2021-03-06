#ifdef USE_SOUND

#include "definitions.h"

#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "sounds.h"
#include "fmod.h"
#include <iostream>

void init_sounds() {
    //FMOD_RESULT result;
    if (fmod_init()) {
        std::cout << "ERROR - fmod_init()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/megamanTheme.wav", 0)) {
        std::cout << "ERROR - fmod_createsound() - megamanTheme\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/tick.wav", 1)) {
        std::cout << "ERROR - fmod_createsound() - tick\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/click.wav", 2)) {
        std::cout << "ERROR - fmod_createsound() - click\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/beep.wav", 3)) {
        std::cout << "ERROR - fmod_createsound() - beep\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/accessDeny.wav", 4)) {
        std::cout << "ERROR - fmod_createsound() - accessDeny\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/button3.wav", 5)) {
        std::cout << "ERROR - fmod_createsound() - button3\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/button4.wav", 6)) {
        std::cout << "ERROR - fmod_createsound() - button4\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/censorBeep.wav", 7)) {
        std::cout << "ERROR - fmod_createsound() - censorBeep\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/groceryScanning.wav", 8)) {
        std::cout << "ERROR - fmod_createsound() - groceryScanning\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/endFx.wav", 9)) {
        std::cout << "ERROR - fmod_createsound() - endFx\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/flyby.wav", 10)) {
        std::cout << "ERROR - fmod_createsound() - flyby\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/bossMusic.wav", 11)) {
        std::cout << "ERROR - fmod_createsound() - bossMusic\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/musicCensor.wav", 12)) {
        std::cout << "ERROR - fmod_createsound() - musicCensor\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/modem.wav", 13)) {
        std::cout << "ERROR - fmod_createsound() - modem\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/pling.wav", 14)) {
        std::cout << "ERROR - fmod_createsound() - pling\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/powerup.wav", 15)) {
        std::cout << "ERROR - fmod_createsound() - powerup\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/robotBlip1.wav", 16)) {
        std::cout << "ERROR - fmod_createsound() - robotBlip1\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/robotBlip2.wav", 17)) {
        std::cout << "ERROR - fmod_createsound() - robotBlip2\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/shortBeep.wav", 18)) {
        std::cout << "ERROR - fmod_createsound() - shortBeep\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/spaceshipTakeoff.wav", 19)) {
        std::cout << "ERROR - fmod_createsound() - spaceshipTakeoff\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/splitSplat.wav", 20)) {
        std::cout << "ERROR - fmod_createsound() - sliptSplat\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/strangeAlien.wav", 21)) {
        std::cout << "ERROR - fmod_createsound() - strangeAlien\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/electronicNoise.wav", 22)) {
        std::cout << "ERROR - fmod_createsound() - electronicNoise\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/introWind.wav", 23)) {
        std::cout << "ERROR - fmod_createsound() - introWind\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/policeScanner.wav", 24)) {
        std::cout << "ERROR - fmod_createsound() - policeScanner\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/scaryAmbience.wav", 25)) {
        std::cout << "ERROR - fmod_createsound() - scaryAmbience\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/dunDunDun.wav", 26)) {
        std::cout << "ERROR - fmod_createsound() - dunDunDun\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/jump1.wav", 27)) {
        std::cout << "ERROR - fmod_createsound() - jump\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/alienMachineGun.wav", 28)) {
        std::cout << "ERROR - fmod_createsound() - alienMachineGun\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/dryFireGun.wav", 29)) {
        std::cout << "ERROR - fmod_createsound() - dryFiregun\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/gunShotMarvin.wav", 30)) {
        std::cout << "ERROR - fmod_createsound() - gunShotMarvin\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/mvalSingle.wav", 31)) {
        std::cout << "ERROR - fmod_createsound() - mvalSingle\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/laser.wav", 32)) {
        std::cout << "ERROR - fmod_createsound() - laser\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/bossDeath.wav", 33)) {
        std::cout << "ERROR - fmod_createsound() - bossDeath\n" << std::endl;
        return;
    }

}

#endif
