#include <3ds.h>
#include "csvc.h"
#include <CTRPluginFramework.hpp>
#include "cheats.hpp"
#include "JPKeyboard.hpp"
#include "KaniCodes.hpp"
#include <ctime>

namespace CTRPluginFramework
{
  static MenuEntry *TopSeparator(MenuEntry *entry)
  {
    entry->UseTopSeparator();
    return entry;
  }

  static MenuEntry *BottomSeparator(MenuEntry *entry)
  {
    entry->UseBottomSeparator();
    return entry;
  }

  // HotKeyの部分
  MenuEntry *EntryWithHotkeys(MenuEntry *entry, const std::vector<Hotkey> &hotkeys, bool addName = false)
  {
    if (entry != nullptr)
    {
      entry->SetArg(new std::string(entry->Name()));
      for (const Hotkey &hotkey : hotkeys)
      {
        entry->Hotkeys += hotkey;
        if (addName)
          entry->Name() += " " + hotkey.ToString(true);
      }
      if (addName)
        entry->Hotkeys.OnHotkeyChangeCallback([](MenuEntry *entry, int index)
                                              {
                                              std::string *name = reinterpret_cast<std::string *>(entry->GetArg()); 
                                              entry->Name() = *name;                                            for (size_t i = 0; i < entry->Hotkeys.Count(); i++)
                                                entry->Name() += " " + entry->Hotkeys[i].ToString(true); });
    }
    return (entry);
  }

  MenuEntry *EnableEntry(MenuEntry *entry)
  {
    if (entry != nullptr)
    {
      entry->SetArg(new std::string(entry->Name()));
      entry->Enable();
    }
    return (entry);
  }

  void StoreBatteryPercentage(float &percentage)
  {
    u8 data[4];
    mcuHwcInit();
    MCUHWC_ReadRegister(0xA, data, 4);
    percentage = data[1] + data[2] / 256.0f;
    percentage = (u32)((percentage + 0.05f) * 10.0f) / 10.0f;
    mcuHwcExit();
  }

  std::string SystemInfomation()
  {
    char date[6400];
    std::time_t unix_time = std::time(nullptr);
    struct tm *time_struct = gmtime((const time_t *)&unix_time);
    std::string am_or_pm;
    const std::vector<std::string> kListTimeOfTheWeek{"日", "月", "火", "水", "木", "金", "土"};
    int time_of_the_week = time_struct->tm_wday;
    const int kStartYear = 1900;
    const int kStartMonth = 1;
    int year = time_struct->tm_year + kStartYear;
    int month = time_struct->tm_mon + kStartMonth;
    int day = time_struct->tm_mday;
    int hour_24 = time_struct->tm_hour;
    int hour_12;
    int minute = time_struct->tm_min;
    int second = time_struct->tm_sec;
    if (hour_24 / 12)
    {
      am_or_pm = "午後";
    }
    else
    {
      am_or_pm = "午前";
    }
    if (hour_24 % 12)
    {
      hour_12 = hour_24 % 12;
    }
    else
    {
      hour_12 = 12;
    }
    std::string systeminfo = Utils::Format("%d/%02d/%02d(%s) %s:%02d:%02d:%02d", year, month, day, kListTimeOfTheWeek[time_of_the_week].c_str(), am_or_pm.c_str(), hour_12, minute, second);
    return systeminfo;
  }

  std::string BatteryInfomation()
  {
    float percentage = 0;
    StoreBatteryPercentage(percentage);
    std::string batteryinfo = Utils::Format("%d%%%", (u32)percentage);
    return batteryinfo;
  }

  bool LoadGameTitle(const Screen &scr)
  {
    std::string tid;
    Process::GetTitleID(tid);
    std::string name;
    Process::GetName(name);
    if (scr.IsTop)
    {
      scr.DrawRect(163, 222, 236, 17, Color::Black);
      scr.DrawRect(163, 218, 236, 17, Color::Black);
      scr.DrawRect(165, 220, 232, 17, Color::White, false);
      scr.DrawSysfont(tid + ":" << Color::LimeGreen << "対応ゲーム", 166, 22 * 10, Color::White);
    }
    return true;
  }

