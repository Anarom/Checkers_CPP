#pragma once
#include "engine.h"
#include "SDL/SDL.h"
#undef main


class BaseUI {
public:
	Move choose_move(Board board, std::vector<Move> moves);
};

class GUI : BaseUI {
public:

	struct ColorPalette {
		SDL_Color bg_color; // background color
		SDL_Color fg_color; // foreground color
		SDL_Color fc_color; // focused cell color
		SDL_Color wp_color; // white piece color
		SDL_Color wq_color; // white queen color
		SDL_Color bp_color; // black piece color
		SDL_Color bq_color; // black queen color
	};

	GUI(BoardSize board_size, GUI::ColorPalette palette);
	ColorPalette palette;

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	BoardSize board_size;
	std::vector<unsigned int> focused_cells;
	std::vector<std::array<unsigned int, 2>> centres;
	void unfocus_all();
	void unfocus_cell(unsigned int cell_id, bool batched = false);
	void focus_cell(unsigned int cell_id);
	void reset_cell(unsigned int cell_id, bool focused = false);
	std::array<unsigned int, 2> get_rect_center(unsigned int dx, unsigned int dy);
	void set_drawing_color(SDL_Color color);
	void draw_board();
	void draw_piece(unsigned int cell_id);
	void draw_circle(std::array<unsigned int, 2> coords, int radius, SDL_Color color);
	void draw_rect(std::array<unsigned int, 2> coords, bool focused = false);
};