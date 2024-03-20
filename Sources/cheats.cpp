#include "cheats.hpp"

namespace CTRPluginFramework {
void Test1(MenuEntry *entry) {
  static CustomFont font("font.otf", 60);
  font.Draw("こんにちは\nせかい", OSD::GetTopScreen(), 50, 100, Color::BlackGrey, Color(0, 0, 0, 0));
}

void JPNotify(MenuEntry *entry) {
  std::vector<std::string> words = {"コ", "ン", "ニ", "チ", "ワ"};
  if (Controller::IsKeyDown(Key::Start) && Controller::IsKeyDown(Key::Y))
    words = {"オ", "チ", "ン", "チ", "ン"};
  if (Controller::IsKeyPressed(Key::Start)) {
    std::string buff;
    for (int j = 0; j < 5; j++) {
      buff += words[Utils::Random(0, 4)];
    }
    u8 red = Utils::Random(0, 255), green = Utils::Random(0, 255), blue = Utils::Random(0, 255);
    u16 sum = std::max({red, green, blue}) + std::min({red, green, blue});
    OSDJPNotify(buff, Color(red, green, blue), Color(sum - red, sum - green, sum - blue));
  }
}

void Pipes(MenuEntry *entry) {
  static u32 xPos = 150, yPos = 120;
  static u8 direct = 3, temp_direct, buff_direct, i = 0, len, j;
  Color colorList[7] = {Color::Red, Color::Blue, Color::Cyan, Color::Green, Color::Yellow, Color::Orange, Color::White};
  static bool isReset = false;
  if (entry->WasJustActivated()) {
    fillScreenBuffer(Color(0, 0, 0, 0));
    OSD::Run(ShowScreenBuffer);
  }
  if (!entry->IsActivated()) {
    OSD::Stop(ShowScreenBuffer);
  }
  if (isReset) {
    switch (direct) {
      case 0:
        xPos = Utils::Random(2, 398);
        yPos = 2;
        break;
      case 1:
        xPos = 2;
        yPos = Utils::Random(2, 238);
        break;
      case 2:
        xPos = 398;
        yPos = Utils::Random(2, 238);
        break;
      default:
        xPos = Utils::Random(2, 398);
        yPos = 238;
        break;
    }
    isReset = false;
    if (i < 6)
      i++;
    else
      i = 0;
  }
  if (len > 0) {
    if (xPos <= 1 || xPos >= 399 || yPos <= 1 || yPos >= 239)
      isReset = true;
    else {
      for (int k = -1; k < 2; k++) {
        setScreenBuffer(xPos + k, yPos + k, colorList[i]);
      }
      switch (direct) {
        case 0:
          if (buff_direct == 1)
            setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
          else
            setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
          yPos++;
          break;
        case 1:
          if (buff_direct == 0)
            setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
          else
            setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
          xPos++;
          break;
        case 2:
          if (buff_direct == 0)
            setScreenBuffer(xPos, yPos + 2, Color(0, 0, 0, 0));
          else
            setScreenBuffer(xPos, yPos - 2, Color(0, 0, 0, 0));
          xPos--;
          break;
        default:
          if (buff_direct == 1)
            setScreenBuffer(xPos + 2, yPos, Color(0, 0, 0, 0));
          else
            setScreenBuffer(xPos - 2, yPos, Color(0, 0, 0, 0));
          yPos--;
          break;
      }
      len--;
    }
  } else {
    len = Utils::Random(3, 30);
    while (1) {
      temp_direct = Utils::Random(0, 3);
      if (direct != 3 - temp_direct)
        break;
    }
    buff_direct = direct;
    direct = temp_direct;
  }
  if (Controller::IsKeyPressed(Key::Start)) {
    fillScreenBuffer(Color(0, 0, 0, 0));
    isReset = true;
  }
}

void addSearch(MenuFolder *folder, MenuFolder *SearchFolder, std::string input, std::vector<MenuFolder *> ancestorFolders) {
  if (folder->Name() == "Search")
    return;
  std::vector<CTRPluginFramework::MenuEntry *> entries = folder->GetEntryList();
  std::vector<MenuFolder *> folders = folder->GetFolderList();
  for (auto &&folder1 : folders) {
    bool flag = true;
    for (auto &&ancestorFolder : ancestorFolders)
      if (folder1 == ancestorFolder) {
        flag = false;
        break;
      }
    if (flag) {
      ancestorFolders.emplace_back(folder1);
      addSearch(folder1, SearchFolder, input, ancestorFolders);
    }
  }
  for (auto &&entry : entries) {
    if ((Convert::toLower(entry->Name()).find(input) != std::string::npos) || (Convert::hiraganaToKatakana(entry->Note()).find(input) != std::string::npos) || (Convert::katakanaToHiragana(entry->Note()).find(input) != std::string::npos)) {
      *SearchFolder += entry;
    }
  }
}

void Search(MenuEntry *entry) {
  std::string input;
  PluginMenu *menu = PluginMenu::GetRunningInstance();
  if (!JPKeyboard("エントリー名を入力してください").SetMaxLength(30).Open(input))
    return;

  input = Convert::hiraganaToKatakana(Convert::toLower(input));
  std::vector<MenuFolder *> folders = menu->GetFolderList();
  MenuFolder *SearchFolder = nullptr;
  for (auto &&folder : folders) {
    if (folder->Name() == "Search") {
      folder->Clear();
      *folder += new MenuEntry("Search", nullptr, Search);
      SearchFolder = folder;
    }
  }
  std::vector<CTRPluginFramework::MenuEntry *> entries = menu->GetEntryList();
  for (auto &&menu_entry : entries) {
    if ((Convert::toLower(menu_entry->Name()).find(input) != std::string::npos) || (Convert::toLower(menu_entry->Note()).find(input) != std::string::npos))
      *SearchFolder += new MenuEntry(menu_entry->Name(), menu_entry->GetGameFunc(), menu_entry->GetMenuFunc(), menu_entry->Note());
  }

  std::vector<MenuFolder *> ancestorFolders;
  for (auto &&folder : folders)
    addSearch(folder, SearchFolder, input, ancestorFolders);
}

float AA, BB, CC;

float cubeWidth = 20;
int width = 400, height = 240;
float zBuffer[400 * 240];
char buffer[400 * 240];
int distanceFromCam = 100;
float horizontalOffset;
float K1 = 40;

float incrementSpeed = 0.6;

float x, y, z;
float ooz;
int xp, yp;
int idx;

float calculateX(int i, int j, int k) {
  return j * sin(AA) * sin(BB) * cos(CC) - k * cos(AA) * sin(BB) * cos(CC) +
         j * cos(AA) * sin(CC) + k * sin(AA) * sin(CC) + i * cos(BB) * cos(CC);
}

float calculateY(int i, int j, int k) {
  return j * cos(AA) * cos(CC) + k * sin(AA) * cos(CC) -
         j * sin(AA) * sin(BB) * sin(CC) + k * cos(AA) * sin(BB) * sin(CC) -
         i * cos(BB) * sin(CC);
}

float calculateZ(int i, int j, int k) {
  return k * cos(AA) * cos(BB) - j * sin(AA) * cos(BB) + i * sin(BB);
}

void calculateForSurface(float cubeX, float cubeY, float cubeZ, int ch) {
  x = calculateX(cubeX, cubeY, cubeZ);
  y = calculateY(cubeX, cubeY, cubeZ);
  z = calculateZ(cubeX, cubeY, cubeZ) + distanceFromCam;

  ooz = 1 / z;

  xp = (int)(width / 2 + horizontalOffset + K1 * ooz * x * 2);
  yp = (int)(height / 2 + K1 * ooz * y);

  idx = xp + yp * width;
  if (idx >= 0 && idx < width * height) {
    if (ooz > zBuffer[idx]) {
      zBuffer[idx] = ooz;
      buffer[idx] = ch;
    }
  }
}
void rotateCube(const Screen &screen) {
  if (!screen.IsTop)
    return;
  memset(buffer, 10, sizeof(buffer));
  memset(zBuffer, 0, width * height * 4);
  horizontalOffset = 1 * cubeWidth;
  // second cube
  for (float cubeX = -cubeWidth; cubeX < cubeWidth; cubeX += incrementSpeed) {
    for (float cubeY = -cubeWidth; cubeY < cubeWidth; cubeY += incrementSpeed) {
      calculateForSurface(cubeX, cubeY, -cubeWidth, '@');
      calculateForSurface(cubeWidth, cubeY, cubeX, '$');
      calculateForSurface(-cubeWidth, cubeY, -cubeX, '~');
      calculateForSurface(-cubeX, cubeY, cubeWidth, '#');
      calculateForSurface(cubeX, -cubeWidth, -cubeY, ';');
      calculateForSurface(cubeX, cubeWidth, cubeY, '+');
    }
  }
  for (size_t i = 0; i < 2; i++) {
    screen.DrawRect(0, 0, 400, 240, Color::Gray);
    screen.DrawSysfont("B:return to menu", 0, 0);
    screen.DrawSysfont("START:toggle auto rotate", 0, 15);
    for (int k = 0; k < width * height; k++) {
      switch (buffer[k]) {
        case 64:
          screen.DrawPixel(k % width, k / width, Color::Red);
          break;
        case 36:
          screen.DrawPixel(k % width, k / width, Color::Blue);
          break;
        case 126:
          screen.DrawPixel(k % width, k / width, Color::Orange);
          break;
        case 35:
          screen.DrawPixel(k % width, k / width, Color::Green);
          break;
        case 59:
          screen.DrawPixel(k % width, k / width, Color::White);
          break;
        case 43:
          screen.DrawPixel(k % width, k / width, Color::Yellow);
          break;
      }
    }
    OSD::SwapBuffers();
  }
}

void Cube(MenuEntry *entry) {
  const Screen &screen = OSD::GetTopScreen();
  bool autoRotate = false;
  rotateCube(screen);
  while (!Controller::IsKeyPressed(Key::B)) {
    if (Controller::IsKeyDown(Key::CPadLeft) || autoRotate)
      BB -= 0.05;
    if (Controller::IsKeyDown(Key::CPadRight))
      BB += 0.05;
    if (Controller::IsKeyDown(Key::CPadUp) || autoRotate)
      AA += 0.15;
    if (Controller::IsKeyDown(Key::CPadDown))
      AA -= 0.05;
    if (Controller::IsKeyDown(Key::CStickUp) || autoRotate)
      CC -= 0.10;
    if (Controller::IsKeyDown(Key::CStickDown))
      CC += 0.05;
    if (Controller::IsKeyDown(Key::CStickLeft))
      distanceFromCam -= 5;
    if (Controller::IsKeyDown(Key::CStickRight))
      distanceFromCam += 5;
    if (Controller::IsKeyPressed(Key::Start))
      autoRotate = !autoRotate;
    if (Controller::GetKeysDown() || autoRotate)
      rotateCube(screen);
    Controller::Update();
  }
}

void BadApple(MenuEntry *entry) {
  static int frame_num = 0;
  if (!entry->IsActivated()) {
    Process::Play();
    return;
  }
  Process::Pause();
  if (entry->WasJustActivated())
    frame_num = 0;

  std::vector<u64> frame = getFrame(frame_num);

  const Screen &screen = OSD::GetTopScreen();
  for (size_t i = 0; i < frame.size(); i++) {
    u8 index = 0;
    for (u64 j = 1; j < 0x1000000000000000; j *= 2) {
      if (frame[i] & j)
        screen.DrawRect(380 - index * 6, 10 + i * 10, 6, 10, Color::White);
      else
        screen.DrawRect(380 - index * 6, 10 + i * 10, 6, 10, Color(0, 0, 0, 0));
      index++;
    }
  }
  Sleep(Milliseconds(100));
  frame_num++;
  OSD::SwapBuffers();
}

void PlayMusic(MenuEntry *entry) {
  std::vector<std::string> files_name;
  Directory("MUSIC", true).ListFiles(files_name, ".bcwav");
  if (!files_name.size()) {
    MessageBox("no files found")();
    return;
  }
  int i = Keyboard("select file:", files_name).Open();
  if (0 <= i)
    if (!PlaySound("MUSIC/" + files_name[i])) {
      MessageBox("failed to play sound")();
      Sleep(Seconds(0.5));
    }
}

void Command(MenuEntry *entry) {
  Command::GetInstance().Command_Loop();
}

void ColorPicker(MenuEntry *entry) {
  Color out;
  colorPicker(out);
}

void Tetris(MenuEntry *entry) {
  if (!Process::IsPaused())
    return;
  Tetris::GetInstance().Start(entry->Hotkeys);
}

void ShowPallet(MenuEntry *entry) {
  if (entry->WasJustActivated()) {
    OSD::Run(ShowScreenBuffer);
  }
  if (!entry->IsActivated()) {
    OSD::Stop(ShowScreenBuffer);
  }
}

void MenuEntryPaint(MenuEntry *entry) {
  Paint paint;

  if (paint.Open() == 0) {
    entry->SetGameFunc(ShowPallet);
  }
}

void LifeGame(MenuEntry *entry) {
  LifeGame::GetInstance().LifeGame_Loop();
}

void threeD(MenuEntry *entry) {
  threeDGame::GetInstance().threeDGameLoop();
}

// based on https://github.com/HaramakiPtr/CalcLED3gx/blob/master/Sources/LED.cpp#L76
void LedEffect(MenuEntry *entry) {
  Led led(entry);

  if (Controller::IsKeyDown(Key::Y)) {
    led.setSmoothing(0x20);
  }

  if (Controller::IsKeyDown(Key::DPadLeft)) {
    // red
    led.setColor(0xFF, 0x00, 0x00);
  } else if (Controller::IsKeyDown(Key::DPadDown)) {
    // green
    led.setColor(0x00, 0xFF, 0x00);
  } else if (Controller::IsKeyDown(Key::DPadRight)) {
    // blue
    led.setColor(0x00, 0x00, 0xFF);
  } else if (Controller::IsKeyDown(Key::DPadUp)) {
    // white
    led.setColor(0xFF, 0xFF, 0xFF);
  } else {
    // off
    led.setColor(0x00, 0x00, 0x00);
  }

  led.update();
}

void Game2048(MenuEntry *entry) {
  Game2048::GetInstance().Game2048_Loop();
}

// made by maru
struct vec3d {
  float x, y, z;
};

struct triangle {
  vec3d p[3];
};

struct mesh {
  std::vector<triangle> tris;
};

struct mat4x4 {
  float m[4][4] = {0};
};

void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) {
  o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
  o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
  o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
  float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

  if (w != 0.0f) {
    o.x /= w;
    o.y /= w;
    o.z /= w;
  }
}

