

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

void output_card(Card* lastPlayed, int row, int col);
void output_display(Game* game);
void setup_ncurse(void);
void print_number_card(Card* card, int* row, int col);
void print_empty_space(Card* card, int* row, int col);
void print_reverse_card(Card* card, int* row, int col);
void print_bottom_wild(int* row, int col);
void print_top_wild(int* row, int col);
void print_wild_card(Card* card, int* row, int col);
void print_skip_card(Card* card, int* row, int col);
void print_card(Card* card, int row, int col);
void print_take_two(Card* card, int* row, int col);
void print_card_detail(Card* card, int* row, int col, int top); 
