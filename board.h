#ifndef TERM2048_BOARD_H
#define TERM2048_BOARD_H

#include <ncurses.h>
#include <unistd.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>

#define BLACK 7
#define RED 1
#define YELLOW 2
#define GREEN 3
#define CYAN 4
#define BLUE 5
#define MAGENTA 6

static int colors[7] = {BLACK, RED, YELLOW, GREEN, CYAN, BLUE, MAGENTA};
static std::string happys[] = {"(-_-)", "(T_T)", "(^_^)", "(^o^)", "(^.^)"};

class Board {
 public:
  typedef std::vector<std::string>::size_type size_type;

 public:
  explicit Board(size_type size = 4, bool dm = false, int dm_number = 0)
      : size_(size),
        scores(0),
        same_count(2),
        difficult_mode(dm),
        difficult_number(dm_number) {
    this->init();
    this->add();
    this->add();
  }

  explicit Board(const std::string &filename) {
    this->init();
    this->resume(filename);
  }

  ~Board() {}

 public:
  void move_right();
  void move_left();
  void move_up();
  void move_down();
  void add();
  void draw();
  void loop();
  void restart();
  void set_one_more_counts(bool c) { this->one_more_counts = c; }
  void set_reversed(bool r) { this->reversed = r; }
  void set_game_same_count(int count) { this->same_count = count; }
  void set_difficult_mode(bool d) { this->difficult_mode = d; }
  void set_keys(int u, int d, int l, int r) {
    this->up = u;
    this->down = d;
    this->left = l;
    this->right = r;
  }
  void set_game_boards(const std::vector<std::string> &b) {
    this->s_values.assign(b.begin(), b.end());
    for (size_type i = 0; i < this->s_values.size(); ++i) {
      this->map_values_[i] = this->s_values[i];
    }
  }

 protected:
  std::map<int, std::string> map_values_;
  std::vector<int> indexs_;
  std::vector<std::vector<int>> boards_;
  size_type size_;
  int same_count;
  int scores;

 private:
  void init() {
    initscr();
    noecho();
    cbreak();
    start_color();
    curs_set(0);
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_YELLOW);
    init_pair(3, COLOR_WHITE, COLOR_GREEN);
    init_pair(4, COLOR_WHITE, COLOR_CYAN);
    init_pair(5, COLOR_WHITE, COLOR_BLUE);
    init_pair(6, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(7, COLOR_WHITE, COLOR_BLACK);
    init_pair(8, COLOR_CYAN, COLOR_BLACK);
    this->s_values = {"0",    "1",    "3",     "9",     "27",    "81",
                      "243",  "729",  "2187",  "6561",  "19683", "59049",
                      "177k", "531k", "1594k", "4782k", "Inf"};

    std::fill_n(std::back_inserter(indexs_), this->size_ * this->size_, 0);
    for (size_type i = 0; i < this->indexs_.size(); ++i) {
      this->map_values_.insert(std::make_pair(
          i, i < this->s_values.size() ? this->s_values[i]
                                       : *(this->s_values.rbegin())));
    }
    std::vector<std::vector<int>> board(this->size_,
                                        std::vector<int>(this->size_));
    this->boards_.assign(board.begin(), board.end());
    this->score_resulted = true;
    this->reversed = false;
    this->same_count = 2;
  }

  void clear() {
    std::string blank(getmaxx(stdscr), ' ');
    mvprintw(0, 0, blank.c_str());
    refresh();
  }

 public:
  void clear_blank();
  void rotate_right();
  void rotate_left();
  void rotate_horizon();
  void rotate_vertical();
  bool finished();

  void save(const std::string &, bool showed = true);
  void resume(const std::string &);

 private:
  bool score_resulted;
  bool reversed;
  bool one_more_counts;
  bool difficult_mode;
  int difficult_number;
  int difficult_mode_row, difficult_mode_col;
  std::vector<std::string> s_values;
  int up, down, right, left;
};

void Board::add() {
  static bool initialized = false;
  int grids[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2};

  if (!initialized) {
    srand(time(0));
    initialized = true;
    if (this->difficult_mode) {
      for (int i = 0; i < this->difficult_number; ++i) {
        while (true) {
          int row = rand() % this->size_;
          int col = rand() % this->size_;
          if (this->boards_[row][col] == 0) {
            this->boards_[row][col] = -1;
            break;
          }
        }
      }
    }
  }

  int choice = rand() % 11;
  while (true) {
    int row = rand() % this->size_;
    int col = rand() % this->size_;
    if (this->boards_[row][col] == 0) {
      this->boards_[row][col] = grids[choice];
      break;
    }
  }
}

