#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include "board.h"

using namespace std;

static vector<string> mode_2_game_board = {
    "0",     "2",     "4",     "8",    "16",   "32",   "64",
    "128",   "256",   "512",   "1024", "2048", "4096", "8192",
    "16384", "32768", "65536", "131k", "262k", "524k", "1048k",
    "2097k", "4194k", "8388k", "😇"};

static vector<string> mode_3_game_board = {
    "0",    "1",     "3",     "9",    "27",   "81",    "243",   "729", "2187",
    "6561", "19683", "59049", "177k", "531k", "1594k", "4782k", "😇"};

int get_arg(const char *argv, const int default_arg) {
  int arg;
  try {
    arg = atoi(argv);
  } catch (std::exception e) {
    arg = default_arg;
  }
}

int main(int argc, char *argv[]) {
  int row = 4;
  bool reversed = false;
  int mode = 2;
  bool difficult_mode = false;
  bool one_more_counts = false;
  int difficult_mode_number;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      string arg(argv[i]);
      if (arg.find("--help") == 0) {
        cout << "Usage: \n"
             << "\t" << argv[0] << " [--mode={number}]"
             << " [--row={2, 3}]"
             << " [--reversed={true|false}]"
             << " [--difficult-mode={number}]" << endl;
        cout << "Examples: \n\t" << argv[0] << " --row=5"
             << " --mode=3"
             << " --difficult-mode=1" << endl;
        cout << "\t" << argv[0] << " --mode=4"
             << " --reversed=true" << endl;
        return 0;
      }
      if (arg.find("--row=") == 0)
        row = get_arg(arg.substr(6).c_str(), 4);
      else if (arg.find("--reversed=") == 0) {
        auto choice = arg.substr(11);
        if (choice == string("true"))
          reversed = true;
        else if (choice == string("false"))
          reversed = false;
        else
          goto wrong_place;
      } else if (arg.find("--mode=") == 0)
        mode = get_arg(arg.substr(7).c_str(), 2);
      else if (arg.find("--difficult-mode=") == 0) {
        auto choice = arg.substr(17);
        difficult_mode = true;
        difficult_mode_number = get_arg(choice.c_str(), 1);
      } else {
      wrong_place:
        cout << "wrong arg : " << arg << "\nJust run or try \'" << argv[0]
             << " --help\' for more information." << endl;
        return -1;
      }
    }
  } else {
    try {
      cout << "请输入行数(方块阶数)：";
      cin >> row;
      if (row > 5) one_more_counts = true;
      cout << "请输入每次能相消的相同方块块数(2 or 3): ";
      cin >> mode;
      cout << "是否反转模式(控制反转)？(1 -> 是，0 -> 否) :";
      cin >> reversed;
      cout << "选择难度(添加障碍物)？(0, 1, 2, ...): ";
      cin >> difficult_mode_number;
      if (difficult_mode_number != 0) difficult_mode = true;
    } catch (std::exception e) {
    }
  }
  Board board(row, difficult_mode, difficult_mode_number);
  board.set_game_same_count(mode);
  board.set_one_more_counts(one_more_counts);
  if (mode == 3)
    board.set_game_boards(mode_3_game_board);
  else
    board.set_game_boards(mode_2_game_board);
  board.set_reversed(reversed);
  board.loop();
  endwin();
  return 0;
}
/*
Board::s_values = {" ", "H", "Hi", "Li",
                   "Be", "B", "C", "N",
                   "O", "F", "Ne", "Na",
                   "Mg", "Al", "Si", "P"};
*/