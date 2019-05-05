#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "client.h"

#define EMPTY "            " // Define empty space
#define TAKE_TWO_TOP "[][]        " // Details for card edges
#define TAKE_TWO_BOTTOM "        [][]"
#define NUMBER_TOP "%d           "
#define NUMBER_BOTTOM "           %d"
#define REVERSE_TOP "<->         "
#define REVERSE_BOTTOM "         <->"
#define SKIP_TOP "<X>         "
#define SKIP_BOTTOM "         <X>"
#define REVERSE_TEXT "   REVERSE  "
#define SKIP_TEXT "    SKIP    "
#define TAKE_TWO_TEXT "  TAKE TWO  "
#define HALF_EMPTY "      "

/* Enum to describe the four colors, and border color, used by card
 * rendering */
enum CardColors {
    BLUE = 1,
    BORDER = 2,
    RED = 3,
    YELLOW = 4,
    GREEN = 5
};


int main() {
    setup_ncurse();

    Game* newGame = (Game*) malloc(sizeof(Game));
    //setup_game();
    output_display(newGame);
    refresh();
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
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    curs_set(0);
    // Setup color attrs 
    init_pair(BLUE, COLOR_WHITE, COLOR_BLUE); 
    init_pair(BORDER, COLOR_WHITE, COLOR_WHITE);
    init_pair(RED, COLOR_WHITE, COLOR_RED);
    init_pair(YELLOW, COLOR_WHITE, COLOR_YELLOW);
    init_pair(GREEN, COLOR_WHITE, COLOR_GREEN);
}


void test_game(Game* game) {
    Player* newPlayer = (Player*) malloc(sizeof(Player));
    Card newCard1, newCard2, newCard3, newCard4;
    Card* cards = (Card*) malloc(sizeof(Card) * 4);
    newCard1.colorOne = BLUE;
    newCard1.cardValue = 6;
    newCard1.cardType = 'N';
    newCard2.colorOne = RED;
    newCard2.cardType = 'R';
    newCard3.cardType = 'W';
    newCard4.colorOne = GREEN;
    newCard4.cardType = 'S';
    cards[0] = newCard1;
    cards[1] = newCard2;
    cards[3] = newCard3;
    cards[2] = newCard4;

    newPlayer->currentCard = 0;
    newPlayer->hand = cards;
    newPlayer->numCards = 4;
    game->clientNum = 0;
    game->players = (Player**) malloc(sizeof(Player*) * 2);
    game->players[0] = newPlayer;
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard*/
void output_display(Game* game) {
    int row, col; // Size of screen in terms of rows and columns
    getmaxyx(stdscr, row,col); // Grab screen size to try and adapt drawing
    /* Test card until Game functionality begins */
    test_game(game);
    Card newCard;
    int key = 0, currentCard = 0;
    newCard.colorOne = BLUE;
    newCard.cardValue = 7;
    newCard.cardType = 'T';
    int initialY = (row/2) - 10; // Place the start 6 offsert
    int initialX = (col/2) - 6; 
    print_card(&(game->players[game->clientNum]->hand[2]), initialY, initialX, 1);
    print_hand(game->players[game->clientNum], row - 30, 0);
    while(1) { // TEST INTERACTION
        refresh();
        key = getch();
        if (key == KEY_LEFT && game->players[game->clientNum]->currentCard != 0) {
            game->players[game->clientNum]->currentCard--;
            print_hand(game->players[game->clientNum], row - 30, 0);
        } else if (key == KEY_RIGHT && game->players[game->clientNum]->currentCard != 3) {
            game->players[game->clientNum]->currentCard++;
            print_hand(game->players[game->clientNum], row - 30, 0);
        } else if (key == 'q' || key == 'Q') {
            return;
        }
        refresh();
    }
}


/*  Prints the clients hand to the screen */ 
void print_hand(Player* player, int row, int col) {
    int newRow = row + 15;
   /* Card arr[4];
    Card newCard;
    newCard.colorOne = BLUE;
    newCard.cardValue = 7;
    newCard.cardType = 'T';

    Card newCard1;
    newCard1.colorOne = RED;
    newCard1.cardValue = 7;
    newCard1.cardType = 'W';


    Card newCard2;
    newCard2.colorOne = RED;
    newCard2.cardValue = 7;
    newCard2.cardType = 'N';

    Card newCard3;
    newCard3.colorOne = GREEN;
    newCard3.cardValue = 7;
    newCard3.cardType = 'S';

    arr[0] = newCard;
    arr[1] = newCard1;
    arr[2] = newCard2;
    arr[3] = newCard3;

    for (int i = 0; i < 4; i++) {
       col += 18; // Jump across ever time
        // NEED TO HANDLE THE FACT YOUR CARDS MIGHT GO OFF SCREEN 
        print_card(&(arr[i]), newRow, col);
    }*/
    
    for (int i = 0; i < player->numCards; i++) {
        if (i == player->currentCard) { // Bold current selections
            print_card(&(player->hand[i]), newRow, col, 1);
            
            if (i == 2) {
                mvprintw(0,0, "AHAHAHA");
            }
        } else {
            print_card(&(player->hand[i]), newRow, col, 0);
        }
        col += 18;
    }
}


/*
 * Outputs last played card on pile. Draws in centre of screen,
 * determined from values passed by value to function.
 * CARD DIMENSIONS: 15 characters wide, 10 characters tall
 */
void output_card(Card* lastPlayed, int row, int col) {
    int initialY = (row/2) - 10; // Place the start 6 offsert
    int initialX = (col/2) - 6; 

    attron(COLOR_PAIR(BORDER)); // Begin drawing
    mvprintw(initialX, initialY, "+-----------+");

    for (int i = 0; i < 5; i++) {
        mvprintw(initialY++, initialX, "+-----------+");
    }
    for (int i = 0; i < 5; i++) {
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(RED));
        mvprintw(initialY++, initialX, EMPTY);
    }

}

