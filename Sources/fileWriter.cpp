#include "cheats.hpp"

namespace CTRPluginFramework
{
  std::string getFilePath()
  {
    std::string tid;
    Process::GetTitleID(tid);
    std::string filePath = tid + ".txt";

    return filePath;
  }

  bool fileWriter(std::string &codeName, u32 &startAddress, u32 &endAddress)
  {
    std::vector<u32> readCode;
    std::string path = getFilePath();
    bool canFileWrite = false;

    File file(path, File::Mode::RW);
    if (!file.Exists(path))
    {
      file.Create(path);
    }
    LineWriter writer(file);

    if (file.IsOpen())
    {
      codeName.insert(0, "[");
      codeName.append("]\n");

      if ((startAddress != 0) && (endAddress != 0))
      {
        for (u32 loopAddress = startAddress; loopAddress <= endAddress; loopAddress += 4)
        {
          u32 writeCodes;
          Process::Read32(loopAddress, writeCodes);
          if (writeCodes != 0x00000000)
            readCode.push_back(writeCodes);
        }

        std::string formatString = Utils::Format("E%07X %08X\n", startAddress, readCode.size() * 4);
        for (size_t i = 0; i < readCode.size(); i++)
        {
          if (i % 2 == 0 && i != 0)
            formatString += "\n";
          formatString += Utils::Format("%08X ", readCode[i]);
        }

        codeName += formatString;
      }

      file.Seek(0, file.END);
      writer << codeName << writer.endl();
      writer.Flush();
      canFileWrite = true;
    }
    if (!file.IsOpen())
    {
      return canFileWrite = false;
    }
    file.Close();

    startAddress = 0;
    endAddress = 0;

    return canFileWrite;
  }

  bool fileDelete(const std::string &filePath)
  {
    if ((!File::Exists(filePath)))
    {
      return false;
    }

    int result = File::Remove(filePath);
    if (result == 0)
    {
      return true;
    }

    return false;
  }

  void autoPatchCode(MenuEntry *entry)
  {
    std::string codeName;
    static u32 startAddress;
    static u32 endAddress;
    static std::vector<std::string> kInputList{"開始アドレスの設定", "終了アドレスの設定", "設定したアドレスの確認", "ファイルに書き込み", "設定したアドレスの初期化", (Color(255, 45, 45) << "ファイルを削除")};

    Keyboard k(kInputList);
    k.IsHexadecimal(true);

    const int kChoice = k.Open();
    switch (kChoice)
    {
    case 0:
      if (k.Open(startAddress) != 0)
        return;

      break;

    case 1:
      if (k.Open(endAddress) != 0)
        return;
      break;

    case 2:
      MessageBox("開始アドレス: 0x" + Utils::ToHex(startAddress) + "\n終了アドレス: 0x" + Utils::ToHex(endAddress) + "\n", DialogType::DialogOk, ClearScreen::Both)();
      break;

    case 3:
      if ((startAddress == 0) && (endAddress == 0))
      {
        MessageBox(Color::Red << "エラー", "開始アドレスと終了アドレスが設定されていません。", DialogType::DialogOk, ClearScreen::Both)();
        return;
      }

      if ((startAddress > 0) && (endAddress > 0))
      {
        if (startAddress > endAddress)
        {
          MessageBox(Color::Red << "エラー", "開始アドレスが終了アドレスよりも大きいため\n処理を中断しました。", DialogType::DialogOk, ClearScreen::Both)();
          startAddress = 0;
          endAddress = 0;
          return;
        }

        if (endAddress - startAddress >= 1000)
        {
          MessageBox(Color::Red << "エラー", "アドレスの差分が大きいため処理を中断しました。\n0x1000以下で設定してください。", DialogType::DialogOk, ClearScreen::Both)();
          startAddress = 0;
          endAddress = 0;
          return;
        }
      }
      else if ((startAddress > 0) != (endAddress > 0))
      {
        if ((startAddress != 0) && (endAddress == 0))
        {
          MessageBox(Color::Red << "エラー", "終了アドレスが設定されていません。", DialogType::DialogOk, ClearScreen::Both)();
          return;
        }

        if ((startAddress == 0) && (endAddress != 0))
        {
          MessageBox(Color::Red << "エラー", "開始アドレスが設定されていません。", DialogType::DialogOk, ClearScreen::Both)();
          return;
        }
      }

      if ((startAddress != 0) && (endAddress != 0))
      {
        if ((k.Open(codeName) != 0))
          return;

        if (codeName == "")
        {
          MessageBox(Color::Red << "エラー", "コード名が入力してください。", DialogType::DialogOk, ClearScreen::Both)();
          return;
        }

        if (codeName != "")
          if (fileWriter(codeName, startAddress, endAddress))
          {
            MessageBox((Color::LimeGreen << "成功"), "コードは /luma/plugins/" + getFilePath() + " に作成されました。\n\n" + (Color::Red << "※コードが存在しない場合があります。詳しくは Note を参照してください。"), DialogType::DialogOk)();
            return;
          }
      }

      break;

    case 4:
      if ((startAddress != 0) || (endAddress != 0))
      {
        if (MessageBox("初期化します。", DialogType::DialogOkCancel)())
        {
          startAddress = 0;
          endAddress = 0;
          return;
        }
      }
      else if ((startAddress == 0) && (endAddress == 0))
      {
        MessageBox(Color::Red << "エラー", "開始アドレスと終了アドレスが設定されていません。", DialogType::DialogOk, ClearScreen::Both)();
        return;
      }
      break;

    case 5:
      if (MessageBox((Color::Red << "ファイルを削除します\n\n") + (Color::White << "本当に削除しますか？"), DialogType::DialogYesNo)())
      {
        if (fileDelete(getFilePath()))
        {
          MessageBox((Color::LimeGreen << "成功"), "完了しました。", DialogType::DialogOk)();
          return;
        }
        else
        {
          MessageBox(Color::Red << "エラー", "ファイルが存在しないため削除できませんでした。", DialogType::DialogOk, ClearScreen::Both)();
          return;
        }
      }
      break;

    default:
      return;
    }
  }

  // main.cpp
  /*
  menu += new MenuEntry(Color::Yellow << "Patch Code Automation!", nullptr, autoPatchCode, "開始アドレスと終了アドレスを設定してください。\n開始アドレスと終了アドレスが同じ場合は、アドレスの初期化をしてください。\n\n次に [ファイルに書き込み] を押下してください。\n\n/lumna/plugins/" + getFilePath() + " にコードが作成されます。\n\n" + (Color::Red << "※注意\n") + "CTRPF上でコードが作成されたファイルを開くと、\n新しくコードが作成できません。その場合はゲームを再起動してからコードを作成してください。\n\n" + (Color::White << "Enjoy coding!"));
  */
}