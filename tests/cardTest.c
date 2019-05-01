#include <ncurses.h>
#include <stdio.h>
#include <string.h>

typedef struct Player {
    char** hand; // The current hand of cards
    int numCards; // How many do we have?
}Player;

void output_card(char* cardPlayed, int row, int col);
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

    int maxRow = row;
    row -= 15; // Put this in the middle top
    output_card(cardPlayed, row/2, (col - 43)/2);
    output_card(cardPlayed, maxRow - 6, 20);
    output_card(cardPlayed, maxRow - 6, 35);
    output_card(cardPlayed, maxRow - 6, 50);
    output_card(cardPlayed, maxRow - 6, 65);
    //ioutput_hand(player, row, col);
}

void output_card(char* cardPlayed, int row, int col) {
    //    attron(COLOR_PAIR(3));
    int rowUpdate = 5;
     init_pair(2, COLOR_WHITE, COLOR_BLUE);
    
    attron(COLOR_PAIR(3));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    attroff(COLOR_PAIR(3));
    for (int i = 0; i < 2; i++) {
        attron(COLOR_PAIR(3));
        mvprintw((row)-rowUpdate--, col , "|");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));
        if (i == 0) {
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            attron(A_BOLD);
            printw("9");
            attroff(A_BOLD);
            attroff(COLOR_PAIR(2));
            attron(COLOR_PAIR(1));
            printw("---------");
        } else {
            printw("----------");
        }
        attroff(COLOR_PAIR(1));
        attron(COLOR_PAIR(3));
        printw("|");
        attroff(COLOR_PAIR(3));
    }
    attron(COLOR_PAIR(3));
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));

    /*for (int i = 0; i < strlen(wordOne); i++) { // First word buffer
        printw("-");
    }*/
    printw("---");

    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    attron(A_BOLD);
    printw("NINE");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(1));
    printw("---");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    printw("|");
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(3));
    attron(COLOR_PAIR(1));
    printw("---");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(1));
    printw("----");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(1));
    printw("---");
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(3));
    printw("|");
    attroff(COLOR_PAIR(3));
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(3));
        mvprintw((row)-rowUpdate--, col, "|");
        attroff(COLOR_PAIR(3));
        attron(COLOR_PAIR(1));
        if (i == 4) {
            printw("---------");
            attroff(COLOR_PAIR(1));
            attron(COLOR_PAIR(2));
            attron(A_BOLD);
            printw("9");
            attroff(A_BOLD);
            attroff(COLOR_PAIR(2));
        } else {
            printw("----------");
        }
        attroff(COLOR_PAIR(1));
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
