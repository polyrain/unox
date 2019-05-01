#include <ncurses.h>
#include <stdio.h>
#include <string.h>

typedef struct Player {
    char** hand; // The current hand of cards
    int numCards; // How many do we have?
}Player;

void output_card(char* cardPlayed, int row, int col, int color, int color2);
void output_display(Player* player, char* cardPlayed);


int main() {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    //getmaxyx(stdscr, row,col);
    //mvprintw(row/2, (col-strlen(message))/2, "%s", message);
    //mvprintw(row-2, 0, "This screen has %d rows and %d cols\n", row, col);
    output_display(NULL, NULL);
    //printw("Try it again after resizing!\n");
    refresh();
    //char c = getch();
    //attron(A_BOLD);
    //printw(" Key pressed was: %c\n", c);
    //attroff(A_BOLD);
    getch();
    endwin();

    return 0;
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard*/
void output_display(Player* player, char* cardPlayed) {
    int row, col;
    getmaxyx(stdscr, row,col);
    init_pair(1, COLOR_BLUE, COLOR_BLUE); // Make an entire red thing
    init_pair(3, COLOR_WHITE, COLOR_WHITE);
    init_pair(4, COLOR_WHITE, COLOR_RED);
    init_pair(5, COLOR_RED, COLOR_RED);

    int maxRow = row;
    row -= 15; // Put this in the middle top
    output_card(cardPlayed, row/2, (col - 43)/2, 4, 5);
    output_card(cardPlayed, maxRow - 6, 20, 2, 1);
    output_card(cardPlayed, maxRow - 6, 35, 2, 1);
    output_card(cardPlayed, maxRow - 6, 50, 2, 1);
    output_card(cardPlayed, maxRow - 6, 65, 2, 1);
    //ioutput_hand(player, row, col);
}

void output_card(char* cardPlayed, int row, int col, int color, int color2) {
    //    attron(COLOR_PAIR(3));
    int rowUpdate = 5;
    attron(COLOR_PAIR(3));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    attroff(COLOR_PAIR(3));

    for (int i = 0; i < 2; i++) {
        attron(COLOR_PAIR(3));
        mvprintw((row)-rowUpdate--, col , "|");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(color2));
        printw("----------");
        attroff(COLOR_PAIR(color2));
        attron(COLOR_PAIR(3));
        printw("|");
        attroff(COLOR_PAIR(3));
    }
    attron(COLOR_PAIR(3));
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(color2));

    for (int i = 0; i < 3; i++) { // First word buffer
        printw("-");
    }

    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(color));
    attron(A_BOLD);
    printw("DRAW");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(color));
    attron(COLOR_PAIR(color2));
    printw("---");
    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(3));
    printw("|");
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(color2));
    printw("---");
    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(color));
    attron(A_BOLD);
    printw("FOUR");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(color));
    attron(COLOR_PAIR(color2));
    printw("---");
    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(3));
    printw("|");
    attroff(COLOR_PAIR(3));
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(3));
        mvprintw((row)-rowUpdate--, col, "|");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(color2));
        printw("----------");
        attroff(COLOR_PAIR(color2));
        attron(COLOR_PAIR(3));
        printw("|");
        attroff(COLOR_PAIR(3));
    }
    attron(COLOR_PAIR(3));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    attroff(COLOR_PAIR(3));

}

void output_hand(Player* player, int row, int col) {
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);


}
