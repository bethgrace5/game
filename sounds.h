
#ifdef USE_SOUND
#include "definitions.h"

#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "fmod.h"
#include <iostream>


#ifndef SOUNDS_H
#define SOUNDS_H
void init_sounds();
enum sound_t {
    // theme song
    megamanTheme = 0,

    // small beeps and clicks
    tick,
    click,
    beep,
    accessDeny,
    button3,
    button4,
    censorBeep,
    groceryScanning,
    endFx,
    flyby,
    bossMusic,
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
    jumpSound,

    // weapon shots
    alienMachineGun,
    dryFiregun,
    gunShotMarvin,
    mvalSingle,
    laser,
    //Death Boss
    bossDeath
};

#endif
#endif
