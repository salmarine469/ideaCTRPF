#include "threeDGame.hpp"


namespace CTRPluginFramework {
threeDGame::threeDGame() {
  _field[0] = 6;
  _field[1] = 5;
  _field[2] = 4;
  _field[3] = 3;
  _field[4] = 2;
  _field[5] = 1;
}

void threeDGame::threeDGameLoop(void) {
  _isOpened = true;
  while (_isOpened) {
    Controller::Update();
    if (Controller::IsKeyPressed(Key::B))
      _isOpened = false;
    if (Controller::IsKeyDown(Key::CPadLeft)) {
      _pos.x -= 0.2;
      if (_pos.x < 0)
        _pos.x = 0;
    }
    if (Controller::IsKeyDown(Key::CPadRight)) {
      _pos.x += 0.2;
      if (_field_width < _pos.x + 0.2)
        _pos.x = _field_width - 0.2;
    }
    if (Controller::IsKeyDown(Key::CPadUp)) {
      _pos.y -= 0.2;
      if (_pos.y < 0)
        _pos.y = 0;
    }
    if (Controller::IsKeyDown(Key::CPadDown)) {
      _pos.y += 0.2;
      if (_field_height < _pos.y + 0.2)
        _pos.y = _field_height - 0.2;
    }
    if (Touch::IsDown()) {
      u8 btmBlockWidth = std::min(320 / _field_width, 240 / _field_height);
      for (u8 i = 0; i < _field_width; i++) {
        for (u8 j = 0; j < _field_height; j++) {
          if (TouchRect((320 - _field_width * btmBlockWidth) / 2 + i * btmBlockWidth, (240 - _field_height * btmBlockWidth) + j * btmBlockWidth, btmBlockWidth, btmBlockWidth)) {
            _field[i + j * _field_width]++;
            if (_block_height < _field[i + j * _field_width])
              _field[i + j * _field_width] = 0;
            break;
          }
        }
      }
    }
    if (Controller::GetKeysDown()) {
      DrawScreen();
      OSD::SwapBuffers();
      DrawScreen();
    }
  }
}

void threeDGame::DrawScreen(void) {
  const Screen &top = OSD::GetTopScreen();
  const Screen &btm = OSD::GetBottomScreen();

  top.DrawRect(0, 0, 400, 240, Color::Gray);
  btm.DrawRect(0, 0, 320, 240, Color::Gray);

  float _sin = _block_width / 2;
  float _cos = _block_width / 2 * 1.7;
  float topPos = (240 - _block_width * _field_height - _sin * _block_height) / 2;

  u8 btmBlockWidth = std::min(320 / _field_width, 240 / _field_height);
  for (u8 i = 0; i < _field_width; i++) {
    for (u8 j = 0; j < _field_height; j++) {
      top.DrawRect(200 - _cos * (j + 1) + _cos * i, topPos + _sin * (i + 1) + _sin * j + _sin * (_block_height - _field[i + j * _field_width]), _cos, 240 - topPos + _sin * (i + 1) + _sin * j + _sin * (_block_height - _field[i + j * _field_width]), _blockColors[_field[i + j * _field_width]] + Color(20, 20, 20));
      top.DrawRect(200 - _cos * j + _cos * i, topPos + _sin * i + _sin * j + _sin * (_block_height - _field[i + j * _field_width]) + _sin, _cos, 240 - topPos + _sin * i + _sin * j + _sin * (_block_height - _field[i + j * _field_width]) + _sin, _blockColors[_field[i + j * _field_width]] - Color(20, 20, 20));
      DrawDiamond(top, 200 - _cos * (j + 1) + _cos * i, topPos + _sin * (i + 1) + _sin * j + _sin * (_block_height - _field[i + j * _field_width]), 200 - _cos * j + _cos * i, topPos + _sin * i + _sin * j + _sin * (_block_height - _field[i + j * _field_width]), _blockColors[_field[i + j * _field_width]], true);
      btm.DrawRect((320 - _field_width * btmBlockWidth) / 2 + i * btmBlockWidth, (240 - _field_height * btmBlockWidth) + j * btmBlockWidth, btmBlockWidth, btmBlockWidth, _blockColors[_field[i + j * _field_width]]);
      if ((u8)_pos.x == i && (u8)_pos.y == j)
        top.DrawRect(200 - _cos * (_pos.y + 1) + _cos * (_pos.x + 1), topPos + _sin * (_pos.x + 1) + _sin * _pos.y + _sin * (_block_height - _field[i + j * _field_width]) - 6 - _sin, 6, 6, Color::White);
    }
  }
  btm.DrawRect((320 - _field_width * btmBlockWidth) / 2 + _pos.x * btmBlockWidth, (240 - _field_height * btmBlockWidth) + _pos.y * btmBlockWidth, 6, 6, Color::White);
}
}  // namespace CTRPluginFramework