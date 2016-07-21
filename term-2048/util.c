#include "util.h"

extern int score;
extern int bestScore;
extern char *lineBreak;

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

int saveToFile(const int board[4][4], const char *dir, const char *filename)
{
	char *fileFullName;
	strcatFullFilePath(&fileFullName, dir, filename);
	FILE *file = fopen(fileFullName, "w");
	if(file == NULL)
		return -1;
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			fprintf(file, "%d ", board[i][j]);
		}
		fprintf(file, "\n");
	}
	fprintf(file, "score: %d", score);
	fclose(file);
	free(fileFullName);
	return 0;
}

int resumeFromFile(int board[4][4], const char *dir, const char *filename)
{
	char *fileFullName;
	strcatFullFilePath(&fileFullName, dir, filename);
	FILE *file = fopen(fileFullName, "r");
	if(file == NULL)
		return -1;
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			fscanf(file, "%d ", &board[i][j]);
		}
		fscanf(file, "\n");
	}
	fscanf(file, "score: %d", &score);
	fclose(file);
	free(fileFullName);
	return 0;
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

char *strcatFullFilePath(char **path, const char *dir, const char *name)
{
	*path = calloc(sizeof(char), strlen(dir)+strlen(name)+1);
	strcat(strcpy(*path, dir), "/");
	strcat(*path, name);
	return *path;
}

char *subString(const char *source, char *substr, int start, int end)
{
    int j = 0;
    for(int i = start; i != end; ++i)
    {
        substr[j] = source[i];
        ++j;
    }
    return substr;
}

char **getSubStrs(const char *source, int length)
{
    int len = strlen(source);
    int size = len /length + 1;
    char **arrStr = calloc(sizeof(char*), size);
    for(int i = 0; i < size; ++i)
    {
        arrStr[i] = calloc(sizeof(char), length);
        subString(source, arrStr[i], i*length, (i+1)*length);
    }
    return arrStr;
    
}

WINDOW *createChoiceBox(const char *title, const char *content, const char *choice, chtype borderColor, chtype fontColor)
{
    int x = getmaxx(stdscr);
    int y = getmaxy(stdscr);
    char **strs;
    
    WINDOW *win = newwin(y/2 - 2, x/2, 3, 3);
    wattron(win, COLOR_PAIR(borderColor));
    box(win, 0,0);
    wattroff(win, COLOR_PAIR(borderColor));
    wattron(win, COLOR_PAIR(fontColor));
    mvwprintw(win, 0, (x/2-strlen(title))/2, "%s", title);
    
    strs = getSubStrs(content, x/2-6);
    for(int i = 0; i < strlen(strs)/4; ++i)
    {
        mvwprintw(win, i+2, 2, "%s", strs[i]);
    }
    
    mvwprintw(win, y/2-4, (x/2-strlen(choice))/2, "%s", choice);
    wattroff(win, COLOR_PAIR(fontColor));
    wrefresh(win);
    return win;
}
