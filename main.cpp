#include <iostream>
#include <game.h>

int main() {

	GUIPlayer::ColorPalette palette = {
		.bg_color = {.r = 170,.g = 127,.b = 84,.a = 100},
		.fg_color = {.r = 247,.g = 244,.b = 242,.a = 100},
		.fc_color = {.r = 15,.g = 0,.b = 0,.a = 100},
		.wp_color = {.r = 20,.g = 0,.b = 0,.a = 100},
		.wq_color = {.r = 25,.g = 0,.b = 0,.a = 100},
		.bp_color = {.r = 30,.g = 0,.b = 0,.a = 100},
		.bq_color = {.r = 35,.g = 0,.b = 0,.a = 100}
	};
	GameRules rules =
	{
		.resolution = 8,
		.cell_size= 50,
		.on_whites = false,
		.always_capture = true,
		.always_max_capture = true,
		.piece_ruleset = {
			.capture_directions = 2,
			.shift_directions = 1,
			.max_shift_distance = 1,
			.after_take_max_distance = 0,
			.promotion_on_move = false,
			.promotion_stop = false
		},
		.queen_ruleset = {
			.capture_directions = 2,
			.shift_directions = 2,
			.max_shift_distance = -1,
			.after_take_max_distance = -1,
			.promotion_on_move = false,
			.promotion_stop = false
		}
	};

	Game game(rules, palette);
	game.run();
	return game.result;
}