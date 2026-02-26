#ifndef ANIMATE_H
#define ANIMATE_H
#include "alice.h"

struct Animation
{
	int alice_frame=0;
	int alice_timer=0;
	bool assending = true;
	
	bool monster_frame_2 = false;
	int monster_timer = 0;

	int fright_flash = false;
	int energrizer_timer = 0;

	bool death_animation = false;

	// genertic pulse for win condition and energizer
	int pulse = true;
	int pulse_timer = 0;

	int pulse_limit = 200;
};

void AnimateUpdate(int ms_elapsed);
void StartaliceDeath();
void ResetAnimation();
void SetAliceMenuFrame();

sf::IntRect GetmonsterFrame(monsterType type, TargetState state, Dir dir);
sf::IntRect GetaliceFrame(Dir dir);

void PulseUpdate(int ms_elapsed);
bool IsPulse();
void SetPulseFrequency(int ms);

#endif // !ANIMATE_H
