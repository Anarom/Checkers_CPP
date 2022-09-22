#include <stdexcept>
#include "UI.h"
#include <iostream>
#include <random>
#include <Windows.h>

Move BaseUI::choose_move(Board board, std::vector<Move> moves) {
	Sleep(3000);
	return moves[rand() % moves.size()];
}

GUI::GUI(BoardSize board_size, GUI::ColorPalette palette) {
	GUI::palette = palette;
	GUI::board_size = board_size;
	GUI::centres.reserve(GUI::board_size.resolution * GUI::board_size.resolution / 2);
	for (unsigned int n = 0; n < GUI::board_size.resolution * GUI::board_size.resolution / 2; n++) {
		centres.push_back({ 0,0 });
	}
	GUI::window = SDL_CreateWindow(
		"Checkers",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		GUI::board_size.cell_size * GUI::board_size.resolution,
		GUI::board_size.cell_size * GUI::board_size.resolution,
		SDL_WINDOW_SHOWN
	);
	GUI::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

void GUI::unfocus_all() {
	for (unsigned int cell_id : GUI::focused_cells) {
		GUI::unfocus_cell(cell_id, true);
	}
	GUI::focused_cells.clear();
}

void GUI::unfocus_cell(unsigned int cell_id, bool batched) {
	if (not batched) {
		GUI::focused_cells.erase(std::remove(GUI::focused_cells.begin(), GUI::focused_cells.end(), cell_id), GUI::focused_cells.end());
	}
	GUI::reset_cell(cell_id);
	GUI::draw_piece(cell_id);
}

void GUI::focus_cell(unsigned int cell_id) {
	GUI::reset_cell(cell_id, true);
	GUI::draw_piece(cell_id);
}

void GUI::reset_cell(unsigned int cell_id, bool focused) {
	unsigned int dx = GUI::centres[cell_id][0] - GUI::board_size.cell_size / 2;
	unsigned int dy = (GUI::centres[cell_id][1] + GUI::board_size.cell_size / 2) - GUI::board_size.resolution * GUI::board_size.cell_size;
	GUI::draw_rect(GUI::get_rect_center(dx, dy), focused);
}

std::array<unsigned int, 2> GUI::get_rect_center(unsigned int dx, unsigned int dy) {
	std::array<unsigned int, 2> coords = { 2 * dx, GUI::board_size.resolution * GUI::board_size.cell_size - dy };
	return coords;
}

void GUI::set_drawing_color(SDL_Color color) {
	SDL_SetRenderDrawColor(GUI::renderer, color.r, color.g, color.b, color.a);
}

void GUI::draw_board() {
	GUI::set_drawing_color(GUI::palette.bg_color);
	SDL_RenderClear(GUI::renderer);
	unsigned int x_offset = GUI::engine.rules.on_whites ? GUI::board_size.cell_size : 0;
	unsigned int half_size = GUI::board_size.resolution / 2;
	for (unsigned int dy = 0; dy < GUI::board_size.resolution; dy++) {
		for (unsigned int dx = 0; dx < half_size; dx++) {
			std::array<unsigned int, 2> coords = GUI::get_rect_center(dx, dy);
			GUI::centres[dy * half_size + dx] = coords;
			GUI::draw_rect(coords);
		}
		x_offset = x_offset != 0 ? 0 : GUI::board_size.cell_size;
	}
}

void GUI::draw_piece(unsigned int cell_id) {
	SDL_Color color;
	Engine::CellInfo cell_info = GUI::engine.get_cell_info(cell_id);
	if (cell_info.has_value) {
		if (cell_info.is_queen) {
			color = cell_info.is_white ? GUI::palette.wq_color : GUI::palette.bq_color;
		}
		else {
			color = cell_info.is_white ? GUI::palette.wp_color : GUI::palette.bp_color;
		}
		GUI::draw_circle(GUI::centres[cell_id], GUI::board_size.cell_size / 2, color);
	}
}

void GUI::draw_rect(std::array<unsigned int, 2> coords, bool focused) {
	SDL_Rect rect{
		.x = (int)coords[0],
		.y = (int)coords[1],
		.w = (int)GUI::board_size.cell_size,
		.h = (int)GUI::board_size.cell_size
	};
	SDL_Color color = focused ? GUI::palette.fc_color : GUI::palette.fg_color;
	GUI::set_drawing_color(color);
	SDL_RenderFillRect(GUI::renderer, &rect);
	SDL_RenderPresent(GUI::renderer);
}

void GUI::draw_circle(std::array<unsigned int, 2> coords, int radius, SDL_Color color) {
	SDL_SetRenderDrawColor(GUI::renderer, color.r, color.g, color.b, color.a);
	int dx = 0;
	int dy = 0;
	for (int w = 0; w < radius * 2; w++) {
		for (int h = 0; h < radius * 2; h++) {
			dx = radius - w;
			dy = radius - h;
			if ((dx * dx + dy * dy) <= (radius * radius)) {
				SDL_RenderDrawPoint(GUI::renderer, coords[0] + dx, coords[1] + dy);
			}
		}
	}
}

