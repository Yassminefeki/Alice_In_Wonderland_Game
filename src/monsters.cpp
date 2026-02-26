#include "monsters.h"

bool InMiddleTile(sf::Vector2f pos, sf::Vector2f prev, Dir dir)
{
	if ((int)pos.x != (int)prev.x || (int)pos.y != (int)prev.y)
		return false;
	float x = pos.x - (int)pos.x;
	float y = pos.y - (int)pos.y;

	float px = prev.x - (int)prev.x;
	float py = prev.y - (int)prev.y;

	switch (dir)
	{
	case UP:
	case DOWN:
		return std::min(y, py) <= 0.5 && std::max(y, py) >= 0.5;
		break;

	case LEFT:
	case RIGHT:
		return std::min(x, px) <= 0.5 && std::max(x, px) >= 0.5;
		break;
	}
	return false;
}
// this should be cached/hardcoded, but its performant enough
std::vector<Dir> GetAvailableSquares(sf::Vector2f pos, Dir dir, bool home_tile)
{
	std::vector<Dir> squares;
	// this is the reverse order of precedence that
	// the original alice game used
	// up is picked before left if the distances are tied etc.
	if (!TileCollision(pos + sf::Vector2f{ 0.9, 0 },home_tile ))
		squares.push_back(RIGHT);
	if (!TileCollision(pos + sf::Vector2f{ 0, 0.9 }, home_tile))
		squares.push_back(DOWN);
	if (!TileCollision(pos + sf::Vector2f{ -0.9, 0 }, home_tile))
		squares.push_back(LEFT);
	if (!TileCollision(pos + sf::Vector2f{ 0, -0.9 }, home_tile))
		squares.push_back(UP);
	for (int i = 0; i < squares.size(); i++) {
		if (squares[i] == opposite_dir[dir])
			squares.erase(squares.begin() + i);
	}
	return squares;
}
// returns squared distance
float Distance(int x, int y, int x1, int y1)
{
	return ((x - x1) * (x - x1)) + ((y - y1) * (y - y1));
}

