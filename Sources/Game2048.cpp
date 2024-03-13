#include "Game2048.hpp"

namespace CTRPluginFramework {
void Game2048::Game2048_Loop(void) {
  _isOpened = true;
  _score = 0;
  _field[Utils::Random(0, FIELD_WIDTH * FIELD_WIDTH - 1)] = 1;
  DrawField();
  OSD::SwapBuffers();
  DrawField();
  while (!Controller::IsKeyPressed(Key::B) && _isOpened) {
    Controller::Update();
    if (Controller::IsKeyPressed(Key::Up))
      MoveField(0);
    if (Controller::IsKeyPressed(Key::Right))
      MoveField(1);
    if (Controller::IsKeyPressed(Key::Down))
      MoveField(2);
    if (Controller::IsKeyPressed(Key::Left))
      MoveField(3);
    if (Controller::IsKeyPressed(Key::X))
      ResetGame();
  }
}

void Game2048::DrawField(void) {
  u8 gap = (240 - FIELD_WIDTH * BLOCK_WIDTH) / (FIELD_WIDTH + 1);
  u16 start = (400 - FIELD_WIDTH * BLOCK_WIDTH - gap * FIELD_WIDTH - 1) / 2;

  topScr.DrawRect(0, 0, 400, 240, Color::DarkGrey);
  topScr.DrawSysfont("Score:", 0, 0);
  topScr.DrawSysfont(Utils::Format("%d", _score), 0, 20);
  topScr.DrawSysfont("B:exit", 0, 200);
  topScr.DrawSysfont("X:reset", 0, 220);

  for (size_t i = 0; i < FIELD_WIDTH * FIELD_WIDTH; i++) {
    topScr.DrawRect(start + i % FIELD_WIDTH * (gap + BLOCK_WIDTH), i / FIELD_WIDTH * (gap + BLOCK_WIDTH) + gap, BLOCK_WIDTH, BLOCK_WIDTH, _field[i] == 0 ? Color::Gray : Color(200 - _field[i] * 10, 200, 255));
    std::string num = std::to_string(int(std::pow(2, _field[i])));
    if (_field[i] != 0)
      topScr.DrawSysfont(num, start + i % FIELD_WIDTH * (gap + BLOCK_WIDTH) + (BLOCK_WIDTH - OSD::GetTextWidth(true, num)) / 2, i / FIELD_WIDTH * (gap + BLOCK_WIDTH) + gap + 17);
  }
}

void Game2048::MoveField(u8 direction) {
  bool moved = false;
  switch (direction) {
    case 0:
      for (size_t i = FIELD_WIDTH; i < FIELD_WIDTH * FIELD_WIDTH; i++) {
        if (_field[i] == 0)
          continue;
        for (ssize_t j = i; FIELD_WIDTH <= j;) {
          if (_field[j - FIELD_WIDTH] == 0) {
            std::swap(_field[j], _field[j - FIELD_WIDTH]);
            moved = true;
            j -= FIELD_WIDTH;
          } else {
            if (_field[j] == _field[j - FIELD_WIDTH]) {
              _field[j - FIELD_WIDTH]++;
              _field[j] = 0;
              _score += std::pow(2, _field[j - FIELD_WIDTH]);
              moved = true;
            }
            break;
          }
        }
      }
      break;
    case 1:
      for (ssize_t i = FIELD_WIDTH - 2; 0 <= i; i--) {
        for (size_t j = 0; j < FIELD_WIDTH; j++) {
          if (_field[i + j * FIELD_WIDTH] == 0)
            continue;
          for (size_t k = i + j * FIELD_WIDTH; k % FIELD_WIDTH < 3;) {
            if (_field[k + 1] == 0) {
              std::swap(_field[k], _field[k + 1]);
              moved = true;
              k++;
            } else {
              if (_field[k] == _field[k + 1]) {
                _field[k + 1]++;
                _field[k] = 0;
                _score += std::pow(2, _field[k + 1]);
                moved = true;
              }
              break;
            }
          }
        }
      }
      break;
    case 2:
      for (ssize_t i = FIELD_WIDTH * FIELD_WIDTH - FIELD_WIDTH - 1; 0 <= i; i--) {
        if (_field[i] == 0)
          continue;
        for (size_t j = i; j < FIELD_WIDTH * FIELD_WIDTH - FIELD_WIDTH;) {
          if (_field[j + FIELD_WIDTH] == 0) {
            std::swap(_field[j], _field[j + FIELD_WIDTH]);
            moved = true;
            j += FIELD_WIDTH;
          } else {
            if (_field[j] == _field[j + FIELD_WIDTH]) {
              _field[j + FIELD_WIDTH]++;
              _field[j] = 0;
              _score += std::pow(2, _field[j + FIELD_WIDTH]);
              moved = true;
            }
            break;
          }
        }
      }
      break;
    case 3:
      for (size_t i = 1; i < FIELD_WIDTH; i++) {
        for (size_t j = 0; j < FIELD_WIDTH; j++) {
          if (_field[i + j * FIELD_WIDTH] == 0)
            continue;
          for (ssize_t k = i + j * FIELD_WIDTH; 0 < k % FIELD_WIDTH;) {
            if (_field[k - 1] == 0) {
              std::swap(_field[k], _field[k - 1]);
              moved = true;
              k--;
            } else {
              if (_field[k] == _field[k - 1]) {
                _field[k - 1]++;
                _field[k] = 0;
                _score += std::pow(2, _field[k - 1]);
                moved = true;
              }
              break;
            }
          }
        }
      }
      break;

    default:
      break;
  }

  if (moved)
    while (1) {
      u8 next = Utils::Random(0, FIELD_WIDTH * FIELD_WIDTH);
      if (_field[next] == 0) {
        _field[next] = Utils::Random(0, 9) == 9 ? 2 : 1;
        break;
      }
    }

  DrawField();
  OSD::SwapBuffers();
  DrawField();

  if (CheckFinish()) {
    MessageBox(Utils::Format("You Lost!\nYour Score is %d", _score))();
    ResetGame();
    _isOpened = false;
    return;
  }

  bool max = false;
  for (size_t i = 0; i < FIELD_WIDTH * FIELD_WIDTH; i++) {
    if (_field[i] == 16) {
      max = true;
      break;
    }
  }
  if (max) {
    MessageBox(Utils::Format("congratulations!\nYour Score is %d", _score))();
    ResetGame();
    _isOpened = false;
  }
}

bool Game2048::CheckFinish(void) {
  for (size_t i = 0; i < FIELD_WIDTH * FIELD_WIDTH; i++) {
    if (FIELD_WIDTH <= i && (_field[i - FIELD_WIDTH] == 0 || _field[i - FIELD_WIDTH] == _field[i]))
      return false;
    if (i % FIELD_WIDTH < FIELD_WIDTH - 1 && (_field[i + 1] == 0 || _field[i + 1] == _field[i]))
      return false;
    if (i < FIELD_WIDTH * FIELD_WIDTH - FIELD_WIDTH && (_field[i + FIELD_WIDTH] == 0 || _field[i + FIELD_WIDTH] == _field[i]))
      return false;
    if (0 < i % FIELD_WIDTH && (_field[i - 1] == 0 || _field[i - 1] == _field[i]))
      return false;
  }
  return true;
}

void Game2048::ResetGame(void) {
  _field = {std::vector<u8>(FIELD_WIDTH * FIELD_WIDTH, 0)};
  _field[Utils::Random(0, FIELD_WIDTH * FIELD_WIDTH - 1)] = 1;
  _score = 0;
  DrawField();
  OSD::SwapBuffers();
  DrawField();
}
}  // namespace CTRPluginFramework
