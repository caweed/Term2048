#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "util.h"

#ifdef USE_DOT
#define BLANK_NUMBER "·"
#else
#define BLANK_NUMBER "-"
#endif

#define BLACK 7
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define MAGENTA 6
#define CHOICE_BOX_COLOR 8

int board[4][4] = { 0 };
int colors[] = { BLACK, RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA };
int score = 0;
int bestScore = 0;

const char dataStoreDir[] = "/sdcard/Android/";//"/data/data/io.github.caweed.term2048/files";
const char gameDataFileName[] = "game.data";
const char dataConfigFileName[] = "config";
const char scoresFileName[] = "scores";
const char lineBreak[] = "/";
bool isResumeFromFile = false;
bool isSaveToFile = false;

void init();
void getSettings();
void saveSettings();
void getBestScore();
void saveBestScore();
void addGrid(int (*)[4]);
void killBlank(int (*)[4]);
bool turnLeft(int (*)[4], bool);
bool turnRight(int (*)[4], bool);
bool turnUp(int (*)[4], bool);
bool turnDown(int (*)[4], bool);
void drawBoard(int (*)[4]);

void init()
{
	//setlocale(LC_ALL, "");
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
	init_pair(8, COLOR_CYAN, COLOR_BLACK);
	
	addGrid(board);
	addGrid(board);
	getSettings();
	getBestScore();
	if(isResumeFromFile)
	    resumeFromFile(board, dataStoreDir, gameDataFileName);
	drawBoard(board);
}

void getSettings()
{
	char *fileFullName;
	strcatFullFilePath(&fileFullName, dataStoreDir, dataConfigFileName);
	
	FILE *file = fopen(fileFullName, "r");
	if(!file)
	{
		FILE *f = fopen(fileFullName, "w");
		fprintf(f, "isResumeFromFile: %d\n", 1);
		fprintf(f, "isSaveToFile: %d\n", 1);
		fclose(f);
	}
	else
	{
		fscanf(file, "isResumeFromFile: %d\n", &isResumeFromFile);
		fscanf(file, "isSaveToFile: %d\n", &isSaveToFile);
		fclose(file);
	}
	free(fileFullName);
}

void saveSettings()
{
	char *fileFullName;
	strcatFullFilePath(&fileFullName, dataStoreDir, dataConfigFileName);
	FILE *file = fopen(fileFullName, "w");
	fprintf(file, "isResumeFromFile: %d\n", isResumeFromFile);
	fprintf(file, "isSaveToFile: %d\n", isSaveToFile);
	fclose(file);
	free(fileFullName);
}

void getBestScore()
{
	char *fileFullName;
	strcatFullFilePath(&fileFullName, dataStoreDir, scoresFileName);
	
	FILE *file = fopen(fileFullName, "r");
	if(!file)
	{
		bestScore = 0;
		FILE *f = fopen(fileFullName, "w");
		fprintf(f, "Best score: %d", bestScore);
		fclose(f);
	}
	else
	{
		fscanf(file, "Best score: %d", &bestScore);
		fclose(file);
	}
	free(fileFullName);
}

void saveBestScore()
{
	if(score > bestScore)
	{
		char *fileFullName;
		strcatFullFilePath(&fileFullName, dataStoreDir, scoresFileName);
	
		bestScore = score;
		FILE *file = fopen(fileFullName, "w");
		fprintf(file, "Best score: %d", bestScore);
		fclose(file);
		free(fileFullName);
	}
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



bool turnLeft(int board[4][4], bool isBoard)
{
	int s = 0;
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
			s += (board[i][j] *= 2);
			board[i][sameIndex] = 0;
		}
	}
	killBlank(board);
	if(isBoard)
		score += s;
	return false;
}

bool turnRight(int board[4][4], bool isBoard)
{
	rollAroundH(board);
	turnLeft(board, isBoard);
	rollAroundH(board);
	return false;
}

bool turnUp(int board[4][4], bool isBoard)
{
	rollRightNinety(board);
	rollRightNinety(board);
	rollRightNinety(board);
	turnLeft(board, isBoard);
	rollRightNinety(board);
	return false;
}

bool turnDown(int board[4][4], bool isBoard)
{
	rollAroundV(board);
	turnUp(board, isBoard);
	rollAroundV(board);
	return false;
}

void drawBoard(int board[4][4])
{
	clear();
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			int start_x = 3 * i + 3;
			int start_y = 7 * j + 2;
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
				mvprintw(start_x + 1, position, BLANK_NUMBER);
			attroff(COLOR_PAIR(color));
			if(value > 64)
				attroff(A_BOLD);
		}
	}
	saveBestScore();
	mvprintw(1, 3, "Scores: %d", score);
	mvprintw(0, 3, "Best scores: %d", bestScore);
	refresh();
}

bool isFinished(int board[4][4])
{
	int temp[4][4];
	copy(temp, board);
	turnLeft(temp, false);
	bool l = equal(temp, board);
	copy(temp, board);
	turnRight(temp, false);
	bool r = equal(temp, board);
	copy(temp, board);
	turnUp(temp, false);
	bool u = equal(temp, board);
	copy(temp, board);
	turnDown(temp, false);
	bool d = equal(temp, board);
	return l && r && u && d;
}

void newGame()
{
    int temp[4][4] = {0};
    copy(board, temp);
	clear();
	addGrid(board);
	score = 0;
	refresh();
}

int main()
{
	init();
	int key;
	char *title = "Notice";
	char *content = "Are you sure you want to start a new game?";
	char *choice = "Yes(y) or No(n)";
	WINDOW *choiceBox;
	while((key = getch()) != 'q')
	{
		int beforeBoard[4][4];
		copy(beforeBoard, board);
		switch(key)
		{
			case '2':
				turnUp(board, true);
				break;
			case '4':
				turnLeft(board, true);
				break;
			case '8':
				turnDown(board, true);
				break;
			case '6':
				turnRight(board, true);
				break;
			case 's':
			    saveToFile(board, dataStoreDir, gameDataFileName);
			    isResumeFromFile = true;
				mvprintw(15, 5, "Saved successfully");
				refresh();
			    break;
			case 'n':
				choiceBox = createChoiceBox(title, content, choice, CHOICE_BOX_COLOR, CHOICE_BOX_COLOR);
				while(true)
				{
					int ch = wgetch(choiceBox);
					if(ch == 'y')
					{
						newGame();
						break;
					}
					if(ch == 'n')
					{
						clear();
						drawBoard(board);
						break;
					}
				}
				delwin(choiceBox);
			    break;
			default:
				break;
		}
		if(isFinished(board))
		{
			saveBestScore();
			mvprintw(15, 5, "Game Over! ");
			mvprintw(16, 3, "\'r\' to restart");
			refresh();
			int ch;
			while((ch = getch()) != 'q')
			{
				if(ch == 'r') 
				{
					newGame();
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
	saveBestScore();
	saveSettings();
	endwin();
	return 0;
}