/* 
 * Helper function to draw top and bottom edge of cards, to ensure
 * a consistent border around all card edges. Updates current row
 * through pointer passed to it
 */
void print_card_edge(int* row, int col) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "+------------+");
    attroff(COLOR_PAIR(BORDER)); // Draw the border on edge

}



/*
 * Prints empty space on cards, which represent the top and bottom 4 
 * spaces of the cards size. Used by card drawing functions to 
 * fill the card space with relevant coloring
 */
void print_empty_space(Card* card, int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(card->colorOne));
        printw(EMPTY);
        attroff(COLOR_PAIR(card->colorOne));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}


/* Method to print the text line of a card. Reverse,
 * skip and take two use this to print the relevant words
 * on their cards, int he correct space */
void print_card_text(Card* card, int* row, int col) {
   
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));

    switch (card->cardType) {
        case 'R':
            printw(REVERSE_TEXT);
            break;
        case 'S':
            printw(SKIP_TEXT);
            break;
        case 'T':
            printw(TAKE_TWO_TEXT);
            break;
        case 'N':
            printw(EMPTY);
            break; // Number cards have no text 
        
        default:
            printf("AAAA");
    }

    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
}


/* Method for printing rainbow cards. These require a different
 * printing method due to the more intricate design process
 * that goes into them. Switches text based on if you have a wild
 * or a draw four card */
void print_wild_card(Card* card, int* row, int col) {
    print_top_wild(row, col);
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(RED));

    switch (card->cardType) {
        case 'F':
            printw("   DRA");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("W4    ");
            attroff(COLOR_PAIR(BLUE));
            break;
        case 'W':
            printw("    WI");
            attroff(COLOR_PAIR(RED));
            attron(COLOR_PAIR(BLUE));
            printw("LD    ");
            attroff(COLOR_PAIR(BLUE));
            break;
    }

    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));

    print_bottom_wild(row, col);
}


/* Method to print the top two rainbow parts of a rainbow card */
void print_top_wild(int* row, int col) {
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(RED));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(RED));
        attron(COLOR_PAIR(YELLOW));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(YELLOW));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}

/* Method to print the bottom two rainbow parts of a rainbow card */
void print_bottom_wild(int* row, int col) {
    for (int i = 0; i < 5; i++) {
        attron(COLOR_PAIR(BORDER));
        mvprintw((*row)++, col, "|");
        attroff(COLOR_PAIR(BORDER));
        attron(COLOR_PAIR(GREEN));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(GREEN));
        attron(COLOR_PAIR(BLUE));
        printw(HALF_EMPTY);
        attroff(COLOR_PAIR(BLUE));
        attron(COLOR_PAIR(BORDER));
        printw("|");
        attroff(COLOR_PAIR(BORDER));
    }
}


/* Prints characters in top left or bottom right corner of card
 * depending on what value the "top" flag is set to. 
 */
void print_card_detail(Card* card, int* row, int col, int top) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, "|");
    attroff(COLOR_PAIR(BORDER));
    attron(COLOR_PAIR(card->colorOne));
    switch (card->cardType) {
        case 'N':
            if (top) {
                printw(NUMBER_TOP, card->cardValue);
            } else {
                printw(NUMBER_BOTTOM, card->cardValue);
            }
            break;
        case 'T':
            if (top) {
                printw(TAKE_TWO_TOP); 
            } else {
                printw(TAKE_TWO_BOTTOM);
            }
            break;
        
        case 'S':
            if (top) {
                printw(SKIP_TOP);
            } else {
                printw(SKIP_BOTTOM);
            }
            break;
            
        case 'R':
            if (top) {
                printw(REVERSE_TOP);
            } else {
                printw(REVERSE_BOTTOM);
            }
            break;
        default:
            //error
            printf("AHH");
    }

    attroff(COLOR_PAIR(card->colorOne));
    attron(COLOR_PAIR(BORDER));
    printw("|");
    attroff(COLOR_PAIR(BORDER));
}

/*
 * Takes a card pointer and prints off the relevant card at the designated
 * position on the screen.
 */
void print_card(Card* card, int row, int col, int bold) {
    if(bold) {
        attron(A_BOLD);
    }
    print_card_edge(&row, col);
    if (card->cardType == 'F' || card->cardType == 'W') {
        print_wild_card(card, &row, col);
    } else {
        print_card_detail(card, &row, col, 1);
        print_empty_space(card, &row, col);
        print_card_text(card, &row, col);
        print_empty_space(card, &row, col);
        print_card_detail(card, &row, col, 0);
    }
    print_card_edge(&row, col);
    if (bold) {
        attroff(A_BOLD);
    }
    refresh();
}
