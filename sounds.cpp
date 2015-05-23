
#define USE_SOUND

#ifdef USE_SOUND
#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "fmod.h"

void init_sounds(void);

enum sound_t {
    // theme song
    megamanTheme = 0,

    // small beeps and clicks
    tick,
    click,
    beep,
    bleep,
    button3,
    button4,
    censorBeep,
    groceryScanning,
    endFx,
    flyby,
    hockeyStickSlap,
    musicCensor,
    modem,
    pling,
    powerup,
    robotBlip1,
    robotBlip2,
    shortBeep,
    spaceshipTakeoff,
    splitSplat,
    strangeAlien,

    // static
    electronicNoise,
    introWind,
    policeScanner,
    scaryAmbience,
    // actions
    dunDunDun,
    marioJump,

    // weapon shots
    alienMachineGun,
    dryFiregun,
    gunShotMarvin,
    mvalSingle,
};

void init_sounds() {
    //FMOD_RESULT result;
    if (fmod_init()) {
        std::cout << "ERROR - fmod_init()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/megamanTheme.wav", 0)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/tick.wav", 1)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/click.wav", 2)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/beep.wav", 3)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/bleep.wav", 4)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/button3.wav", 5)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/button4.wav", 6)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/censorBeep.wav", 7)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/groceryScanning.wav", 8)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/endFx.wav", 9)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/flyby.wav", 10)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/electronicNoise.wav", 11)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/dunDunDun.wav", 12)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/alienMachineGun.wav", 13)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/dryFireGun.wav", 14)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/gunShotMarvin.wav", 15)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }
    if (fmod_createsound((char *)"./sounds/marioJump.wav", 16)) {
        std::cout << "ERROR - fmod_createsound()\n" << std::endl;
        return;
    }

    fmod_setmode(0,FMOD_LOOP_OFF);
    //fmod_playsound(0);
    //fmod_systemupdate();
}
#endif
