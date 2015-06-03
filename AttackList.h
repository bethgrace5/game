//====================================================================
//  Attack Storage
//====================================================================
/*
-------------------
 Attack_Editor
 Attack_Usage
 Outside_Influence
-----------------
*/
#ifndef ATTACK_LIST_H
#define ATTACK_LIST_H

#include "Object.h"
#include "Sprite.h"
#include "chadD.h"
#include "Enemy.h"
#include "Player.h"
#include "sounds.h"

extern Player *hero;

struct attack_list{
  Sprite  sprite_sheet[MAX_ATTACKS];
  Attack attacks[MAX_ATTACKS];
  int attacks_length;

  Attack *currents[MAX_CURRENTS];
  int currents_length;

  void makeAttacks();
  void copyAttack(Object *caster, int tId);
  void copyAttack(Object *caster, int tId, bool mirror);
  void copyAttack(Player *caster, int tId, bool mirror);
  void copyAttack(Enemy *caster, int tId, bool mirror);
  void deleteAttack(int id);

  bool detectAttack (Object *obj, Attack *targetAttack);
  void renderAttacks(int x, int y);

};

enum attacks_t{
 a_fireUp = 0,
 a_pushingLaser,
 a_fireShield,
 a_fireDown,
 a_skiping,
 a_explode,
 a_speedArrow,
 a_shield,
 a_simpleBlast
};
#endif
