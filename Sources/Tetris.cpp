#include "Tetris.hpp"

namespace CTRPluginFramework
{
  const std::vector<Color> Mino::Colors = {Color::SkyBlue, Color::Blue, Color::Orange, Color::Yellow, Color::LimeGreen, Color::Purple, Color::Red};
  const std::vector<std::vector<std::vector<IntVector>>> Mino::Templates =
      {
          {
              {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, // I
              {{1, 1}, {1, 2}, {2, 2}, {3, 2}}, // J
              {{2, 1}, {0, 2}, {1, 2}, {2, 2}}, // L
              {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
              {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, // S
              {{1, 0}, {0, 1}, {1, 1}, {2, 1}}, // T
              {{0, 1}, {1, 1}, {1, 2}, {2, 2}}  // Z
          },
          {
              {{2, 0}, {2, 1}, {2, 2}, {2, 3}}, // I
              {{2, 0}, {2, 1}, {1, 2}, {2, 2}}, // J
              {{1, 1}, {2, 1}, {2, 2}, {2, 3}}, // L
              {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
              {{1, 0}, {1, 1}, {2, 1}, {2, 2}}, // S
              {{1, 0}, {0, 1}, {1, 1}, {1, 2}}, // T
              {{2, 1}, {1, 2}, {2, 2}, {1, 3}}  // Z
          },
          {
              {{0, 2}, {1, 2}, {2, 2}, {3, 2}}, // I
              {{0, 1}, {1, 1}, {2, 1}, {2, 2}}, // J
              {{1, 1}, {2, 1}, {3, 1}, {1, 2}}, // L
              {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
              {{1, 1}, {2, 1}, {0, 2}, {1, 2}}, // S
              {{0, 1}, {1, 1}, {2, 1}, {1, 2}}, // T
              {{1, 1}, {2, 1}, {2, 2}, {3, 2}}  // Z
          },
          {
              {{1, 0}, {1, 1}, {1, 2}, {1, 3}}, // I
              {{1, 1}, {2, 1}, {1, 2}, {1, 3}}, // J
              {{1, 0}, {1, 1}, {1, 2}, {2, 2}}, // L
              {{1, 1}, {2, 1}, {1, 2}, {2, 2}}, // O
              {{1, 1}, {1, 2}, {2, 2}, {2, 3}}, // S
              {{1, 0}, {1, 1}, {2, 1}, {1, 2}}, // T
              {{2, 0}, {1, 1}, {2, 1}, {1, 2}}  // Z
          },
  };

  Mino::Mino(void)
  {
    _kind = Kind::None;
    _rotate = 0;
  }

  Mino::Mino(Kind kind)
  {
    _kind = kind;
    _rotate = 0;
    _blocks = GetBlockTemplate();
  }

  Mino::Kind Mino::GetKind() const
  {
    return _kind;
  }

  const std::vector<IntVector> &Mino::GetBlockTemplate() const
  {
    return Templates[_rotate][(u8)_kind];
  }

  const std::vector<IntVector> &Mino::GetBlocks() const
  {
    return _blocks;
  }

  const Color &Mino::GetColor() const
  {
    return Colors[(u8)_kind];
  }

  const Color &Mino::GetColor(Kind kind)
  {
    return Colors[(u8)kind];
  }

  const Color &Mino::GetMonoChromeColor(void)
  {
    return Color::White;
  }

  const std::vector<IntVector> &Mino::GetBlockTemplate(Kind kind)
  {
    return Templates[0][(u8)kind];
  }

  void Mino::Set(u32 fieldWidth)
  {
    for (auto &&block : _blocks)
      block.x += fieldWidth / 2 - 2;
  }

  void Mino::Move(int moveX, int moveY)
  {
    for (auto &&block : _blocks)
    {
      block.x += moveX;
      block.y += moveY;
    }
  }

  u32 Mino::GetRotate() const
  {
    return _rotate;
  }

  Mino::Kind Tetris::GenerateNextMino(void)
  {
    if (_srcNexts.empty())
    {
      _srcNexts = {Mino::Kind::I, Mino::Kind::J, Mino::Kind::L, Mino::Kind::O, Mino::Kind::S, Mino::Kind::T, Mino::Kind::Z};

      for (u8 i = 0; i < MINO_KINDS_COUNT; i++)
      {
        u8 r = (u8)Utils::Random(i, 6);
        std::swap(_srcNexts[i], _srcNexts[r]);
      }
    }

    Mino::Kind next = _srcNexts[0];
    _srcNexts.erase(_srcNexts.begin());

    return next;
  }

  void Tetris::NextMino(void)
  {
    // 新しいミノを設置
    _mino = Mino(_nexts[0]);
    _mino.Set(FIELD_WIDTH);
    dropClock.Restart();

    // ネクストミノの削除&再生成
    _nexts.erase(_nexts.begin());
    _nexts.emplace_back(GenerateNextMino());

    // ホールドフラグをクリア
    _isHeld = false;
  }

  void Tetris::GameOver(void)
  {
    MessageBox(Utils::Format("Game Over\nyour score is %d", _score))();
    _score = 0;
    _level = 0;
    _hold = Mino::Kind::None;
    _field = std::vector<std::vector<u8>>(FIELD_WIDTH, std::vector<u8>(FIELD_HEIGHT, 0));
  }

  Tetris &Tetris::GetInstance(void)
  {
    static Tetris instance;
    return instance;
  }

  void Tetris::SetFieldWidth(u8 width)
  {
    width = width < 4 || 23 < width ? 10 : width;
    if (FIELD_WIDTH > width)
    {
      for (int i = 0; i < FIELD_WIDTH - width; i++)
      {
        _field.erase(_field.end());
      }
    }

    if (FIELD_WIDTH < width)
    {
      for (int i = 0; i < width - FIELD_WIDTH; i++)
      {
        _field.emplace_back(std::vector<u8>(FIELD_HEIGHT + 1, 0));
      }
    }

    FIELD_WIDTH = width;
    NextMino();
  }

  void Tetris::SetLevel(u8 level)
  {
    _level = level;
  }

  void Tetris::SetColorful(bool colorful)
  {
    _colorfulMode = colorful;
  }

  bool Tetris::Put(void)
  {
    for (auto &&block : _mino.GetBlocks())
    {
      if (block.y == 0 || _field[block.x][block.y])
      {
        GameOver();
        return false;
      }

      _field[block.x][block.y] = (u8)_mino.GetKind() + 1;
    }

    NextMino();
    return true;
  }

  void Mino::SetRotate(const TetrisField &field, u32 rotate)
  {
    u32 before = _rotate;
    IntVector diffPos = {(_blocks[0].x - GetBlockTemplate()[0].x), (_blocks[0].y - GetBlockTemplate()[0].y)};

    _rotate = rotate;

    for (ssize_t i = 0; i < (ssize_t)GetBlockTemplate().size();i++)
    {
      if (int(GetBlockTemplate()[i].x) + diffPos.x < 0)
      {
        diffPos.x = 0;
        i = -1;
        continue;
      }
      if (Tetris::GetInstance().FIELD_WIDTH - 1 < GetBlockTemplate()[i].x + diffPos.x)
      {
        diffPos.x -= GetBlockTemplate()[i].x + diffPos.x - Tetris::GetInstance().FIELD_WIDTH + 1;
        i = -1;
        continue;
      }
      if (GetBlockTemplate()[i].y + diffPos.y < 0)
        diffPos.y = 0;
      if (Tetris::FIELD_HEIGHT - 1 < GetBlockTemplate()[i].y + diffPos.y || field[GetBlockTemplate()[i].x + diffPos.x][GetBlockTemplate()[i].y + diffPos.y])
      {
        _rotate = before;
        return;
      }
    }

    // 新しいブロックを適用
    _blocks = GetBlockTemplate();
    for (auto &&block : _blocks)
    {
      block.x += diffPos.x;
      block.y += diffPos.y;
    }
  }

  void Tetris::HoldMino(void)
  {
    _isHeld = true;

    if (_hold == Mino::Kind::None)
    {
      _hold = _mino.GetKind();
      NextMino();
    }
    else
    {
      Mino::Kind curKind = _mino.GetKind();

      _mino = Mino(_hold);
      _mino.Set(FIELD_WIDTH);
      dropClock.Restart();

      _hold = curKind;
    }
  }

  bool Tetris::CheckSpace(Direction dir)
  {
    for (auto &&block : _mino.GetBlocks())
    {
      if (dir == Direction::Under && (FIELD_HEIGHT - 1 <= block.y || _field[block.x][block.y + 1]))
        return false;
      else if (dir == Direction::Left && (block.x <= 0 || _field[block.x - 1][block.y]))
        return false;
      else if (dir == Direction::Right && (FIELD_WIDTH - 1 <= block.x || _field[block.x + 1][block.y]))
        return false;
    }
    return true;
  }

  void Tetris::FillScreen(const Screen &scr, const Color &color)
  {
    scr.DrawRect(0, 0, scr.IsTop ? 400 : 320, 240, color);
    OSD::SwapBuffers();
    scr.DrawRect(0, 0, scr.IsTop ? 400 : 320, 240, color);
  }

  bool Tetris::CheckLine(u32 line)
  {
    u8 count = 0;
    for (u8 i = 0; i < FIELD_WIDTH; i++)
    {
      if (_field[i][line])
        count++;
    }
    return (count == FIELD_WIDTH);
  }

  void Tetris::RemoveLine(u32 line)
  {
    for (int i = line - 1; i >= 0; i--)
    {
      for (int j = 0; j < FIELD_WIDTH; j++)
      {
        _field[j][i + 1] = _field[j][i];
      }
    }
  }

  void Tetris::Start(HotkeyManager Hotkeys)
  {
    bool isOpened = true;
    const Screen &topScr = OSD::GetTopScreen();
    const Screen &btmScr = OSD::GetBottomScreen();

    _nexts.clear();
    for (u8 i = 0; i < NEXT_COUNT; i++)
    {
      _nexts.emplace_back(GenerateNextMino());
    }
    _hold = Mino::Kind::None;

    NextMino();

    FillScreen(topScr, Color::Gray);
    FillScreen(btmScr, Color::Gray);

    while (isOpened)
    {
      Controller::Update();

      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2, 9, FIELD_WIDTH * BLOCK_WIDTH + 6, 222, Color::Black);
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 2, 11, FIELD_WIDTH * BLOCK_WIDTH + 6 - 4, 218, Color::White, false);

      // 左移動
      if (Hotkeys[0].IsDown() && CheckSpace(Direction::Left))
      {
        if (Hotkeys[0].IsPressed())
        {
          _mino.Move(-1, 0);
          moveClock.Restart();
        }
        else if (moveClock.HasTimePassed(Seconds(0.2)))
        {
          _mino.Move(-1, 0);
        }
      }

      // 右移動
      else if (Hotkeys[1].IsDown() && CheckSpace(Direction::Right))
      {
        if (Hotkeys[1].IsPressed())
        {
          _mino.Move(1, 0);
          moveClock.Restart();
        }
        else if (moveClock.HasTimePassed(Seconds(0.2)))
        {
          _mino.Move(1, 0);
        }
      }

      // ソフトドロップ
      else if (Hotkeys[2].IsDown() && CheckSpace(Direction::Under) && softdropClock.HasTimePassed(Seconds(0.08)))
      {
        _mino.Move(0, 1);
        softdropClock.Restart();
      }

      // ハードドロップ
      else if (Hotkeys[3].IsPressed())
      {
        bool dropped = false;

        for (size_t i = 0; i < FIELD_HEIGHT; i++)
        {
          for (auto &&block : _mino.GetBlocks())
          {
            if (block.y + i >= FIELD_HEIGHT - 1 || _field[block.x][block.y + 1 + i])
            {
              _mino.Move(0, i);
              isOpened = Put();
              dropped = true;
              break;
            }
          }
          if (dropped)
            break;
        }
      }

      // 右回転
      if (Hotkeys[4].IsPressed())
      {
        u32 rotate = _mino.GetRotate();
        if (rotate-- == 0)
          rotate = 3;
        _mino.SetRotate(_field, rotate);
      }

      // 左回転
      if (Hotkeys[5].IsPressed())
      {
        u32 rotate = _mino.GetRotate();
        if (rotate++ == 3)
          rotate = 0;
        _mino.SetRotate(_field, rotate);
      }

      // ホールド
      if (Hotkeys[6].IsPressed() && !_isHeld)
        HoldMino();

      // ポーズ
      if (Hotkeys[7].IsPressed())
      {
        s8 ans;
        if (0 <= (ans = Keyboard("Paused", {"continue", "settings", "quit"}).Open()))
        {
          if (ans == 1)
          {
            s8 answer;
            u8 width;
            if (0 <= (answer = Keyboard("tetris color", {"monochrome", "colorful"}).Open()))
              SetColorful(answer);
            if (0 <= (answer = Keyboard("level", {"easy", "normal", "difficult"}).Open()))
              SetLevel(answer * 5);
            Keyboard key("input field's width\ndefault is 10\n4以上23以下でよろ");
            key.IsHexadecimal(false);
            if (0 <= key.Open(width))
              SetFieldWidth(width);
          }
          else if (ans == 2)
            isOpened = false;
        }
        FillScreen(topScr, Color::Gray);
        FillScreen(btmScr, Color::Gray);
      }

      // ミノ落下
      if (dropClock.HasTimePassed(Milliseconds(1000 - _level * 30)))
      {
        for (auto &&block : _mino.GetBlocks())
        {
          if (block.y >= FIELD_HEIGHT - 1 || _field[block.x][block.y + 1])
          {
            isOpened = Put();
            break;
          }
        }
        _mino.Move(0, 1);
        dropClock.Restart();
      }

      // フィールド上のブロック描画
      for (size_t i = 0; i < FIELD_WIDTH; i++)
        for (size_t j = 0; j < FIELD_HEIGHT; j++)
          if (_field[i][j])
            topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 3 + i * BLOCK_WIDTH + 1, BLOCK_WIDTH + j * BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _colorfulMode ? Mino::GetColor((Mino::Kind)(_field[i][j] - 1)) : Mino::GetMonoChromeColor());

      // 落下中のミノ描画
      for (auto &&block : _mino.GetBlocks())
        topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 3 + block.x * BLOCK_WIDTH + 1, BLOCK_WIDTH + block.y * BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _colorfulMode ? _mino.GetColor() : Mino::GetMonoChromeColor());

      // Next
      for (u8 i = 0; i < NEXT_COUNT; i++)
      {
        Mino::Kind next = _nexts[i];
        u32 posY = 12 + 40 * i;
        topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + FIELD_WIDTH * BLOCK_WIDTH + 6 + 2, posY, 55, 40, Color::White, false);
        topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + FIELD_WIDTH * BLOCK_WIDTH + 6 + 3, posY + 1, 53, 38, Color::Black);
        for (auto &&block : Mino::GetBlockTemplate(next))
          topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + FIELD_WIDTH * BLOCK_WIDTH + 6 + 7 + block.x * BLOCK_WIDTH + 1, posY + block.y * BLOCK_WIDTH + 3, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _colorfulMode ? Mino::GetColor(next) : Mino::GetMonoChromeColor());
      }