void Board::draw() {
  this->clear();
  for (size_type i = 0; i < this->size_; ++i) {
    for (size_type j = 0; j < this->size_; ++j) {
      int start_x = 3 * i + 2;
      int start_y = 7 * j + 1;
      int value = this->boards_[i][j];
      int color;
      int position = start_y + (7 - this->map_values_[value].size()) / 2;
      if (value == -1) {
        color = rand() % 7;
        position = start_y + (7 - happys[rand() % 5].size()) / 2;
        attron(COLOR_PAIR(color) | A_REVERSE | A_BOLD);
        mvprintw(start_x, start_y, "       ");
        mvprintw(start_x + 1, start_y, "       ");
        mvprintw(start_x + 2, start_y, "       ");
        mvprintw(start_x + 1, position, "%s", happys[rand() % 5].c_str());
        attroff(COLOR_PAIR(color) | A_REVERSE | A_BOLD);
        continue;
      }
      switch (value) {
        case 7:
          color = colors[1];
          break;
        case 8:
          color = colors[2];
          break;
        case 9:
          color = colors[3];
          break;
        case 10:
          color = colors[4];
          break;
        case 11:
          color = colors[5];
          break;
        case 12:
          color = colors[6];
          break;
        case 13:
          color = colors[1];
          break;
        case 14:
          color = colors[2];
          break;
        case 15:
          color = colors[3];
          break;
        default:
          color = colors[value];
          break;
      }
      attron(COLOR_PAIR(color));
      if (value > 6 && value < 13) attron(A_BOLD);
      mvprintw(start_x, start_y, "       ");
      mvprintw(start_x + 1, start_y, "       ");
      mvprintw(start_x + 2, start_y, "       ");
      mvprintw(start_x + 1, position, "%s", this->map_values_[value].c_str());
      if (value == 0) mvprintw(start_x + 1, position, "-");
      attroff(COLOR_PAIR(color));
      if (value > 6 && value < 13) attroff(A_BOLD);
    }
  }
  mvprintw(1, 3, "Scores: %8d", this->scores);
  refresh();
}

void Board::loop() {
  this->draw();
  int key;
  static bool quit_but_result = false;
  while ((key = getch()) != 'q') {
    auto before_boards = this->boards_;
    switch (key) {
      case '2':
        if (!this->reversed)
          this->move_up();
        else
          this->move_down();
        break;
      case '4':
        if (!this->reversed)
          this->move_left();
        else
          this->move_right();
        break;
      case '6':
        if (!this->reversed)
          this->move_right();
        else
          this->move_left();
        break;
      case '8':
        if (!this->reversed)
          this->move_down();
        else
          this->move_up();
        break;
      case 'n':
      game_restart:
        this->restart();
        quit_but_result = false;
        break;
      case 's':
        this->save("/data/data/com.lonelyweed.term2048/files/Data");
        quit_but_result = true;
        break;
      case 'r':
        this->resume("/data/data/com.lonelyweed.term2048/files/Data");
        quit_but_result = true;
        goto game_resume;
        break;
      default:
        break;
    }
    if (this->boards_ != before_boards) {
      this->draw();
      usleep(150000);
      this->add();
      this->draw();
      if (quit_but_result)
        this->save("/data/data/com.lonelyweed.term2048/files/Data", false);
    }
  game_resume:
    if (this->finished()) {
      mvprintw(0, 0, "Game Over! \'n\' to restart | \'q\' to leave");
      refresh();
      if (quit_but_result)
        this->save("/data/data/com.lonelyweed.term2048/files/Data", false);
      int ch;
      while ((ch = getch()) != 'q') {
        if (ch == 'n') {
          goto game_restart;
        }
      }
      break;
    }
  }
}

void Board::restart() {
  for (auto &rows : this->boards_) {
    for (auto &col : rows) {
      col = 0;
    }
  }

  if (this->difficult_mode) {
    for (int i = 0; i < this->difficult_number; ++i) {
      while (true) {
        int row = rand() % this->size_;
        int col = rand() % this->size_;
        if (this->boards_[row][col] == 0) {
          this->boards_[row][col] = -1;
          break;
        }
      }
    }
  }
  this->scores = 0;
  this->draw();
  this->add();
}

bool Board::finished() {
  this->score_resulted = false;
  auto filled = [&]() -> bool {
    for (auto &rows : this->boards_)
      for (auto &col : rows)
        if (col == 0) return false;
    return true;
  };
  auto backup = this->boards_;
  auto boards_left_right = this->boards_;
  auto boards_up_down = this->boards_;

  this->move_left();
  bool left_right_moved = !(boards_left_right == this->boards_);
  this->boards_ = backup;
  this->move_up();
  bool up_down_moved = !(boards_up_down == this->boards_);
  this->boards_ = backup;

  this->score_resulted = true;
  return filled() && !(left_right_moved || up_down_moved);
}

