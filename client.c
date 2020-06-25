#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
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
#define NUM_CENTER "      %d     "
#define CARD_EDGE "+------------+"
#define CARD_HEIGHT 13
#define CARD_WIDTH 14
#define CHAT_PROMPT "Type your message: "
#define CHAT_LOG "%d:%d:%d %s"
#define MESSAGE_SIZE 80


// TODO: Make magic numbers be reactive to terminal size 
/* Enum to describe the four colors, and border color, used by card
 * rendering */
enum CardColors {
    BLUE = 1,
    BORDER = 2,
    RED = 3,
    YELLOW = 4,
    GREEN = 5
};

enum InputFlags {
    QUIT = 0,
    CHAT = 1,
    SELECT = -1
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
 * establish color pairings for use by drawing functions
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

/*
 * Initialize a new game. Allocate memory for players, chat
 * and a set of cards. 
 */
void init_game(Game* game) {
    for (int i = 0; i < CHAT_LOG_COUNT; i++) {
        game->chatLog[i] = (Message*) malloc(sizeof(Message)); // 30 char chat
        game->chatLog[i]->content = (char*) malloc(sizeof(char) * MESSAGE_SIZE);
    }

    game->numMessages = 0;

    // Malloc players here, then malloc the array that holds them 
    // As it's a VLA of one dimension, but it contains pointers
    // as it's elements, which hold onto player data
    // i.e -> game.players[0] -> player* -> <player info here>
    // while c99 supports VLA's, not backwards compatible
}


// Test method to construct a hand and player 
void test_game(Game* game) {
    init_game(game);
    Player* newPlayer = (Player*) malloc(sizeof(Player));
    Card newCard1, newCard2, newCard3, newCard4, newCard5;
    Card* cards = (Card*) malloc(sizeof(Card) * 5);
    newCard1.colorOne = BLUE;
    newCard1.cardValue = 9;
    newCard1.cardType = 'N';
    newCard2.colorOne = RED;
    newCard2.cardType = 'R';
    newCard3.cardType = 'F';
    newCard4.colorOne = GREEN;
    newCard4.cardType = 'S';
    newCard5.cardType = 'W';
    cards[0] = newCard1;
    cards[1] = newCard2;
    cards[3] = newCard3;
    cards[2] = newCard4;
    cards[4] = newCard5;

    newPlayer->currentCard = 0;
    newPlayer->hand = cards;
    newPlayer->numCards = 5; // 1 index'd
    game->clientNum = 0;
    game->players = (Player**) malloc(sizeof(Player*) * 2);
    game->players[0] = newPlayer;
}

/*
 * Draws on the screen the card last played, as well
 * as the players current hand. Gets the terminal 
 * size and ensures drawing is done to the best 
 * standard relative to the size received.
 * */
void output_display(Game* game) {
    int row, col; // Size of screen in terms of rows and columns
    getmaxyx(stdscr, row,col); // Grab screen size to try and adapt drawing
    /* Test card until Game functionality begins */
    test_game(game);
    int key = 0;
    int initialY = (row / 2) - CARD_HEIGHT / 2; // Place the start 6 offset so it positions in middle
    int initialX = (col / 2) - CARD_WIDTH / 2;

    print_card(&(game->players[game->clientNum]->hand[2]), initialY, initialX, 1); // Initial card to print
    print_hand(game->players[game->clientNum], row - CARD_HEIGHT - 2, 0); // -15 as Card is 13 tall, 2 spaces under for style
    init_game(game); 
    
    while(1) { // TEST INTERACTION
        key = getch(); // What did they press?
        int status = handle_input(key, game);
        if (status == QUIT) {
            return;
        } else if (status == CHAT) {
            handle_chat(game, row, col);
        }
        print_hand(game->players[game->clientNum], row - CARD_HEIGHT - 2, 0);
        refresh(); // Keep drawing
    }
}


void handle_chat(Game* game, int row, int col) {
    
    if (++(game->numMessages) > CHAT_LOG_COUNT) { // Check we haven't filled
        game->numMessages = CHAT_LOG_COUNT; // the bank
         
        for (int i = 0; i < CHAT_LOG_COUNT - 1; i++) {
            strcpy(game->chatLog[i]->content, game->chatLog[i + 1]->content); // 5 goes to 4 and so on
        } // If so shift it all down one
        strcpy(game->chatLog[CHAT_LOG_COUNT - 1]->content, "");
    }

    mvprintw(row - 2, (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE, CHAT_PROMPT); // Display prompt
    capture_message(game); // -2 stops overflow on screen size, without it touches card
    print_chat_buffer(game, row, col);
    move(row - 2, (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE); // Move cursor bacl
    clrtoeol(); // Wipe the prompt

    
}

/* Grabs a message from player via stdin, stores in the game */
void capture_message(Game* game) {
    Message* newMessage = game->chatLog[game->numMessages - 1];
    echo();
    getstr(newMessage->content); // Get input
    noecho();
    time_t rawTime; // Get the sys time
    time(&rawTime);
    struct tm* tm_struct = localtime(&rawTime);
    newMessage->hour = tm_struct->tm_hour;
    newMessage->min = tm_struct->tm_min;
    newMessage->sec = tm_struct->tm_sec; 
}

/* Prints the messages stored in game to terminal */
void print_chat_buffer(Game* game, int row, int col) {
   
    int spacing = 3; // Used for spacing messages; should only be 1 char
    for (int i = CHAT_LOG_COUNT - 1; i >= 0; i--) {
        if (strlen(game->chatLog[i]->content) > 0) {
            move((row - spacing), (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE);
            clrtoeol(); // Above wipes old line off screen
            mvprintw((row - spacing), (col - strlen(CHAT_PROMPT)) - MESSAGE_SIZE, 
                CHAT_LOG, game->chatLog[i]->hour,
                game->chatLog[i]->min, 
                game->chatLog[i]->sec, game->chatLog[i]->content);
            spacing++; // Print message to screen and incr spacing
        } 
    }

}

int handle_input(int key, Game* game) {
    Player* player = game->players[game->clientNum];
    switch (key) {
        case KEY_LEFT:
            if (player->currentCard != 0) {
                player->currentCard--;
            }
            break;
        case KEY_RIGHT:
            if (player->currentCard != player->numCards - 1) {
                player->currentCard++;
            }
            break;
        case 'q':
        case 'Q':
            return QUIT;
        case 't':
        case 'T':
            return CHAT;
        default:
            return SELECT;


    }
    return SELECT;
}


/*  Prints the clients hand to the screen */ 
void print_hand(Player* player, int row, int col) {
    
    // Above will increment forward in array by specified amount always
    // Where amount is 5 cards per page. 
    
    for (int i = 0; i < player->numCards; i++) {
        if (i == player->currentCard) { // Bold current selections
            print_card(&(player->hand[i]), row, col, 1);   
        } else {
            print_card(&(player->hand[i]), row, col, 0);
        }
        col += CARD_WIDTH + 4; // Jump over card we drew + 4 char gap TODO fix magic make reactive
    }
}


/* 
 * Helper function to draw top and bottom edge of cards, to ensure
 * a consistent border around all card edges. Updates current row
 * through pointer passed to it
 */
void print_card_edge(int* row, int col) {
    attron(COLOR_PAIR(BORDER));
    mvprintw((*row)++, col, CARD_EDGE);
    attroff(COLOR_PAIR(BORDER)); // Draw the border on edge
}



/*
 * Prints empty space on cards, which represent the top and bottom 4 
 * spaces of the cards size. Used by card drawing functions to 
 * fill the card space with relevant coloring
 */
void print_empty_space(Card* card, int* row, int col) {
    for (int i = 0; i < 4; i++) {
        attron(COLOR_PAIR(BORDER)); // Turn on the color of the border
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
            printw(NUM_CENTER, card->cardValue);
            break;  
        
        default:
            fprintf(stderr, "Invalid card type detected\n");
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

    attron(COLOR_PAIR(BORDER)); // Top colors, now draw special interior
    mvprintw((*row)++, col, "|"); // Text differs from WILD and DRAW4
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

        default:
            fprintf(stderr, "Invalid wild card found!");
    }

    attron(COLOR_PAIR(BORDER));
    printw("|"); // Close up this one line, and draw rest of bottom
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
            // error, shouldn't be reached
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
