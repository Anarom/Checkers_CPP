#pragma once
#include <vector>

struct Target {
	unsigned int target_cell_id = 0;
	bool is_queen = false;
};

struct Move {
	unsigned int origin = NULL;
	unsigned int destanation = NULL;
	bool promotes = false;
	bool by_white = true;
	std::vector<Target> targets;
};

struct PieceRuleset {
	unsigned int capture_directions;
	unsigned int shift_directions;
	int max_shift_distance;
	int after_take_max_distance;
	bool promotion_on_move;
	bool promotion_stop;
};

struct Board {
	std::vector<char> state;
	bool white_to_move = true;
};

struct GameRules {
	bool white_moves_first = true;
	unsigned int resolution = 0;
	unsigned int cell_size = 0;
	bool on_whites;
	bool always_capture;
	bool always_max_capture;
	PieceRuleset piece_ruleset;
	PieceRuleset queen_ruleset;
};

