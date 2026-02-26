#include "Render.h"
#include "Animate.h"

static RenderItems RItems;
static Textures RTextures;
static int current_story_text = 0;
static int story_timer = 0;
static bool story_completed = false;
static bool characters_introduced = false;


struct StoryText {
    std::string text;
    sf::Color color;
    int display_time; // in frames
    sf::IntRect image_rect; // Texture rectangle for the associated image
    bool show_image; // Whether to show an image with this text
};

static std::vector<StoryText> story_sequence = {
    {"Once upon a time...", {255, 255, 255}, 120, {0, 0, 0, 0}, false},
    {"A curious girl named Alice", {255, 255, 0}, 120, {64, 32, 60, 60}, true},
    {"Fell down a rabbit hole", {255, 192, 203}, 120, {0, 0, 0, 0}, false},
    {"Into a world of wonder", {0, 255, 255}, 120, {0, 0, 0, 0}, false},
    {"Where cheshires chase", {255, 0, 0}, 120, {0, 126, 32, 32}, true}, 
    {"And mashrooms give power", {248, 187, 85}, 120, {0, 256, 32, 32}, true}, 
    {"Collect them all", {255, 255, 255}, 120, {0, 0, 0, 0}, false},
    {"To survive the maze", {255, 255, 0}, 120, {0, 0, 0, 0}, false},
    {"MEET THE CHARACTERS..", {255, 255, 255}, 120, {0, 0, 0, 0}, false} 
};

void InitRender()
{
	InitTextures();
	InitWalls();
	RItems.pellet_va.setPrimitiveType(sf::Quads);
	RItems.sprite_va.setPrimitiveType(sf::Quads);
	RItems.text_va.setPrimitiveType(sf::Quads);

	RItems.wall_map.setTexture(RTextures.wall_map_t);
	RItems.wall_map.setScale({ 0.5,0.5 });

	InitPellets();

	for (int i = 0; i < 4; i++)
	{
		RItems.monsters[i].setTexture(RTextures.sprites);
		RItems.monsters[i].setScale({ 0.5,0.5 });
		RItems.monsters[i].setOrigin({ 16,16 });
	}

	RItems.player.setTexture(RTextures.sprites);
	RItems.player.setScale({ 0.5,0.5 });
	RItems.player.setOrigin({ 15,15 });

	RItems.float_score.setTexture(RTextures.sprites);
	RItems.float_score.setScale({ 0.5,0.5 });
	RItems.float_score.setOrigin({ 16,16 });

	current_story_text = 0;
    story_timer = 0;
    story_completed = false;
    characters_introduced = false;

}

bool ShouldDisplayCharacters() {
    return characters_introduced;
}
void MakeQuad(sf::VertexArray& va, float x, float y, int w, int h, sf::Color color, sf::FloatRect t_rect)
{
	sf::Vertex vert;
	vert.color = color;
	vert.position = { x,y };
	vert.texCoords = { t_rect.left,t_rect.top };
	va.append(vert);
	vert.position = { x + w,y };
	vert.texCoords = { t_rect.left+t_rect.width,t_rect.top };
	va.append(vert);
	vert.position = { x + w,y + h };
	vert.texCoords = { t_rect.left+t_rect.width,t_rect.top+t_rect.height };
	va.append(vert);
	vert.position = { x,y + h };
	vert.texCoords = { t_rect.left,t_rect.top+t_rect.height };
	va.append(vert);
}
void InitWalls()
{
	RItems.wall_va.setPrimitiveType(sf::Quads);
	for (int y = 0; y < gState.board.size(); y++) {
		for (int x = 0; x < gState.board.at(y).size(); x++) {
			if(gState.board.at(y).at(x)== '|')
				MakeQuad(RItems.wall_va, x * TSIZE, y * TSIZE + YOFFSET, TSIZE, TSIZE, { 150,150,150 });
		}
	}
}
void InitTextures()
{
	RTextures.pellets.loadFromFile("assets/textures/dots.png");
	RTextures.sprites.loadFromFile("assets/textures/sprites.png");
	RTextures.wall_map_t.loadFromFile("assets/textures/map.png");
	RTextures.font.loadFromFile("assets/textures/font.png");
	RTextures.wall_map_t_white.loadFromFile("assets/textures/map.png");

	RTextures.wall_map_t_white.loadFromFile("assets/textures/map_white.png");
}

