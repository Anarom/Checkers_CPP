#pragma once
#include "engine.h"
#include "SDL/SDL.h"
#undef main

class Player {
public:
	Player(bool is_white, GameRules rules);
	bool is_white = true;
	Move choose_move(std::vector<Move>);
	void update(Board board);
protected:
	Board board;
	GameRules rules;
};

class GUIPlayer : public Player {
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
	GUIPlayer(bool is_white, GameRules rules, ColorPalette palette);
	void update(Board board);

private:
	ColorPalette palette;
	SDL_Window* window;
	SDL_Renderer* renderer;
	std::vector<unsigned int> focused_cells;
	std::vector<std::array<unsigned int, 2>> centres;
	std::array<bool, 3> get_cell_info(unsigned int cell_id);
	std::array<unsigned int, 2> get_rect_center(unsigned int dx, unsigned int dy, unsigned int x_offset=0);
	void set_drawing_color(SDL_Color color);
	void unfocus_all();
	void unfocus_cell(unsigned int cell_id, bool batched = false);
	void focus_cell(unsigned int cell_id);
	void reset_cell(unsigned int cell_id, bool focus = false);
	void draw_board();
	void draw_piece(unsigned int cell_id);
	void draw_circle(std::array<unsigned int, 2> coords, int radius, SDL_Color color);
	void draw_rect(std::array<unsigned int, 2> coords, bool focus = false);
};