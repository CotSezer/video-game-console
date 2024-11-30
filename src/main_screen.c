#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/wait.h>

#include <libgen.h>   // For dirname()
#include <limits.h>   // For PATH_MAX

#define MAX_GAMES 100
#define MAX_NAME_LENGTH 256

// Global terminal attributes
struct termios original_termios;
pid_t child_pid = -1; // Initialize to -1 to indicate no child running

// Function prototypes
void setInputMode();
void restoreInputMode();
void signalHandler(int sig);
int scanGames(char games[MAX_GAMES][MAX_NAME_LENGTH]);
void printMenu(char games[MAX_GAMES][MAX_NAME_LENGTH], int gameCount, int selectedGame, int exitSelected);
void startGame(char* gameName);

int main(int argc, char *argv[]) {
    // Buffer to hold the path
    char exe_path[PATH_MAX];

    // Get the directory containing the executable
    realpath(argv[0], exe_path);   // Resolve full path of the executable
    char *dir_name = dirname(exe_path);

    // Change the current working directory to the directory of the executable
    if (chdir(dir_name) != 0) {
        perror("chdir failed");
        return 1;
    }

    // Array to store game names
    char games[MAX_GAMES][MAX_NAME_LENGTH];
    int gameCount = scanGames(games);
    int selectedGame = 0;
    int exitSelected = 0;
    char input;
    int running = 1;

    // Check if there are games to display
    if (gameCount == 0) {
        printf("No games found in the current directory.\n");
        return 1;
    }

    // Setup terminal and signal handling
    setInputMode(); // Set non-canonical input mode
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Main menu loop
    while (running) {
        system("clear");
        printMenu(games, gameCount, selectedGame, exitSelected);

        // Handle user input
        if (read(STDIN_FILENO, &input, 1) > 0) {
            if (input == 'q') {
                running = 0; // Exit the main screen
            } else if (input == 'w') {
                // Navigate up
                if (exitSelected) {
                    exitSelected = 0;
                    selectedGame = gameCount - 1;
                } else if (selectedGame > 0) {
                    selectedGame--;
                } else {
                    exitSelected = 1;
                }
            } else if (input == 's') {
                // Navigate down
                if (!exitSelected && selectedGame == gameCount - 1) {
                    exitSelected = 1;
                } else if (exitSelected) {
                    exitSelected = 0;
                    selectedGame = 0;
                } else {
                    selectedGame++;
                }
            } else if (input == 'a' || input == 'd') {
                // Toggle between menu and exit
                exitSelected = !exitSelected;
            } else if (input == '\n') {
                if (exitSelected) {
                    running = 0; // Exit the main screen
                } else {
                    startGame(games[selectedGame]); // Launch selected game
                }
            }
        }
    }

    restoreInputMode();
    printf("\nThank you for using the video game console! Goodbye!\n");
    return 0;
}

// Set terminal input mode for non-canonical input
void setInputMode() {
    tcgetattr(STDIN_FILENO, &original_termios); // Get current terminal attributes
    struct termios new_termios = original_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios); // Apply new settings
}

// Restore the original terminal settings
void restoreInputMode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

// Signal handler for graceful exit
void signalHandler(int sig) {
    // If a child process is running, terminate it
    if (child_pid > 0) {
        kill(child_pid, sig);   // Forward the signal to the child
        waitpid(child_pid, NULL, 0); // Wait for the child to terminate
    }

    restoreInputMode(); // Restore terminal settings

    printf("\nMain screen exited due to signal %d. Goodbye!\n", sig);
    fflush(stdout); // Ensure the message is printed immediately

    exit(0);
}

// Scan the current directory for games with names starting with "game_"
int scanGames(char games[MAX_GAMES][MAX_NAME_LENGTH]) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("Unable to open directory");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "game_", 5) == 0 && strstr(entry->d_name, ".c") == NULL) {
            strncpy(games[count], entry->d_name, MAX_NAME_LENGTH);
            games[count][MAX_NAME_LENGTH - 1] = '\0'; // Ensure null termination
            count++;
            if (count >= MAX_GAMES) break; // Limit reached
        }
    }
    closedir(dir);
    return count;
}

// Print the main menu
void printMenu(char games[MAX_GAMES][MAX_NAME_LENGTH], int gameCount, int selectedGame, int exitSelected) {
    printf("=== Video Game Console ===\n");
    printf("Use 'w' and 's' to navigate, 'a' and 'd' to toggle, 'Enter' to select, and 'q' to quit.\n");
    printf("---------------------------\n");

    for (int i = 0; i < gameCount; i++) {
        if (!exitSelected && i == selectedGame) {
            printf(" > %s <\n", games[i]); // Highlight selected game
        } else {
            printf("   %s\n", games[i]);
        }
    }

    if (exitSelected) {
        printf(" > Exit <\n"); // Highlight Exit when selected
    } else {
        printf("   Exit\n");
    }

    printf("---------------------------\n");
}

// Start the selected game using fork()
// Start the selected game using fork()
void startGame(char* gameName) {
    child_pid = fork();

    if (child_pid == -1) {
        perror("Failed to fork");
        return;
    }

    if (child_pid == 0) {
        // Child process: Execute the game
        char command[PATH_MAX];
        snprintf(command, sizeof(command), "./%s", gameName); // Add "./" prefix for relative path

        execl(command, gameName, (char *)NULL);

        // If execl fails, print an error and exit
        perror("Failed to start game");
        exit(EXIT_FAILURE);
    } else {
        // Parent process: Wait for the child to finish
        int status;
        waitpid(child_pid, &status, 0);
        child_pid = -1; // Reset child_pid after the child process exits
    }
}