void DrawLine(s32 x1, s32 y1, s32 x2, s32 y2, Color c) {
  int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
  dx = x2 - x1;
  dy = y2 - y1;
  const Screen &scr = OSD::GetTopScreen();

  // straight lines idea by gurkanctn
  if (dx == 0)  // Line is vertical
  {
    if (y2 < y1)
      std::swap(y1, y2);
    for (y = y1; y <= y2; y++)
      scr.DrawPixel(x1, y, c);
    return;
  }

  if (dy == 0)  // Line is horizontal
  {
    if (x2 < x1)
      std::swap(x1, x2);
    for (x = x1; x <= x2; x++)
      scr.DrawPixel(x, y1, c);
    return;
  }

  // Line is Funk-aye
  dx1 = std::abs(dx);
  dy1 = std::abs(dy);
  px = 2 * dy1 - dx1;
  py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = x1;
      y = y1;
      xe = x2;
    } else {
      x = x2;
      y = y2;
      xe = x1;
    }

    scr.DrawPixel(x, y, c);

    for (i = 0; x < xe; i++) {
      x = x + 1;
      if (px < 0)
        px = px + 2 * dy1;
      else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          y = y + 1;
        else
          y = y - 1;
        px = px + 2 * (dy1 - dx1);
      }

      scr.DrawPixel(x, y, c);
    }
  } else {
    if (dy >= 0) {
      x = x1;
      y = y1;
      ye = y2;
    } else {
      x = x2;
      y = y2;
      ye = y1;
    }

    scr.DrawPixel(x, y, c);

    for (i = 0; y < ye; i++) {
      y = y + 1;
      if (py <= 0)
        py = py + 2 * dx1;
      else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          x = x + 1;
        else
          x = x - 1;
        py = py + 2 * (dx1 - dy1);
      }

      scr.DrawPixel(x, y, c);
    }
  }
}

