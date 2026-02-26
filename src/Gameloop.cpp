#include "Gameloop.h"
#include "Render.h"
#include "alice.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
GameState gState;


void OnStart()
{
	Init();
	LoadHighScore();
}
void OnQuit()
{
	SaveHighScore();
}

void LoadHighScore()
{
	std::ifstream infile;
	std::stringstream ss;
	std::string line;
	int hs = 0;
	infile.open("highscore.txt");
	if (!infile) {
		gState.high_score = 0;
		return;
	}
	getline(infile, line);
	ss << line;
	ss >> hs;

	gState.high_score = hs;
}
void SaveHighScore()
{
	std::ofstream outfile("highscore.txt");
	if (!outfile.is_open())
		printf("Cant open file!");
	
	outfile << gState.high_score;
	outfile.close();
}
void InitBoard()
{
	std::string line;
	std::ifstream infile("assets/Map.txt");
	if (!infile)
		return;
	while (getline(infile, line))
	{
		gState.board.push_back(line);
	}

	infile.close();

}
void Init()
{
	Player* pl = new Player();
	pl->cur_dir = UP;
	pl->pos = { 14,23.5 };
	pl->stopped = true;
	gState.player = pl;

	// monsters init

	monster* temp = new monster();
	temp->type = RED;
	gState.monsters.push_back(temp);

	temp = new monster();
	temp->type = PINK;
	gState.monsters.push_back(temp);

	temp = new monster();
	temp->type = BLUE;
	gState.monsters.push_back(temp);

	temp = new monster();
	temp->type = ORANGE;
	gState.monsters.push_back(temp);

	InitBoard();
	InitRender();
	ResetmonstersAndPlayer();
	
	InitSounds();
	
	SetupMenu();
	gState.game_state = MENU;
	gState.pause_time = 2000;
}
void ResetmonstersAndPlayer()
{
	monster* temp = gState.monsters[0];
	temp->pos = { 14, 11.5 };
	temp->cur_dir = LEFT;
	temp->target_state = CORNER;
	temp->in_house = false;
	temp->move_speed = monster_speed;
	//temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.monsters[1];
	temp->pos = { 14, 14.5 };
	temp->cur_dir = UP;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	//temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.monsters[2];
	temp->pos = { 12, 14.5 };
	temp->cur_dir = DOWN;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	//temp->dot_counter = 0;
	temp->enable_draw = true;

	temp = gState.monsters[3];
	temp->pos = { 16, 14.5 };
	temp->cur_dir = DOWN;
	temp->target_state = HOMEBASE;
	temp->in_house = true;
	temp->move_speed = inhome_speed;
	//temp->dot_counter = 0;
	temp->enable_draw = true;

	gState.player->cur_dir = UP;
	gState.player->pos = { 14,23.5 };
	gState.player->stopped = true;
	gState.player->enable_draw = true;

	ResetAnimation();
	gState.energizer_time = 0;
	gState.wave_counter = 0;
	gState.wave_time = 0;

	ResetPPelletFlash();

	if (!gState.first_life)
		gState.using_global_counter = true;
	
	gState.global_dot_counter = 0;
}
void ResetBoard()
{
	gState.board.clear();
	InitBoard();

	gState.pellets_left = 244;

	gState.first_life = true;
	gState.using_global_counter = false;
	for (int i = 0; i < 4; i++)
		gState.monsters[i]->dot_counter = 0;
}

