#include "LifeGame.hpp"

namespace CTRPluginFramework {
void LifeGame::LifeGame_Loop(void) {
  Clock genClock;
  Clock keyClock;
  bool isOpened = true;
  while (!Controller::IsKeyPressed(Key::B) && isOpened) {
    if (Controller::IsKeyPressed(Key::A)) {
      if (_field[_selector.x + _selector.y * FIELD_WIDTH])
        _field.reset(_selector.x + _selector.y * FIELD_WIDTH);
      else
        _field.set(_selector.x + _selector.y * FIELD_WIDTH);
    }
    if (Controller::IsKeyDown(Key::Up) && _selector.y != 0) {
      if (Controller::IsKeyPressed(Key::Up)) {
        _selector.y--;
        if (_selector.y == _btmPos.y && _btmPos.y != 0)
          _btmPos.y--;
        keyClock.Restart();
      } else if (keyClock.HasTimePassed(Seconds(0.2))) {
        _selector.y--;
        if (_selector.y == _btmPos.y && _btmPos.y != 0)
          _btmPos.y--;
      }
    }
    if (Controller::IsKeyDown(Key::Left) && _selector.x != 0) {
      if (Controller::IsKeyPressed(Key::Left)) {
        _selector.x--;
        if (_selector.x == _btmPos.x && _btmPos.x != 0)
          _btmPos.x--;
        keyClock.Restart();
      } else if (keyClock.HasTimePassed(Seconds(0.2))) {
        _selector.x--;
        if (_selector.x == _btmPos.x && _btmPos.x != 0)
          _btmPos.x--;
      }
    }
    if (Controller::IsKeyDown(Key::Right) && _selector.x != FIELD_WIDTH - 1) {
      if (Controller::IsKeyPressed(Key::Right)) {
        _selector.x++;
        if (_selector.x == _btmPos.x + 320 / BLOCK_WIDTH - 1 && _btmPos.x != FIELD_WIDTH - 320 / BLOCK_WIDTH)
          _btmPos.x++;
        keyClock.Restart();
      } else if (keyClock.HasTimePassed(Seconds(0.2))) {
        _selector.x++;
        if (_selector.x == _btmPos.x + 320 / BLOCK_WIDTH - 1 && _btmPos.x != FIELD_WIDTH - 320 / BLOCK_WIDTH)
          _btmPos.x++;
      }
    }
    if (Controller::IsKeyDown(Key::Down) && _selector.y != FIELD_HEIGHT - 1) {
      if (Controller::IsKeyPressed(Key::Down)) {
        _selector.y++;
        if (_selector.y == _btmPos.y + 240 / BLOCK_WIDTH - 1 && _btmPos.y != FIELD_HEIGHT - 240 / BLOCK_WIDTH)
          _btmPos.y++;
        keyClock.Restart();
      } else if (keyClock.HasTimePassed(Seconds(0.2))) {
        _selector.y++;
        if (_selector.y == _btmPos.y + 240 / BLOCK_WIDTH - 1 && _btmPos.y != FIELD_HEIGHT - 240 / BLOCK_WIDTH)
          _btmPos.y++;
      }
    }
    u32 lastTouch = FIELD_WIDTH * BLOCK_WIDTH + 1;
    while (Touch::IsDown()) {
      for (size_t i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++)
        if (_btmPos.x < i % FIELD_WIDTH + 1 && _btmPos.y < i / FIELD_WIDTH + 1 && i % FIELD_WIDTH < _btmPos.x + 320 / BLOCK_WIDTH && i / FIELD_WIDTH < _btmPos.y + 240 / BLOCK_WIDTH)
          if (TouchRect((i % FIELD_WIDTH - _btmPos.x) * BLOCK_WIDTH, (i / FIELD_WIDTH - _btmPos.y) * BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH) && i != lastTouch) {
            _selector.x = i % FIELD_WIDTH;
            _selector.y = i / FIELD_WIDTH;
            if (_field[i])
              _field.reset(i);
            else
              _field.set(i);
            if (_selector.y == _btmPos.y && _btmPos.y != 0)
              _btmPos.y--;
            if (_selector.x == _btmPos.x && _btmPos.x != 0)
              _btmPos.x--;
            if (_selector.x == _btmPos.x + 320 / BLOCK_WIDTH - 1 && _btmPos.x != FIELD_WIDTH - 320 / BLOCK_WIDTH)
              _btmPos.x++;
            if (_selector.y == _btmPos.y + 240 / BLOCK_WIDTH - 1 && _btmPos.y != FIELD_HEIGHT - 240 / BLOCK_WIDTH)
              _btmPos.y++;
            DrawField();
            lastTouch = i;
            break;
          }
      Controller::Update();
    }
    if (Controller::IsKeyPressed(Key::X))
      _isLoopingGen = !_isLoopingGen;
    if (Controller::IsKeyPressed(Key::Y)) {
      switch (Keyboard("Paused", {"continue", "clear", "random", "settings", "quit"}).Open()) {
        case 1:
          _field.reset();
          break;
        case 2: {
          Keyboard key("how many?");
          key.IsHexadecimal(false);
          u16 ans;
          if (0 <= key.Open(ans))
            for (u16 i = 0; i < ans; i++)
              _field.set(Utils::Random(0, FIELD_WIDTH * FIELD_HEIGHT - 1));
          break;
        }
        case 3: {
          u16 ans;
          Keyboard key("speed\ndefault is 200");
          key.IsHexadecimal(false);
          if (0 <= key.Open(ans))
            _speed = ans;
          switch (Keyboard("torus?\nUp, down, left, and right will connect.", {"true", "false"}).Open()) {
            case 0:
              _isTorus = true;
              break;
            case 1:
              _isTorus = false;
              break;
            default:
              break;
          }
          break;
        }
        case 4:
          isOpened = false;
          break;
        default:
          break;
      }
    }

    if (_isLoopingGen && genClock.HasTimePassed(Milliseconds(_speed))) {
      NextGen();
      DrawField();
      genClock.Restart();
    }
    if (Controller::GetKeysDown())
      DrawField();
    Controller::Update();
  }
}

void LifeGame::DrawField(void) {
  for (size_t j = 0; j < 2; j++) {
    topScr.DrawRect(0, 0, 400, 240, Color::Gray);
    btmScr.DrawRect(0, 0, 320, 240, Color::Gray);
    for (size_t i = 0; i < FIELD_WIDTH * FIELD_HEIGHT; i++) {
      topScr.DrawRect((i % FIELD_WIDTH) * (400 / FIELD_WIDTH), (i / FIELD_WIDTH) * (240 / FIELD_HEIGHT), (400 / FIELD_WIDTH), (240 / FIELD_HEIGHT), _selector.x == i % FIELD_WIDTH && _selector.y == i / FIELD_WIDTH && !_isLoopingGen ? Color::DimGrey : (_field[i] ? Color::SkyBlue : Color::Gray));
      if (_btmPos.x < i % FIELD_WIDTH + 1 && _btmPos.y < i / FIELD_WIDTH + 1 && i % FIELD_WIDTH < _btmPos.x + 320 / BLOCK_WIDTH && i / FIELD_WIDTH < _btmPos.y + 240 / BLOCK_WIDTH) {
        btmScr.DrawRect((i % FIELD_WIDTH - _btmPos.x) * BLOCK_WIDTH, (i / FIELD_WIDTH - _btmPos.y) * BLOCK_WIDTH, BLOCK_WIDTH, BLOCK_WIDTH, _selector.x == i % FIELD_WIDTH && _selector.y == i / FIELD_WIDTH ? Color::DimGrey : Color::White, false);
        btmScr.DrawRect((i % FIELD_WIDTH - _btmPos.x) * BLOCK_WIDTH + 1, (i / FIELD_WIDTH - _btmPos.y) * BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _field[i] ? Color::SkyBlue : Color::Gray);
      }
    }
    if (!_isLoopingGen)
      topScr.DrawRect(_btmPos.x * (400 / FIELD_WIDTH), _btmPos.y * (240 / FIELD_HEIGHT), 320 / BLOCK_WIDTH * (400 / FIELD_WIDTH), 240 / BLOCK_WIDTH * (240 / FIELD_HEIGHT), Color::White, false);
    OSD::SwapBuffers();
  }
}

void LifeGame::NextGen(void) {
  std::bitset<FIELD_HEIGHT * FIELD_WIDTH> newGen;
  for (size_t i = 0; i < _field.size(); i++) {
    s8 count = LivesAround(i % FIELD_WIDTH, i / FIELD_WIDTH);
    if (_field[i]) {
      if (count == 2 || count == 3)
        newGen.set(i);
    } else if (count == 3)
      newGen.set(i);
  }
  _field = newGen;
}

s8 LifeGame::LivesAround(s16 x, s16 y) {
  s8 count = 0;
  for (s16 a = x - 1; a < x + 2; a++)
    for (s16 b = y - 1; b < y + 2; b++) {
      if (a == x && b == y)
        continue;
      s16 xx = a;
      s16 yy = b;
      if (_isTorus) {
        xx = (xx + FIELD_WIDTH) % FIELD_WIDTH;
        yy = (yy + FIELD_HEIGHT) % FIELD_HEIGHT;
      }
      if (-1 < xx && xx < FIELD_WIDTH && -1 < yy && yy < FIELD_HEIGHT && _field[xx + yy * FIELD_WIDTH])
        count++;
    }
  return count;
}
}  // namespace CTRPluginFramework