      // Hold
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 - 58, 12, 55, 40, Color::White, false);
      topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 - 57, 12 + 1, 53, 38, Color::Black);
      if (_hold != Mino::Kind::None)
      {
        for (auto &&block : Mino::GetBlockTemplate(_hold))
        {
          topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 - 53 + block.x * BLOCK_WIDTH + 1, 12 + block.y * BLOCK_WIDTH + 3, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _colorfulMode ? Mino::GetColor(_hold) : Mino::GetMonoChromeColor());
        }
      }

      // 落下位置描画
      bool drawn = false;
      for (int i = 0; i < FIELD_HEIGHT; i++)
      {
        for (auto &&block : _mino.GetBlocks())
        {
          if (block.y + i >= FIELD_HEIGHT - 1 || _field[block.x][block.y + 1 + i])
          {
            for (auto &&block : _mino.GetBlocks())
            {
              u32 y = block.y + i;
              topScr.DrawRect(200 - (FIELD_WIDTH * BLOCK_WIDTH + 6) / 2 + 3 + block.x * BLOCK_WIDTH + 1, BLOCK_WIDTH + y * BLOCK_WIDTH + 1, BLOCK_WIDTH - 2, BLOCK_WIDTH - 2, _colorfulMode ? _mino.GetColor() : Mino::GetMonoChromeColor(), false);
            }
            drawn = true;
            break;
          }
        }
        if (drawn)
          break;
      }

      // 1行ずつチェック
      for (int i = 0; i < FIELD_HEIGHT; i++)
      {
        if (CheckLine(i))
        {
          RemoveLine(i);

          _score++;

          // 10ごとにレベルを上げる
          if (_level < 11 && _score % 10 == 0)
          {
            _level++;
          }
        }
      }
      OSD::SwapBuffers();
    }
  }
}