#include "header.h"
#include <math.h>
#include <stdlib.h>

// Enhanced opponent tracking
typedef struct {
    float aggression;       // 0 (cautious) to 1 (aggressive)
    float self_shot_ratio;  // Tracks opponent's self-preservation tendency
    int item_usage[6];      // Tracks which items opponent uses
} OpponentProfile;

// Bullet pattern memory
typedef struct {
    int last_bullets[3];    // Circular buffer of last 3 bullets
    int pointer;            // Current position in buffer
    float streak_adjustment;// Probability multiplier for streaks
} BulletMemory;

// Game state tracker
static OpponentProfile opponent = {0.5f, 0.5f, {0}};
static BulletMemory bullets = {{0}, 0, 1.0f};
static int successful_self_shots = 0;
static int successful_other_shots = 0;

// Helper function
static int has_item(Player p, Item item) {
    return p.item1 == item || p.item2 == item;
}

// threshold calc
float get_threshold(Player self, Player other) {
    float base = 0.5f;
    
    // Adjust based on health difference
    base += (self.lives - other.lives) * 0.1f;
    
    // Adjust based on opponent aggression
    base -= opponent.aggression * 0.1f;
    
    // Adjust based on successful moves
    float self_success = successful_self_shots / (float)(successful_self_shots + successful_other_shots + 1);
    base += (self_success - 0.5f) * 0.2f;
    
    return fmax(0.2f, fmin(0.8f, base)); 
}

int player1_abstain(Player self, Player other, Gun state, int opponentRequest) {
    // Only consider abstention when at severe disadvantage
    float disadvantage = (other.lives - self.lives) + 
                       (state.currentLiveBullets - state.currentBullets/2)*0.5f;
    return (disadvantage > 2.0f) ? 1 : 0;
}

ItemAction player1_item(Player self, Player other, Gun state) {
    float live_prob = (float)state.currentLiveBullets / state.currentBullets;
    live_prob *= bullets.streak_adjustment; // Apply streak adjustment

    // Survival
    if (self.lives == 1) {
        if (has_item(self, ITEM_HealthKit)) {
            return (self.item1 == ITEM_HealthKit) ? ACTION_Item1 : ACTION_Item2;
        }
    }

    // Opponent suppression
    if (opponent.aggression > 0.7f && has_item(self, ITEM_SkipEnemy)) {
        return (self.item1 == ITEM_SkipEnemy) ? ACTION_Item1 : ACTION_Item2;
    }

    // Information control (late game)
    if (state.currentBullets <= 4 && has_item(self, ITEM_PeekCurrent)) {
        return (self.item1 == ITEM_PeekCurrent) ? ACTION_Item1 : ACTION_Item2;
    }

    // Bullet manipulation (when dangerous)
    if (live_prob > 0.65f && state.currentBullets >= 4) {
        if (has_item(self, ITEM_EjectCurrent)) {
            return (self.item1 == ITEM_EjectCurrent) ? ACTION_Item1 : ACTION_Item2;
        }
        if (has_item(self, ITEM_ResetGun)) {
            return (self.item1 == ITEM_ResetGun) ? ACTION_Item1 : ACTION_Item2;
        }
    }

    return ACTION_NoItem;
}

TurnAction player1_action(Player self, Player other, Gun state, Bullet nextBullet) {
    float live_prob = (float)state.currentLiveBullets / state.currentBullets;
    live_prob *= bullets.streak_adjustment;
    float threshold = get_threshold(self, other);

    //  bullet cases
    if (nextBullet == BULLET_Blank) return ACTION_ShootSelf;
    if (nextBullet == BULLET_Live) {
        // 10% chance to bluff even with live round
        if (other.lives > 1 && (rand() % 10) == 0) return ACTION_ShootSelf;
        return ACTION_ShootOther;
    }

    // Endgame special cases
    if (state.currentBullets <= 2) {
        if (self.lives > other.lives && live_prob > 0.3f) return ACTION_ShootOther;
        if (self.lives < other.lives && live_prob > 0.4f) return ACTION_ShootOther;
    }

    // Probabilistic decision making
    if (live_prob > threshold + 0.1f) {
        // Occasionally mix up strategy (15% chance)
        if (opponent.aggression > 0.6f && (rand() % 100) < 15) {
            return ACTION_ShootSelf;
        }
        return ACTION_ShootOther;
    } 
    else if (live_prob < threshold - 0.1f) {
        return ACTION_ShootSelf;
    }
    
    // Near threshold - randomize to avoid predictability
    return (rand() % 2) ? ACTION_ShootSelf : ACTION_ShootOther;
}

// function to update models after each turn
void update_models(TurnAction my_action, TurnAction their_action, 
                   ItemAction their_item, Bullet bullet_fired) {
    // Update bullet memory
    bullets.last_bullets[bullets.pointer] = bullet_fired;
    bullets.pointer = (bullets.pointer + 1) % 3;
    
    // detect streaks
    if (bullets.last_bullets[0] == BULLET_Live &&
        bullets.last_bullets[1] == BULLET_Live &&
        bullets.last_bullets[2] == BULLET_Live) {
        bullets.streak_adjustment = 1.2f;
    } else if (bullets.last_bullets[0] == BULLET_Blank &&
               bullets.last_bullets[1] == BULLET_Blank &&
               bullets.last_bullets[2] == BULLET_Blank) {
        bullets.streak_adjustment = 0.8f;
    } else {
        bullets.streak_adjustment = 1.0f;
    }
    
    // Update opponent model
    if (their_action == ACTION_ShootSelf) {
        opponent.self_shot_ratio = opponent.self_shot_ratio * 0.9f + 0.1f;
    } else {
        opponent.self_shot_ratio = opponent.self_shot_ratio * 0.9f;
        opponent.aggression = opponent.aggression * 0.9f + 0.1f;
    }
    
    if (their_item != ACTION_NoItem) {
        opponent.item_usage[their_item]++;
    }
}
