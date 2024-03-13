#include "Paint.hpp"

namespace CTRPluginFramework {
Paint::Paint() {
  _ctx = std::vector<std::vector<Color>>(200, std::vector<Color>(200, Color(0, 0, 0, 0)));

  _pen.color = Color(0, 0, 0, 0);
  _pen.size = 1;
}

void Paint::_drawLine(int srcX, int srcY, int dstX, int dstY) {
  float x, y, dx, dy, step;
  int i;

  dx = dstX - srcX;
  dy = dstY - srcY;

  step = abs(dx) >= abs(dy) ? abs(dx) : abs(dy);

  dx /= step;
  dy /= step;
  x = srcX;
  y = srcY;
  i = 1;

  std::vector<UIntVector> poses;
  while (i <= step) {
    if (x < 0 || y < 0 || std::ssize(_ctx) <= x || std::size(_ctx[0]) <= y)
      break;

    u32 originX = x - (_pen.size / 2);
    u32 originY = y - (_pen.size / 2);
    for (u32 drawX = 0; drawX < _pen.size; drawX++) {
      for (u32 drawY = 0; drawY < _pen.size; drawY++) {
        u32 posX = originX + drawX;
        u32 posY = originY + drawY;

        if (posX < 200 && posY < 200) {
          _ctx[posX][posY] = _pen.color;
          poses.push_back({static_cast<u32>(posX), static_cast<u32>(posY)});
        }
      }
    }

    x += dx;
    y += dy;
    i++;
  }

  for (u8 i = 0; i < 2; i++) {
    for (auto &&pos : poses) {
      if (_pen.color.a)  // pen
        _screen->DrawPixel(pos.x + 20, pos.y + 10, _pen.color);
      else  // eraser
        _screen->DrawPixel(pos.x + 20, pos.y + 10, (int(pos.x) / 10 + int(pos.y) / 10) % 2 ? Color::White : Color::DarkGrey);
    }
    if (i == 0)
      OSD::SwapBuffers();
  }
}

bool Paint::_isValid(int x, int y, Color prevC, Color newC) {
  return x >= 0 && x < std::ssize(_ctx) && y >= 0 && y < std::ssize(_ctx[0]) && _ctx[x][y] == prevC && _ctx[x][y] != newC;
}

void Paint::_floodFill(int x, int y, Color prevC, Color newC) {
  std::vector<std::pair<int, int>> queue = {{x, y}};

  std::pair<int, int> p(x, y);
  queue.emplace_back(p);

  _ctx[x][y] = newC;

  while (queue.size() > 0) {
    std::pair<int, int> currPixel = queue[queue.size() - 1];
    queue.pop_back();

    int posX = currPixel.first;
    int posY = currPixel.second;

    if (_isValid(posX + 1, posY, prevC, newC)) {
      _ctx[posX + 1][posY] = newC;
      p.first = posX + 1;
      p.second = posY;
      queue.emplace_back(p);
    }
    if (_isValid(posX - 1, posY, prevC, newC)) {
      _ctx[posX - 1][posY] = newC;
      p.first = posX - 1;
      p.second = posY;
      queue.emplace_back(p);
    }
    if (_isValid(posX, posY + 1, prevC, newC)) {
      _ctx[posX][posY + 1] = newC;
      p.first = posX;
      p.second = posY + 1;
      queue.emplace_back(p);
    }
    if (_isValid(posX, posY - 1, prevC, newC)) {
      _ctx[posX][posY - 1] = newC;
      p.first = posX;
      p.second = posY - 1;
      queue.emplace_back(p);
    }
  }
}

int Paint::Open(void) {
  _screen = &OSD::GetBottomScreen();
  Color paintColor = Color::Black;
  UIntVector lastPos;
  std::string paintModeName[] = {"PEN", "ERASER", "BUCKET"};
  u8 paintMode = PEN;
  Clock dropperClock;
START:
  for (size_t i = 0; i < 2; i++) {
    _screen->DrawRect(0, 0, 320, 240, Color::Gray);
    _screen->DrawRect(19, 9, 202, 202, Color::Black, false);
    for (size_t x = 0; x < 200; x++) {
      for (size_t y = 0; y < 200; y++) {
        Color color = _ctx[x][y];
        if (color.a)
          _screen->DrawPixel(x + 20, y + 10, color);
        else
          _screen->DrawPixel(x + 20, y + 10, (x / 10 + y / 10) % 2 ? Color::White : Color::DarkGrey);
      }
    }
    _screen->DrawRect(200, 215, 50, 22, Color::Gray);
    _screen->DrawRect(200, 215, 50, 22, Color::White, false);
    _screen->DrawSysfont("cancel", 202, 218);
    _screen->DrawRect(260, 215, 50, 22, Color::Gray);
    _screen->DrawRect(260, 215, 50, 22, Color::White, false);
    _screen->DrawSysfont("OK", 272, 218);
    _screen->DrawSysfont("モード", 230, 10);
    _screen->DrawRect(230, 50, 12, 12, Color::White, false);
    _screen->DrawSysfont(paintModeName[paintMode], 230, 25);
    _screen->DrawRect(231, 51, 10, 10, paintColor);
    OSD::SwapBuffers();
  }

  while (true) {
    Controller::Update();
    lastPos = Touch::GetPosition();
    UIntVector dropperPos = lastPos;
    dropperClock.Restart();
    while (TouchRect(20, 10, 199, 199)) {
      if (paintMode == Mode::BUCKET) {
        UIntVector pos = Touch::GetPosition();
        _floodFill(pos.x - 20, pos.y - 10, _ctx[pos.x - 20][pos.y - 10], paintColor);
        goto START;
      }
      while (TouchRect(dropperPos.x - 2, dropperPos.y - 2, 5, 5)) {
        if (dropperClock.HasTimePassed(Seconds(1))) {
          if (_ctx[dropperPos.x - 20][dropperPos.y - 10].a)
            paintColor = _ctx[dropperPos.x - 20][dropperPos.y - 10];
          _screen->DrawRect(231, 51, 10, 10, paintColor);
          OSD::SwapBuffers();
          _screen->DrawRect(231, 51, 10, 10, paintColor);
          goto START;
        }
        Controller::Update();
      }
      if ((paintMode == PEN || paintMode == ERASER) && TouchRect(20, 10, 199, 199)) {
        UIntVector pos = Touch::GetPosition();
        _pen.color = (paintMode ? Color(0, 0, 0, 0) : paintColor);
        _drawLine(pos.x - 20, pos.y - 10, lastPos.x - 20, lastPos.y - 10);
        lastPos = Touch::GetPosition();
      }
      OSD::SwapBuffers();
      Controller::Update();
    }

    if (Controller::IsKeyPressed(Key::B) || TouchRect(200, 215, 50, 22)) {
      return -1;
    }

    if (TouchRect(260, 215, 50, 22)) {
      for (size_t x = 0; x < 200; x++) {
        for (size_t y = 0; y < 200; y++) {
          Color color = _ctx[x][y];
          setScreenBuffer(x + 100, y + 20, color);
        }
      }
      break;
    }

    if (Controller::IsKeyPressed(Key::X) || TouchRect(230, 50, 12, 12)) {
      colorPicker(paintColor);
      goto START;
    }

    if (Controller::IsKeyPressed(Key::Y)) {
      if (paintMode == Mode::BUCKET)
        paintMode = Mode::PEN;
      else
        paintMode++;
    }
    if (Controller::IsKeyPressed(Key::R)) {
      if (_pen.size < 30)
        _pen.size++;
    }
    if (Controller::IsKeyPressed(Key::L)) {
      if (_pen.size > 1)
        _pen.size--;
    }
    _screen->DrawRect(230, 25, 90, 20, Color::Gray);
    _screen->DrawSysfont(paintModeName[paintMode], 230, 25);

    _screen->DrawRect(231, 51, 10, 10, paintColor);

    _screen->DrawRect(230, 65, 90, 20, Color::Gray);
    _screen->DrawSysfont(Utils::Format("Pen: %upx", _pen.size), 230, 65);
    OSD::SwapBuffers();
  }

  return 0;
}
}  // namespace CTRPluginFramework
