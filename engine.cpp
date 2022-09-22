#include <iostream>
#include <algorithm>
#include <math.h>
#include "engine.h"
#include <Windows.h>

Move Engine::construct_move(Engine::CellInfo piece_info, Engine::MoveData data) {
	Move move = {
		.origin = piece_info.pos,
		.destanation = data.cell_id,
		.promotes = data.promotes,
		.by_white = piece_info.is_white,
		.targets = data.targets
	};
	return move;
}

std::vector<Move> Engine::enforce_capture_rules(std::vector<Move> moves) {
	unsigned int size;
	unsigned int max_captures = 0;
	unsigned int captures_to_return = 0;
	std::vector<Move> new_moves;
	for (Move move : moves) {
		if (not move.targets.empty()) {
			if (Engine::rules.always_max_capture) {
				size = move.targets.size();
				if (size < max_captures) {
					continue;
				}
				else if (size == max_captures) {
					captures_to_return++;
				}
				else {
					captures_to_return = 1;
					max_captures = size;
				}
			}
			new_moves.push_back(move);
		}
	}
	new_moves.erase(new_moves.begin(), new_moves.end() - captures_to_return);
	return new_moves;
}

std::vector<Move> Engine::get_piece_moves(Board board, CellInfo piece_info) {
	std::vector<Move> shifts;
	std::vector<Move> captures;
	std::vector<Move> extended_moves;
	std::vector<Engine::MoveData> move_data;
	Engine::uvector uvector;
	Move move;
	bool right_direction;
	for (unsigned int y_direction = 0; y_direction < piece_info.ruleset.capture_directions; y_direction++) {
		right_direction = y_direction < piece_info.ruleset.shift_directions;
		for (unsigned int x_direction = 0; x_direction < 2; x_direction++) {
			uvector[0] = piece_info.is_white ? y_direction : 1 - y_direction;
			uvector[1] = x_direction;
			move_data = Engine::get_move_data(board, piece_info, uvector);
			for (Engine::MoveData data : move_data) {
				if (data.uvector[0] != 2) {
					for (Move move : Engine::get_captures(board, piece_info, data)) {
						extended_moves = Engine::extend_move(board, move);
						captures.insert(captures.end(), extended_moves.begin(), extended_moves.end());
					}
				}
				else if (right_direction) {
					shifts.push_back(Engine::construct_move(piece_info, data));
				}
			}
		}
	}
	if (not Engine::rules.always_capture) {
		captures.insert(captures.end(), shifts.begin(), shifts.end());
		return captures;
	}
	else {
		return (captures.empty() ? shifts : captures);
	}
}

std::vector<Engine::MoveData> Engine::get_move_data(Board board, Engine::CellInfo piece_info, Engine::uvector uvector, unsigned int provided_cell_id, bool promoted) {
	std::vector<Engine::MoveData> move_data;
	Engine::CellInfo curr_cell;
	PieceRuleset ruleset = promoted ? Engine::rules.queen_ruleset : piece_info.ruleset;
	unsigned int distance = 0;
	unsigned int curr_cell_id = 0;
	unsigned int src_cell_id = provided_cell_id;
	int max_distance = ruleset.after_take_max_distance;
	if (provided_cell_id == NULL) {
		src_cell_id = piece_info.pos;
		max_distance = ruleset.max_shift_distance;
	}
	while (max_distance == -1 or distance < max_distance) {
		curr_cell_id = Engine::get_neighbour(board, src_cell_id, uvector);
		if (curr_cell_id == src_cell_id) {
			break;
		}
		else {
			curr_cell = Engine::get_cell_info(board, curr_cell_id);
			if (not curr_cell.has_value) {
				Engine::MoveData new_data = {
					.cell_id = curr_cell_id,
					.promotes = Engine::can_promote(board, curr_cell_id, piece_info)
				};
				move_data.push_back(new_data);
				src_cell_id = curr_cell_id;
				distance++;
				uvector[1] = 1 - uvector[1]; // x_direction = 1 - x_direction
			}
			else if (curr_cell.is_white != piece_info.is_white) {
				Engine::MoveData new_data = {
					.cell_id = curr_cell_id,
					.uvector = {uvector[0],1 - uvector[1]}
				};
				move_data.push_back(new_data);
				break;
			}
			else {
				break;
			}
		}
	}
	return move_data;
}

