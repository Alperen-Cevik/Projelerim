#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define SIZE 3

#ifdef _WIN32
#define _scanf scanf_s
#define clearConsole() system("cls")
#else
#define _scanf scanf
#define clearConsole() system("clear")
#endif

char board[SIZE][SIZE];

void initializeGameBoard() {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			board[i][j] = ' ';
		}
	}
}

void printGameBoard() {
	clearConsole();

	printf("\n");
	for (int i = 0; i < SIZE; i++) {
		printf(" %c | %c | %c ", board[i][0], board[i][1], board[i][2]);
		if (i != SIZE - 1) {
			printf("\n---|---|---\n");
		}
	}
	printf("\n");
}

int isValidMove(int row, int col) {
	if (row < 0 || row >= SIZE || col < 0 || col >= SIZE) { // check if row and col are within bounds
		return 0;
	}
	if (board[row][col] == ' ') { // check for space
		return 1;
	}
	return 0;
}

int isPlayerWinning(char symbol) {
	// Check rows
	for (int i = 0; i < SIZE; i++) {
		if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) {
			return 1;
		}
	}

	// Check columns
	for (int i = 0; i < SIZE; i++) {
		if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) {
			return 1;
		}
	}

	// Check diagonals
	if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) {
		return 1;
	}

	if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) {
		return 1;
	}

	return 0;
}

int isGameBoardFull() {
	int i, j;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (board[i][j] == ' ') {
				return 0;
			}
		}
	}
	return 1;
}

void makeMove(int row, int col, char symbol) {
	board[row][col] = symbol;
}

void flushStdin() { // clear input buffer
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

void makePlayerMove() {
	int row, col;
	printf("Enter your move (row [1-%d] and column [1-%d]): ", SIZE, SIZE);
	_scanf("%d %d", &row, &col);
	flushStdin();
	row--;  // Adjust for 0-based index
	col--;  // Adjust for 0-based index
	if (isValidMove(row, col)) {
		makeMove(row, col, 'X');
	}
	else {
		printf("Invalid move. Try again.\n");
		makePlayerMove();
	}
}

void makeComputerMove(int difficulty) {
	int row, col;
	int i, j;

	// Check for winning move
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (isValidMove(i, j)) {
				makeMove(i, j, 'O');
				if (isPlayerWinning('O')) {
					return;
				}
				board[i][j] = ' ';  // Undo the move
			}
		}
	}

	int chance = rand() % 10;
	if (difficulty > chance) {
		// Check for user's winning move and block it
		for (i = 0; i < SIZE; i++) {
			for (j = 0; j < SIZE; j++) {
				if (isValidMove(i, j)) {
					makeMove(i, j, 'X');
					if (isPlayerWinning('X')) {
						makeMove(i, j, 'O');
						return;
					}
					board[i][j] = ' ';  // Undo the move
				}
			}
		}
	}

	// If no winning move or blocking move, make a random move
	do {
		row = rand() % SIZE;
		col = rand() % SIZE;
	} while (!isValidMove(row, col));
	makeMove(row, col, 'O');
}

int main() {
	srand(time(NULL));

	while (1) {
		int gameDifficulty = 0;

		do {
			printf("Enter game difficulty [1-10]: ");
			_scanf("%d", &gameDifficulty);
			flushStdin();
		}
		while (gameDifficulty < 1 || gameDifficulty > 10);

		initializeGameBoard();
		printGameBoard();

		int player = 0;
		int status = 0;
		while (!status) {
			if (player == 0) {
				makePlayerMove();
			}
			else {
				makeComputerMove(gameDifficulty);
			}

			printGameBoard();
			status = isPlayerWinning(player == 0 ? 'X' : 'O');
			if (status) {
				printf("Player %c wins!\n", player == 0 ? 'X' : 'O');
				break;
			}

			if (isGameBoardFull()) {
				printf("It's a tie!\n");
				break;
			}

			player = !player;
		}

		printf("Would you like to play again? [y/n]: ");
		char ans;
		_scanf("%c", &ans);
		flushStdin();

		if (tolower(ans) == 'n')
			break;

		clearConsole();
	}


	return 0;
}