void IncrementmonsterHouse()
{
	monster* first_monster = nullptr;
	// using global counter, increment it
	if (gState.using_global_counter) {
		gState.global_dot_counter++;
	}
	for (int i = 0; i < 4; i++) {
		if (gState.monsters[i]->target_state == HOMEBASE) {
			first_monster = gState.monsters[i];
			break;
		}
	}
	if (first_monster == nullptr) {
		// no more monsters in house, switch back to local counters
		if (gState.using_global_counter) {
			gState.using_global_counter = false;
		}
	}
	// if not using global and monster is in house, use local counter
	else if(!gState.using_global_counter) {
		first_monster->dot_counter++;
	}
}
void CheckPelletCollision()
{
	char tile = GetTile(gState.player->pos.x, gState.player->pos.y);
	bool collided = false;
	if (tile == '.') {
		collided = true;
		// play sound
		gState.game_score += 10;
		PlayMunch();
	}
	else if (tile == 'o') {
		collided = true;
		gState.game_score += 50;
		gState.energizer_time = fright_time*1000;

		gState.monsters_eaten_in_powerup = 0;

		SetAllmonsterState(FRIGHTENED);
	}

	if (collided) {
		RemovePellet(gState.player->pos.x, gState.player->pos.y);
		SetTile(gState.player->pos.x, gState.player->pos.y, ' ');
		IncrementmonsterHouse();
		gState.pellet_eaten = true;
		gState.pellets_left--;
	}
}
void CheckmonsterCollision()
{
	int px = (int)gState.player->pos.x;
	int py = (int)gState.player->pos.y;

	for (int i = 0; i < 4; i++) {
		if ((int)gState.monsters[i]->pos.x == px && (int)gState.monsters[i]->pos.y == py) {
			if (gState.monsters[i]->target_state == FRIGHTENED) {
				SetmonsterState(*gState.monsters[i], GOHOME);
				gState.recent_eaten = gState.monsters[i];
				gState.monsters_eaten_in_powerup++;
				gState.game_score += (pow(2,gState.monsters_eaten_in_powerup) * 100);
				
				gState.player_eat_monster = true;
				gState.pause_time = 500;

				gState.monsters[i]->enable_draw = false;
				gState.player->enable_draw = false;

				PlayEatmonster();
			}
			else if (gState.monsters[i]->target_state != GOHOME) {
				gState.game_state = GAMELOSE;
				gState.pause_time = 2000;
				gState.player_lives -= 1;
				gState.first_life = false;
				StartaliceDeath();
				StopSounds();
				PlayDeathSound();
				printf("RESET\n");
			}
		}
	}
}
void UpdateWave(int ms_elapsed)
{
	// indefinte chase mode
	if (gState.wave_counter >= 7)
		return;

	gState.wave_time += ms_elapsed;
	if (gState.wave_time / 1000 >= wave_times[gState.wave_counter]) {
		gState.wave_counter++;
		printf("New wave\n");
		if(gState.energizer_time <= 0)
			SetAllmonsterState(GetGlobalTarget());
		gState.wave_time = 0;
	}

}
void UpdateEnergizerTime(int ms_elasped)
{
	if (gState.energizer_time <= 0)
		return;

	gState.energizer_time -= ms_elasped;
	if (gState.energizer_time <= 0) {
		SetAllmonsterState(GetGlobalTarget());
	}
}
void CheckHighScore()
{
	if (gState.game_score > gState.high_score)
		gState.high_score = gState.game_score;
}
void CheckWin()
{
	if (gState.pellets_left <= 0) {
		gState.game_state = GAMEWIN;
		for (int i = 0; i < 4; i++) {
			gState.monsters[i]->enable_draw = false;
		}
		gState.player->stopped = true;
		gState.pause_time = 2000;
		StopSounds();
		SetPulseFrequency(200);
	}
}
void MainLoop(int ms_elapsed)
{
	if (gState.player_eat_monster) {
		gState.pause_time -= ms_elapsed;
		if (gState.pause_time < 0) {
			gState.recent_eaten->enable_draw = true;
			gState.player->enable_draw = true;
			gState.player_eat_monster = false;
		}
		DrawFrame();

		return;
	}

	// alice doesnt move for one frame if he eats a pellet
	// from the original game
	if (!gState.pellet_eaten)
		PlayerMovement();
	else gState.pellet_eaten = false;
	// check collision first so less funny stuff
	CheckmonsterCollision();
	CheckPelletCollision();
	Updatemonsters();
	UpdateWave(ms_elapsed);
	UpdateEnergizerTime(ms_elapsed);
	CheckHighScore();
	CheckWin();
	UpdateGameSounds(ms_elapsed);

	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void GameStart(int ms_elasped)
{
	gState.pause_time -= ms_elasped;
	if (gState.pause_time <= 0) {
		gState.game_state = MAINLOOP;
		SetPulseFrequency(150);
	}

	DrawFrame();
}
void GameLose(int ms_elapsed)
{
	gState.pause_time -= ms_elapsed;
	if (gState.pause_time <= 0) {
		if (gState.player_lives < 0) {
			gState.game_state = GAMEOVER;
			gState.pause_time = 5000;
			for (int i = 0; i < 4; i++)
				gState.monsters[i]->enable_draw = false;
			gState.player->enable_draw = false;
		}
		else {
			gState.game_state = GAMESTART;
			gState.pause_time = 2000;
			
			ResetmonstersAndPlayer();
		}
	}
	UpdateGameSounds(ms_elapsed);
	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void GameWin(int ms_elapsed)
{
	gState.pause_time -= ms_elapsed;
	if (gState.pause_time <= 0) {
		ResetPellets();
		ResetBoard();
		ResetmonstersAndPlayer();
		gState.pause_time = 2000;
		gState.game_state = GAMESTART;
	}
	AnimateUpdate(ms_elapsed);
	DrawFrame();
}
void SetupMenu()
{
	for (int i = 0; i < 4; i++) {
		gState.monsters[i]->enable_draw = true;
		gState.monsters[i]->pos = { 6,5.5f + i * 3.f };
		gState.monsters[i]->cur_dir = RIGHT;
		gState.monsters[i]->target_state = CHASE;
		gState.monsters[i]->in_house = false;
	}
	gState.player->enable_draw = true;
	gState.player->pos = { 6,17.5f };
	gState.player->cur_dir = RIGHT;
	SetAliceMenuFrame();
	SetPulseFrequency(200);
}


void Menu(int ms_elapsed)
{
	PulseUpdate(ms_elapsed);
	DrawMenuFrame();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
	{
		ResetPellets();
		ResetBoard();
		ResetmonstersAndPlayer();
		gState.game_score = 0;
		gState.player_lives = 3;
		PlayGameStart();
		gState.pause_time = 4000;
		gState.game_state = GAMESTART;
	}
}
void GameLoop(int ms_elapsed)
{
	switch (gState.game_state)
	{
	case MAINLOOP:
		MainLoop(ms_elapsed);
		break;
	case GAMESTART:
		GameStart(ms_elapsed);
		break;
	case GAMELOSE:
		GameLose(ms_elapsed);
		break;
	case GAMEOVER:
		gState.pause_time -= ms_elapsed;
		if (gState.pause_time < 0) {
			SetupMenu();
			gState.game_state = MENU;
		}
			//gState.game_state = MENU;
		DrawFrame();
		break;
	case GAMEWIN:
		GameWin(ms_elapsed);
		break;
	case MENU:
		Menu(ms_elapsed);
		break;
	}

}

