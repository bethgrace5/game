
#ifdef USE_SOUND
#ifndef SOUNDS_H
#define SOUNDS_H
#include "definitions.h"

#include "./include/FMOD/fmod.h"
#include "./include/FMOD/wincompat.h"
#include "fmod.h"
#include <iostream>
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
    mvalSingle
};

void init_sounds(void);

#endif
#endif
