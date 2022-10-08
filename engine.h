#pragma once
#include <array>
#include <string>
#include "structs.h"

class Engine {
private:
	typedef std::array<unsigned int, 2> uvector;

public:
	GameRules rules;

	struct MoveData {
		unsigned int cell_id;
		uvector uvector{ 2,2 };
		bool promotes;
		std::vector<Target> targets;
	};


	struct CellInfo
	{
		bool has_value = false;
		unsigned int pos = 0;
		bool is_white = true;
		bool is_queen = false;
		PieceRuleset ruleset = {};
	};

	std::vector<Move> get_player_moves(Board board,bool is_white);
	CellInfo get_cell_info(Board board, unsigned int cell_id);
	char get_cell_code(Board board, unsigned int cell_id);
	Board play_move(Board board, Move move);
	Board load_board(std::string string);
	std::string export_board(Board board, bool white_to_move = true, int depth = -1);

private:
	Move construct_move(CellInfo piece_info, MoveData data);
	std::vector<Move> enforce_capture_rules(std::vector<Move> moves);
	std::vector<Move> get_piece_moves(Board board, CellInfo piece_info);
	std::vector<MoveData> get_move_data(Board board, CellInfo piece_info, uvector uvector, unsigned int provided_cell_id = 0, bool promoted = false);
	std::vector<Move> get_captures(Board board ,CellInfo piece_info, MoveData data, unsigned int depth = 0);
	std::vector<Move> extend_move(Board board, Move move);
	unsigned int get_neighbour(Board board,unsigned int src_cell_id, uvector uvector);
	bool target_in_targets(std::vector<Target> targets, Target target);
	bool can_promote(Board board, int cell_id, CellInfo piece_info);
};

