#include "header.h"

// Tracks opponent's tendencies
static int opponent_shoots_self = 0;
static int opponent_shoots_other = 0;

int player1_abstain(Player self, Player other, Gun state, int opponentRequest)
{
  // Accept if opponent requests AND we're at severe disadvantage
  if (opponentRequest && (self.lives <= 1 && other.lives >= 3))
  {
    return 1; // Accept abstention as last resort
  }
  return 0; // Default: play to win
}

ItemAction player1_item(Player self, Player other, Gun state)
{
  float live_prob = (float)state.currentLiveBullets / state.currentBullets;

  // Use Health Kit at 1 HP
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

  return ACTION_NoItem; // Hold of no good items
}

TurnAction player1_action(Player self, Player other, Gun state, Bullet nextBullet)
{
  float live_prob = (float)state.currentLiveBullets / state.currentBullets;

  // bluff
  if (nextBullet == BULLET_Blank)
    return ACTION_ShootSelf;

  // aggressive: If live_prob > 60%, shoot opponent
  if (live_prob > 0.6f)
    return ACTION_ShootOther;

  // play safe If opponent is near death, shoot them
  if (other.lives == 1)
    return ACTION_ShootOther;

  // default Shoot self if blank odds are good
  return (live_prob < 0.5f) ? ACTION_ShootSelf : ACTION_ShootOther;
}