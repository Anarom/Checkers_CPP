#include "game.h"


Game::Game(GameRules rules, GUIPlayer::ColorPalette palette, std::string string) : player_1(true, rules, palette), player_2(false, rules, palette) {
	Game::player_1 = player_1;
	Game::player_2 = player_2;
	Game::engine.rules = rules;
	Game::white_to_move = rules.white_moves_first;
	Game::update(Game::engine.load_board(string));
}

void Game::update(Board board) {
	Game::player_1.update(board);
	Game::player_2.update(board);
	Game::history.push_back(board);
}

void Game::run() {
	bool game_over = false;
	unsigned int draw_counter = 0;
	std::vector<Move> moves;
	Move move;
	GUIPlayer active_player = Game::engine.rules.white_moves_first ? player_1 : player_2;
	while (not game_over) {
		moves = Game::engine.get_player_moves(Game::history.back(), active_player.is_white);
		if (moves.empty()) {
			moves = Game::engine.get_player_moves(Game::history.back(), not active_player.is_white);
			if (moves.empty()) {
				Game::result = 1;
			}
			else {
				Game::result = active_player.is_white ? 3 : 2;
			}
			game_over = true;
		}
		else if (Game::draw_counter == 10) {
			Game::result = 1;
			game_over = true;
		}
		else {
			move = active_player.choose_move(moves);
			if (Game::engine.get_cell_info(Game::history.back(), move.origin).is_queen and move.targets.empty()) {
				Game::draw_counter++;
			}
			else {
				draw_counter = 0;
			}
			Board new_board = Game::engine.play_move(Game::history.back(), move);
			Game::update(new_board);
			active_player = active_player.is_white == player_1.is_white ? player_2 : player_1;
		}
	}
}