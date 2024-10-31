# TerminaX

TerminaX is a ncurses game engine with the following features.

* Responsive: You set up the game size, and it will adapt and resize depending the terminal size.
* UI Elements: UI elements to easily render stuff.
  - UIText: to render text with different alignments.
  - UIRect: to render a rectangle.
  - UIPoint: to render a point.
  - UIBorder: to render a border.

## How to use to make my own game.

```c
//main.c

void game_frame(void) {
  //game logic goes here
}

int main (void) {
  //Create a game with 60 cols and 15 rows of size.
	my_game = create_game(60, 15, game_frame);
	game_start(my_game);
	return 0;
}
```

## Credits

* (C-Simple-JSON-Parser by stevemolloy)[https://github.com/forkachild/C-Simple-JSON-Parser]
