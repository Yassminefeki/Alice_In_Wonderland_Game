#ifndef GAMELOOP_H
#define GAMELOOP_H
#include "alice.h"
#include "Render.h"
#include "monsters.h"
#include "Player.h"
#include "Animate.h"
#include "Sound.h"

void OnStart();
void OnQuit();
void GameLoop(int ms_elapsed);

void Init();
void InitBoard();
void ResetmonstersAndPlayer();
void ResetBoard();
void SetupMenu();

void IncrementmonsterHouse();
void CheckPelletCollision();
void CheckmonsterCollision();
void UpdateWave(int ms_elapsed);
void UpdateEnergizerTime(int ms_elasped);
void CheckWin();

void CheckHighScore();
void LoadHighScore();
void SaveHighScore();

// game states
void MainLoop(int ms_elasped);
void GameStart(int ms_elasped);
void GameLose(int ms_elasped);
void GameWin(int ms_elasped);
void Menu(int ms_elapsed);




#endif // !GAMELOOP_H

