#include "Animate.h"

static Animation animate;

sf::IntRect GetmonsterFrame(monsterType type, TargetState state, Dir dir)
{
	sf::IntRect monster = { 0,128,32,32 };

	int offset = 0;
	if (state != FRIGHTENED) {
		switch (dir)
		{
		case UP:
			offset = 128;
			break;
		case DOWN:
			offset = 128 + 64;
			break;
		case LEFT:
			offset = 64;
			break;
		}
	}
	
	if (state == FRIGHTENED) {
		monster.left = 256;
		if (animate.fright_flash)
			monster.left += 64;
	}
	else if (state == GOHOME || state == ENTERHOME) {
		monster.left = 256 + (offset/2);
		monster.top = 128 + 32;
	}
	else {
		monster.top += 32 * type;
		monster.left = offset;
	}
	if(state != GOHOME && state != ENTERHOME)
		monster.left += animate.monster_frame_2 * 32;

	return monster;
}
void AnimateUpdate(int ms_elapsed)
{
	PulseUpdate(ms_elapsed);

	if (gState.player->stopped && !animate.death_animation) {
		animate.alice_frame = 0;
		animate.assending = true;
	}
	else {
		animate.alice_timer += ms_elapsed;
	}
	
	if (animate.death_animation) {
		if (animate.alice_timer > 100) {
			animate.alice_timer = 0;
			animate.alice_frame++;
		}
		if (animate.alice_frame > 10)
			gState.player->enable_draw = false;
	}
	else if (animate.alice_timer > 25 && !gState.player->stopped) {
		animate.alice_frame += (animate.assending) ? 1 : -1;
		animate.alice_timer = 0;
	}
	if (!animate.death_animation && animate.alice_frame > 2 || animate.alice_frame < 0) {
		animate.assending = !animate.assending;
		animate.alice_frame = (animate.alice_frame > 2) ? 2 : 0;
	}

	animate.monster_timer += ms_elapsed;
	if (animate.monster_timer > 200) {
		animate.monster_frame_2 = !animate.monster_frame_2;
		animate.monster_timer = 0;
	}

	// start flashing with 2 seconds to go
	if (gState.energizer_time > 0 && gState.energizer_time < 2000) {
		animate.energrizer_timer += ms_elapsed;
		if (animate.energrizer_timer > 200) {
			animate.fright_flash = !animate.fright_flash;
			animate.energrizer_timer = 0;
		}
	}
	else animate.fright_flash = false;
}
sf::IntRect GetaliceFrame(Dir dir)
{
	sf::IntRect rect = { 0,0,30,30 };
	rect.left = (2 - animate.alice_frame) * 32;

	if (animate.death_animation) {
		rect.left = 96 + animate.alice_frame * 32;

		return rect;
	}

	if (animate.alice_frame == 0)
		return rect;

	switch (dir)
	{
	case UP:
		rect.top = 64;
		break;
	case DOWN:
		rect.top = 96;
		break;
	case LEFT:
		rect.top = 32;
	}

	return rect;
}
void StartaliceDeath()
{
	animate.death_animation = true;
	animate.alice_frame = 0;
	animate.alice_timer = -250;
}
void ResetAnimation()
{
	animate.alice_frame = 0;
	animate.death_animation = false;
}
void SetAliceMenuFrame()
{
	animate.alice_frame = 1;
	animate.death_animation = false;

	animate.monster_frame_2 = false;
}
void PulseUpdate(int ms_elapsed)
{
	animate.pulse_timer += ms_elapsed;
	if (animate.pulse_timer > animate.pulse_limit) {
		animate.pulse = !animate.pulse;
		animate.pulse_timer = 0;
	}
}
void SetPulseFrequency(int ms)
{
	animate.pulse_limit = ms;
}
bool IsPulse()
{
	return animate.pulse;
}