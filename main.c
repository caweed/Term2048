#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLACK 7
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define MAGENTA 6

int board[4][4] = {
	0
};
int colors[] = { BLACK, RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA };
int score = 0;

void init();
void addGrid(int (*)[4]);
void killBlank(int (*)[4]);
void rollRightNinety(int (*)[4]);
void rollAroundH(int (*)[4]);
void rollAroundV(int (*)[4]);
bool turnLeft(int (*)[4]);
bool turnRight(int (*)[4]);
bool turnUp(int (*)[4]);
bool turnDown(int (*)[4]);
void drawBoard(int (*)[4]);

void swap(int *a, int *b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

void copy(int boardA[4][4], const int board[4][4])
{
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			boardA[i][j] = board[i][j];
}

bool equal(const int boardA[4][4], const int boardB[4][4])
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			if(boardA[i][j] != boardB[i][j])
			{
				return false;
				break;
			}
		}
	}
	return true;
}

void init()
{
	initscr();
	noecho();
	cbreak();
	curs_set(0);
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_RED);
	init_pair(2, COLOR_WHITE, COLOR_YELLOW);
	init_pair(3, COLOR_WHITE, COLOR_GREEN);
	init_pair(4, COLOR_WHITE, COLOR_CYAN);
	init_pair(5, COLOR_WHITE, COLOR_BLUE);
	init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(7, COLOR_WHITE, COLOR_BLACK);
	
	addGrid(board);
	addGrid(board);
	drawBoard(board);
}

void killBlank(int board[4][4])
{
	for(int i = 0; i < 4; ++i)
		for(int count = 0; count < 2; ++count)
			for(int j = 0; j < 3; ++j)
				if(board[i][j] == 0)
					swap(&board[i][j], &board[i][j+1]);
}

void addGrid(int board[4][4])
{
	static bool initialized = false;
	int grids[11] = { 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4 };
	
	if(!initialized)
	{
		srand(time(0));
		initialized = true;
	}
	
	int choice = rand() % 11;
	while(true)
	{
		int row = rand() % 4;
		int col = rand() % 4;
		if(board[row][col] == 0)
		{
			board[row][col] = grids[choice];
			break;
		}
	}
}

void rollRightNinety(int board[4][4])
{
	int temp[4][4];
	copy(temp, board);
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			board[i][j] = temp[3-j][i];
}

void rollAroundH(int board[4][4])
{
	int temp[4][4];
	copy(temp, board);
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			board[i][j] = temp[i][3-j];
}

void rollAroundV(int board[4][4])
{
	int temp[4][4];
	copy(temp, board);
	for(int i = 0; i < 4; ++i)
		for(int j = 0; j < 4; ++j)
			board[i][j] = temp[3-i][j];
}

bool turnLeft(int board[4][4])
{
	killBlank(board);
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 3; ++j)
		{
			int start = board[i][j];
			if(start == 0) break;
			int tempJ = j;
			int sameIndex = -1;
			while(tempJ < 3 && start == board[i][tempJ+1])
			{
				++tempJ;
				sameIndex = tempJ;
			}
			if(sameIndex == -1)
				continue;
			board[i][j] *= 2;
			board[i][sameIndex] = 0;
		}
	}
	killBlank(board);
	return false;
}

bool turnRight(int board[4][4])
{
	rollAroundH(board);
	turnLeft(board);
	rollAroundH(board);
	return false;
}

bool turnUp(int board[4][4])
{
	rollRightNinety(board);
	rollRightNinety(board);
	rollRightNinety(board);
	turnLeft(board);
	rollRightNinety(board);
	return false;
}

bool turnDown(int board[4][4])
{
	rollAroundV(board);
	turnUp(board);
	rollAroundV(board);
	return false;
}

void drawBoard(int board[4][4])
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			int start_x = 3 * i;
			int start_y = 7 * j;
			int value = board[i][j];
			int color;
			char str[4];
			sprintf(str, "%d", value);
			int position = start_y + (7-strlen(str))/2;
			
			switch(value)
			{
				default:
				case 0: color = colors[0]; break;
				case 2: color = colors[1]; break;
				case 4: color = colors[2]; break;
				case 8: color = colors[3]; break;
				case 16: color = colors[4]; break;
				case 32: color = colors[5]; break;
				case 64: color = colors[6]; break;
				case 128: color = colors[1]; break;
				case 256: color = colors[2]; break;
				case 512: color = colors[3]; break;
				case 1024: color = colors[4]; break;
				case 2048: color = colors[5]; break;
				case 4096: color = colors[6]; break;
				case 8192: color = colors[0]; break;
			}
			attron(COLOR_PAIR(color));
			if(value > 64)
				attron(A_BOLD);
			mvprintw(start_x, start_y, "       ");
			mvprintw(start_x + 1, start_y, "       ");
			mvprintw(start_x + 2, start_y, "       ");
			mvprintw(start_x + 1, position, "%d", value);
			if(value == 0)
				mvprintw(start_x + 1, position, "·");
			attroff(COLOR_PAIR(color));
			if(value > 64)
				attroff(A_BOLD);
		}
	}
	//mvprintw(12, 3, "Scores: %d", score);
	refresh();
}

bool isFinished(int board[4][4])
{
	int temp[4][4];
	copy(temp, board);
	turnLeft(temp);
	bool l = equal(temp, board);
	copy(temp, board);
	turnRight(temp);
	bool r = equal(temp, board);
	copy(temp, board);
	turnUp(temp);
	bool u = equal(temp, board);
	copy(temp, board);
	turnDown(temp);
	bool d = equal(temp, board);
	return l && r && u && d;
}

int main()
{
	init();
	int key;
	while((key = getch()) != 'q')
	{
		int beforeBoard[4][4];
		copy(beforeBoard, board);
		switch(key)
		{
			case '2':
				turnUp(board);
				break;
			case '4':
				turnLeft(board);
				break;
			case '8':
				turnDown(board);
				break;
			case '6':
				turnRight(board);
				break;
			default:
				break;
		}
		if(isFinished(board))
		{
			mvprintw(13, 5, "Game Over! ");
			mvprintw(14, 3, "\'r\' to restart, or \'q\' to exit");
			refresh();
			int ch;
			while((ch = getch()) != 'q')
			{
				if(ch == 'r') 
				{
					int temp[4][4] = {0};
					copy(board, temp);
					addGrid(board);
					mvprintw(13, 5, "            ");
					mvprintw(14, 3, "                            ");
					refresh();
					break;
				}
			}
		}
		if(!equal(beforeBoard, board))
		{
			addGrid(board);
			drawBoard(board);
		}
	}
	endwin();
	return 0;
}

