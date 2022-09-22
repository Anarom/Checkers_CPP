#include "Screen.h"

Screen::Screen(GameRules rules, ColorPalette palette){
	Screen::palette = palette;
	Screen::centres.reserve(Screen::resolution * Screen::resolution / 2);
	for (unsigned int n = 0; n < Screen::resolution * Screen::resolution / 2; n++) {
		centres.push_back({ 0,0 });
	}
	Screen::window = SDL_CreateWindow(
		"Checkers",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		Screen::cell_size * Screen::resolution,
		Screen::cell_size * Screen::resolution,
		SDL_WINDOW_SHOWN
	);
	Screen::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	Screen::draw_board(rules.on_whites);
	Screen::resolution = rules.resolution;
	Screen::cell_size = cell_size;
}

void Screen::update(Board new_board) {
	Board temp_board = Screen::last_board;
	Screen::last_board = new_board;
	bool reset_all = Screen::last_board.state.empty();
	for (unsigned int cell_id = 0; cell_id < Screen::last_board.state.size(); cell_id++) {
		if (reset_all or Screen::last_board.state[cell_id] != temp_board.state[cell_id]) {
			Screen::reset_cell(cell_id);
		}
	}
}


std::array<bool, 3> Screen::get_cell_info(unsigned int cell_id){
	std::array<bool, 3> cell_info = { false,false,false };
	char cell_code = Screen::last_board.state[cell_id];
	if (cell_code != 'N') {
		cell_info[0] = true;
		cell_info[1] = isupper(cell_code);
		cell_info[2] = tolower(cell_code) == 'w';
	}
	return cell_info;
}

std::array<unsigned int, 2> Screen::get_rect_center(unsigned int dx, unsigned int dy, unsigned int x_offset) {
	std::array<unsigned int, 2> coords = { x_offset + 2 * dx * Screen::cell_size, dy * Screen::cell_size };
	std::cout << coords[0] << "," << coords[1] << "\n";
	return coords;
}

void Screen::set_drawing_color(SDL_Color color) {
	SDL_SetRenderDrawColor(Screen::renderer, color.r, color.g, color.b, color.a);
}

void Screen::unfocus_all() {
	for (unsigned int cell_id : Screen::focused_cells) {
		Screen::unfocus_cell(cell_id, true);
	}
	Screen::focused_cells.clear();
}

void Screen::unfocus_cell(unsigned int cell_id, bool batched) {
	if (not batched) {
		Screen::focused_cells.erase(std::remove(Screen::focused_cells.begin(), Screen::focused_cells.end(), cell_id), Screen::focused_cells.end());
	}
	Screen::reset_cell(cell_id);
}

void Screen::focus_cell(unsigned int cell_id) {
	Screen::reset_cell(cell_id, true);
	Screen::focused_cells.push_back(cell_id);
}

void Screen::reset_cell(unsigned int cell_id, bool focus) {
	unsigned int dx = Screen::centres[cell_id][0] - Screen::cell_size / 2;
	unsigned int dy = (Screen::centres[cell_id][1] + Screen::cell_size / 2) - Screen::resolution * Screen::cell_size;
	Screen::draw_rect(Screen::get_rect_center(dx, dy), focus);
	Screen::draw_piece(cell_id);
}

void Screen::draw_board(bool on_whites) {
	Screen::set_drawing_color(Screen::palette.bg_color);
	SDL_RenderClear(Screen::renderer);
	unsigned int x_offset = on_whites ? Screen::cell_size : 0;
	unsigned int half_size = Screen::resolution / 2;
	for (unsigned int dy = 0; dy < Screen::resolution; dy++) {
		for (unsigned int dx = 0; dx < half_size; dx++) {
			std::array<unsigned int, 2> coords = Screen::get_rect_center(dx, dy, x_offset);
			Screen::centres[(Screen::resolution - 1 - dy) * half_size + dx] = coords;
			std::cout << (Screen::resolution - 1 - dy) * half_size + dx << " " << coords[0] << " " << coords[1] << "\n";
			Screen::draw_rect(coords);
		}
		x_offset = x_offset != 0 ? 0 : Screen::cell_size;
	}
}

void Screen::draw_piece(unsigned int cell_id) {
	SDL_Color color;
	std::array<bool, 3> cell_info = Screen::get_cell_info(cell_id);
	if (cell_info[0]) {
		if (cell_info[1]) {
			color = cell_info[2] ? Screen::palette.wq_color : Screen::palette.bq_color;
		}
		else {
			color = cell_info[2] ? Screen::palette.wp_color : Screen::palette.bp_color;
		}
		Screen::draw_circle(Screen::centres[cell_id], Screen::cell_size / 2, color);
	}
}

void Screen::draw_rect(std::array<unsigned int, 2> coords, bool focus) {
	SDL_Rect rect{
		.x = (int)coords[0],
		.y = (int)coords[1],
		.w = (int)Screen::cell_size,
		.h = (int)Screen::cell_size
	};
	SDL_Color color = focus ? Screen::palette.fc_color : Screen::palette.fg_color;
	Screen::set_drawing_color(color);
	SDL_RenderFillRect(Screen::renderer, &rect);
	SDL_RenderPresent(Screen::renderer);
}

void Screen::draw_circle(std::array<unsigned int, 2> coords, int radius, SDL_Color color) {
	SDL_SetRenderDrawColor(Screen::renderer, color.r, color.g, color.b, color.a);
	int dx = 0;
	int dy = 0;
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			dx = radius - w;
			dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius)) {
				SDL_RenderDrawPoint(Screen::renderer, coords[0] + dx, coords[1] + dy);
			}
		}
	}
}