// the original game didnt use a pathfinding algorithm
// simple distance comparisons are fine enough
Dir GetShortestDir(const std::vector<Dir>& squares, const monster& monster, sf::Vector2f target)
{
	int min_dist = 20000000;
	Dir min_dir = NONE;
	if (squares.size() == 0)
		printf("EMPTY\n");

	for (auto dir : squares) {
		sf::Vector2f square = monster.pos + dir_addition[dir];
		float dir_dist = Distance(target.x, target.y, square.x, square.y);
		if (dir_dist <= min_dist) {
			min_dir = dir;
			min_dist = dir_dist;
		}
	}

	return min_dir;
}
sf::Vector2f Red_CheshireUpdate(monster& monster)
{
	return gState.player->pos;
}
sf::Vector2f Pink_CheshireUpdate(monster& monster)
{
	return gState.player->pos + dir_addition[gState.player->cur_dir] * 4;
}
sf::Vector2f Blue_CheshireUpdate(monster& monster)
{
	sf::Vector2f target = gState.player->pos + dir_addition[gState.player->cur_dir] * 2;
	sf::Vector2f offset = gState.monsters[RED]->pos - target;
	target = target + offset * -1;
	return target;
}
sf::Vector2f Yellow_CheshireUpdate(monster& monster)
{
	sf::Vector2f target = gState.player->pos;

	// if Yellow_Cheshire is 8 tiles away, target player, else target corner
	if (Distance(monster.pos.x, monster.pos.y, gState.player->pos.x, gState.player->pos.y) < 64)
		target = { 0,31 };
	return target;
}
bool PassedEntrence(monster& monster)
{
	float prev_x = monster.pos.x - dir_addition[monster.cur_dir].x * monster.move_speed;

	return FComp(monster.pos.y, 11.5)
		&& std::min(monster.pos.x, prev_x) <= 14.1
		&& std::max(monster.pos.x, prev_x) >= 13.9;
}
void UpdateDirection(std::vector<Dir> squares, monster& monster)
{
	sf::Vector2f target;
	bool update_dir = false;
	if (squares.size() == 1) {
		monster.cur_dir = squares.at(0);
	}

	switch (monster.target_state)
	{
	case FRIGHTENED:
		monster.cur_dir = squares.at(rand() % (squares.size()));
		break;
	case GOHOME:
		target = { 14,11.5 };
		update_dir = true;
		break;
	case CHASE:
		switch (monster.type)
		{
		case RED:
			target = Red_CheshireUpdate(monster);
			break;
		case PINK:
			target = Pink_CheshireUpdate(monster);
			break;
		case BLUE:
			target = Blue_CheshireUpdate(monster);
			break;
		case ORANGE:
			target = Yellow_CheshireUpdate(monster);
			break;
		}
		update_dir = true;
		break;
	case CORNER:
		target = corners[monster.type];
		update_dir = true;
		break;
	}

	if (update_dir)
		monster.cur_dir = GetShortestDir(squares, monster, target);

	CenterObject(monster.cur_dir, monster.pos);
}
// held together with duct tape
// my waypoint system works on the assumption that movement is only
// made in the center of tiles, in the house, movement is made off center,
// which breaks it
void HouseUpdate(monster& monster)
{
	switch (monster.target_state)
	{
	case ENTERHOME:
		if (monster.pos.y >= 15) {
			monster.cur_dir = enter_dir[monster.type];
			monster.pos.y = 15;
			if (FComp(monster.pos.x, starting_x[monster.type])
				|| (monster.cur_dir == LEFT && monster.pos.x <= starting_x[monster.type])
				|| (monster.cur_dir == RIGHT && monster.pos.x >= starting_x[monster.type])) {
				monster.target_state = HOMEBASE;
				monster.move_speed = inhome_speed;
			}
		}
		break;
	case LEAVEHOME:
		if (FComp(monster.pos.x, 14) || (monster.cur_dir == LEFT && monster.pos.x <= 14) || (monster.cur_dir == RIGHT && monster.pos.x >= 14)) {
			monster.cur_dir = UP;
			monster.pos.x = 14;
			if (monster.pos.y <= 11.5) {
				monster.move_speed = monster_speed;
				monster.target_state = GetGlobalTarget();
				monster.pos.y = 11.5;
				monster.in_house = false;
				monster.cur_dir = LEFT;
			}
		}
		else {
			monster.cur_dir = opposite_dir[enter_dir[monster.type]];
		}
		break;
	case HOMEBASE:
		monster.move_speed = inhome_speed;

		// check timer if its time to leave
		if (gState.using_global_counter) {
			if (gState.global_dot_counter >= global_dot_limit[monster.type])
				monster.target_state = LEAVEHOME;
		}
		else if (monster.dot_counter >= dot_counters[monster.type])
			monster.target_state = LEAVEHOME;

		if (monster.pos.y <= 14) {
			monster.pos.y = 14;
			monster.cur_dir = DOWN;
		}
		else if (monster.pos.y >= 15) {
			monster.pos.y = 15;
			monster.cur_dir = UP;
		}
		break;
	}
}
void Updatemonsters()
{
	for (int i = 0; i < 4; i++) {
		monster* monster = gState.monsters[i];
		sf::Vector2f prev_pos = monster->pos;

		monster->pos += dir_addition[monster->cur_dir] * monster->move_speed;
		if (monster->in_house) {
			HouseUpdate(*monster);
		}
		// if monster pos is in the middle of the tile, didnt update last turn, and isnt in the tunnel, 
		// do waypoint calculations, else do nothing
		else if (!monster->update_dir &&
			InMiddleTile(monster->pos, prev_pos, monster->cur_dir) &&
			!InTunnel(monster->pos)) {
			UpdateDirection(GetAvailableSquares(monster->pos, monster->cur_dir, false), *monster);
			monster->update_dir = true;
		}
		else {
			monster->update_dir = false;
		}

		// blech, at least it terminates quickly
		if (monster->target_state == GOHOME &&
			PassedEntrence(*monster)) {
			monster->target_state = ENTERHOME;
			monster->in_house = true;
			monster->cur_dir = DOWN;
			//monster->move_speed = 0.02;
			monster->pos.x = 14;
		}

		// tunneling
		if (monster->pos.x < -1) {
			monster->pos.x += 29;
		}
		else if (monster->pos.x >= 29) {
			monster->pos.x -= 29;
		}
	}
}
Dir GetOppositeTile(monster& monster)
{
	// this will be the case 99% of times
	if (!TileCollision(monster.pos + dir_addition[opposite_dir[monster.cur_dir]] * 0.9))
		return opposite_dir[monster.cur_dir];


	auto squares = GetAvailableSquares(monster.pos, monster.cur_dir, false);
	if (!squares.empty())
		return squares.at(0);

	// last case scenario, dont reverse
	return monster.cur_dir;

}
void SetAllmonsterState(TargetState new_state)
{
	for (int i = 0; i < 4; i++) {
		SetmonsterState(*gState.monsters[i], new_state);
	}
}

void SetmonsterState(monster& monster, TargetState new_state)
{
	if (monster.target_state == GOHOME)
		return;
	switch (new_state)
	{
	case LEAVEHOME:
		if (monster.in_house)
			monster.target_state = new_state;
	case ENTERHOME:
	case HOMEBASE:
		break;
	case FRIGHTENED:
		if (!monster.in_house) {
			monster.cur_dir = GetOppositeTile(monster);
			monster.move_speed = monster_fright;
			monster.target_state = new_state;
		}
		break;
	case GOHOME:
		if (!monster.in_house) {
			monster.move_speed = gohome_speed;
			monster.target_state = new_state;
		}
		break;
	default:
		if (!monster.in_house) {
			// monster reverses direction whenever changing states,
			// except when coming from frightened
			if (monster.target_state != FRIGHTENED)
				monster.cur_dir = GetOppositeTile(monster);
			monster.move_speed = monster_speed;
			monster.target_state = new_state;
		}
		break;
	}
}
