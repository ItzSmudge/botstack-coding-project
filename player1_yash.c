#include "header.h"
#include <stdio.h>


/* ----------------------------------------
The terminal commands for testing the bot can be found in the "cmds.txt" file.
  Ensure the directory you are running the terminal in is correct or things will
  not load correctly at runtime
---------------------------------------- */

int player1_abstain(Player self, Player other, Gun state, int opponentRequest) {
  /* ----------------------------------------
  This is a technically optional piece of logic. The abstention mechanic is
    detailed in the game mechanics, but you have the option to not interact
    at all with this system - in that case, simply leave this function
    untouched.

  If you do want to utilise this mechanic, return 1 to request an abstain,
    or return 0 to deny.
  If the "opponentRequest" value is 1, then your opponent has also requested
    an abstain, and you are accepting or denying this. Otherwise, it is your
    initiative to request an abstention.
  ---------------------------------------- */

  if (opponentRequest == 0){
    return 0;
  }

  int chance_of_death = ((float)(state.currentLiveBullets) / (float)(state.currentBullets)) * 100.0 ;

  if (chance_of_death == 100){
    return 0;
  }

  if (other.lives - self.lives >= 3){
    return 1 ;
  }

  if (self.rounds == 1 && other.rounds == 0){
    return 0 ;
  }

  if (self.rounds == 0 && other.rounds == 1){
    return 1 ;
  }

  if (self.rounds == 0 && other.rounds == 0){
    return 0 ;
  }

  if (self.rounds == 1 && other.rounds == 1){
    return 0 ;
  }
}
ItemAction player1_item(Player self, Player other, Gun state) {
  /* ----------------------------------------
  This is the first major function you will be modifying in order to create
    your bot

  It is responsible for using your available items if you wish to do so - you
   can return one of [ACTION_NoItem, ACTION_Item1, ACTION_Item2].
  Your available items are visible at [self.item1] and [self.item2] (as well
   as all other data for both yourself and opponent)
  ---------------------------------------- */

  int chance_of_death = ((float)(state.currentLiveBullets) / (float)(state.currentBullets)) * 100.0 ;
  

  if (state.currentLiveBullets == 0 && self.item1 == ITEM_HealthKit){
    return ACTION_Item1;
  }
  else if (state.currentLiveBullets == 0 && self.item2 == ITEM_HealthKit){
    return ACTION_Item2;
  }

  if (state.currentBullets == 2 && self.item1 == ITEM_SkipEnemy){
    return ACTION_Item1;
  }
  else if (state.currentBullets == 2 && self.item2 == ITEM_SkipEnemy){
    return ACTION_Item2;
  }

  if (state.currentBullets == state.currentLiveBullets && self.item1 == ITEM_SkipEnemy){
    return ACTION_Item1;
  }
  else if (state.currentBullets == state.currentLiveBullets && self.item2 == ITEM_SkipEnemy){
    return ACTION_Item2;
  }

  if (self.lives == 1 && self.item1 == ITEM_HealthKit){
    return ACTION_Item1;
  }
  else if (self.lives == 1 && self.item2 == ITEM_HealthKit){
    return ACTION_Item2;
  }

  if (chance_of_death == 100){
    return ACTION_NoItem;
  }

  if (self.item1 == ITEM_PeekCurrent){
    return ACTION_Item1;
  }
  else if (self.item2 == ITEM_PeekCurrent){
    return ACTION_Item2;
  }
  else {
    if (self.lives < other.lives && self.item1 == ITEM_HealthKit){
      return ACTION_Item1;
    }
    else if (self.lives < other.lives && self.item2 == ITEM_HealthKit){
      return ACTION_Item2;
    }
    else {
      if (chance_of_death == 50 && self.item1 == ITEM_EjectCurrent){
        return ACTION_Item1;
      }
      else if (chance_of_death == 50 && self.item2 == ITEM_EjectCurrent){
        return ACTION_Item2;
      }
      else {
        if (chance_of_death == 50 && self.item1 == ITEM_ResetGun){
          return ACTION_Item1;
        }
        else if (chance_of_death == 50 && self.item2 == ITEM_ResetGun){
          return ACTION_Item2;
        }
        else {
          if (chance_of_death >= 66 && self.item1 == ITEM_SkipEnemy){
            return ACTION_Item1;
          }
          else if (chance_of_death >= 66 && self.item2 == ITEM_SkipEnemy){
            return ACTION_Item2;
          }
        }
      }
    }
  }
}
TurnAction player1_action(Player self, Player other, Gun state, Bullet nextBullet) {
  /* ----------------------------------------
  This is the second major function you will be modifying in order to create
    your bot

  It is responsible for deciding who to shoot. You can see your own and your
   opponent's data, and the current total bullets as well as the total live
   bullets in the [state] struct.

  If your previous action was [ITEM_PeekCurrent], the value of nextBullet will
   be either [BULLET_Live | BULLET_Blank] depending on the observed bullet.
   Otherwise, this value will be BULLET_UNKNOWN
  ---------------------------------------- */

  int chance_of_death = ((float)(state.currentLiveBullets) / (float)(state.currentBullets)) * 100.0 ;

  if (nextBullet == BULLET_Live){
   return ACTION_ShootOther;
  }
  else if (nextBullet == BULLET_Blank){
    return ACTION_ShootSelf;
  }
  else {
    if (chance_of_death >= 50){
      return ACTION_ShootOther;
    }
    else {
      return ACTION_ShootSelf;
    }
  }
}
