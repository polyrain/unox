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

/* Contains information describing a card */
typedef struct Card {
    char cardType; //Can be N for number, F for Draw4, T for Draw2, R for 
    int colorOne; // Reverse, W for Wild and S for Skip {N,F,T,R,W,B}
    int colorTwo; // Used when one of F or W
    int cardValue; // Number on the card, used for N
}Card;


/* Contains information describing a player.
 *  - Cards in hand
 *  - Number of cards in hand
 */
typedef struct Player {
    Card* hand; // The current hand of cards
    int numCards; // How many do we have?
}Player;

/* Contains information describing a game.
 * - List of players
 * - Last played card (card on top of stack)
 * - Player who played last card
 */
typedef struct Game {
    Player* players; // All players in this game; p0 is always current client
    Card* lastPlayed; // What was last played by a player?
    int lastPlayer; // Who played the last card?
    int numPlayers; // How many are playing with us?
    int clientNum; // Which player are we in the list?
}Game;


/* ALL CARD TYPES NOTE FOR ME
 * DRAW FOUR: Two lines for words
 * REVERSE: One line 
 * DRAW TWO: Two lines
 * NUMBER CARD: One line, numbers in corner?
 * SKIP: One line
 * WILD: One line
 */

/* Card dimensions are 12 by 11 iirc */

void output_card(char* cardPlayed, int row, int col, int color, int color2);
void output_display(Game* game);
void setup_ncurse(void); 

int main() {
    setup_ncurse();
    //setup_game();
    output_display(NULL);
    refresh();
    getch();
    endwin();

    return 0;
}

/* 
 * All backend ncurse content setup; 
 * Initialize screen, take raw input,
 * establish color pairings
 */
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
void output_display(Game* game) {
    int row, col; // Size of screen in terms of rows and columns
    getmaxyx(stdscr, row,col); // Move all these inits into another func
    // Grabbing the screensize
    output_card(game->lastPlayed, row, col); // We pass this due to consistency
    output_hand(game->players[game->clientNum], row, col);

}

/*
    // Everything below this is now defunct
    int maxRow = row; // ??? Not sure why this is here
    row -= 15; // Put this in the middle top and fix this!!
    output_card(player.hand[0], row/2, (col - 43)/2, 4, 5); // what are these #'s?
}

*/


/*

void output_card(char* cardPlayed, int row, int col, int color, int color2) {
    int rowUpdate = 5; // This is how many rows there are in the top of it
    attron(COLOR_PAIR(BORDER)); // Move it elsewherw
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

*/


/*
 * Draws the blank space in the card
 */

/*
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

void print_card(Card card, int row, int col) {
    int rowUpdate = 5; // Causes this to become idk
    attron(COLOR_PAIR(BORDER));
    mvprintw((row)-rowUpdate--, col, "+----------+");
    // SWITCH NOW BASED ON THE CARD
    switch (card.cardType) {
        case N:
            print_number_card(card, row, col);
            break;

        case F:
        case W:
            print_wild_card(card, row, col);
            break;
        case T:
            print_take_two(card, row, col);
            break;
        case S:
            print_skip_card(card, row, col);
            break;
        case R:
            print_reverse_card(card, row, col);
            break;
        default:
            // Error out, free memory, kill client
    }
}


void output_hand(Player* player, int row, int col) {
    init_pair(1, COLOR_WHITE, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
}

*/
