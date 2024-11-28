#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <signal.h>

#define ROWS 15
#define COLS 15
#define INITIAL_SNAKE_LENGTH 1

// Snake structure
typedef struct {
    int row, col;
} SnakePart;

// Global terminal settings and snake pointer
struct termios original_termios;
SnakePart* snake = NULL; // Declare globally

// Function prototypes
void setInputMode();
void restoreInputMode();
void initializeGrid(char grid[ROWS][COLS]);
void printGrid(char grid[ROWS][COLS], int score);
void placeBait(int* baitRow, int* baitCol, SnakePart* snake, int length);
void updateGrid(char grid[ROWS][COLS], SnakePart* snake, int length, int baitRow, int baitCol);
int moveSnake(SnakePart* snake, int* length, char direction, int baitRow, int baitCol);
int isMoveValid(SnakePart* snake, int length, SnakePart nextHead);
void handleSignal(int sig);

int main() {
    // Dynamically allocate memory for snake
    snake = malloc(sizeof(SnakePart) * (ROWS * COLS)); 
    if (!snake) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    char grid[ROWS][COLS];
    int snakeLength = INITIAL_SNAKE_LENGTH;
    int baitRow, baitCol, score = 0;
    char direction = 'd'; // Start moving to the right
    char input;
    int running = 1;

    // Set up signal handling
    signal(SIGINT, handleSignal);
    signal(SIGTERM, handleSignal);

    setInputMode();
    atexit(restoreInputMode);

    srand(time(NULL));
    initializeGrid(grid);

    // Initialize snake position
    snake[0].row = ROWS / 2;
    snake[0].col = COLS / 2;

    placeBait(&baitRow, &baitCol, snake, snakeLength);
    updateGrid(grid, snake, snakeLength, baitRow, baitCol);

    while (running) {
        usleep(200000); // Adjust game speed
        system("clear");
        printGrid(grid, score);
        printf("Use 'w', 'a', 's', 'd' to move. Press 'q' to quit.\n");

        // Non-blocking input
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0;
                break;
            } else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
                direction = input;
            }
        }

        // Move snake
        if (!moveSnake(snake, &snakeLength, direction, baitRow, baitCol)) {
            continue; // Invalid move, wait for next input
        }

        // Check if bait is eaten
        if (snake[0].row == baitRow && snake[0].col == baitCol) {
            score++;
            placeBait(&baitRow, &baitCol, snake, snakeLength);
        }

        updateGrid(grid, snake, snakeLength, baitRow, baitCol);
       
    }

    printf("\nGame Over. Final Score: %d\n", score);

    // Free allocated memory before exiting
    free(snake);

    return 0;
}

// Set terminal input mode for non-canonical input
void setInputMode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    new_termios.c_cc[VMIN] = 0; // Non-blocking input
    new_termios.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

// Restore terminal settings
void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// Handle graceful exit for signals
void handleSignal(int sig) {
    restoreInputMode();
    if (snake) {
        free(snake);  // Free dynamically allocated memory
    }
    exit(0);      // Exit the program
}

// Initialize the grid
void initializeGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = '.'; // Reset grid
        }
    }
}

// Print the grid and score
void printGrid(char grid[ROWS][COLS], int score) {
    printf("Score: %d\n\n", score);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Place bait at a random location
void placeBait(int* baitRow, int* baitCol, SnakePart* snake, int length) {
    int valid = 0;
    while (!valid) {
        *baitRow = rand() % ROWS;
        *baitCol = rand() % COLS;
        valid = 1;

        // Ensure bait doesn't overlap with snake
        for (int i = 0; i < length; i++) {
            if (snake[i].row == *baitRow && snake[i].col == *baitCol) {
                valid = 0;
                break;
            }
        }
    }
}

// Update the grid
void updateGrid(char grid[ROWS][COLS], SnakePart* snake, int length, int baitRow, int baitCol) {
    initializeGrid(grid); // Clear grid

    // Place snake
    for (int i = 0; i < length; i++) {
        if (i == 0) {
            grid[snake[i].row][snake[i].col] = 'O'; // Head
        } else {
            grid[snake[i].row][snake[i].col] = '#'; // Body
        }
    }

    // Place bait
    grid[baitRow][baitCol] = 'X';
}

// Move the snake
int moveSnake(SnakePart* snake, int* length, char direction, int baitRow, int baitCol) {
    SnakePart nextHead = snake[0];

    // Calculate next position
    switch (direction) {
        case 'w': nextHead.row--; break;
        case 'a': nextHead.col--; break;
        case 's': nextHead.row++; break;
        case 'd': nextHead.col++; break;
    }

    // Check for border collision
    if (nextHead.row < 0 || nextHead.row >= ROWS || nextHead.col < 0 || nextHead.col >= COLS) {
        return 0; // Invalid move
    }

    // Check for self-collision
    for (int i = 0; i < *length; i++) {
        if (nextHead.row == snake[i].row && nextHead.col == snake[i].col) {
            return 0; // Invalid move
        }
    }

    // Move body
    for (int i = *length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Update head
    snake[0] = nextHead;

    // Handle snake growth
    if (nextHead.row == baitRow && nextHead.col == baitCol) {
        (*length)++;
        snake[*length - 1] = snake[*length - 2]; // Initialize new tail
    }

    return 1; // Valid move
}