void Board::clear_blank() {
  for (size_type i = 0; i < this->size_; ++i)
    for (int count = 0; count < this->size_ - 2; ++count)
      for (size_type j = 0; j < this->size_ - 1; ++j) {
        if (this->boards_[i][j] == this->indexs_[0])
          std::swap(this->boards_[i][j], this->boards_[i][j + 1]);
      }
}

void Board::rotate_left() {
  auto temp = this->boards_;
  for (size_type i = 0; i < this->size_; ++i)
    for (size_type j = 0; j < this->size_; ++j)
      this->boards_[i][j] = temp[j][this->size_ - 1 - i];
}

void Board::rotate_right() {
  auto temp = this->boards_;
  for (size_type i = 0; i < this->size_; ++i)
    for (size_type j = 0; j < this->size_; ++j)
      this->boards_[i][j] = temp[this->size_ - 1 - j][i];
}

void Board::rotate_horizon() {
  auto temp = this->boards_;
  for (size_type i = 0; i < this->size_; ++i)
    for (size_type j = 0; j < this->size_; ++j)
      this->boards_[i][j] = temp[i][this->size_ - 1 - j];
}

void Board::rotate_vertical() {
  auto temp = this->boards_;
  for (size_type i = 0; i < this->size_; ++i)
    for (size_type j = 0; j < this->size_; ++j)
      this->boards_[i][j] = temp[this->size_ - 1 - i][j];
}

void Board::move_left() {
  int counts = 1;
  if (this->one_more_counts)
    counts = 2;
  for (int count = 1; count <= counts; ++count) {
    this->clear_blank();
    for (size_type row = 0; row < this->size_; ++row) {
      for (size_type col = 0; col < this->size_ - (this->same_count - 1);
           ++col) {
        bool still_same = true;
        int start = this->boards_[row][col];
        if (start == 0) break;
        auto current_col = col;
        int same_index = -1;
        while (current_col < this->size_ - (this->same_count - 1)) {
          for (int i = 1; i <= this->same_count - 1; ++i) {
            if (start != this->boards_[row][current_col + i]) {
              still_same = false;
              break;
            }
          }
          if (!still_same) break;
          ++current_col;
          same_index = current_col;
        }
        if (same_index == -1 || start == -1) continue;
        if (this->score_resulted) {
          this->scores += pow(this->same_count, ++this->boards_[row][col]);
        } else {
          ++this->boards_[row][col];
        }
        for (int i = 0; i < this->same_count - 1; ++i)
          this->boards_[row][same_index + i] = 0;
      }
    }
    this->clear_blank();
  }
}

void Board::move_right() {
  this->rotate_horizon();
  this->move_left();
  this->rotate_horizon();
}

void Board::move_up() {
  this->rotate_left();
  this->move_left();
  this->rotate_right();
}

void Board::move_down() {
  this->rotate_vertical();
  this->move_up();
  this->rotate_vertical();
}

void Board::save(const std::string &filename, bool showed) {
  std::fstream os(filename, std::fstream::out);
  os << std::string(20, '=') << "\n";
  os << "Best " << this->same_count << " "
     << "Scores " << this->scores << " "
     << "Size " << this->size_ << "\n";
  os << "Boards ";
  for (auto &rows : this->boards_) {
    for (auto &col : rows) {
      os << col << " ";
    }
  }
  os << "\n";
  os << "Values ";
  for (auto &value : this->map_values_) {
    os << value.second << " ";
  }
  os << "\n";
  os.close();
  if (showed) mvprintw(0, 0, "Saved Successfully");
  refresh();
}

void Board::resume(const std::string &filename) {
  auto size = this->size_;
  auto count = this->same_count;
  std::fstream is(filename, std::fstream::in);
  std::string temp;
  int score = this->scores;
  std::getline(is, temp);
  is >> temp >> this->same_count >> temp >> this->scores >> temp >> this->size_;
  if (size != this->size_) {
    mvprintw(0, 0, "current row or col is %d, but the saved is %d", size,
             this->size_);
    refresh();
    this->scores = score;
    this->size_ = size;
    return;
  } else if (count != this->same_count) {
    mvprintw(0, 0, "current mode is %d, but the saved is %d", count,
             this->same_count);
    refresh();
    this->scores = score;
    this->same_count = count;
    return;
  }
  is >> temp;
  for (auto &rows : this->boards_) {
    for (auto &col : rows) {
      is >> col;
    }
  }
  is >> temp;
  this->s_values.clear();
  for (auto &value : this->s_values) is >> value;
  is.close();
  this->draw();
}

#endif