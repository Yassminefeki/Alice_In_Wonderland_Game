#ifndef monster_H
#define monster_H
#include "alice.h"

bool InMiddleTile(sf::Vector2f pos, sf::Vector2f prev, Dir dir);
std::vector<Dir> GetAvailableSquares(sf::Vector2f pos, Dir dir, bool home_tiles);
float Distance(int x, int y, int x1, int y1);
Dir GetShortestDir(const std::vector<Dir>& squares, const monster& monster, sf::Vector2f target);
Dir GetOppositeTile(monster& monster);

sf::Vector2f Red_CheshireUpdate(monster& monster);
sf::Vector2f Pink_CheshireUpdate(monster& monster);
sf::Vector2f Blue_CheshireUpdate(monster& monster);
sf::Vector2f Yellow_CheshireUpdate(monster& monster);

bool PassedEntrence(monster& monster);
void HouseUpdate(monster& monster);

void UpdateDirection(std::vector<Dir> squares, monster& monster);
void Updatemonsters();

void SetAllmonsterState(TargetState new_state);
void SetmonsterState(monster& monster, TargetState new_state);

const float starting_x[4] = { 14,14,12,16 };
const Dir enter_dir[4] = { UP, UP, LEFT, RIGHT };
const sf::Vector2f corners[4] = { {31,0},{0,0},{31,31}, {0,31} };
const int dot_counters[4] = { 0,0,30,60 };

const int global_dot_limit[4] = { 0,7,17,32 };

#endif // !monster_H

