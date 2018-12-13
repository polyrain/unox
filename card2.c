#include <ncurses.h>
#include <stdio.h>
#include <string.h>

enum CardColors {
    BLUE_BG = 1,
    BLUE_WORDS = 2,
    BORDER = 3,
    RED_WORDS = 4,
    RED_BG = 5,
    YELLOW_BG = 6,
    YELLOW_WORDS = 7,
    GREEN_BG = 8,
    GREEN_WORDS = 9
};

typedef struct Game {
    Player* players; // All players in this game
    Card lastPlayed; // What was last played by a player?
    int lastPlayer; // Who played the last card?
}Game;


typedef struct Player {
    Card* hand; // The current hand of cards
    int numCards; // How many do we have?
}Player;

/* Contains information describing a card */
typedef struct Card {
    char* cardType;
    int colorOne;
    int colorTwo;
    int cardValue;
}Card;

/* ALL CARD TYPES NOTE FOR ME
 * DRAW FOUR: Two lines for words
 * REVERSE: One line 
 * DRAW TWO: Two lines
 * NUMBER CARD: One line, numbers in corner?
 * SKIP: One line
 * WILD: One line
 */

void output_card(char* cardPlayed, int row, int col, int color, int color2);
void output_display(Player* player, char* cardPlayed);


int main() {
    setup_ncurse();
    output_display(NULL, NULL);
    refresh();
    getch();
    endwin();

    return 0;
}

void setup_ncurse(void) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    init_pair(BLUE_BG, COLOR_BLUE, COLOR_BLUE); // Setup color attrs 
    init_pair(BLUE_WORDS, COLOR_WHITE, COLOR_BLUE); 
    init_pair(BORDER, COLOR_WHITE, COLOR_WHITE);
    init_pair(RED_WORDS, COLOR_WHITE, COLOR_RED);
    init_pair(RED_BG, COLOR_RED, COLOR_RED);
    init_pair(YELLOW_BG, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(YELLOW_WORDS, COLOR_WHITE, COLOR_YELLOW);
    init_pair(GREEN_BG, COLOR_GREEN, COLOR_GREEN);
    init_pair(GREEN_WORDS, COLOR_WHITE, COLOR_GREEN);
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard*/
void output_display(Player* player) {
    int row, col;
    getmaxyx(stdscr, row,col); // Move all these inits into another func
    
    int maxRow = row; // WHY
    row -= 15; // Put this in the middle top and fix this!!
    output_card(cardPlayed, row/2, (col - 43)/2, 4, 5);
    output_card(cardPlayed, maxRow - 6, 20, 2, 1); // Fix this function
    output_card(cardPlayed, maxRow - 6, 35, 2, 1);
    output_card(cardPlayed, maxRow - 6, 50, 2, 1);
    output_card(cardPlayed, maxRow - 6, 65, 2, 1);
    //ioutput_hand(player, row, col);
}

void draw_card_space(int* row, int* col, int* rowUpdate, int color, int num) {

    for (int i = 0; i < num; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)-(*rowUpdate)--, *col , "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(color));
        printw("----------");
        attroff(COLOR_PAIR(color));
        attron(COLOR_PAIR(BORDER));
        printw("|");
    }
}

void draw_card_word(Card card) { //11 as the first char has been drawn
    for (int i = 0; i < ((11 - strlen(card.cardPlayed))/2); i++) { 
        printw("-");
    }

    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(color));
    attron(A_BOLD);
    printw("DRAW");
    attroff(A_BOLD);
    attroff(COLOR_PAIR(color));
    attron(COLOR_PAIR(color2));
    printw("---");
    attroff(COLOR_PAIR(color2));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(BORDER));
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
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));

}

void output_card(char* cardPlayed, int row, int col, int color, int color2) {
    int rowUpdate = 5;
    attron(COLOR_PAIR(BORDER));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    draw_card_space(&row, &col, &rowUpdate, color2, 2);

    attron(COLOR_PAIR(BORDER));
    mvprintw((row)-rowUpdate--, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(color2));

        
    draw_card_space(&row, &col, &rowUpdate, color2, 5);
    
    attron(COLOR_PAIR(BORDER));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    attroff(COLOR_PAIR(BORDER));

}

void output_hand(Player* player, int row, int col) {
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);


}