std::vector<Move> Engine::get_captures(Board board, Engine::CellInfo piece_info, Engine::MoveData data, unsigned int depth) {
	bool promotion_undecided = false;
	std::vector<Move> captures;
	Engine::CellInfo curr_cell;
	unsigned int curr_cell_id = Engine::get_neighbour(board, data.cell_id, data.uvector);
	if (curr_cell_id == data.cell_id) {
		return captures;
	}
	curr_cell = Engine::get_cell_info(board, curr_cell_id);
	if (curr_cell.has_value) {
		return captures;
	}
	Target target = { .target_cell_id = data.cell_id, .is_queen = Engine::get_cell_info(board, data.cell_id).is_queen };
	data.targets.push_back(target);
	data.cell_id = curr_cell_id;
	// promotion
	if (not data.promotes and Engine::can_promote(board, curr_cell_id, piece_info)) {
		if (piece_info.ruleset.promotion_stop) {
			data.promotes = true;
			captures.push_back(Engine::construct_move(piece_info, data));
			return captures;
		}
		else {
			if (piece_info.ruleset.promotion_on_move) {
				data.promotes = true;
			}
			else {
				data.promotes = false;
				promotion_undecided = true;
			}
		}
	}
	// new_captures
	PieceRuleset ruleset = data.promotes ? Engine::rules.queen_ruleset : piece_info.ruleset;
	unsigned int y_dir_swapped;
	unsigned int next_cell_id = 0;
	Engine::CellInfo next_cell;
	for (unsigned int y_direction = 0; y_direction < ruleset.capture_directions; y_direction++) {
		y_dir_swapped = piece_info.is_white ? y_direction : 1 - y_direction;
		for (unsigned int x_direction = 0; x_direction < 2; x_direction++) {
			if (y_dir_swapped != data.uvector[0] and x_direction == data.uvector[1]) {
				continue;
			}
			Engine::uvector uvector = { y_dir_swapped, x_direction };
			next_cell_id = get_neighbour(board, curr_cell_id, uvector);
			if (next_cell_id != curr_cell_id) {
				next_cell = get_cell_info(board, next_cell_id);
				if (not next_cell.has_value) {
					// std::cout << "bad next cell";
				}
				else if (next_cell.is_white == piece_info.is_white) {
					// std::cout << "own piece at next cell";
				}
				else {
					target = { .target_cell_id = next_cell_id, .is_queen = next_cell.is_queen };
					if (Engine::target_in_targets(data.targets, target)) {
						continue;
					}
					uvector[1] = 1 - uvector[1];
					Engine::MoveData new_data = {
						.cell_id = next_cell_id,
						.uvector = uvector,
						.promotes = data.promotes,
						.targets = data.targets
					};
					std::vector<Move> new_captures = Engine::get_captures(board, piece_info, new_data, depth + 1);
					captures.insert(captures.end(), new_captures.begin(), new_captures.end());
				}
			}
		}
	}
	// Return
	if (captures.empty()) {
		if (promotion_undecided) {
			data.promotes = true;
		}
		captures.push_back(Engine::construct_move(piece_info, data));
	}
	return captures;
}

bool Engine::target_in_targets(std::vector<Target> targets, Target target) {
	for (Target existing_target : targets) {
		if (existing_target.target_cell_id == target.target_cell_id) {
			return true;
		}
	}
	return false;
}

std::vector<Move> Engine::extend_move(Board board, Move move) {
	Engine::CellInfo piece_info = Engine::get_cell_info(board, move.origin);
	std::vector<Move> moves = { move };
	PieceRuleset ruleset = move.promotes ? Engine::rules.queen_ruleset : piece_info.ruleset;
	Target last_target = move.targets.back();
	std::vector<Engine::MoveData> move_data;
	Engine::uvector uvector = { (last_target.target_cell_id - move.destanation > 0 ? 1 : 0), 0 };
	uvector[1] = int(Engine::get_neighbour(board, last_target.target_cell_id, uvector) == move.destanation);
	move_data = Engine::get_move_data(board, piece_info, uvector, move.destanation, move.promotes);
	for (Engine::MoveData data : move_data) {
		if (data.uvector[0] == -1) {
			data.promotes = data.promotes or move.promotes;
			data.targets = move.targets;
			moves.push_back(Engine::construct_move(piece_info, data));
		}
	}
	return moves;
}