void InitPellets()
{
	RItems.pellet_va.clear();
	int VA_Index = 0;
	int Pow_index = 0;
	for (int y = 0; y < gState.board.size(); y++) {
		for (int x = 0; x < gState.board.at(y).size(); x++) {
			char temp = GetTile(x, y);
			if (temp == '.') {
				MakeQuad(RItems.pellet_va, x * TSIZE, y * TSIZE + YOFFSET, TSIZE, TSIZE, { 255,255,255 }, pel_r);
				RItems.pellet_va_indicies.insert({ y * 28 + x, VA_Index });
				VA_Index += 4;
			}
			else if (temp == 'o') {
				MakeQuad(RItems.pellet_va, x * TSIZE, y * TSIZE + YOFFSET, TSIZE, TSIZE, { 255,255,255 }, pow_r);
				RItems.pellet_va_indicies.insert({ y * 28 + x, VA_Index });
				RItems.pow_indicies[Pow_index] = VA_Index;
				Pow_index++;
				VA_Index += 4;
			}
		}
	}
}
void ResetPellets()
{
	for (int i = 0; i < RItems.pellet_va.getVertexCount(); i++) {
		RItems.pellet_va[i].color = { 255,255,255,255 };
	}

	RItems.wall_map.setTexture(RTextures.wall_map_t);
}
void RemovePellet(int x, int y)
{
	int va_idx = RItems.pellet_va_indicies.at(y * 28 + x);
	sf::Vertex* vert = &RItems.pellet_va[va_idx];
	vert[0].color = { 0,0,0,0 };
	vert[1].color = { 0,0,0,0 };
	vert[2].color = { 0,0,0,0 };
	vert[3].color = { 0,0,0,0 };
}
void DrawGameUI()
{
	ClearText();
	
	MakeText("High Score", 9, 0, { 255,255,255 });
	
	if (gState.game_state == GAMESTART)
		MakeText("Ready!", 11, 20, { 255,255,00 });
	else if (gState.game_state == GAMEOVER)
		MakeText("GAME  OVER", 9, 20, { 255,0,00 });

	std::string score  = std::to_string(gState.game_score);
	if (score.size() == 1)
		score.insert(score.begin(), '0');
	MakeText(score, 7-score.size(), 1, { 255,255,255 });

	score = std::to_string(gState.high_score);
	if (score.size() == 1)
		score.insert(score.begin(), '0');
	MakeText(score, 17 - score.size(), 1, { 255,255,255 });

	RItems.player.setTextureRect({ 256,32,30,30 });
	for (int i = 0; i < gState.player_lives; i++) {
		RItems.player.setPosition({ 24.f + 16 * i,35 * TSIZE });
		gState.window->draw(RItems.player);
	}
}
void FlashPPellets()
{
	sf::Uint8 new_alpha = (RItems.pow_is_off) ? 255 : 1;

	for (int i = 0; i < 4; i++) {
		int index = RItems.pow_indicies[i];
		sf::Vertex* vert = &RItems.pellet_va[index];
		// I am using alpha 0 to hide pellets, so for flashing, Ill just use alpha 1
		if (vert->color.a == 0)
			continue;
		vert[0].color.a = new_alpha;
		vert[1].color.a = new_alpha;
		vert[2].color.a = new_alpha;
		vert[3].color.a = new_alpha;
	}
}
void ResetPPelletFlash()
{
	RItems.pow_is_off = true;
	FlashPPellets();
}
void DrawFrame()
{
	gState.window->clear(sf::Color::Black);
	DrawGameUI();

	if (RItems.pow_is_off != IsPulse()) {
		FlashPPellets();
		RItems.pow_is_off = !RItems.pow_is_off;
	}

	static bool white_texture = false;
	if (gState.game_state == GAMEWIN) {
		if (IsPulse() && !white_texture) {
			RItems.wall_map.setTexture(RTextures.wall_map_t_white);
			white_texture = true;
		}
		else if (!IsPulse() && white_texture) {
			RItems.wall_map.setTexture(RTextures.wall_map_t);
			white_texture = false;
		}
	}
		
	gState.window->draw(RItems.wall_map);
	gState.window->draw(RItems.text_va, &RTextures.font);
	gState.window->draw(RItems.pellet_va, &RTextures.pellets);
	
	for (int i = 0; i < 4; i++) {
		if (!gState.monsters[i]->enable_draw)
			continue;
		RItems.monsters[i].setPosition(gState.monsters[i]->pos.x * TSIZE, gState.monsters[i]->pos.y * TSIZE + YOFFSET);
		RItems.monsters[i].setTextureRect(GetmonsterFrame(gState.monsters[i]->type, gState.monsters[i]->target_state, gState.monsters[i]->cur_dir));
		gState.window->draw(RItems.monsters[i]);
	}

	if (gState.player->enable_draw) {
		RItems.player.setPosition(gState.player->pos.x * TSIZE, gState.player->pos.y * TSIZE + YOFFSET);
		RItems.player.setTextureRect(GetaliceFrame(gState.player->cur_dir));
		
		gState.window->draw(RItems.player);
	}

	if (gState.player_eat_monster) {
		RItems.float_score.setPosition(gState.player->pos.x * TSIZE, gState.player->pos.y * TSIZE + YOFFSET);
		RItems.float_score.setTextureRect({ (gState.monsters_eaten_in_powerup - 1) * 32,256,32,32 });

		gState.window->draw(RItems.float_score);
	}

	gState.window->display();
}

