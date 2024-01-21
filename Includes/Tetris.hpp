#pragma once

#include <CTRPluginFramework.hpp>
#include "types.h"

namespace CTRPluginFramework
{
  using TetrisField = std::vector<std::vector<u8>>;

  class Mino {
  public:
    enum class Kind {
      I,
      J,
      L,
      O,
      S,
      T,
      Z,
      None
    };

    Mino(void);
    Mino(Kind kind);

    void Set(u32 fieldWidth);
    void SetRotate(const TetrisField &field, u32 rotate);
    u32 GetRotate() const;
    void Move(int moveX, int moveY);
    const std::vector<IntVector> &GetBlocks() const;

    Kind GetKind() const;
    const Color &GetColor() const;
    static const Color &GetColor(Kind kind);
    static const Color &GetMonoChromeColor(void);
    const std::vector<IntVector> &GetBlockTemplate() const;
    static const std::vector<IntVector> &GetBlockTemplate(Kind kind);
  private:
    Kind _kind;
    u8 _rotate;
    std::vector<IntVector> _blocks;

    static const std::vector<std::vector<std::vector<IntVector>>> Templates;
    static const std::vector<Color> Colors;
  };

  class Tetris
  {
  public:
    enum class Direction
    {
      Under,
      Left,
      Right
    };

    void Start(HotkeyManager Hotkeys);

    void SetLevel(u8 level);
    void SetColorful(bool colorful);
    void SetFieldWidth(u8 width);

    static Tetris &GetInstance();

    Tetris(Tetris const &) = delete;
    Tetris &operator=(Tetris const &) = delete;
    Tetris(Tetris &&) = delete;
    Tetris &operator=(Tetris &&) = delete;

  private:
    friend class Mino;

    static constexpr u8 MINO_KINDS_COUNT = 7;
    u8 FIELD_WIDTH = 10;
    static constexpr u8 FIELD_HEIGHT = 18;
    static constexpr u8 BLOCK_WIDTH = 12;
    static constexpr u8 NEXT_COUNT = 5;

    TetrisField _field = TetrisField(FIELD_WIDTH, std::vector<u8>(FIELD_HEIGHT + 1, 0));
    std::vector<Mino::Kind> _nexts;
    std::vector<Mino::Kind> _srcNexts;

    // 落下中のミノ
    Mino _mino;
    Clock dropClock;
    Clock moveClock;
    Clock softdropClock;

    // ホールド
    Mino::Kind _hold;
    bool _isHeld;

    u8 _score = 0;
    u8 _level = 0;
    bool _colorfulMode = true;

    bool Put(void);
    void GameOver(void);
    void NextMino(void);
    void HoldMino(void);
    Mino::Kind GenerateNextMino(void);
    bool CheckSpace(Direction dir);
    void FillScreen(const Screen &scr, const Color &color);
    bool CheckLine(u32 line);
    void RemoveLine(u32 line);

    Tetris() = default;
    ~Tetris() = default;
  };
}