void DrawTriangle(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, Color c) {
  DrawLine(x1, y1, x2, y2, c);
  DrawLine(x2, y2, x3, y3, c);
  DrawLine(x3, y3, x1, y1, c);
}

void Projection(MenuEntry *entry) {
  // Init
  mesh meshCube;
  mat4x4 matProj;

  float fTheta = 0.0f;

  meshCube.tris = {

      // SOUTH
      {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

      // EAST
      {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

      // NORTH
      {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
      {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

      // WEST
      {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

      // TOP
      {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
      {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

      // BOTTOM
      {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

  };

  // Projection Matrix
  const int screenWidth = 240, screenHeight = 240;
  float fNear = 0.1f;
  float fFar = 1000.0f;
  float fFov = 90.0f;
  float fAspectRatio = (float)screenWidth / (float)screenHeight;
  float fFovRad = 1.0f / std::tan(fFov * 0.5f / 180.0f * 3.14159f);

  matProj.m[0][0] = fAspectRatio * fFovRad;
  matProj.m[1][1] = fFovRad;
  matProj.m[2][2] = fFar / (fFar - fNear);
  matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
  matProj.m[2][3] = 1.0f;
  matProj.m[3][3] = 0.0f;

  const Screen &scr = OSD::GetTopScreen();

  while (1) {
    scr.DrawRect(0, 0, screenWidth, screenHeight, Color::Black);

    // Set up rotation matrices
    mat4x4 matRotZ, matRotX;
    fTheta += 0.05;

    // Rotation Z
    matRotZ.m[0][0] = std::cos(fTheta);
    matRotZ.m[0][1] = std::sin(fTheta);
    matRotZ.m[1][0] = -std::sin(fTheta);
    matRotZ.m[1][1] = std::cos(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = std::cos(fTheta * 0.5f);
    matRotX.m[1][2] = std::sin(fTheta * 0.5f);
    matRotX.m[2][1] = -std::sin(fTheta * 0.5f);
    matRotX.m[2][2] = std::cos(fTheta * 0.5f);
    matRotX.m[3][3] = 1;

    // Draw Triangles
    for (auto tri : meshCube.tris) {
      triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

      // Rotate in Z-Axis
      MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
      MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
      MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

      // Rotate in X-Axis
      MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
      MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
      MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

      // Offset into the screen
      triTranslated = triRotatedZX;
      triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
      triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
      triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

      // Project triangles from 3D --> 2D
      MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj);
      MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
      MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

      // Scale into view
      triProjected.p[0].x += 1.0f;
      triProjected.p[0].y += 1.0f;
      triProjected.p[1].x += 1.0f;
      triProjected.p[1].y += 1.0f;
      triProjected.p[2].x += 1.0f;
      triProjected.p[2].y += 1.0f;
      triProjected.p[0].x *= 0.5f * (float)screenWidth;
      triProjected.p[0].y *= 0.5f * (float)screenHeight;
      triProjected.p[1].x *= 0.5f * (float)screenWidth;
      triProjected.p[1].y *= 0.5f * (float)screenHeight;
      triProjected.p[2].x *= 0.5f * (float)screenWidth;
      triProjected.p[2].y *= 0.5f * (float)screenHeight;

      // Rasterize triangle
      DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                   triProjected.p[1].x, triProjected.p[1].y,
                   triProjected.p[2].x, triProjected.p[2].y,
                   Color::Yellow);
    }
    OSD::SwapBuffers();

    Controller::Update();
    if (Controller::IsKeyPressed(Key::B))
      break;
  }
  // Update
}

void SendDiscord(MenuEntry *entry) {
  std::string message;
  std::vector<u8> out;
  CURLcode res;
  if (!JPKeyboard("send a message to my server, but you can't see it.").Open(message))
    return;
  message = "{\"content\":\"" + message + "\"}";
  if ((res = Curl::Post("https://discord.com/api/webhooks/1143805416581648435/twidx7vuqngG9XFhxd8AkddpdMBb4k7CZp7UrIta-nKh2f3MAF_yKKLdycIW7vbK5Ftl", message.c_str(), out)) == CURLE_OK)
    MessageBox("success!")();
  else
    OSD::Notify(Utils::Format("failed %d", res));
  Sleep(Seconds(1));
}

struct DownloadScreenArg {
  bool finish;
  Color foreground;
  Color background;
  const Screen &screen;
  std::string message;
};

s32 DownloadScreen(void *arg) {
  int UI_Pos = 0;
  DownloadScreenArg *_arg = (DownloadScreenArg *)arg;
  if (!_arg->screen.IsTop)
    return -1;
  while (!_arg->finish) {
    std::string load = (_arg->message == "") ? "Downloading now..." : _arg->message;
    switch (UI_Pos++ / 10 % 8) {
      case 0:
        load += "\uE020";
        break;
      case 1:
        load += "\uE021";
        break;
      case 2:
        load += "\uE022";
        break;
      case 3:
        load += "\uE023";
        break;
      case 4:
        load += "\uE024";
        break;
      case 5:
        load += "\uE025";
        break;
      case 6:
        load += "\uE026";
        break;
      case 7:
        load += "\uE027";
        break;
    }
    DrawSysfontPlus(_arg->screen, load, 200 - OSD::GetTextWidth(true, load) / 2, 120, 0, 0, _arg->foreground, _arg->background, Color::White, true, 0, 0);
    if (Process::IsPaused())
      OSD::SwapBuffers();
  }
  return 0;
}

struct sockTaskArg {
  DownloadScreenArg arg;
  int sockfd;
};

s32 sockTask(void *arg) {
  DownloadScreenArg &_arg = *(DownloadScreenArg *)arg;
  int sockfd = (*(sockTaskArg *)arg).sockfd;
  if (sockfd < 0) {
    return -1;
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(5000);
  serv_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    return -1;

  if (listen(sockfd, 1) < 0)
    return -1;

  struct sockaddr_in cli_addr;
  socklen_t cli_len = sizeof(cli_addr);
  int connect = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);

  if (connect < 0)
    return -1;
  _arg.message = "connected!";

  u32 size = 0;
  recv(connect, &size, sizeof(size), 0);

  _arg.message = Utils::Format("size: %d", size);

  File file("buff.3gx", File::Mode::RWC);
  int res = file.Clear();
  if (!file.IsOpen() || res) {
    close(connect);
    file.Close();
    return -1;
  }

  u8 *buffer = (u8 *)malloc(0x1500);
  u32 pos = 0, chunk = 0x1500;
  while (pos < size && !_arg.finish) {
    if (size < pos + chunk)
      chunk = size - pos;

    int res = recv(connect, buffer, chunk, 0);
    if (res <= 0) {
      close(connect);
      file.Close();
      return -1;
    }
    file.Write(buffer, res);
    pos += res;
    _arg.message = Utils::Format("downloaded: %d/%d(%.2f%%)", pos, size, (float)pos / size * 100);
  }
  free(buffer);

  file.Flush();
  file.Close();

  close(connect);

  return 0;
}

void Update3gx(MenuEntry *entry) {
  auto ans = Keyboard("which do you want?", {"from latest release", "from latest commit", "from your pc with socket"}).Open();
  Task task(DownloadScreen);
  DownloadScreenArg arg = {false, Color::SkyBlue, Color::Black, OSD::GetTopScreen(), ""};
  bool successDownload = false;
  switch (ans) {
    case 0:
      CURLcode res;
      task.Start((void *)&arg);
      if ((res = Curl::Download("https://github.com/salmarine469/ideaCTRPF/releases/latest/download/ideaCTRPF.3gx", "buff.3gx")) != CURLE_OK) {
        arg.finish = true;
        MessageBox(Utils::Format("failed download %d", res))();
        break;
      }
      arg.finish = true;
      successDownload = true;
      break;
    case 1:
      MessageBox("not supported yet")();
      Sleep(Seconds(0.5));
      break;
    case 2:
      sockTaskArg _arg = {false, Color::SkyBlue, Color::Black, OSD::GetTopScreen(), "connecting...", socket(AF_INET, SOCK_STREAM, 0)};
      task.Start((void *)&_arg);
      Task updateTask(sockTask);
      updateTask.Start((void *)&_arg);
      while (updateTask.Status() != Task::Status::Finished) {
        Controller::Update();
        if (Controller::IsKeyPressed(Key::B)) {
          _arg.arg.finish = true;
          close(_arg.sockfd);
          task.Wait();
          updateTask.Wait();
          successDownload = false;
          break;
        }
      }
      _arg.arg.finish = true;
      close(_arg.sockfd);
      task.Wait();
      updateTask.Wait();
      if (task.context.get()->result != 0) {
        MessageBox("failed download")();
        break;
      }
      successDownload = true;
      break;
  }
  arg.finish = true;
  if (successDownload) {
    if (MessageBox("download success\ndo you want update?", DialogType::DialogYesNo)()) {
      if (File::Remove("ideaCTRPF.3gx") != File::OPResult::SUCCESS)
        MessageBox("failed delete file")();
      if (File::Rename("buff.3gx", "ideaCTRPF.3gx") != File::OPResult::SUCCESS)
        MessageBox("failed rename file")();
      if (MessageBox("success\ndo you want reload?", DialogType::DialogYesNo)())
        Process::ReturnToHomeMenu();
    }
  } else if (File::Remove("buff.3gx") != File::OPResult::SUCCESS)
    MessageBox("failed delete buffer")();
  task.Wait();
  Sleep(Seconds(1));
}

void ParrotingBack(MenuEntry *entry) {
  std::vector<std::string> console = {};
  bool init = true;

  u32 micBuffer_pos = 0;

  Result ret = RL_SUCCESS;
  if (R_FAILED(ncsndInit(false))) {
    console.push_back("Could not initialize sound.\n");
    init = false;
  }
  if (!micBuffer)
    ret = svcControlMemoryUnsafe((u32 *)&micBuffer, MIC_BUFFER_ADDR, MIC_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
  if (R_FAILED(ret)) {
    console.push_back("didn't allocate memory for MIC buffer.\n");
    init = false;
  } else if (R_FAILED(micInit(micBuffer, MIC_BUFFER_SIZE))) {
    console.push_back("Could not initialize MIC.\n");
    init = false;
  }
  u32 micBuffer_datasize = micGetSampleDataSize();
  u32 audioBuffer_pos = 0;
  if (!soundBuffer)
    ret = svcControlMemoryUnsafe((u32 *)&soundBuffer, SOUND_BUFFER_ADDR, SOUND_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
  else
    ret = RL_SUCCESS;
  if (R_FAILED(ret) || !soundBuffer) {
    console.push_back("didn't allocate memory for sound buffer.\n");
    init = false;
  }

  Sleep(Seconds(0.5));

  if (init)
    console.push_back("Press A to start sampling.\n");
  console.push_back("Press B to exit.\n");

  const Screen &screen = OSD::GetTopScreen();

  while (true) {
    Controller::Update();
    if (init && Controller::IsKeyPressed(Key::A)) {
      micBuffer_pos = 0;
      audioBuffer_pos = 0;
      ncsndSetVolume(0x8, 1, 0);
      ncsndSetRate(0x8, 16360, 1);
      if (R_FAILED(MICU_StartSampling(MICU_ENCODING_PCM16_SIGNED, MICU_SAMPLE_RATE_16360, 0, micGetSampleDataSize(), true)))
        console.push_back("Could not start sampling.\n");
      else
        console.push_back("Sampling started.\n");
    }

    if (init && Controller::IsKeyDown(Key::A) && audioBuffer_pos < SOUND_BUFFER_SIZE) {
      u32 micBuffer_readpos = micBuffer_pos;
      micBuffer_pos = micGetLastSampleOffset();
      while (audioBuffer_pos < SOUND_BUFFER_SIZE && micBuffer_readpos != micBuffer_pos) {
        soundBuffer[audioBuffer_pos++] = micBuffer[micBuffer_readpos];
        micBuffer_readpos = (micBuffer_readpos + 1) % MIC_BUFFER_SIZE;
      }
    }

    if (init && Controller::IsKeyReleased(Key::A)) {
      if (R_FAILED(MICU_StopSampling()))
        console.push_back("Could not stop sampling.\n");
      else
        console.push_back("Sampling stopped.\n");
      svcFlushProcessDataCache(CUR_PROCESS_HANDLE, (u32)soundBuffer, audioBuffer_pos);
      console.push_back(Utils::Format("sound size: %d\n", audioBuffer_pos));
      ncsndSound sound;
      ncsndInitializeSound(&sound);
      sound.isPhysAddr = true;
      sound.sampleData = (u8 *)svcConvertVAToPA((const void *)soundBuffer, false);
      sound.totalSizeBytes = audioBuffer_pos;
      sound.encoding = NCSND_ENCODING_PCM16;
      sound.sampleRate = 16360;
      sound.pan = 0.0;
      sound.volume = 1.0;
      if (R_FAILED(ncsndPlaySound(0x8, &sound)))
        console.push_back("Could not play sound.\n");
      else
        console.push_back("Playing sound...\n");
    }

    if (Controller::IsKeyPressed(Key::B))
      break;

    while (11 < console.size())
      console.erase(console.begin());

    screen.DrawRect(30, 20, 340, 200, Color::Black);
    screen.DrawRect(32, 22, 336, 196, Color::White, false);
    for (const auto &log : console)
      screen.DrawSysfont(log, 35, 22 + (&log - &console[0]) * 18);

    OSD::SwapBuffers();
  }
}
}  // namespace CTRPluginFramework