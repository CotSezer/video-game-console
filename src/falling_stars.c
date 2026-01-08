#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define ROWS 15
#define COLS 20
#define PADDLE_WIDTH 3

// Global variables
struct termios original_termios;
int paddle_pos = COLS / 2 - 1;
char direction = '\0';
int running = 1;
int score = 0;

// Function prototypes
void initializeGrid(char grid[ROWS][COLS]);
void printGrid(char grid[ROWS][COLS]);
void updateGrid(char grid[ROWS][COLS]);
void movePaddle();
void dropStar(char grid[ROWS][COLS]);
int checkCollision(char grid[ROWS][COLS]);
int checkGameOver(char grid[ROWS][COLS]);
void setInputMode();
void restoreInputMode();
void signalHandler(int signo);
void *handleInput(void *arg);
void finishScreen(int score);

int main() {
    char grid[ROWS][COLS];
    pthread_t inputThread;

    srand(time(NULL));
    setInputMode();
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    initializeGrid(grid);

    // Create a thread to handle continuous input
    pthread_create(&inputThread, NULL, handleInput, NULL);

    while (running) {
        system("clear");
        printGrid(grid);
        printf("Score: %d\n", score);
        printf("Use 'a' to move left, 'd' to move right, 'q' to quit.\n");

        if (direction) {
            movePaddle();
        }

        dropStar(grid);
        if (checkCollision(grid)) {
            score++;
        }
        if (checkGameOver(grid)) {
            running = 0;
            break;
        }
        updateGrid(grid);
        usleep(200000 - (score * 1000)); // Increase speed with score
    }

    restoreInputMode();
    finishScreen(score); // Call the finish screen
    pthread_cancel(inputThread); // End the input thread
    pthread_join(inputThread, NULL);
    return 0;
}

void initializeGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = ' ';
        }
    }
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        grid[ROWS - 1][i] = '=';
    }
}

void printGrid(char grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%c", grid[i][j]);
        }
        printf("\n");
    }
}

void updateGrid(char grid[ROWS][COLS]) {
    for (int i = ROWS - 2; i >= 0; i--) {
        for (int j = 0; j < COLS; j++) {
            grid[i + 1][j] = grid[i][j];
        }
    }
    for (int i = 0; i < COLS; i++) {
        grid[0][i] = ' ';
    }
    for (int i = 0; i < COLS; i++) {
        grid[ROWS - 1][i] = ' ';
    }
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        grid[ROWS - 1][i] = '=';
    }
}

void movePaddle() {
    if (direction == 'a' && paddle_pos > 0) {
        paddle_pos--;
    } else if (direction == 'd' && paddle_pos + PADDLE_WIDTH < COLS) {
        paddle_pos++;
    }
}

void dropStar(char grid[ROWS][COLS]) {
    int col = rand() % COLS;
    grid[0][col] = '*';
}

int checkCollision(char grid[ROWS][COLS]) {
    for (int i = paddle_pos; i < paddle_pos + PADDLE_WIDTH; i++) {
        if (grid[ROWS - 2][i] == '*') {
            grid[ROWS - 2][i] = ' '; // Remove the star on collision
            return 1;
        }
    }
    return 0;
}

int checkGameOver(char grid[ROWS][COLS]) {
    for (int i = 0; i < COLS; i++) {
        if (grid[ROWS - 1][i] == '*') {
            return 1;
        }
    }
    return 0;
}

void setInputMode() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &original_termios);
    new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void signalHandler(int signo) {
    restoreInputMode();
    printf("\nGame exited due to signal %d. Goodbye!\n", signo);
    exit(0);
}

void *handleInput(void *arg) {
    char input;
    while (1) {
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0;
                break;
            } else if (input == 'a' || input == 'd') {
                direction = input;
            } else {
                direction = '\0';
            }
        }
    }
    return NULL;
}

void finishScreen(int score) {
    system("clear");
    printf("\n");
    printf("####################################\n");
    printf("#                                  #\n");
    printf("#          GAME OVER!              #\n");
    printf("#                                  #\n");
    printf("#     Your Final Score: %d          #\n", score);
    printf("#                                  #\n");
    printf("####################################\n");
    printf("\nThank you for playing!\n");
    sleep(1);
}