unsigned int Engine::get_neighbour(Board board, unsigned int src_cell_id, Engine::uvector uvector) {
	unsigned int half_size = Engine::rules.resolution / 2;
	unsigned int row = src_cell_id / half_size;
	if ((row == Engine::rules.resolution - 1 and uvector[0] == 0) or (row == 0 and uvector[0] == 1)) {
		return src_cell_id;
	}
	if (uvector[1] == 0) {
		return src_cell_id - (uvector[0] == 1 ? half_size : -1*half_size);
	}
	else {
		bool row_parity = (Engine::rules.on_whites ? not bool(row % 2) : bool(row % 2));
		// condition = signoring either first or last cell in row depending on row parity
		bool condition = (row_parity ? src_cell_id % half_size < (half_size - 1) : src_cell_id % half_size != 0);
		if (condition) {
			int y_sign = (uvector[0] ? -1 : 1);
			int x_sign = (row_parity ? 1 : -1);
			return src_cell_id + y_sign * half_size + x_sign;
		}
	}
	return src_cell_id;
}

bool Engine::can_promote(Board board,int cell_id, CellInfo piece_info) {
	if (piece_info.is_queen) {
		return false;
	}
	int row = cell_id / Engine::rules.resolution;
	if (piece_info.is_white) {
		if (row == Engine::rules.resolution - 1) {
			return true;
		}
	}
	else if (row == 0) {
		return true;
	}
	return false;
}

std::vector<Move> Engine::get_player_moves(Board board, bool is_white) {
	bool capturing = false;
	std::vector<Move> moves;
	std::vector<Move> new_moves;
	Engine::CellInfo cellinfo;
	for (unsigned int cell_id = 0; cell_id < board.state.size(); cell_id++) {
		cellinfo = Engine::get_cell_info(board, cell_id);
		if (cellinfo.has_value and cellinfo.is_white == is_white) {
			new_moves = Engine::get_piece_moves(board, cellinfo);
			if (not new_moves.empty()) {
				moves.insert(moves.end(), new_moves.begin(), new_moves.end());
				if (not capturing and not new_moves[0].targets.empty()) {
					capturing = true;
				}
			}
		}
	}
	if (not (capturing and Engine::rules.always_capture)) {
		return moves;
	}
	moves = Engine::enforce_capture_rules(moves);
	return moves;
}

Engine::CellInfo Engine::get_cell_info(Board board, unsigned int cell_id) {
	Engine::CellInfo cell_info;
	char piece_code = board.state[cell_id];
	if (piece_code != 'N') {
		cell_info.has_value = true;
		cell_info.pos = cell_id;
		cell_info.is_queen = tolower(piece_code) == 'q';
		cell_info.is_white = isupper(piece_code);
		cell_info.ruleset = cell_info.is_queen ? Engine::rules.queen_ruleset : Engine::rules.piece_ruleset;
	}
	return cell_info;
}

char Engine::get_cell_code(Board board, unsigned int cell_id) {
	Engine::CellInfo cell_info = Engine::get_cell_info(board, cell_id);
	if(cell_info.has_value){
		char piece_type = (cell_info.is_queen ? 'q' : 'p');
		return (cell_info.is_white ? toupper(piece_type) : piece_type);
	}
	else {
		return 'N';
	}
}
	

Board Engine::play_move(Board board, Move move) {
	for (Target target : move.targets) {
		board.state[target.target_cell_id] = 'N';
	}
	board.state[move.destanation] = board.state[move.origin];
	board.state[move.origin] = 'N';
	if (move.promotes) {
		board.state[move.destanation] = std::isupper(board.state[move.destanation]) ? 'Q' : 'q';
	}
	board.white_to_move = not move.by_white;
	return board;
}

Board Engine::load_board(std::string string) {
	Board board;
	char last_char = string.back();
	if (std::tolower(last_char) == 'w') {
		board.white_to_move = std::isupper(last_char);
		string.pop_back();
	}
	else {
		board.white_to_move = true;
	}
	for (char c : string) {
		if (c != '/') {
			board.state.push_back(c);
		}
	}
	return board;
}

std::string Engine::export_board(Board board, bool white_to_move, int depth) {
	std::string string;
	for (unsigned int cell_id = 0; cell_id < board.state.size(); cell_id++) {
		string += get_cell_code(board, cell_id);
	}
	string += white_to_move ? 'W' : 'w';
	string += depth != -1 ? std::to_string(depth) : "";
	return string;
}