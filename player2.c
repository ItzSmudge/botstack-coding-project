#include "header.h"
#include <stdlib.h>
#include <time.h>
#ifndef PLAYER_MODE


// Tracks opponent's tendencies
static int opponent_shoots_self = 0;
static int opponent_shoots_other = 0;

int player2_abstain(Player self, Player other, Gun state, int opponentRequest)
{
  // Accept if opponent requests AND we're at severe disadvantage
  if (opponentRequest && (self.lives <= 1 && other.lives >= 3))
  {
    return 1; // Accept abstention as last resort
  }
  return 0; // Default: play to win
}

ItemAction player2_item(Player self, Player other, Gun state)
{
  float live_prob = (float)state.currentLiveBullets / state.currentBullets;

  // EMERGENCY: Use Health Kit at 1 HP
  if (self.lives == 1 && (self.item1 == ITEM_HealthKit || self.item2 == ITEM_HealthKit))
  {
    return (self.item1 == ITEM_HealthKit) ? ACTION_Item1 : ACTION_Item2;
  }

  // If opponent is aggressive, skip their turn
  if (opponent_shoots_other > 3 && (self.item1 == ITEM_SkipEnemy || self.item2 == ITEM_SkipEnemy))
  {
    return (self.item1 == ITEM_SkipEnemy) ? ACTION_Item1 : ACTION_Item2;
  }

  // Peek if bullet count is low (for bluffing)
  if (state.currentBullets <= 3 && (self.item1 == ITEM_PeekCurrent || self.item2 == ITEM_PeekCurrent))
  {
    return (self.item1 == ITEM_PeekCurrent) ? ACTION_Item1 : ACTION_Item2;
  }

  // Eject if next bullet is likely live
  if (live_prob > 0.6 && (self.item1 == ITEM_EjectCurrent || self.item2 == ITEM_EjectCurrent))
  {
    return (self.item1 == ITEM_EjectCurrent) ? ACTION_Item1 : ACTION_Item2;
  }

  return ACTION_NoItem; // No good items? Hold.
}

TurnAction player2_action(Player self, Player other, Gun state, Bullet nextBullet)
{
  float live_prob = (float)state.currentLiveBullets / state.currentBullets;

  // BLUFF MODE: If we peeked a blank, shoot self for extra turn
  if (nextBullet == BULLET_Blank)
    return ACTION_ShootSelf;

  // AGGRESSIVE MODE: If live_prob > 60%, shoot opponent
  if (live_prob > 0.6f)
    return ACTION_ShootOther;

  // SAFE MODE: If opponent is near death, shoot them
  if (other.lives == 1)
    return ACTION_ShootOther;

  // DEFAULT: Shoot self if blank odds are good
  return (live_prob < 0.5f) ? ACTION_ShootSelf : ACTION_ShootOther;
}
// Do not touch the following code:
#else
#include <stdio.h>

void gameState(Player self, Player other, Gun state, int displayItems) {
  printf("\tCurrent game state:\n");
  printf("\t  Health | Self: %d   | Opponent: %d\n", self.lives, other.lives);
  printf("\t  Gun:   | Total: %d  | Live: %d\n\n", state.currentBullets, state.currentLiveBullets);

  if (!displayItems)
    return;
  printf("\t  Available items: [%s, %s]\n", ItemToString(self.item1), ItemToString(self.item2));
  printf("\t  Opponents items: [%s, %s]\n\n", ItemToString(other.item1), ItemToString(other.item2));
}

int player2_abstain(Player self, Player other, Gun state, int opponentRequest) {
  gameState(self, other, state, 1);

  char str[5];
  while (1) {
    if (!opponentRequest)
      printf("\tWould you like to request an abstain - [0:no, 1:yes]: ");
    else
      printf("\tYour opponent has requested an abstain; do you accept - [0:no, 1:yes]: ");

    if (fgets(str, 5, stdin) == NULL)
      continue;

    if (str[0] == '0')
      return 0;
    else if (str[0] == '1')
      return 1;
  }
}

ItemAction player2_item(Player self, Player other, Gun state) {
  gameState(self, other, state, 1);

  char str[5];
  while (1) {
    printf("\tEnter the item you want to use - [0:none, 1:item 1, 2:item 2]: ");

    if (fgets(str, 5, stdin) == NULL)
      continue;

    if (str[0] == '0')
      return ACTION_NoItem;
    else if (str[0] == '1')
      return ACTION_Item1;
    else if (str[0] == '2')
      return ACTION_Item2;
  }
}
TurnAction player2_action(Player self, Player other, Gun state, Bullet nextBullet) {
  gameState(self, other, state, 0);

  char str[5];
  while (1) {
    printf("\tEnter who you want to fire at - [0:self, 1:opponent]: ");
    if (fgets(str, 5, stdin) == NULL)
      continue;

    if (str[0] == '0')
      return ACTION_ShootSelf;
    else if (str[0] == '1')
      return ACTION_ShootOther;
  }
}
#endif