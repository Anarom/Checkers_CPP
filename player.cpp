#include "Player.h"
#include <Windows.h>
#include <iostream>

Player::Player(bool is_white, GameRules rules) {
	Player::is_white = is_white;
	Player::rules = rules;
}

Move Player::choose_move(std::vector<Move> moves) {
	Sleep(1000);
	return moves.front();
}

void Player::update(Board board) {
	Player::board = board;
}

GUIPlayer::GUIPlayer(bool is_white, GameRules rules, ColorPalette palette) : Player(is_white, rules) {
	GUIPlayer::palette = palette;
	GUIPlayer::centres.reserve(GUIPlayer::rules.resolution * GUIPlayer::rules.resolution / 2);
	for (unsigned int n = 0; n < GUIPlayer::rules.resolution * GUIPlayer::rules.resolution / 2; n++) {
		centres.push_back({ 0,0 });
	}
	GUIPlayer::window = SDL_CreateWindow(
		"Checkers",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		GUIPlayer::rules.cell_size * GUIPlayer::rules.resolution,
		GUIPlayer::rules.cell_size * GUIPlayer::rules.resolution,
		SDL_WINDOW_SHOWN
	);
	GUIPlayer::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	GUIPlayer::draw_board();
	char a;
	std::cin >> a;
}

void GUIPlayer::update(Board board) {
	Board old_board = GUIPlayer::board;
	bool reset_all = false;
	if (old_board.state.empty()) {
		reset_all = true;
	}
	Player::update(board);
	for (unsigned int cell_id = 0; cell_id < board.state.size(); cell_id++) {
		if (reset_all or old_board.state[cell_id] != GUIPlayer::board.state[cell_id]) {
			GUIPlayer::reset_cell(cell_id);
		}
	}
}

std::array<bool, 3> GUIPlayer::get_cell_info(unsigned int cell_id) {
	std::array<bool, 3> cell_info = { false,false,false };
	char cell_code = GUIPlayer::board.state[cell_id];
	if (cell_code != 'N') {
		cell_info[0] = true;
		cell_info[1] = isupper(cell_code);
		cell_info[2] = tolower(cell_code) == 'w';
	}
	return cell_info;
}

std::array<unsigned int, 2> GUIPlayer::get_rect_center(unsigned int dx, unsigned int dy, unsigned int x_offset) {
	std::array<unsigned int, 2> coords = { x_offset + 2 * dx* GUIPlayer::rules.cell_size, dy * GUIPlayer::rules.cell_size};
	std::cout << coords[0] << "," << coords[1] << "\n";
	return coords;
}

void GUIPlayer::set_drawing_color(SDL_Color color) {
	SDL_SetRenderDrawColor(GUIPlayer::renderer, color.r, color.g, color.b, color.a);
}

void GUIPlayer::unfocus_all() {
	for (unsigned int cell_id : GUIPlayer::focused_cells) {
		GUIPlayer::unfocus_cell(cell_id, true);
	}
	GUIPlayer::focused_cells.clear();
}

void GUIPlayer::unfocus_cell(unsigned int cell_id, bool batched) {
	if (not batched) {
		GUIPlayer::focused_cells.erase(std::remove(GUIPlayer::focused_cells.begin(), GUIPlayer::focused_cells.end(), cell_id), GUIPlayer::focused_cells.end());
	}
	GUIPlayer::reset_cell(cell_id);
	GUIPlayer::draw_piece(cell_id);
}

void GUIPlayer::focus_cell(unsigned int cell_id) {
	GUIPlayer::reset_cell(cell_id, true);
	GUIPlayer::focused_cells.push_back(cell_id);
}

void GUIPlayer::reset_cell(unsigned int cell_id, bool focus) {
	unsigned int dx = GUIPlayer::centres[cell_id][0] - GUIPlayer::rules.cell_size / 2;
	unsigned int dy = (GUIPlayer::centres[cell_id][1] + GUIPlayer::rules.cell_size / 2) - GUIPlayer::rules.resolution * GUIPlayer::rules.cell_size;
	GUIPlayer::draw_rect(GUIPlayer::get_rect_center(dx, dy), focus);
	GUIPlayer::draw_piece(cell_id);
}

void GUIPlayer::draw_board() {
	GUIPlayer::set_drawing_color(GUIPlayer::palette.bg_color);
	SDL_RenderClear(GUIPlayer::renderer);
	unsigned int x_offset = GUIPlayer::rules.on_whites ? GUIPlayer::rules.cell_size : 0;
	unsigned int half_size = GUIPlayer::rules.resolution / 2;
	for (unsigned int dy = 0; dy < GUIPlayer::rules.resolution; dy++) {
		for (unsigned int dx = 0; dx < half_size; dx++) {
			std::array<unsigned int, 2> coords = GUIPlayer::get_rect_center(dx, dy, x_offset);
			GUIPlayer::centres[(GUIPlayer::rules.resolution - 1 - dy) * half_size + dx] = coords;
			std::cout << (GUIPlayer::rules.resolution - 1 - dy) * half_size + dx << " " << coords[0] << " " << coords[1] << "\n";
			GUIPlayer::draw_rect(coords);
		}
		x_offset = x_offset != 0 ? 0 : GUIPlayer::rules.cell_size;
	}
}

void GUIPlayer::draw_piece(unsigned int cell_id) {
	SDL_Color color;
	std::array<bool, 3> cell_info = GUIPlayer::get_cell_info(cell_id);
	if (cell_info[0]) {
		if (cell_info[1]) {
			color = cell_info[2] ? GUIPlayer::palette.wq_color : GUIPlayer::palette.bq_color;
		}
		else {
			color = cell_info[2] ? GUIPlayer::palette.wp_color : GUIPlayer::palette.bp_color;
		}
		GUIPlayer::draw_circle(GUIPlayer::centres[cell_id], GUIPlayer::rules.cell_size / 2, color);
	}
}

void GUIPlayer::draw_rect(std::array<unsigned int, 2> coords, bool focus) {
	SDL_Rect rect{
		.x = (int)coords[0],
		.y = (int)coords[1],
		.w = (int)GUIPlayer::rules.cell_size,
		.h = (int)GUIPlayer::rules.cell_size
	};
	SDL_Color color = focus ? GUIPlayer::palette.fc_color : GUIPlayer::palette.fg_color;
	GUIPlayer::set_drawing_color(color);
	SDL_RenderFillRect(GUIPlayer::renderer, &rect);
	SDL_RenderPresent(GUIPlayer::renderer);
}

void GUIPlayer::draw_circle(std::array<unsigned int, 2> coords, int radius, SDL_Color color) {
	SDL_SetRenderDrawColor(GUIPlayer::renderer, color.r, color.g, color.b, color.a);
	int dx = 0;
	int dy = 0;
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			dx = radius - w;
			dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius)) {
				SDL_RenderDrawPoint(GUIPlayer::renderer, coords[0] + dx, coords[1] + dy);
			}
		}
	}
}