  void DrawCallBack(Time time)
  {
    const Screen &scr = OSD::GetTopScreen();
    scr.DrawRect(30, 4, 340, 18, Color::Black, true);
    scr.DrawRect(32, 6, 336, 14, Color::White, false);
    scr.DrawSysfont(SystemInfomation(), 35, 5, Color::White);
    if (System::IsCitra())
      return;
    scr.DrawSysfont(BatteryInfomation(), 321, 5, Color::White);
  }

  bool checkPass(void)
  {
    std::vector<u16> answer = {0x6A, 0x3EC, 0x2175, 0x29FB, 0x2CF, 0x5C, 0xBB};
    std::vector<u16> diff = {0x84, 0x25, 0x266, 0x2999, 0x2114, 0x37A, 0x7};
    u8 answer_length = answer.size();
    u64 hash;
    CFGU_GenHashConsoleUnique(0, &hash);
    hash = (hash % 0x1000) * 2 + (hash >> 0x36);

    File::Create("pass.bin");
    File file("pass.bin");
    for (int j = 0; j < answer_length; j++)
    {
      u16 buff;
      file.Read((void *)&buff, sizeof(u16));
      if (buff != (answer[j] + diff[j] + hash))
      {
        u16 utf16[answer_length];
        std::string output;
        Keyboard key("");
        key.CanAbort(false);
        key.Open(output);
        Process::WriteString((u32)&utf16, output.substr(0, answer_length), StringFormat::Utf16);
        for (int i = 0; i < answer_length; i++)
          utf16[i] += diff[answer_length - i - 1];
        Sleep(Seconds(1));

        for (int i = 0; i < answer_length; i++)
        {
          if (utf16[i] != answer[i])
          {
            MessageBox("invalid")();
            file.Close();
            return false;
          }
        }
        file.Rewind();
        for (int i = 0; i < answer_length; i++)
        {
          buff = answer[i] + diff[i] + hash;
          file.Write((void *)&buff, sizeof(u16));
        }
        file.Close();

        return true;
      }
    }
    file.Close();
    return true;
  }

  // This patch the NFC disabling the touchscreen when scanning an amiibo, which prevents ctrpf to be used
  static void ToggleTouchscreenForceOn(void)
  {
    static u32 original = 0;
    static u32 *patchAddress = nullptr;

    if (patchAddress && original)
    {
      *patchAddress = original;
      return;
    }

    static const std::vector<u32> pattern =
        {
            0xE59F10C0, 0xE5840004, 0xE5841000, 0xE5DD0000,
            0xE5C40008, 0xE28DD03C, 0xE8BD80F0, 0xE5D51001,
            0xE1D400D4, 0xE3510003, 0x159F0034, 0x1A000003};

    Result res;
    Handle processHandle;
    s64 textTotalSize = 0;
    s64 startAddress = 0;
    u32 *found;

    if (R_FAILED(svcOpenProcess(&processHandle, 16)))
      return;

    svcGetProcessInfo(&textTotalSize, processHandle, 0x10002);
    svcGetProcessInfo(&startAddress, processHandle, 0x10005);
    if (R_FAILED(svcMapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, processHandle, (u32)startAddress, textTotalSize)))
      goto exit;

    found = (u32 *)Utils::Search<u32>(0x14000000, (u32)textTotalSize, pattern);

    if (found != nullptr)
    {
      original = found[13];
      patchAddress = (u32 *)PA_FROM_VA((found + 13));
      found[13] = 0xE1A00000;
    }

