#pragma once
#include "player.h"
class Game{
public:
	Game(GameRules rules, GUIPlayer::ColorPalette palette, std::string string = "PPPP/PPPP/PPPP/NNNN/NNNN/pppp/pppp/pppp/W");
	std::vector<Board> history;
	unsigned int result = 0;
	void run();
private:
	bool white_to_move = true;
	unsigned int draw_counter = 0;
	GUIPlayer player_1;
	GUIPlayer player_2;
	Engine engine;
	void update(Board board);
};