void DrawMenuFrame()
{
    gState.window->clear(sf::Color::Black);
    DrawGameUI();
    
    // Title
    MakeText("Alice In Wonder Land", 4, 4, { 255, 255, 255 });
    
    // Story telling script
    if (!story_completed) {
        // Display the current story text
        if (current_story_text < story_sequence.size()) {
            // Center the text based on its length
            int text_x = 14 - (story_sequence[current_story_text].text.length() / 2);
            MakeText(story_sequence[current_story_text].text, 
                    std::max(1, text_x), 
                    12, 
                    story_sequence[current_story_text].color);
            
            // Display associated image if needed
            if (story_sequence[current_story_text].show_image) {
                // Set image in center of screen
                if (story_sequence[current_story_text].image_rect.width > 0) {
                    RItems.float_score.setPosition(14 * TSIZE, 18 * TSIZE);
                    RItems.float_score.setTextureRect(story_sequence[current_story_text].image_rect);
                    gState.window->draw(RItems.float_score);
                }
            }
            
            // Update the story timer
            story_timer++;
            
            // Move to the next text when timer expires
            if (story_timer >= story_sequence[current_story_text].display_time) {
                current_story_text++;
                story_timer = 0;
                
                // Check if we've reached the "MEET THE CHARACTERS" text
                if (current_story_text == story_sequence.size()) {
                    story_completed = true;
                    characters_introduced = false; // Will be set to true after a brief delay
                }
            }
        }
    }
    
    // Game item introductions - these appear after the story sequence completes
    if (story_completed) {
        // Show character introduction texts first
        if (!characters_introduced) {
            // Show "MEET THE CHARACTERS" for some time before showing actual characters
            if (story_timer < 60) { // 1 second delay
    
                story_timer++;
            } else {
                characters_introduced = true;
                story_timer = 0;
            }
        } else {

			MakeText("Pink Cheshire", 9, 8, {252, 181, 255});
			MakeText("Red Cheshire", 9, 11,{255, 0, 0} );
			MakeText("Blue Cheshire", 9, 14, {0, 255, 255});
			MakeText("Yellow Cheshire", 9, 17, {248, 187, 85});
			
			// Alice introduction
			MakeText("ALICE: YOU", 9, 20, {255, 255, 0});
			
			// Items introduction
			MakeText("COLLECT PELLETS TO WIN", 4, 25, {255, 255, 255});
			MakeText("POWER MASHROOMS HELP YOU", 3, 27, {255, 255, 255});
            
            
            // Draw characters only after they've been introduced
            for (int i = 0; i < 4; i++) {
                RItems.monsters[i].setPosition(gState.monsters[i]->pos.x * TSIZE, gState.monsters[i]->pos.y * TSIZE + YOFFSET);
                RItems.monsters[i].setTextureRect(GetmonsterFrame(gState.monsters[i]->type, gState.monsters[i]->target_state, gState.monsters[i]->cur_dir));
                gState.window->draw(RItems.monsters[i]);
            }
            
            RItems.player.setPosition(gState.player->pos.x * TSIZE, gState.player->pos.y * TSIZE + YOFFSET);
            RItems.player.setTextureRect(GetaliceFrame(gState.player->cur_dir));
            gState.window->draw(RItems.player);
        }
    }
    
    // Show 'ENTER' text flashing
    if (IsPulse())
        MakeText("PRESS ENTER TO START", 5, 30, {255, 255, 255});

    gState.window->draw(RItems.text_va, &RTextures.font);
    gState.window->display();
}








void ClearText()
{
	RItems.text_va.clear();
}
void MakeText(std::string string, int x, int y, sf::Color color)
{
	for (int i = 0; i < string.size(); i++) {
		sf::FloatRect font_let = { 0,0,16,16 };
		char letter = toupper(string.at(i));
		font_let.left = (letter - ' ') * 16 + 1;
		MakeQuad(RItems.text_va, x*TSIZE + 8 * i, y*TSIZE, 8, 8, color, font_let);
	}
}
void ResetMenuStory()
{
    current_story_text = 0;
    story_timer = 0;
    story_completed = false;
    characters_introduced = false;
}