    svcUnmapProcessMemoryEx(CUR_PROCESS_HANDLE, 0x14000000, textTotalSize);
  exit:
    svcCloseHandle(processHandle);
  }

  // This function is called before main and before the game starts
  // Useful to do code edits safely
  void PatchProcess(FwkSettings &settings)
  {
    ToggleTouchscreenForceOn();
  }

  u32 *socBuffer = nullptr;
  constexpr u32 SOC_BUFFER_ADDR = 0x7500000;
  constexpr u32 SOC_BUFFER_SIZE = 0x100000;

  u8 *soundBuffer = nullptr;
  u8 *micBuffer = nullptr;
  // This function is called when the process exits
  // Useful to save settings, undo patchs or clean up things
  void EventCallback(Process::Event event)
  {
    if (event == Process::Event::EXIT)
    {
      ToggleTouchscreenForceOn();
      socExit();
      if (System::IsCitra())
      {
        free(socBuffer);
      }
      else
      {
        MemInfo info;
        PageInfo out;
        svcQueryMemory(&info, &out, SOC_BUFFER_ADDR);
        if (info.state != MemState::MEMSTATE_FREE)
          svcControlMemoryUnsafe(nullptr, SOC_BUFFER_ADDR, SOC_BUFFER_SIZE, MEMOP_FREE, MemPerm(0));
        svcQueryMemory(&info, &out, SOUND_BUFFER_ADDR);
        if (info.state != MemState::MEMSTATE_FREE)
          svcControlMemoryUnsafe(nullptr, SOUND_BUFFER_ADDR, SOUND_BUFFER_SIZE, MEMOP_FREE, MemPerm(0));
        svcQueryMemory(&info, &out, MIC_BUFFER_ADDR);
        if (info.state != MemState::MEMSTATE_FREE)
          svcControlMemoryUnsafe(nullptr, MIC_BUFFER_ADDR, MIC_BUFFER_SIZE, MEMOP_FREE, MemPerm(0));
      }
    }
  }

  void InitBuffers(void)
  {
    Result ret = RL_SUCCESS;
    if (System::IsCitra())
    {
      socBuffer = (u32 *)aligned_alloc(0x1000, SOC_BUFFER_SIZE);
      if (socBuffer)
        ret = RL_SUCCESS;
    }
    else
      ret = svcControlMemoryUnsafe((u32 *)&socBuffer, SOC_BUFFER_ADDR, SOC_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
    if (R_FAILED(ret) || !socBuffer)
      OSD::Notify("alloc socBuffer failed");
    else
    {
      ret = socInit(socBuffer, SOC_BUFFER_SIZE);
      if (R_FAILED(ret))
      {
        OSD::Notify(Utils::Format("socInit: 0x%lX failed", ret));
        socExit();
      }
      else
        OSD::Notify("socInit success");
    }

    ret = svcControlMemoryUnsafe((u32 *)&soundBuffer, SOUND_BUFFER_ADDR, SOUND_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
    if (R_FAILED(ret) || !soundBuffer)
      OSD::Notify("alloc soundBuffer failed");
    else
      OSD::Notify("alloc soundBuffer success");

    ret = svcControlMemoryUnsafe((u32 *)&micBuffer, MIC_BUFFER_ADDR, MIC_BUFFER_SIZE, MemOp(MEMOP_REGION_SYSTEM | MEMOP_ALLOC), MemPerm(MEMPERM_READ | MEMPERM_WRITE));
    if (R_FAILED(ret) || !micBuffer)
      OSD::Notify("alloc micBuffer failed");
    else
      OSD::Notify("alloc micBuffer success");
  }

  void InitMenu(PluginMenu &menu)
  {
    MenuFolder *searchFolder = new MenuFolder("Search");
    *searchFolder += new MenuEntry("Search", nullptr, Search);
    menu += searchFolder;
    MenuFolder *gameFolder = new MenuFolder("Games");
    *gameFolder += EntryWithHotkeys(new MenuEntry("Tetris", nullptr, Tetris, "テトリスゲーム\n一回Hotkeys Modifierを見に行ってください\n操作できません\nMenuFunc(キーボードのボタン)から設定を変えれます"), {Hotkey(Key::DPadLeft, "左"), Hotkey(Key::DPadRight, "右"), Hotkey(Key::DPadDown, "下"), Hotkey(Key::DPadUp, "ハードドロップ"), Hotkey(Key::A, "右回転"), Hotkey(Key::B, "左回転"), Hotkey(Key::R, "ホールド"), Hotkey(Key::Y, "Pause")}, true);
    *gameFolder += new MenuEntry("LifeGame", nullptr, LifeGame, "ライフゲーム\nA:put life\nB:exit\nX:start/stop\nY:menu");
    *gameFolder += new MenuEntry("2048", nullptr, Game2048, "2048ゲーム\nB:exit\nX:reset");
    *gameFolder += new MenuEntry("3D", nullptr, threeD, "threeD3\nB:exit");
    menu += gameFolder;
    menu += new MenuEntry("Test1", nullptr, Test1);
    menu += new MenuEntry("command", nullptr, Command, "コマンド");
    menu += new MenuEntry("pipes", Pipes, "パイプス\nstartで消せます");
    menu += new MenuEntry("Cube", nullptr, Cube, "キューブ\nCPadとCStickで回転,拡大,縮小できるよ\nBで終了");
    menu += new MenuEntry("Bad Apple!!", BadApple, "バッドアップル!!");
    menu += new MenuEntry("JPNotify", JPNotify, "ジェーピーノティファイ\nstartで表示\n(Y押しながら押すんじゃないぞ！)");
    menu += new MenuEntry("PlayMusic", nullptr, PlayMusic, "プレイミュージック\nMUSICフォルダにbcwavを入れてください");
    menu += new MenuEntry("ColorPicker", nullptr, ColorPicker, "カラーピッカー");
    menu += new MenuEntry("Paint", nullptr, MenuEntryPaint, "ペイント\nXで色変更,Yでモード変更");
    menu += new MenuEntry("LED", LedEffect, FONT_DL ":赤\n" FONT_DD ":緑\n" FONT_DR ":青\n" FONT_DU ":白\n" FONT_Y "を押しながら各種キー:滑らかに点灯");
    menu += new MenuEntry("Projection", nullptr, Projection, "プロジェクション\nmade by maru");
    menu += new MenuEntry(Color::Yellow << "Patch Code Automation!", nullptr, autoPatchCode, "パッチコードオートメーション!\n開始アドレスと終了アドレスを設定してください。\n開始アドレスと終了アドレスが同じ場合は、アドレスの初期化をしてください。\n\n次に [ファイルに書き込み] を押下してください。\n\n/luma/plugins/" + getFilePath() + " にコードが作成されます。\n\n" + (Color::Red << "※注意\n") + "CTRPF上でコードが作成されたファイルを開くと、\n新しくコードが作成できません。その場合はゲームを再起動してからコードを作成してください。\n\n" + (Color::White << "Enjoy coding!\nmade by xv"));
    menu += new MenuEntry("Discord", nullptr, SendDiscord, "ディスコード\nDiscordにメッセージを送信");
    menu += new MenuEntry("Update 3gx", nullptr, Update3gx, "アップデート\n3gxを更新する");
    menu += new MenuEntry("Parroting Back", nullptr, ParrotingBack, "パロットバック\nマイクで録音した音声をオウム返しする\nA:録音\nB:終了");
  }

  int main(void)
  {
    PluginMenu *menu = new PluginMenu("IDEA CTRPF", 1, 1, 2, "made by salmarine469\n協力者(敬称略)\n・Tekito_256\n・hide0123\n・HidegonSan\n・bomkei\n・maru(kwkts)\n・xv\nありがとうございます！！！\n変な機能ばっかりの自己満ロマンCTRPFです");

    // OSD::Run(LoadGameTitle);
    // Sleep(Seconds(1));
    // OSD::Stop(LoadGameTitle);

    // if (!checkPass())
    //   return (0);
    OSD::Notify("verified");
    JPKeyboard::LoadKanjiList();

    menu->OnNewFrame = DrawCallBack;
    menu->SynchronizeWithFrame(true);
    menu->ShowWelcomeMessage(false);

    EventCallback(Process::Event::EXIT);
    InitBuffers();

    // Init our menu entries & folders
    InitMenu(*menu);
    Process::SetProcessEventCallback(EventCallback);

    // Launch menu and mainloop
    menu->Run();
    delete menu;

    // Exit plugin
    return (0);
  }
}
