/* ==================================================================================================== */
/*
 *  UDS.h
 *  Unified Diagnostics Services (UDS)
 *    ISO: 14229 Part 1   - Diagonostics Services
 *    ISO: 14229 Part 2   - Timing
 *    ISO: 14229 Part 3   - CAN
 *  Version: v2.7:0
 *  Developed By: Alakshendra Singh
 *  For Reporting Any Issue Don't Contact Me. Fix Yourself
*/
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @file        UDS.h
 * @author      Alakshendra Singh
 * @brief       Unified Diagnostics Services on CAN (UDS)
 * @version     1.3
 *
 * @copyright   Copyright (c) 2025
 */
/* ==================================================================================================== */

#ifndef _UDS
#define _UDS

#include <iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <conio.h>
#include <string>
#include <vector>
#include <deque>
#include "DoCAN.hpp"
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

#define UDS_ERR_ALLOK                     0
#define UDS_ERR_INTERNALISSUE             1


/* ==================================================================================================== */
/**
 * @class       ISO_UDS
 * @brief       UDS Driver According To ISO 14229-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_UDS {

  public : // INFO: Public Members
    ISO_DoCAN DoCAN;

  protected : // INFO: Protected Members
    struct UDSTIME { int YY; int MM; int DD; int HH; int mm; int SS; }; UDSTIME TIME;
    ofstream LogFile;

    enum CONSOLEKEYS {
      CONARROWPREFIX1 = 0, CONARROWPREFIX2 = 224,
      CONARROWUP = 72, CONARROWDOWN = 80, CONARROWLEFT = 75, CONARROWRIGHT = 77,
      CONENTER = 13, CONBACKSPACE = 8, CONESCAPE = 27,
    };
    enum CONSOLECOLOURS {
      CONSOLECOLOUR_BLACK = 0, CONSOLECOLOUR_BLUE = 1, CONSOLECOLOUR_GREEN = 2,
      CONSOLECOLOUR_AQUA = 3, CONSOLECOLOUR_RED = 4, CONSOLECOLOUR_PURPLE = 5,
      CONSOLECOLOUR_YELLOW = 6, CONSOLECOLOUR_WHITE = 7, CONSOLECOLOUR_GRAY = 8,
      CONSOLECOLOUR_LIGHTBLUE = 9, CONSOLECOLOUR_LIGHTGREEN = 10, CONSOLECOLOUR_LIGHTAQUA = 11,
      CONSOLECOLOUR_LIGHTRED = 12, CONSOLECOLOUR_LIGHTPURPLE = 13, CONSOLECOLOUR_LIGHTYELLOW = 14,
      CONSOLECOLOUR_BRIGHTWHITE = 15,
    };
    enum CONFIGATIONS {
      ARROWDOWN = 0, ARROWUP = 1, ARROWCALIBRATE = 2,
      NORMALDASH = 0, PLAYLISTDASH = 1,
      LISTSERVICE = 0, RAWSERVICE = 1,
    };
    enum FLASHERROR {
      FILENOTFOUND = 1, DLLNOTWORKING = 2, HEXFILENOTFOUND = 3, HEXFILEISSUE = 4, WRONGCONFIG = 5, FORMATWRONG = 6,
      ALLOK = 0,
    };

    typedef int (__stdcall *GenerateKeyExFunc)(
        const unsigned char* ipSeedArray,
        unsigned int iSeedArraySize,
        const unsigned int iSecurityLevel,
        const char* ipVariant,
        unsigned char* iopKeyArray,
        unsigned int iMaxKeyArraySize,
        unsigned int& oActualKeyArraySize
    );

    struct UDSFLASHTREE {
      std::string SERVICE; std::string TYPE;
      uint64_t WAIT;
      uint32_t EFFECTIVELENGTH;
      json FLASHJSON;
      uint8_t STARTPERCENT; uint8_t ENDPERCENT;
    };
    std::vector<UDSFLASHTREE> FLASHTREES;

    struct UDSFLASHREQS {
      std::vector<std::string> REQ;
      std::vector<std::string> RES;
      std::vector<std::string> RESMASK;
    };
    struct UDSFLASHSERVICE {
      std::string TYPE;
      std::string SERVICE;
      std::vector<UDSFLASHREQS> REQS;
    };
    std::vector<UDSFLASHSERVICE> FLASHBLOCKS;

    struct UDSAECURITYACCESS {
      unsigned int LEVEL;
      unsigned int LENGTH;
      unsigned char DATA[16];
    }; UDSAECURITYACCESS SECURE;

    struct UDSFUNCTION { std::string FUNCTION; std::string REQUEST; };
    struct UDSSERVICES {
      std::string SERVICE;
      std::string SID;
      std::vector<UDSFUNCTION> FUNCTIONS;
    };
    std::vector<UDSSERVICES> SERVICEDIRECTORY;

    struct UDSPLAYLIST {
      std::string FUNCTION;
      std::string REQUEST;
      std::string RESPONSE;
      std::string ADDRESS; std::string SUPPRESS;
    };
    std::vector<UDSPLAYLIST> PLAYLIST;

    struct UDSFLASHMEMORYINFO {
      uint32_t ADDRESSSTART; uint32_t ADDRESSEND; uint32_t SIZE; uint32_t CRC;
    };

    struct UDSREQRES {
      uint8_t LEN;
      std::vector<uint8_t> DATA;
    };

    struct UDSCONFIGURATION {
      uint32_t REQID; uint32_t RESID; uint32_t FUNID;
      uint8_t PADDING; uint8_t BLOCKS; uint8_t STMIN;
      uint64_t P2STAR; uint64_t P2;
      uint16_t FLASHMAGINFIER;
      uint8_t ERRORCODE;
      std::string DASHBOARDPATH;
      std::string CONFIGPATH;
      std::string FLASHJSONFOLDER;
      std::string LOGPATH;
      std::string DLLPATH;
      uint8_t LOGGINGMODE;
    }; UDSCONFIGURATION CONFIG;

    struct UDSFLAGS {
      int INIT_PAGE; int DASHBOARD; int FLASH; int FAULT; int EXIT;
      int CONSOLE; int DRIVER; int LOG;
      int CONFIGURATION;
      int MIDADD; int MIDINDEX;
      int PLAYOUT; int PLAYLIST;
      int FLASHOUT;
    }; UDSFLAGS FLAGS;

    struct UDSLIST { int TOTAL; int PAGE; int START; int END; int SELECTED; };
    struct UDSUI {
      int WINDOWHEIGHT; int WINDOWWIDTH; int LOGO_X; int LOGO_Y;
      UDSLIST INIT; UDSLIST DASH; UDSLIST PLAY;
      int REQRESWIDHTH; int REQHEIGHT; int RESHEIGHT;
    }; UDSUI UI;

  private : // INFO: Private Members

    void RenderHeader (void);
    void RenderLOGO (int X, int Y);
    void RenderSHORTLOGO (int X, int Y);

    void ConsoleColor (CONSOLECOLOURS Colour = CONSOLECOLOUR_WHITE, CONSOLECOLOURS Background = CONSOLECOLOUR_BLACK);
    void ConsoleCursor (uint8_t Show = 0);
    void ClearScreen (void);

    uint8_t ValidateDashboard (void);
    void ShowDashboard (void);
    uint8_t ReadDashboard (void);
    uint8_t ReadConfiguration (void);

    UDSLIST ListCalculatePosition (UDSLIST LUI, CONFIGATIONS Direction);
    UDSLIST CalculatePosition(UDSLIST LUI, CONFIGATIONS Direction);

    std::string ErrorCodeDoCAN (void);
    std::string NegativeResponse (uint8_t NR);

    uint8_t ValidRequestString (std::string& Input);
    std::string HEXToString (uint8_t Data);
    uint8_t StringToHEX (uint8_t &Number ,const std::string& NumberSTR);

    void RawServiceBuilder (const std::string& SID, const std::string& SERVICE, CONFIGATIONS Add, uint8_t Issue = 0);
    void LoggingMode (uint8_t Mode);

    uint8_t Executor (uint8_t Suppress, std::string& Line);

    uint8_t FileHEX_Validate (const std::string& Filename);
    uint16_t FileHEX_CRC16 (const std::string& Filename, uint16_t Polynomial, uint16_t InitialValue,
        uint16_t FinalValue, uint8_t InputConvert = 0, uint8_t OutputConvert = 0);
    uint8_t FileHEX_MemoryInfo (UDSFLASHMEMORYINFO &INFO, const std::string& Filename, uint16_t Polynomial,
        uint16_t InitialValue, uint16_t FinalValue, uint8_t InputConvert, uint8_t OutputConvert);
    void FileHEX_LoadData (const std::string& Filename, std::vector<std::string>& Data);

    uint8_t ValidateSecurityDll(const std::string& DLLPath);
    uint8_t SecurityDLL (const std::string& DLLPath, UDSAECURITYACCESS& SEED, UDSAECURITYACCESS& KEY);
    void ExecuteSecurityAccess (const std::string& DLLPath, const std::string& SERVICE, uint8_t Type,
        uint8_t Level, uint8_t Add);

    void RenderDebugVariableList (UDSLIST LUI, int STR, int END, int RAN);
    void RenderSelector (uint8_t Selected, CONSOLECOLOURS Colour, CONSOLECOLOURS Background = CONSOLECOLOUR_BLACK);
    void RequestRender (UDSREQRES REQS, uint8_t Offset, CONSOLECOLOURS Colour = CONSOLECOLOUR_WHITE);
    void ResponseRender (UDSREQRES REQS, UDSLIST RUI, uint8_t Offset, CONSOLECOLOURS Colour = CONSOLECOLOUR_WHITE);

    void RenderSettings (void);
    void RenderWorkInPorgress (void);
    void RenderExitPage (void);

    void HandlerViewTraffic (void);
    void RenderViewTraffic (const std::deque<std::string>& Data, UDSLIST TRAF, uint32_t Tester);
    void HandlerTrace (void);
    void RenderTrace (const std::deque<std::string>& Data, UDSLIST TRAF);

    void HandlerPlaylist (void);
    void RenderPlaylistPage (void);
    std::string PlayListStrings (const std::string& Input, size_t Length);
    void SaveToPlaylist (const std::string& REQUEST, const std::string& SERVICE, uint8_t Issue = 0);

    void HandlerDiagnosticsPage (CONFIGATIONS Add);
    void RenderDiagnosticsPage (CONFIGATIONS Add);
    void HandlerDiagnosticsServicePage (uint8_t Service, CONFIGATIONS Add);
    void RenderDiagnosticsServicePage (UDSLIST DIAG, uint8_t SID, uint8_t RequestSize);
    void HandlerSecurityAccess (CONFIGATIONS Add);
    void RenderSecurityAccess (UDSLIST SEC);
    void ServiceExecuter (const std::string& REQUEST, const std::string& SERVICE, uint8_t Add);

    void HandlerInitialPage (void);
    void RenderInitialPage (void);

    void HandlerFlashList (void);
    void RenderFlashList (std::vector<std::string>& Lists, UDSLIST FUI);
    void HandlerFlashJSONCheck (const std::string& Filename);
    void RenderFlashJSONCheck (std::vector<std::string>& Lines, std::string& ErrorReason, int Result, int Option,
    UDSLIST FUI);
    void HandlerFlashInspect (const std::string& Filename);
    void RenderFlashInspect (const json& JSON, size_t Index, int Option);
    uint8_t ValidateFlashJSON (const std::string& Filename, std::vector<std::string>& Lines,
        std::string& ErrorReason);
    uint8_t ValidateFlashREQS (std::string STR, uint8_t Type, uint8_t Special);




  public : // INFO: Public Members

    struct UDSMESSAGE {
      uint16_t LEN; uint32_t ID; uint8_t DATA[4095];
    }; UDSMESSAGE MESSAGE;

    ISO_UDS (void) {
      FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
      FLAGS.CONSOLE = 0; FLAGS.DRIVER = 0; FLAGS.LOG = 0;
      CONFIG.REQID = 0x785; CONFIG.RESID = 0x78D; CONFIG.FUNID = 0x7DF;
      CONFIG.DASHBOARDPATH = "config\\dashboard.json";
      CONFIG.CONFIGPATH = "config\\config.json";
      CONFIG.FLASHJSONFOLDER = "flash";
      CONFIG.DLLPATH = "dll\\NewPROGDLL.dll";
      CONFIG.LOGPATH = "logs";
      CONFIG.ERRORCODE = UDS_ERR_ALLOK;
      UI.WINDOWHEIGHT = 30; UI.WINDOWWIDTH = 125;
      UI.LOGO_X = UI.WINDOWWIDTH - 15; UI.LOGO_Y = 1;
      UI.INIT.TOTAL = 5; UI.INIT.PAGE = 6; UI.INIT.END = 5; UI.INIT.START = 0; UI.INIT.SELECTED = 0;
      UI.DASH.TOTAL = 0; UI.DASH.PAGE = 12; UI.DASH.END = 0; UI.DASH.START = 0; UI.DASH.SELECTED = 0;
      UI.PLAY.TOTAL = 0; UI.PLAY.PAGE = 12; UI.PLAY.END = 0; UI.PLAY.START = 0; UI.PLAY.SELECTED = 0;
      UI.PLAY.TOTAL = PLAYLIST.size(); UI.PLAY.PAGE = 10; UI.PLAY.END = 0; UI.PLAY.START = 0;
      UI.PLAY.SELECTED = 0;
      FLAGS.MIDADD = 0; FLAGS.MIDINDEX = 0;
      FLAGS.PLAYOUT = 0; FLAGS.FLASHOUT = 0;
      UI.REQRESWIDHTH = 30; UI.REQHEIGHT = 2; UI.RESHEIGHT = 5;
    }
    ~ISO_UDS (void) {
      Stop();
    }

    void Start (void);
    void Stop (void);
    void Kill (void);

    void Application (void);
    uint8_t GetERROR (void);
    void Load (uint16_t Index, uint8_t Data) { if (Index < 4095) { MESSAGE.DATA[Index] = Data; } }
    uint8_t Get (uint16_t Index) { if (Index < 4095) { return MESSAGE.DATA[Index]; } else { return 0xFF; } }
    uint16_t Length (void) { return MESSAGE.LEN; }

    void ShowFlashTree (std::vector<UDSFLASHTREE>& FLASHTREE);
    void CalculateFlashTree (const std::string& Filename, std::vector<UDSFLASHTREE>& FLASHTREE);
    void FlashECU (const std::string& Filename);

    void RenderFlashPercent (uint8_t Percent);
    uint8_t FlashNOTE (UDSFLASHTREE FLASHTREE);
    uint8_t FlashNORMAL (UDSFLASHTREE FLASHTREE, COORD XY);
    uint8_t FlashSECURITY (UDSFLASHTREE FLASHTREE, COORD XY);
    uint8_t FlashDOWNLOAD (UDSFLASHTREE FLASHTREE, COORD XY);

    uint8_t FlashExecutor (COORD XY, uint8_t RemoveLines, std::vector<std::string>& REQ, std::vector<std::string>& RES,
      std::vector<std::string>& RESMASK, uint8_t Suppress = 0, uint8_t Check = 0, uint8_t Address = 0);
    void FlashDownloadRequestBuilder (UDSFLASHMEMORYINFO MEMORY, uint8_t CRCSSIZE, std::vector <std::string>& MASK,
      std::vector <std::string>& REQ);


// INFO: Class End
};
ISO_UDS UDS;
/* ==================================================================================================== */












// MARK: Flash Initial List
/* ==================================================================================================== */
/**
 * @name        HandlerFlashList
 * @class       ISO_UDS (Private)
 * @brief       Handler To Render the Available Flash Sequence List
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerFlashList (void) {
  std::vector<std::string> FlashJSONs; FLAGS.FLASHOUT = 0;
  std::string SearchPaths = CONFIG.FLASHJSONFOLDER + "\\*.json";
  WIN32_FIND_DATA FindData;
  HANDLE hFIND = FindFirstFile(SearchPaths.c_str(), &FindData);
  if (hFIND != INVALID_HANDLE_VALUE) {
    do {
      std::string FilePATH = CONFIG.FLASHJSONFOLDER + "\\" + FindData.cFileName;
      FlashJSONs.push_back(FilePATH);
    } while (FindNextFile(hFIND, &FindData) != 0);
    FindClose(hFIND);
  }
  UDSLIST FUI; FUI.TOTAL = FlashJSONs.size() + 2; FUI.PAGE = 10; FUI.START = 0; FUI.SELECTED = 0; FUI.END = FUI.TOTAL;
  FUI = CalculatePosition(FUI, ARROWCALIBRATE);
  RenderFlashList(FlashJSONs, FUI);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              FUI = CalculatePosition(FUI, ARROWUP);
              RenderFlashList(FlashJSONs, FUI);
              break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              FUI = CalculatePosition(FUI, ARROWDOWN);
              RenderFlashList(FlashJSONs, FUI);
              break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (FUI.SELECTED < (FUI.TOTAL - 2)) {
          HandlerFlashJSONCheck(FlashJSONs[FUI.SELECTED]);
          if (FLAGS.FLASHOUT) { FLAGS.INIT_PAGE = 1; break; }
          else { RenderFlashList(FlashJSONs, FUI); continue; }
        }
        else if (FUI.SELECTED == (FUI.TOTAL - 2)) {
          FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
          break;
        }
        else if (FUI.SELECTED == (FUI.TOTAL - 1)) { RenderExitPage(); return; } // EXIT
        else {  // RARE
          FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 1; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
          FUI.SELECTED = 0;
          break;
        }
      }
      else if (KEY == CONBACKSPACE) { // BACKSPACE KEY
        FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
        break;
      }
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (1);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderFlashList
 * @class       ISO_UDS (Private)
 * @brief       Render the Available Flash Sequence List
 * @param [Lists]     List of Available Flash JSON Files
 * @param [FUI]       UI List Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderFlashList (std::vector<std::string>& Lists, UDSLIST FUI) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n FLASH ECU :";
  cout << "\n Select Flashing ECU Sequence\n";
  ConsoleColor();
  int RenderRange = min((FUI.TOTAL - 2), (FUI.PAGE));
  int RenderStart, RenderEnd;
  if (FUI.END > (FUI.TOTAL - 2)) { RenderEnd = FUI.TOTAL  - 2; RenderStart = RenderEnd - RenderRange; }
  else { RenderStart = FUI.START; RenderEnd = RenderStart + RenderRange; }
  RenderDebugVariableList (FUI, RenderStart, RenderEnd, RenderRange);
  for (uint16_t I = 0; I < Lists.size(); I++) {
    if ((I >= RenderStart) && (I < RenderEnd)) {
      RenderSelector((FUI.SELECTED == I), CONSOLECOLOUR_LIGHTAQUA);
      cout << Lists[I];
    }
  }
  cout << "\n";
  RenderSelector((FUI.SELECTED == (FUI.TOTAL - 2)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  RenderSelector((FUI.SELECTED == (FUI.TOTAL - 1)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "EXIT";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HandlerFlashJSONCheck
 * @class       ISO_UDS (Private)
 * @brief       Handler of Flash JSON Results
 * @param [Filename]    File Path of JSON Flash Sequence
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerFlashJSONCheck (const std::string& Filename) {
  std::vector<std::string> Lines; std::string ErrorReason;
  int Result = ValidateFlashJSON(Filename, Lines, ErrorReason);
  UDSLIST FUI;
  FUI.TOTAL = Lines.size(); FUI.PAGE = 16; FUI.START = 0; FUI.SELECTED = 0; FUI.END = FUI.TOTAL;
  int Option = 0, OptionList = 0;
  FUI = ListCalculatePosition (FUI, ARROWCALIBRATE);
  if (Result) { OptionList = 2; Option = 0; }
  else { OptionList = 4; Option = 2; }
  RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              FUI = ListCalculatePosition(FUI, ARROWUP);
              RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
              break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              FUI = ListCalculatePosition(FUI, ARROWDOWN);
              RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
              break;
            }
            case CONARROWLEFT : {
              if (Option <= 0) { Option = OptionList - 1; }
              else { Option--; }
              RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
              break;
            }
            case CONARROWRIGHT : {
              Option = (Option + 1) % OptionList;
              RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
              break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (Option == 0) { return; } // BACK
        else if (Option == 1) { RenderExitPage(); return; } // EXIT
        else if (Option == 2) { // PLAY
          FlashECU(Filename);
          return;
        }
        else if (Option == 3) { // INSPECT
          HandlerFlashInspect(Filename);
          RenderFlashJSONCheck (Lines, ErrorReason, Result, Option, FUI);
          continue;
        }
        else { Option = 0; }
      }
      else if (KEY == CONBACKSPACE) { return; } // BACKSPACE KEY
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (1);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderFlashJSONCheck
 * @class       ISO_UDS (Private)
 * @brief       Render the Flash JSON Results
 * @param [Lines]         Validation Result Lines
 * @param [ErrorReason]   Error Reason String
 * @param [Result]        Validation Result with Error Code
 * @param [Option]        Selection Option Number
 * @param [FUI]           UI List Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderFlashJSONCheck (std::vector<std::string>& Lines, std::string& ErrorReason, int Result, int Option,
    UDSLIST FUI) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n FLASH ECU :";
  cout << "\n JSON File Validation\n";
  ConsoleColor();
  RenderDebugVariableList (FUI, FUI.START, FUI.END, FUI.PAGE);
  if (Result) {
    ConsoleColor(CONSOLECOLOUR_LIGHTRED);
    cout << "\n   Flash ECU Sequence Validated Sequence Failed";
    ConsoleColor(); cout << "\n   Failed Code [";
    ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << std::dec << Result;
    ConsoleColor(); cout << "] \n   Failed Reason : ";
    ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << ErrorReason;
    cout << "\n\n\n ";
    ConsoleColor();
    (Option == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "BACK";
    (Option == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "  EXIT";
  }
  else {
    for (int I = FUI.START; I < FUI.END; I++) {
      cout << "\n     " << Lines[I];
    }
    cout << "\n\n ";
    ConsoleColor();
    (Option == 2) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "PLAY";
    (Option == 3) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "  INSPECT";
    (Option == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "  BACK";
    (Option == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "  EXIT";
  }
}
/* ==================================================================================================== */

// MARK: FLash Inspect
/* ==================================================================================================== */
/**
 * @name        HandlerFlashInspect
 * @class       ISO_UDS (Private)
 * @brief       Handler of Flash JSON Sequences Inspect
 * @param [Filename]    File Path of JSON Flash Sequence
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerFlashInspect (const std::string& Filename) {
  ifstream FlashFile(Filename); if (!FlashFile) { return; }
  json JSON; FlashFile >> JSON;
  FlashFile.close();
  int Total = JSON.size();
  int Option = 0, OptionList = 4;
  int Index = 0;
  RenderFlashInspect(JSON[Index], Index, Option);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWLEFT : {
              if (Option <= 0) { Option = OptionList - 1; }
              else { Option--; }
              RenderFlashInspect(JSON[Index], Index, Option);
              break;
            }
            case CONARROWRIGHT : {
              Option = (Option + 1) % OptionList;
              RenderFlashInspect(JSON[Index], Index, Option);
              break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (Option == 0) { // PREVIOUS
          if (Index <= 0) { Index = Total - 1; } else { Index--; }
          RenderFlashInspect(JSON[Index], Index, Option);
          continue;
        }
        else if (Option == 1) { // NEXT
          Index = (Index + 1) % Total;
          RenderFlashInspect(JSON[Index], Index, Option);
          continue;
        }
        else if (Option == 2) { return; } // BACK
        else if (Option == 3) { RenderExitPage(); return; } // EXIT
        else { Option = 0; }
      }
      else if (KEY == CONBACKSPACE) { return; } // BACKSPACE KEY
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (1);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderFlashInspect
 * @class       ISO_UDS (Private)
 * @brief       Render of Flash JSON Sequences Inspect Page
 * @param [JSON]    JSON Array Object
 * @param [Index]   Index of JSON Object
 * @param [Option]  Option on Footer Selected
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderFlashInspect (const json& JSON, size_t Index, int Option) {
  ClearScreen();
  RenderSHORTLOGO (UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n FLASH ECU :";
  cout << "\n JSON File Sequence Inspection\n";
  UDSLIST FUI; FUI.START = 0; FUI.END = 18; FUI.TOTAL = 18; FUI.PAGE = 18; FUI.SELECTED = Option;
  RenderDebugVariableList (FUI, 0, 18, 18);
  ConsoleColor();

  if (JSON["TYPE"] == "NORMAL") {
    cout << "\n   NORMAL [" << std::dec << Index << "]\n";
    cout << "\n     SERVICE : " << JSON["SERVICE"].get<std::string>();
    cout << "\n     WAIT : " << (int)JSON["WAIT"] << " ms";
    cout << "\n     REQUEST :";
    std::istringstream StreamREQ(JSON["REQUEST"].get<std::string>());
    std::istringstream StreamRES(JSON["RESPONSE"].get<std::string>());
    std::string StreamByte;
    while (StreamREQ >> StreamByte) { cout << " " << StreamByte; }
    cout << "\n     ADDRESSING : " << ((JSON["ADDRESS"] == "FUN") ? "FUNCTIONAL" : "PHYSICAL");
    if (JSON["SUPPRESS"] == "Y") {
      cout << "\n     RESPONSE : SUPPRESSED";
    } else {
      cout << "\n     RESPONSE :";
      while (StreamRES >> StreamByte) { cout << " " << StreamByte; }
      if (JSON["CHECK"] == "Y") { cout << "\n     RESPONSE CHECK : WILL BE CHECKED"; }
      else { cout << "\n     RESPONSE CHECK : POSITVE RESPONSE CHECKED"; }
    }
  }
  else if (JSON["TYPE"] == "SECURITY") {
    cout << "\n   SECURITY [" << std::dec << Index << "]\n";
    cout << "\n     SERVICE : " << JSON["SERVICE"].get<std::string>();
    cout << "\n     WAIT : " << (int)JSON["WAIT"] << " ms";
    cout << "\n     DLL : " << JSON["DLL"].get<std::string>();
    int Level = (int)JSON["LEVEL"];
    cout << "\n     LEVEL : " << (Level) << ":" << (Level + 1);
    UDSAECURITYACCESS SEED, KEY;
    cout << "\n     SEED :";
    for (int I = 0; I < 16; I++)
      { cout << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)I; SEED.DATA[I] = I; }
    SEED.LENGTH = 16; SEED.LEVEL = Level;
    SecurityDLL (JSON["DLL"].get<std::string>(), SEED, KEY);
    cout << "\n     KEY :";
    for (uint16_t I = 0; I < KEY.LENGTH; I++)
      { cout << " " << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)KEY.DATA[I]; }
    cout << "\n     KEY LENGTH : " << std::dec << (int)KEY.LENGTH;
  }
  else if (JSON["TYPE"] == "DOWNLOAD") {
    cout << "\n   SECURITY [" << std::dec << Index << "]\n";
    cout << "\n     SERVICE : " << JSON["SERVICE"].get<std::string>();
    cout << "\n     WAIT : " << (int)JSON["WAIT"] << " ms";
    cout << "\n     FILE : " << JSON["FILE"].get<std::string>();
    int S = 0, L = 0, M = 0, R = 0, T = 0, cR = 0, cT = 0;
    std::string StreamByte;
    if (JSON["ERASE-ENABLE"] == "Y") {
      cout << "\n     ERASE FUNCTION";
      cout << "\n       ERASE REQUEST : ";
      std::istringstream StreamREQC(JSON["ERASE-REQ"].get<std::string>());
      std::istringstream StreamREQ(JSON["ERASE-REQ"].get<std::string>());
      S = 0, L = 0, M = 0, R = 0, cR = 0;
      while (StreamREQC >> StreamByte) { if (StreamByte == "RR") { cR++; } }
      while (StreamREQ >> StreamByte) {
        if (StreamByte == "SS")      { if (S == 0) { cout << " <4-BYTE START ADDRESS>"; S = 1; } else { continue; } }
        else if (StreamByte == "LL") { if (L == 0) { cout << " <4-BYTE END ADDRESS>"; L = 1; } else { continue; } }
        else if (StreamByte == "MM") { if (M == 0) { cout << " <4-BYTE MEMORY SIZE>"; M = 1; } else { continue; } }
        else if (StreamByte == "RR")
          { if (R == 0) { cout << " <" << std::dec << cR << "-BYTE CRC>"; R = 1; } else { continue; } }
        else { cout << " " << StreamByte; S = 0; L = 0; M = 0; R = 0; }
      }
      if (JSON["ERASE-CHECK"] == "Y") {
        cout << "\n       ERASE RESPONSE : ";
        std::istringstream StreamRESC(JSON["ERASE-RES"].get<std::string>());
        std::istringstream StreamRES(JSON["ERASE-RES"].get<std::string>());
        T = 0, cT = 0;
        while (StreamRESC >> StreamByte) { if (StreamByte == "TT") { cT++; } }
        while (StreamRES >> StreamByte) {
          if (StreamByte == "TT")
            { if (T == 0) { cout << " <" << std::dec << cT << "-BYTE TRANSFER BLOCK SIZE>"; T = 1; } else { continue; } }
          else { cout << " " << StreamByte; T = 0; }
        }
      } else {
        cout << "\n       ERASE RESPONSE : POSITVE RESPONSE NEEDED";
      }
    }
    if (JSON["CRC-ENABLE"] == "Y") {
      cout << "\n     CRC FUNCTION";
      cout << "\n       CRC REQUEST : ";
      std::istringstream StreamREQC(JSON["CRC-REQ"].get<std::string>());
      std::istringstream StreamREQ(JSON["CRC-REQ"].get<std::string>());
      S = 0, L = 0, M = 0, R = 0, cR = 0;
      while (StreamREQC >> StreamByte) { if (StreamByte == "RR") { cR++; } }
      while (StreamREQ >> StreamByte) {
        if (StreamByte == "SS")      { if (S == 0) { cout << " <4-BYTE ADDRESS>"; S = 1; } else { continue; } }
        else if (StreamByte == "LL") { if (L == 0) { cout << " <4-BYTE ADDRESS>"; L = 1; } else { continue; } }
        else if (StreamByte == "MM") { if (M == 0) { cout << " <4-BYTE SIZE>"; M = 1; } else { continue; } }
        else if (StreamByte == "RR")
          { if (R == 0) { cout << " <" << std::dec << cR << "-BYTE CRC>"; R = 1; } else { continue; } }
        else { cout << " " << StreamByte; S = 0; L = 0; M = 0; R = 0; }
      }
      if (JSON["CRC-CHECK"] == "Y") {
        cout << "\n       CRC RESPONSE : ";
        std::istringstream StreamRESC(JSON["CRC-RES"].get<std::string>());
        std::istringstream StreamRES(JSON["CRC-RES"].get<std::string>());
        T = 0, cT = 0;
        while (StreamRESC >> StreamByte) { if (StreamByte == "TT") { cT++; } }
        while (StreamRES >> StreamByte) {
          if (StreamByte == "TT")
            { if (T == 0) { cout << " <" << std::dec << cT << "-BYTE TRANSFER BLOCK SIZE>"; T = 1; } else { continue; } }
          else { cout << " " << StreamByte; T = 0; }
        }
      } else {
        cout << "\n       ERASE RESPONSE : POSITVE RESPONSE NEEDED";
      }
    }
    cout << "\n     DOWNLOAD - REQUEST : ";
    std::istringstream StreamREQCD(JSON["SER-DOWN-REQ"].get<std::string>());
    std::istringstream StreamREQD(JSON["SER-DOWN-REQ"].get<std::string>());
    S = 0, L = 0, M = 0, R = 0, cR = 0;
    while (StreamREQCD >> StreamByte) { if (StreamByte == "RR") { cR++; } }
    while (StreamREQD >> StreamByte) {
      if (StreamByte == "SS")      { if (S == 0) { cout << " <4-BYTE START ADDRESS>"; S = 1; } else { continue; } }
      else if (StreamByte == "LL") { if (L == 0) { cout << " <4-BYTE END ADDRESS>"; L = 1; } else { continue; } }
      else if (StreamByte == "MM") { if (M == 0) { cout << " <4-BYTE SIZE>"; M = 1; } else { continue; } }
      else if (StreamByte == "RR")
        { if (R == 0) { cout << " <" << std::dec << cR << "-BYTE CRC>"; R = 1; } else { continue; } }
      else { cout << " " << StreamByte; S = 0; L = 0; M = 0; R = 0; }
    }
    cout << "\n     DOWNLOAD - RESPONSE : ";
    std::istringstream StreamRESCD(JSON["SER-DOWN-RES"].get<std::string>());
    std::istringstream StreamRESD(JSON["SER-DOWN-RES"].get<std::string>());
    T = 0, cT = 0;
    while (StreamRESCD >> StreamByte) { if (StreamByte == "TT") { cT++; } }
    while (StreamRESD >> StreamByte) {
      if (StreamByte == "TT")
        { if (T == 0) { cout << " <" << std::dec << cT << "-BYTE TRANSFER BLOCK SIZE>"; T = 1; } else { continue; } }
      else { cout << " " << StreamByte; T = 0; }
    }
    cout << "\n     EXIT - REQUEST : ";
    std::istringstream StreamREQCE(JSON["SER-EXIT-REQ"].get<std::string>());
    std::istringstream StreamREQE(JSON["SER-EXIT-REQ"].get<std::string>());
    S = 0, L = 0, M = 0, R = 0, cR = 0;
    while (StreamREQCE >> StreamByte) { if (StreamByte == "RR") { cR++; } }
    while (StreamREQE >> StreamByte) {
      if (StreamByte == "SS")      { if (S == 0) { cout << " <4-BYTE START ADDRESS>"; S = 1; } else { continue; } }
      else if (StreamByte == "LL") { if (L == 0) { cout << " <4-BYTE END ADDRESS>"; L = 1; } else { continue; } }
      else if (StreamByte == "MM") { if (M == 0) { cout << " <4-BYTE MEMORY SIZE>"; M = 1; } else { continue; } }
      else if (StreamByte == "RR")
        { if (R == 0) { cout << " <" << std::dec << cR << "-BYTE CRC>"; R = 1; } else { continue; } }
      else { cout << " " << StreamByte; S = 0; L = 0; M = 0; R = 0; }
    }
    cout << "\n     EXIT - RESPONSE : ";
    std::istringstream StreamRESCE(JSON["SER-EXIT-RES"].get<std::string>());
    std::istringstream StreamRESE(JSON["SER-EXIT-RES"].get<std::string>());
    T = 0, cT = 0;
    while (StreamRESCE >> StreamByte) { if (StreamByte == "TT") { cT++; } }
    while (StreamRESE >> StreamByte) {
      if (StreamByte == "TT")
        { if (T == 0) { cout << " <" << std::dec << cT << "-BYTE TRANSFER BLOCK SIZE>"; T = 1; } else { continue; } }
      else { cout << " " << StreamByte; T = 0; }
    }
  }
  else {
    cout << "\n   NOTE [" << std::dec << Index << "]\n";
    std::string STRDUMP = JSON.dump();
    uint16_t I = 0;
    while (I < STRDUMP.length()) {
      if ((I % 100) == 0) { cout << "\n       "; }
      cout << STRDUMP[I];
      I++;
    }
  }

  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD Coordinates = { 0, 26 };
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
  ConsoleColor();
  (Option == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
  cout << " < PREVIOUS";
  (Option == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
  cout << "   NEXT >";
  (Option == 2) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
  cout << "   BACK";
  (Option == 3) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
  cout << "   EXIT";
}
/* ==================================================================================================== */

// MARK: Flash Validate JSON
/* ==================================================================================================== */
/**
 * @name        ValidateFlashJSON
 * @class       ISO_UDS (Private)
 * @brief       Validate Format and Depend Files of Flash Sequence JSON
 * @param [Filename]      Flash JSON File Path
 * @param [Lines]         Validation Result Lines
 * @param [ErrorReason]   Error Reason String
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ValidateFlashJSON (const std::string& Filename, std::vector<std::string>& Lines,
    std::string& ErrorReason) {
  ifstream FlashFile(Filename);
  if (!FlashFile) { ErrorReason = "FILE: JSON File Not Found"; return FILENOTFOUND; }
  json JSON;
  try { FlashFile >> JSON; } catch (const json::parse_error& E)
    { ErrorReason = "FILE: JSON File Format Worng"; return FORMATWRONG; }
  if (!JSON.is_array()) { ErrorReason = "FILE: JSON File Format Worng"; return FORMATWRONG; }
  Lines.push_back("{");

  for (const auto& FLASH : JSON) {
    if (  !FLASH.contains("WAIT")     || !FLASH["WAIT"].is_number_integer() ||
          !FLASH.contains("TYPE")     || !FLASH["TYPE"].is_string()         ||
          !FLASH.contains("SERVICE")  || !FLASH["SERVICE"].is_string()      ){
      ErrorReason = "FILE: Headers Not Found"; return WRONGCONFIG;
    }
    if (FLASH["TYPE"] == "NOTE") { continue; }

    else if (FLASH["TYPE"] == "NORMAL") {
      if ( !FLASH.contains("ADDRESS")  || !FLASH["ADDRESS"].is_string()  )
        { ErrorReason = "NORMAL: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("SUPPRESS") || !FLASH["SUPPRESS"].is_string() )
        { ErrorReason = "NORMAL: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CHECK")    || !FLASH["CHECK"].is_string()    )
        { ErrorReason = "NORMAL: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("REQUEST")  || !FLASH["REQUEST"].is_string()  )
        { ErrorReason = "NORMAL: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("RESPONSE") || !FLASH["RESPONSE"].is_string() )
        { ErrorReason = "NORMAL: Missing or Wrong Structure"; return FORMATWRONG; }

      if ((FLASH["ADDRESS"] != "PHY") && (FLASH["ADDRESS"] != "FUN"))
          { ErrorReason = "NORMAL: Wrong Address"; return WRONGCONFIG; }
      if ((FLASH["SUPPRESS"] != "Y") && (FLASH["SUPPRESS"] != "N"))
          { ErrorReason = "NORMAL: Wrong Suppress Value"; return WRONGCONFIG; }
      if ((FLASH["CHECK"] != "Y") && (FLASH["CHECK"] != "N"))
          { ErrorReason = "NORMAL: Wrong Address"; return WRONGCONFIG; }
      if ((FLASH["SUPPRESS"] == "Y") && (FLASH["CHECK"] == "Y"))
          { ErrorReason = "NORMAL: Ambiguous Response"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["REQUEST"], 1, 0))
          { ErrorReason = "NORMAL: Request Format Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["RESPONSE"], 0, 0))
          { ErrorReason = "NORMAL: Response Format Wrong"; return WRONGCONFIG; }
    }

    else if (FLASH["TYPE"] == "SECURITY") {
      if ( !FLASH.contains("DLL")   || !FLASH["DLL"].is_string()           )
          { ErrorReason = "SECURITY: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("LEVEL") || !FLASH["LEVEL"].is_number_integer() )
          { ErrorReason = "SECURITY: Missing or Wrong Structure"; return FORMATWRONG; }
      int Result = ValidateSecurityDll(FLASH["DLL"]);
      if (Result == 1) { ErrorReason = "SECURITY: DLL File Not Found"; return FILENOTFOUND; }
      else if (Result == 2) { ErrorReason = "SECURITY: DLL File Not Right"; return DLLNOTWORKING; }
      if ((FLASH["LEVEL"] != 1) && (FLASH["LEVEL"] != 3))
          { ErrorReason = "SECURITY: Level Not Supported"; return WRONGCONFIG; }
      std:: string L = "  {"; Lines.push_back(L);
      L = "    SERVICE : " +  FLASH["SERVICE"].get<std::string>(); Lines.push_back(L);
      L = "    DLL PATH : " +  FLASH["DLL"].get<std::string>(); Lines.push_back(L);
      L = "  }"; Lines.push_back(L);
    }

    else if (FLASH["TYPE"] == "DOWNLOAD") {
      if ( !FLASH.contains("CRC-ALGORITHM")  || !FLASH["CRC-ALGORITHM"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( FLASH["CRC-ALGORITHM"] != "CRC16" ) { ErrorReason = "DOWNLOAD: Only CRC16 Supported"; return WRONGCONFIG; }
      if ( !FLASH.contains("CRC-POLY") || !FLASH["CRC-POLY"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-INIT") || !FLASH["CRC-INIT"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-FINAL")|| !FLASH["CRC-FINAL"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-INPUTREVERSE")  || !FLASH["CRC-INPUTREVERSE"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-OUTPUTREVERSE") || !FLASH["CRC-OUTPUTREVERSE"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( (FLASH["CRC-INPUTREVERSE"] != "Y")  && (FLASH["CRC-INPUTREVERSE"] != "N") )
          { ErrorReason = "DOWNLOAD: CRC Configuration Wrong"; return WRONGCONFIG; }
      if ( (FLASH["CRC-OUTPUTREVERSE"] != "Y") && (FLASH["CRC-OUTPUTREVERSE"] != "N") )
          { ErrorReason = "DOWNLOAD: CRC Configuration Wrong"; return WRONGCONFIG; }
      uint16_t CRC_Poly = 0, CRC_Init = 0, CRC_Final = 0, CRC_INR = 0, CRC_OTR = 0;
      try {
        std::string sCRC;
        sCRC = FLASH["CRC-POLY"];  if (sCRC.length() != 4) { ErrorReason = "DOWNLOAD: 16 Bit Only"; return WRONGCONFIG; }
        CRC_Poly = std::stoul(sCRC, nullptr, 16);
        sCRC = FLASH["CRC-INIT"];  if (sCRC.length() != 4) { ErrorReason = "DOWNLOAD: 16 Bit Only"; return WRONGCONFIG; }
        CRC_Init = std::stoul(sCRC, nullptr, 16);
        sCRC = FLASH["CRC-FINAL"]; if (sCRC.length() != 4) { ErrorReason = "DOWNLOAD: 16 Bit Only"; return WRONGCONFIG; }
        CRC_Final = std::stoul(sCRC, nullptr, 16);
      }
      catch (const std::invalid_argument& E)  { ErrorReason = "DOWNLOAD: 16 Bit Only"; return WRONGCONFIG; }
      catch (const std::out_of_range& E)      { ErrorReason = "DOWNLOAD: 16 Bit Only"; return WRONGCONFIG; }
      CRC_INR = (FLASH["CRC-INPUTREVERSE"] == "Y") ? 1 : 0;
      CRC_OTR = (FLASH["CRC-OUTPUTREVERSE"] == "Y") ? 1 : 0;

      if ( !FLASH.contains("ERASE-ENABLE") || !FLASH["ERASE-ENABLE"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("ERASE-CHECK")  || !FLASH["ERASE-CHECK"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("ERASE-REQ")    || !FLASH["ERASE-REQ"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("ERASE-RES")    || !FLASH["ERASE-RES"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( (FLASH["ERASE-ENABLE"] != "Y") && (FLASH["ERASE-ENABLE"] != "N") )
          { ErrorReason = "DOWNLOAD: ERASE Configuration Wrong"; return WRONGCONFIG; }
      if ( (FLASH["ERASE-CHECK"] != "Y")  && (FLASH["ERASE-CHECK"] != "N")  )
          { ErrorReason = "DOWNLOAD: ERASE Configuration Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["ERASE-REQ"], 1, 1))
          { ErrorReason = "DOWNLOAD: ERASE Request Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["ERASE-RES"], 0, 1))
          { ErrorReason = "DOWNLOAD: ERASE Response Wrong"; return WRONGCONFIG; }

      if ( !FLASH.contains("CRC-ENABLE") || !FLASH["CRC-ENABLE"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-CHECK")  || !FLASH["CRC-CHECK"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-REQ")    || !FLASH["CRC-REQ"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("CRC-RES")    || !FLASH["CRC-RES"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( (FLASH["CRC-ENABLE"] != "Y") && (FLASH["CRC-ENABLE"] != "N") )
          { ErrorReason = "DOWNLOAD: CRC Configuration Wrong"; return WRONGCONFIG; }
      if ( (FLASH["CRC-CHECK"] != "Y")  && (FLASH["CRC-CHECK"] != "N")  )
          { ErrorReason = "DOWNLOAD: CRC Configuration Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["CRC-REQ"], 1, 1))
          { ErrorReason = "DOWNLOAD: CRC Request Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["CRC-RES"], 0, 1))
          { ErrorReason = "DOWNLOAD: CRC Response Wrong"; return WRONGCONFIG; }

      if ( !FLASH.contains("SER-DOWN-REQ")    || !FLASH["SER-DOWN-REQ"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("SER-DOWN-RES")    || !FLASH["SER-DOWN-RES"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if (ValidateFlashREQS(FLASH["SER-DOWN-REQ"], 1, 1))
          { ErrorReason = "DOWNLOAD: SER-DOWN Request Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["SER-DOWN-RES"], 0, 1))
          { ErrorReason = "DOWNLOAD: SER-DOWN Response Wrong"; return WRONGCONFIG; }
      if ( !FLASH.contains("SER-EXIT-REQ")    || !FLASH["SER-EXIT-REQ"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if ( !FLASH.contains("SER-EXIT-RES")    || !FLASH["SER-EXIT-RES"].is_string() )
          { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      if (ValidateFlashREQS(FLASH["SER-EXIT-REQ"], 1, 1))
          { ErrorReason = "DOWNLOAD: SER-EXIT Request Wrong"; return WRONGCONFIG; }
      if (ValidateFlashREQS(FLASH["SER-EXIT-RES"], 0, 1))
          { ErrorReason = "DOWNLOAD: SER-EXIT Response Wrong"; return WRONGCONFIG; }

      if ( !FLASH.contains("FILE") || !FLASH["FILE"].is_string() )
         { ErrorReason = "DOWNLOAD: Missing or Wrong Structure"; return FORMATWRONG; }
      int Result = FileHEX_Validate(FLASH["FILE"]);
      if (Result == 1) { ErrorReason = "DOWNLOAD: .Hex File Tampered or Wrong"; return HEXFILEISSUE; }
      if (Result == 2) { ErrorReason = "DOWNLOAD: .Hex File Not Found"; return HEXFILENOTFOUND; }

      UDSFLASHMEMORYINFO MEMORY;
      Result = FileHEX_MemoryInfo(MEMORY, FLASH["FILE"], CRC_Poly, CRC_Init, CRC_Final, CRC_INR, CRC_OTR);
      if (Result) { ErrorReason = "DOWNLOAD: .Hex File Tampered or Wrong"; return HEXFILEISSUE; }
      std::stringstream SS1, SS2, SS3, SS4;
      SS1 << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << MEMORY.ADDRESSSTART;
      SS2 << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << MEMORY.ADDRESSEND;
      SS3 << std::uppercase << std::setfill('0') << std::setw(8) << std::hex << MEMORY.SIZE;
      SS4 << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << MEMORY.CRC;
#ifdef DEVICE_DEBUG_MODE
      std::stringstream SS5, SS6, SS7;
      SS5 << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << CRC_Poly;
      SS6 << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << CRC_Init;
      SS7 << std::uppercase << std::setfill('0') << std::setw(4) << std::hex << CRC_Final;
#endif
      std:: string L = "  {"; Lines.push_back(L);
      L = "    SERVICE : " +  FLASH["SERVICE"].get<std::string>(); Lines.push_back(L);
      L = "    FILE PATH : " +  FLASH["FILE"].get<std::string>(); Lines.push_back(L);
#ifdef DEVICE_DEBUG_MODE
      L = "    CRC : {"; Lines.push_back(L);
      L = "      POLYNOMIAL    : " + SS5.str(); Lines.push_back(L);
      L = "      INITIAL VALUE : " + SS6.str(); Lines.push_back(L);
      L = "      FINAL VALUE   : " + SS7.str(); Lines.push_back(L);
      L = "      REVERSE I/O   : " + to_string(CRC_INR) + "/" + to_string(CRC_OTR);
      L = "    }"; Lines.push_back(L);
#endif
      L = "    FILE : {"; Lines.push_back(L);
      L = "      START ADDRESS : " + SS1.str(); Lines.push_back(L);
      L = "      END ADDRESS   : " + SS2.str(); Lines.push_back(L);
      L = "      DATA LENGTH   : " + SS3.str(); Lines.push_back(L);
      L = "      CRC           : " + SS4.str(); Lines.push_back(L);
      L = "    }"; Lines.push_back(L);
      L = "  }"; Lines.push_back(L);
    }
    else { ErrorReason = "FILE: Invalid Type"; return FORMATWRONG; }
  }

  Lines.push_back("}");
  FlashFile.close();
  ErrorReason = "FILE: All Good";
  return ALLOK;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ValidateFlashREQS
 * @class       ISO_UDS (Private)
 * @brief       Validate Request and Response Strings of Flash JSON
 * @param [STR]       Validation String
 * @param [Type]      0 means Response and else means Request
 * @param [Special]   0 means Normal Check and else means Special Identifiers Check
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ValidateFlashREQS (std::string STR, uint8_t Type, uint8_t Special) {
  if (Type) {
    std::vector<std::string> ByteToken;
    std::istringstream Stream(STR);
    std::string StreamByte;
    int OK = 0;
    while (Stream >> StreamByte) {
      if (StreamByte.length() == 2) {
        if ((Special) && ((StreamByte == "SS") || (StreamByte == "LL") || (StreamByte == "MM") || (StreamByte == "RR")))
          { ByteToken.push_back(StreamByte); }
        else if (
          (((StreamByte[0] >= '0') && (StreamByte[0] <= '9')) || ((StreamByte[0] >= 'A') && (StreamByte[0] <= 'F'))) ||
          (((StreamByte[1] >= '0') && (StreamByte[1] <= '9')) || ((StreamByte[1] >= 'A') && (StreamByte[1] <= 'F'))) )
          { ByteToken.push_back(StreamByte); }
        else { OK = 2; break; }
      } else { OK = 1; break; }
    } int cT = 0, tT = 0;
    if (Special) {
      if (OK) { return OK; } cT = 0; tT = 0;
      for (uint8_t I = 0; I < ByteToken.size(); I++) {
        if (ByteToken[I] == "SS") { if (tT == 0) { if (cT == 0) { tT = 1; } else { OK = 3; break; } } cT++; }
        else { tT = 0; }
      } if ((cT != 0) && (cT != 4)) { OK = 4; }
      if (OK) { return OK; } cT = 0; tT = 0;
      for (uint8_t I = 0; I < ByteToken.size(); I++) {
        if (ByteToken[I] == "LL") { if (tT == 0) { if (cT == 0) { tT = 1; } else { OK = 3; break; } } cT++; }
        else { tT = 0; }
      } if ((cT != 0) && (cT != 4)) { OK = 4; }
      if (OK) { return OK; } cT = 0; tT = 0;
      for (uint8_t I = 0; I < ByteToken.size(); I++) {
        if (ByteToken[I] == "MM") { if (tT == 0) { if (cT == 0) { tT = 1; } else { OK = 3; break; } } cT++; }
        else { tT = 0; }
      } if ((cT != 0) && (cT != 4)) { OK = 4; }
      if (OK) { return OK; } cT = 0; tT = 0;
      for (uint8_t I = 0; I < ByteToken.size(); I++) {
        if (ByteToken[I] == "RR") { if (tT == 0) { if (cT == 0) { tT = 1; } else { OK = 3; break; } } cT++; }
        else { tT = 0; }
      } if ((cT != 0) && (cT != 2) && (cT != 4)) { OK = 4; }
      if (OK) { return OK; }
    } return OK;
  }
  else { // Response Check
    std::vector<std::string> ByteToken;
    std::istringstream Stream(STR);
    std::string StreamByte;
    int OK = 0;
    while (Stream >> StreamByte) {
      if (StreamByte.length() == 2) {
        if ((Special) && ((StreamByte == "TT"))) { ByteToken.push_back(StreamByte); }
        else if (StreamByte == "XX") { ByteToken.push_back(StreamByte); }
        else if (
          (((StreamByte[0] >= '0') && (StreamByte[0] <= '9')) || ((StreamByte[0] >= 'A') && (StreamByte[0] <= 'F'))) ||
          (((StreamByte[1] >= '0') && (StreamByte[1] <= '9')) || ((StreamByte[1] >= 'A') && (StreamByte[1] <= 'F'))) )
          { ByteToken.push_back(StreamByte); }
        else { OK = 2; break; }
      } else { OK = 1; break; }
    } int cT = 0, tT = 0;
    if (Special) {
      if (OK) { return OK; }
      for (uint8_t I = 0; I < ByteToken.size(); I++) {
        if (ByteToken[I] == "TT") { if (tT == 0) { if (cT == 0) { tT = 1; } else { OK = 3; break; } } cT++; }
        else { tT = 0; }
      } if (cT > 2) { OK = 4; }
      if (OK) { return OK; }
    } return OK;
  }
}
/* ==================================================================================================== */


// MARK: Flash Support Functions
/* ==================================================================================================== */
/**
 * @name        RenderFlashPercent
 * @class       ISO_UDS (Private)
 * @brief       Show the Flash Percentage
 * @param [Percent]   Flash Tree Vector
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderFlashPercent (uint8_t Percent) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
    COORD Coordinates = {1, 28};
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "   " << std::dec << setw(3) << setfill(' ') << (int)Percent;
  cout << " % :  [";
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  for (int I = 0; I < 100; I++) {
    if (I == Percent) { ConsoleColor(CONSOLECOLOUR_GRAY); }
    if (I < Percent) { cout << "#"; } else { cout << "."; }
  }
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE); cout << "]";
  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
  ConsoleColor();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ShowFlashTree
 * @class       ISO_UDS (Private)
 * @brief       Show the Flash Tree
 * @param [FLASHTREE] Flash Tree Vector
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ShowFlashTree (std::vector<UDSFLASHTREE>& FLASHTREE) {
  for (uint16_t I = 0; I < FLASHTREE.size(); I++) {
    cout << "\n\n TREE BRANCH : " << I;
    cout << "\n     SERVICE : " << FLASHTREE[I].SERVICE;
    cout << "\n     TYPE : " << FLASHTREE[I].TYPE;
    cout << "\n     WAIT : " << FLASHTREE[I].WAIT;
    cout << "\n     EFFECTIVELENGTH : " << FLASHTREE[I].EFFECTIVELENGTH;
    cout << "\n     START % : " << (int)FLASHTREE[I].STARTPERCENT;
    cout << "\n     END % : " << (int)FLASHTREE[I].ENDPERCENT;
    cout << "\n     JSON : " << FLASHTREE[I].FLASHJSON;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        CalculateFlashTree
 * @class       ISO_UDS (Private)
 * @brief       Calculate The Flash Tree
 * @param [Filename]  File Path for Flash JSON
 * @param [FLASHTREE] Flash Tree Vector
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::CalculateFlashTree (const std::string& Filename, std::vector<UDSFLASHTREE>& FLASHTREE) {
  ifstream FlashFile(Filename); if (!FlashFile) { return; }
  json JSON; FlashFile >> JSON;
  FlashFile.close();
  int Total = JSON.size();
  CONFIG.FLASHMAGINFIER = 1000;
  uint64_t Length = 0;
  for (int I = 0; I < Total; I++) {
    json J = JSON[I]; UDSFLASHTREE TREE;
    std::string StreamByte;
    if (J["TYPE"] == "NOTE") {
      TREE.SERVICE = J["SERVICE"].get<std::string>();
      TREE.TYPE = J["TYPE"].get<std::string>();
      TREE.WAIT = 0;
      TREE.EFFECTIVELENGTH = 0;
      TREE.FLASHJSON = J;
      FLASHTREE.push_back(TREE);
    }
    else if (J["TYPE"] == "NORMAL") {
      TREE.SERVICE = J["SERVICE"].get<std::string>();
      TREE.TYPE = J["TYPE"].get<std::string>();
      int W = J["WAIT"]; TREE.WAIT = W * 1000;
      int L = 0;
      std::istringstream StreamREQ(J["REQUEST"].get<std::string>());
      while (StreamREQ >> StreamByte) { L++; }
      TREE.EFFECTIVELENGTH = L * CONFIG.FLASHMAGINFIER;
      Length = Length + (L * CONFIG.FLASHMAGINFIER);
      TREE.FLASHJSON = J;
      FLASHTREE.push_back(TREE);
    }
    else if (J["TYPE"] == "SECURITY") {
      TREE.SERVICE = J["SERVICE"].get<std::string>();
      TREE.TYPE = J["TYPE"].get<std::string>();
      int W = J["WAIT"]; TREE.WAIT = W * 1000;
      TREE.EFFECTIVELENGTH = 16 * CONFIG.FLASHMAGINFIER;
      Length = Length + (16 * CONFIG.FLASHMAGINFIER);
      TREE.FLASHJSON = J;
      FLASHTREE.push_back(TREE);
    }
    else if (J["TYPE"] == "DOWNLOAD") {
      TREE.SERVICE = J["SERVICE"].get<std::string>();
      TREE.TYPE = J["TYPE"].get<std::string>();
      int W = J["WAIT"]; TREE.WAIT = W * 1000;
      int L = 0; int cL = 0;
      if (J["ERASE-ENABLE"] == "Y") {
        std::istringstream StreamREQER(J["ERASE-REQ"].get<std::string>());
        while (StreamREQER >> StreamByte) { L++; }
      } cL = cL + L; L = 0;
      if (J["CRC-ENABLE"] == "Y") {
        std::istringstream StreamREQC(J["CRC-REQ"].get<std::string>());
        while (StreamREQC >> StreamByte) { L++; }
      } cL = cL + L; L = 0;
      std::istringstream StreamREQD(J["SER-DOWN-REQ"].get<std::string>());
      while (StreamREQD >> StreamByte) { L++; }
      cL = cL + L; L = 0;
      std::istringstream StreamREQE(J["SER-EXIT-REQ"].get<std::string>());
      while (StreamREQE >> StreamByte) { L++; }
      cL = cL + L; L = 0;
      uint16_t CRC_Poly = 0, CRC_Init = 0, CRC_Final = 0, CRC_INR = 0, CRC_OTR = 0;
      std::string sCRC;
      sCRC = J["CRC-POLY"]; CRC_Poly = std::stoul(sCRC, nullptr, 16);
      sCRC = J["CRC-INIT"]; CRC_Init = std::stoul(sCRC, nullptr, 16);
      sCRC = J["CRC-FINAL"];CRC_Final = std::stoul(sCRC, nullptr, 16);
      CRC_INR = (J["CRC-INPUTREVERSE"] == "Y") ? 1 : 0;
      CRC_OTR = (J["CRC-OUTPUTREVERSE"] == "Y") ? 1 : 0;
      UDSFLASHMEMORYINFO MEMORY;
      FileHEX_MemoryInfo(MEMORY, J["FILE"], CRC_Poly, CRC_Init, CRC_Final, CRC_INR, CRC_OTR);
      TREE.EFFECTIVELENGTH = cL + MEMORY.SIZE;
      Length = Length + cL + MEMORY.SIZE;
      TREE.FLASHJSON = J;
      FLASHTREE.push_back(TREE);
    }
  }
  uint8_t P = 0;
  for (int I = 0; I < Total; I++) {
    uint8_t H = (uint8_t)((FLASHTREE[I].EFFECTIVELENGTH * 100) / Length);
    FLASHTREE[I].STARTPERCENT = P;
    if ((P + H) >= 100) { FLASHTREE[I].ENDPERCENT = 99; P = 99; }
    else { FLASHTREE[I].ENDPERCENT = P + H; P += H; }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderFlashMessage
 * @class       ISO_UDS (Private)
 * @brief       Flash Request or Response According to proper Offsets and Dimensions
 * @param [Offset]  Offset from Left Side Screen
 * @param [MSG]     Message Vector
 * @param [Width]   Width of Message Matrix
 * @param [Height]  Height of Message Matrix
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void RenderFlashMessage (int Offset, std::vector<std::string>& MSG, int Width, int Height) {
  int Total = MSG.size(); int OverFlow = ((Width * Height) < Total) ? 1 : 0; int C = 0;
  for (int I = 0; I < Height; I++) {
    cout << "\n"; for (int K = 0; K < Offset; K++) { cout << " "; }
    for (int J = 0; J < Width; J++) {
      if (C >= Total) { return; }
      if (C == ((Width * Height) - 1) && (OverFlow)) { cout << " .."; }
      else if (C < Total) { cout << " " << MSG[C]; }
      C++;
    }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FlashExecutor
 * @class       ISO_UDS (Private)
 * @brief       Flash Service Executer, Checker and Error Screen Renderer
 * @param [XY]            Start Coordinate for Re Rendering Error Screen
 * @param [RemoveLines]   Number of Lines to Remove
 * @param [REQ]           Request String Vector Pointer
 * @param [RES]           Response String Vector Pointer
 * @param [RESMASK]       Response Check Mask
 * @param [Suppress]      Suppress Response Active High
 * @param [Check]         Check Response According to Response Check Mask Active High
 * @param [Address]       1 for Functional Addressing else Physical Addressing
 * @return      Status of Flashing Service Execution Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FlashExecutor (COORD XY, uint8_t RemoveLines, std::vector<std::string>& REQ, std::vector<std::string>& RES,
    std::vector<std::string>& RESMASK, uint8_t Suppress, uint8_t Check, uint8_t Address) {
  uint8_t Data = 0;
  for (uint16_t I = 0; I < REQ.size(); I++) { StringToHEX(Data, REQ[I]); MESSAGE.DATA[I] = Data; } MESSAGE.LEN = REQ.size();
  uint8_t SID = MESSAGE.DATA[0];
  if (Address == 1) { DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_FUN); }
  else { DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_PHY); }
  int REQFAILED = 0, NEGRES = 0, CHECKFAILED = 0, NRC = 0;
  std::string DoCANErrorLine;

  if (Suppress) { if (Executor(0, DoCANErrorLine)) { REQFAILED = 1; } else { return 0; } }
  else {
    if (Executor(1, DoCANErrorLine)) { REQFAILED = 1; }
    else {
      if (MESSAGE.DATA[0] == (SID + 0x40)) { NEGRES = 0; }
      else if (MESSAGE.DATA[0] == 0x7F) { NEGRES = 1; NRC = MESSAGE.DATA[2]; }
      else { NEGRES = 2; }
      for (int I = 0; I < MESSAGE.LEN; I++) { RES.push_back( HEXToString(MESSAGE.DATA[I]) ); }

      if (NEGRES == 0) {
        if (Check) {
          if (RES.size() != RESMASK.size()) { CHECKFAILED = 1; }
          if (CHECKFAILED == 0) {
            for (uint16_t I = 0; I < RESMASK.size(); I++) {
              if (RESMASK[I] == "XX") { continue; }
              else if (RESMASK[I] == "TT") { continue; }
              else if (RESMASK[I] != RES[I]) { CHECKFAILED = 1; break; }
            }
          }
          if (CHECKFAILED == 0) { return 0; }
        }
        else { return 0; }
      }
    }
  }

  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer; DWORD Written;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  SetConsoleCursorPosition(ConsoleHandle, XY);
  for (int I = 0; I < RemoveLines; I++) {
      COORD NewLine = { 0, static_cast<SHORT>(XY.Y + I) };
      FillConsoleOutputCharacter(ConsoleHandle, ' ', ConsoleBuffer.dwSize.X, NewLine, &Written);
      FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, ConsoleBuffer.dwSize.X, NewLine, &Written);
  } SetConsoleCursorPosition(ConsoleHandle, XY);

  ConsoleColor();
  cout << "\n     REQUEST MESSAGE :";
  RenderFlashMessage(7, REQ, 30, 7);
  if (REQFAILED) { ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n     " << DoCANErrorLine; ConsoleColor(); }
  else {
    cout << "\n\n     REQUEST MESSAGE : ";
    if (NEGRES == 0) { ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "POSITIVE RESPONSE"; ConsoleColor(); }
    else if (NEGRES == 1)
      { ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "NEGATIVE RESPONSE : " << NegativeResponse(NRC); ConsoleColor(); }
    else { ConsoleColor(CONSOLECOLOUR_GRAY); cout << "UNEXPECTED RESPONSE"; ConsoleColor(); }
    RenderFlashMessage(7, RES, 30, 7);
    if (CHECKFAILED) {
      cout << "\n\n     EXPECTED RESPONSE MESSAGE :";
      for (uint16_t I = 0; I < RESMASK.size(); I++) { cout << " " << RESMASK[I]; }
    }
  }

  ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n\n     FLASHING FAILED !";
  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n   BACK";
  ConsoleColor();
  return 1;
}
/* ==================================================================================================== */





// struct UDSFLASHTREE {
//   std::string SERVICE; std::string TYPE;
//   uint64_t WAIT;
//   uint32_t EFFECTIVELENGTH;
//   json FLASHJSON;
//   uint8_t STARTPERCENT; uint8_t ENDPERCENT;
// };
// std::vector<UDSFLASHTREE> FLASHTREES;

// struct UDSFLASHREQS {
//   std::vector<std::string> REQ;
//   std::vector<std::string> RES;
//   std::vector<std::string> RESMASK;
// };

// struct UDSFLASHSERVICE {
//   std::string TYPE;
//   std::string SERVICE;
//   std::vector<UDSFLASHREQS> REQS;
// };
// std::vector<UDSFLASHSERVICE> FLASHBLOCKS;
// TODO: START




/* ==================================================================================================== */
/**
 * @name        FlashDownloadRequestBuilder
 * @class       ISO_UDS (Private)
 * @brief       Flash The DOWNLOAD Block
 * @param [MEMORY]    File Memory Information Structure
 * @param [CRCSSIZE]  CRC Size
 * @param [MASK]      JSON File Response Builder Mask
 * @param [REQ]       Request Vector Building
 * @return      Status of Flashing Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::FlashDownloadRequestBuilder (UDSFLASHMEMORYINFO MEMORY, uint8_t CRCSSIZE,
    std::vector <std::string>& MASK, std::vector <std::string>& REQ) {
  uint16_t I = 0;
  while ( I < MASK.size()) {
    if (MASK[I] == "SS") {
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSSTART >> 24) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSSTART >> 16) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSSTART >>  8) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSSTART >>  0) ) ); I++;
    } else if (MASK[I] == "LL") {
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSEND >> 24) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSEND >> 16) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSEND >>  8) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.ADDRESSEND >>  0) ) ); I++;
    } else if (MASK[I] == "MM") {
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.SIZE >> 24) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.SIZE >> 16) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.SIZE >>  8) ) ); I++;
      REQ.push_back( HEXToString( (uint8_t)(MEMORY.SIZE >>  0) ) ); I++;
    } else if (MASK[I] == "RR") {
      if (CRCSSIZE == 2) {
        REQ.push_back ( HEXToString( (uint8_t)(MEMORY.CRC >>  8) ) ); I++;
        REQ.push_back ( HEXToString( (uint8_t)(MEMORY.CRC >>  0) ) ); I++;
      } else {
        REQ.push_back( HEXToString( 0x00 ) ); I++;
        REQ.push_back( HEXToString( 0x00 ) ); I++;
        REQ.push_back( HEXToString( (uint8_t)(MEMORY.CRC >>  8) ) ); I++;
        REQ.push_back( HEXToString( (uint8_t)(MEMORY.CRC >>  0) ) ); I++;
      }
    } else { REQ.push_back(MASK[I]); I++; }
  }
}
/* ==================================================================================================== */



/* ==================================================================================================== */
/**
 * @name        FlashDOWNLOAD
 * @class       ISO_UDS (Private)
 * @brief       Flash The DOWNLOAD Block
 * @param [FLASHTREE] Flash Tree Vector
 * @param [XY]        Start Console Coordinates
 * @return      Status of Flashing Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FlashDOWNLOAD (UDSFLASHTREE FLASHTREE, COORD XY) {
  DoCAN.PCAN.ResetFIFO();
  DoCAN.MicroDelay(FLASHTREE.WAIT);
  UDSFLASHSERVICE SER; UDSFLASHREQS erREQS, crREQS, rREQS, eREQS;
  SER.TYPE = FLASHTREE.FLASHJSON["TYPE"]; SER.SERVICE = FLASHTREE.FLASHJSON["SERVICE"];
  ConsoleColor();

  uint16_t CRC_Poly = 0, CRC_Init = 0, CRC_Final = 0, CRC_INR = 0, CRC_OTR = 0;
  std::string sCRC;
  sCRC = FLASHTREE.FLASHJSON["CRC-POLY"]; CRC_Poly = std::stoul(sCRC, nullptr, 16);
  sCRC = FLASHTREE.FLASHJSON["CRC-INIT"]; CRC_Init = std::stoul(sCRC, nullptr, 16);
  sCRC = FLASHTREE.FLASHJSON["CRC-FINAL"]; CRC_Final = std::stoul(sCRC, nullptr, 16);
  CRC_INR = (FLASHTREE.FLASHJSON["CRC-INPUTREVERSE"] == "Y") ? 1 : 0;
  CRC_OTR = (FLASHTREE.FLASHJSON["CRC-OUTPUTREVERSE"] == "Y") ? 1 : 0;
  UDSFLASHMEMORYINFO MEMORY;
  FileHEX_MemoryInfo(MEMORY, FLASHTREE.FLASHJSON["FILE"], CRC_Poly, CRC_Init, CRC_Final, CRC_INR, CRC_OTR);

  std::istringstream STRerRQ(FLASHTREE.FLASHJSON["ERASE-REQ"].get<std::string>());
  std::istringstream STRerRS(FLASHTREE.FLASHJSON["ERASE-RES"].get<std::string>());
  std::istringstream STRcrRQ(FLASHTREE.FLASHJSON["CRC-REQ"].get<std::string>());
  std::istringstream STRcrRS(FLASHTREE.FLASHJSON["CRC-RES"].get<std::string>());
  std::istringstream STRrRQ(FLASHTREE.FLASHJSON["SER-DOWN-REQ"].get<std::string>());
  std::istringstream STRrRS(FLASHTREE.FLASHJSON["SER-DOWN-RES"].get<std::string>());
  std::istringstream STReRQ(FLASHTREE.FLASHJSON["SER-EXIT-REQ"].get<std::string>());
  std::istringstream STReRS(FLASHTREE.FLASHJSON["SER-EXIT-RES"].get<std::string>());
  std::string ByteString; uint8_t C = 0;
  std::vector <std::string> erRQ, crRQ, rRQ, eRQ;
  while (STRerRS >> ByteString) { erREQS.RESMASK.push_back(ByteString); }
  while (STRcrRS >> ByteString) { crREQS.RESMASK.push_back(ByteString); }
  while (STRrRS >> ByteString) { rREQS.RESMASK.push_back(ByteString); }
  while (STReRS >> ByteString) { eREQS.RESMASK.push_back(ByteString); }
  C = 0; while (STRerRQ >> ByteString) { erRQ.push_back(ByteString); if (ByteString == "RR") { C++; } }
  FlashDownloadRequestBuilder(MEMORY, C, erRQ, erREQS.REQ);
  C = 0; while (STRcrRQ >> ByteString) { crRQ.push_back(ByteString); if (ByteString == "RR") { C++; } }
  FlashDownloadRequestBuilder(MEMORY, C, crRQ, crREQS.REQ);
  C = 0; while (STRrRQ >> ByteString) { rRQ.push_back(ByteString); if (ByteString == "RR") { C++; } }
  FlashDownloadRequestBuilder(MEMORY, C, rRQ, rREQS.REQ);
  C = 0; while (STReRQ >> ByteString) { eRQ.push_back(ByteString); if (ByteString == "RR") { C++; } }
  FlashDownloadRequestBuilder(MEMORY, C, eRQ, eREQS.REQ);

  cout << "\n   FLASH DOWNLOAD BLOCK";
  cout << "\n     SERVICE : " << FLASHTREE.FLASHJSON["SERVICE"].get<std::string>();
  cout << "\n     FILE : " << FLASHTREE.FLASHJSON["FILE"].get<std::string>();
  cout << "\n";
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD Coordinates = ConsoleBuffer.dwCursorPosition;
  uint16_t TRANSFERLENGTH = 0;

  DoCAN.MicroDelay(1000);
  if (FLASHTREE.FLASHJSON["ERASE-ENABLE"] == "Y") {
    uint8_t CHECK = ((FLASHTREE.FLASHJSON["ERASE-CHECK"] == "Y") ? 1 : 0);
    cout << "\n     ERASING ECU MEMORY : ";
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    Coordinates = ConsoleBuffer.dwCursorPosition;
    if (FlashExecutor(Coordinates, 11, erREQS.REQ, erREQS.RES, erREQS.RESMASK, 0, CHECK, 0)) {
      int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
      FLAGS.FLASHOUT = 1; return 1;
    }
    ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "DONE"; ConsoleColor();
    SER.REQS.push_back(erREQS);
  } else { SER.REQS.push_back(erREQS); }

  DoCAN.MicroDelay(1000);
  cout << "\n     REQUEST DOWNLOAD : ";
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  Coordinates = ConsoleBuffer.dwCursorPosition;
  if (FlashExecutor(Coordinates, 10, rREQS.REQ, rREQS.RES, rREQS.RESMASK, 0, 1, 0)) {
    int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    FLAGS.FLASHOUT = 1; return 1;
  }
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "DONE"; ConsoleColor();
  for (uint16_t I = 0; I < rREQS.RES.size(); I++) {
    if (rREQS.RESMASK[I] == "TT") {
      uint8_t DATA; StringToHEX (DATA, rREQS.RES[I]);
      TRANSFERLENGTH = (TRANSFERLENGTH << 8) + DATA;
    }
  } SER.REQS.push_back(rREQS);
  if (TRANSFERLENGTH == 0) { FLAGS.FLASHOUT = 1; return 1; }

  DoCAN.MicroDelay(1000);
  cout << "\n     TRANSFERRING DATA . . . ";
  uint8_t PERCENT = FLASHTREE.STARTPERCENT;
  uint8_t DeltaPercent = FLASHTREE.ENDPERCENT - FLASHTREE.STARTPERCENT;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  Coordinates = ConsoleBuffer.dwCursorPosition;

  std::vector<std::string> FILEDATA;
  FileHEX_LoadData (FLASHTREE.FLASHJSON["FILE"], FILEDATA);
  if (FILEDATA.size() != MEMORY.SIZE) { FLAGS.FLASHOUT = 1; return 1; }
  uint32_t FILEPOINTER = 0; uint8_t COUNTER = 1;
  while (FILEPOINTER < MEMORY.SIZE) {
    std::vector<std::string> cREQ, cRES, cRESMASK;
    cREQ.clear(); cRES.clear(); cRESMASK.clear();
    cRESMASK.push_back("76");
    cRESMASK.push_back( HEXToString(COUNTER) );
    cREQ.push_back("36");
    cREQ.push_back( HEXToString(COUNTER) );
    COUNTER++;
    uint16_t I = 0;

    do {
      if (FILEPOINTER == MEMORY.SIZE) { break; }
      else { cREQ.push_back(FILEDATA[FILEPOINTER]); }
      FILEPOINTER++;
      I++;
    } while (I < (TRANSFERLENGTH - 2));

    if (FlashExecutor(Coordinates, 9, cREQ, cRES, cRESMASK, 0, 1, 0)) {
      int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
      FLAGS.FLASHOUT = 1; return 1;
    }
    double Written = ((double)(FILEPOINTER) / (double)(MEMORY.SIZE)) * ((double)DeltaPercent);
    uint8_t TempPercent = (uint8_t)(Written) + FLASHTREE.STARTPERCENT;
    TempPercent = std::min(FLASHTREE.ENDPERCENT, TempPercent);
    if (PERCENT != TempPercent) { PERCENT = TempPercent; RenderFlashPercent(PERCENT); }
  }

  DoCAN.MicroDelay(1000);
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "\n     TRANSFER DATA COMPLETED"; ConsoleColor();
  cout << "\n     REQUEST EXIT : ";
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  Coordinates = ConsoleBuffer.dwCursorPosition;
  if (FlashExecutor(Coordinates, 8, eREQS.REQ, eREQS.RES, eREQS.RESMASK, 0, 1, 0)) {
    int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    FLAGS.FLASHOUT = 1; return 1;
  }  SER.REQS.push_back(eREQS);
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "DONE"; ConsoleColor();

  DoCAN.MicroDelay(1000);
  if (FLASHTREE.FLASHJSON["CRC-ENABLE"] == "Y") {
    uint8_t CHECK = ((FLASHTREE.FLASHJSON["CRC-CHECK"] == "Y") ? 1 : 0);
    cout << "\n     CRC CHECK : ";
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    Coordinates = ConsoleBuffer.dwCursorPosition;
    if (FlashExecutor(Coordinates, 5, crREQS.REQ, crREQS.RES, crREQS.RESMASK, 0, CHECK, 0)) {
      int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
      FLAGS.FLASHOUT = 1; return 1;
    }
    ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "DONE"; ConsoleColor();
    SER.REQS.push_back(crREQS);
  } else { SER.REQS.push_back(crREQS); }

  FLASHBLOCKS.push_back(SER);
  RenderFlashPercent(FLASHTREE.ENDPERCENT);
#ifdef DEVICE_SLOW_FLASHING
  Sleep(500);
#endif
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FlashSECURITY
 * @class       ISO_UDS (Private)
 * @brief       Flash The SECURITY Block
 * @param [FLASHTREE] Flash Tree Vector
 * @param [XY]        Start Console Coordinates
 * @return      Status of Flashing Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FlashSECURITY (UDSFLASHTREE FLASHTREE, COORD XY) {
  DoCAN.PCAN.ResetFIFO();
  UDSFLASHREQS sREQS, kREQS; UDSFLASHSERVICE SER;
  SER.TYPE = FLASHTREE.FLASHJSON["TYPE"]; SER.SERVICE = FLASHTREE.FLASHJSON["SERVICE"];
  uint8_t LEVEL = FLASHTREE.FLASHJSON["LEVEL"];
  sREQS.REQ.push_back( HEXToString(0x27) );
  sREQS.REQ.push_back( HEXToString(LEVEL) );
  ConsoleColor();

  cout << "\n   FLASH SECURITY BLOCK";
  cout << "\n     SERVICE : " << FLASHTREE.FLASHJSON["SERVICE"].get<std::string>();
  cout << "\n";
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;

  if (FlashExecutor(OriginalCoordinates, 14, sREQS.REQ, sREQS.RES, sREQS.RESMASK, 0, 0, 0)) {
    int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    FLAGS.FLASHOUT = 1; return 1;
  }
  UDSAECURITYACCESS SEED, KEY;
  uint8_t LENGTH = sREQS.RES.size(); uint8_t DATA;
  cout << "\n     SEED RECEIVED : ";
  for (uint8_t I = 2; I < LENGTH; I++)
      { cout << " " << sREQS.RES[I]; StringToHEX (DATA, sREQS.RES[I]); SEED.DATA[I - 2] = DATA; }
  SEED.LEVEL = LEVEL; SEED.LENGTH = LENGTH - 2;
  SER.REQS.push_back(sREQS);

  SecurityDLL (FLASHTREE.FLASHJSON["DLL"].get<std::string>(), SEED, KEY);

  DoCAN.MicroDelay(1000);
  kREQS.REQ.push_back( HEXToString(0x27) );
  kREQS.REQ.push_back( HEXToString(LEVEL + 1) );
  cout << "\n     KEY GENERATED : ";
  for (uint8_t I = 0; I < KEY.LENGTH; I++)
      { std::string STR = HEXToString(KEY.DATA[I]); cout << " " << STR; kREQS.REQ.push_back(STR); }

  cout << "\n";
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD NewCoordinates = ConsoleBuffer.dwCursorPosition;
  if (FlashExecutor(NewCoordinates, 10, kREQS.REQ, kREQS.RES, kREQS.RESMASK, 0, 0, 0)) {
    int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    FLAGS.FLASHOUT = 1; return 1;
  }
  SER.REQS.push_back(kREQS);
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "\n\n     ACCESS GRANTED"; ConsoleColor();
  FLASHBLOCKS.push_back(SER);
  RenderFlashPercent(FLASHTREE.ENDPERCENT);
#ifdef DEVICE_SLOW_FLASHING
  Sleep(500);
#endif
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FlashNORMAL
 * @class       ISO_UDS (Private)
 * @brief       Flash The NORMAL Block
 * @param [FLASHTREE] Flash Tree Vector
 * @param [XY]        Start Console Coordinates
 * @return      Status of Flashing Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FlashNORMAL (UDSFLASHTREE FLASHTREE, COORD XY) {
  DoCAN.PCAN.ResetFIFO();
  UDSFLASHREQS REQS; UDSFLASHSERVICE SER;
  SER.TYPE = FLASHTREE.FLASHJSON["TYPE"]; SER.SERVICE = FLASHTREE.FLASHJSON["SERVICE"];
  std::istringstream StreamRESPONSE(FLASHTREE.FLASHJSON["RESPONSE"].get<std::string>());
  std::istringstream StreamREQUEST(FLASHTREE.FLASHJSON["REQUEST"].get<std::string>());
  std::string ByteString;
  ConsoleColor();

  while (StreamRESPONSE >> ByteString) { REQS.RESMASK.push_back(ByteString); }
  while (StreamREQUEST >> ByteString) { REQS.REQ.push_back(ByteString); }
  uint8_t RESSUPPRESS = ((FLASHTREE.FLASHJSON["SUPPRESS"] == "Y") ? 1 : 0);
  uint8_t RESCHECK = ((FLASHTREE.FLASHJSON["CHECK"] == "Y") ? 1 : 0);
  uint8_t ADDRESS = ((FLASHTREE.FLASHJSON["ADDRESS"] == "FUN") ? 1 : 0);

  cout << "\n   FLASH NORMAL BLOCK";
  cout << "\n     SERVICE : " << FLASHTREE.FLASHJSON["SERVICE"].get<std::string>();
  cout << "\n";
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;

  if (FlashExecutor(OriginalCoordinates, 14, REQS.REQ, REQS.RES, REQS.RESMASK, RESSUPPRESS, RESCHECK, ADDRESS)) {
    int KEY = 0; do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    FLAGS.FLASHOUT = 1; return 1;
  }
  cout << "\n     REQUEST MESSAGE :";
  RenderFlashMessage(7, REQS.REQ, 30, 7);
  cout << "\n\n     RESPONSE MESSAGE :";
  RenderFlashMessage(7, REQS.RES, 30, 7);
  SER.REQS.push_back(REQS);
  FLASHBLOCKS.push_back(SER);
  RenderFlashPercent(FLASHTREE.ENDPERCENT);
#ifdef DEVICE_SLOW_FLASHING
  Sleep(500);
#endif
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FlashNOTE
 * @class       ISO_UDS (Private)
 * @brief       Flash The NOTE Block
 * @param [FLASHTREE] Flash Tree Vector
 * @return      Status of Flashing Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FlashNOTE (UDSFLASHTREE FLASHTREE) {
  UDSFLASHREQS R; R.REQ.push_back("NOT ALLOWED"); R.RES.push_back("NOT ALLOWED"); R.RESMASK.push_back("NOT ALLOWED");
  UDSFLASHSERVICE S; S.TYPE = FLASHTREE.FLASHJSON["TYPE"]; S.SERVICE = FLASHTREE.FLASHJSON["SERVICE"];
  S.REQS.push_back(R);
  FLASHBLOCKS.push_back(S);
  RenderFlashPercent(FLASHTREE.ENDPERCENT);
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        CalculateFlashTree
 * @class       ISO_UDS (Private)
 * @brief       Calculate The Flash Tree
 * @param [Filename]  File Path for Flash JSON
 * @param [FLASHTREE] Flash Tree Vector
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::FlashECU (const std::string& Filename) {
  FLASHBLOCKS.clear(); FLASHTREES.clear();
  CalculateFlashTree(Filename, FLASHTREES);
  int FLASHTOTALBLOCKS = FLASHTREES.size();
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n FLASHING ECU USING UNIVERSAL UDS";
  cout << "\n FLASH JSON : " << Filename << "\n";
  ConsoleColor();
  RenderFlashPercent(0);
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer; DWORD Written;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  DoCAN.PCAN.ResetFIFO();

  for (int I = 0; I < FLASHTOTALBLOCKS; I++) {
    DoCAN.MicroDelay(FLASHTREES[I].WAIT);
    SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
    for (int I = 0; I < 18; I++) {
        COORD NewLine = { 0, static_cast<SHORT>(OriginalCoordinates.Y + I) };
        FillConsoleOutputCharacter(ConsoleHandle, ' ', ConsoleBuffer.dwSize.X, NewLine, &Written);
        FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, ConsoleBuffer.dwSize.X, NewLine, &Written);
    } SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);

    if (FLASHTREES[I].FLASHJSON["TYPE"] == "NOTE") {
      if (FlashNOTE(FLASHTREES[I])) { FLAGS.FLASHOUT = 1; return; }
    }
    else if (FLASHTREES[I].FLASHJSON["TYPE"] == "NORMAL") {
      if (FlashNORMAL(FLASHTREES[I], OriginalCoordinates)) { FLAGS.FLASHOUT = 1; return; }
    }
    else if (FLASHTREES[I].FLASHJSON["TYPE"] == "SECURITY") {
      if (FlashSECURITY(FLASHTREES[I], OriginalCoordinates)) { FLAGS.FLASHOUT = 1; return; }
    }
    else if (FLASHTREES[I].FLASHJSON["TYPE"] == "DOWNLOAD") {
      if (FlashDOWNLOAD(FLASHTREES[I], OriginalCoordinates)) { FLAGS.FLASHOUT = 1; return; }
    }
    else { FLAGS.FLASHOUT = 1; return; }
  }

  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
    for (int I = 0; I < 18; I++) {
        COORD NewLine = { 0, static_cast<SHORT>(OriginalCoordinates.Y + I) };
        FillConsoleOutputCharacter(ConsoleHandle, ' ', ConsoleBuffer.dwSize.X, NewLine, &Written);
        FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, ConsoleBuffer.dwSize.X, NewLine, &Written);
    }
  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
  ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "\n   FLASH COMPLETED";
  RenderFlashPercent(100);
  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA);
  cout << "\n\n\n\n\n   BACK";
  ConsoleColor();
  cout << "   REPORT";
  int KEY = 0;
  do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
  FLAGS.FLASHOUT = 1;
}
/* ==================================================================================================== */






































// MARK: Intel HEX Handler
/* ==================================================================================================== */
/**
 * @name        FileHEX_Validate
 * @class       ISO_UDS (Private)
 * @brief       Validate .hex files for Corruption or Tampering
 * @param [Filename]      File Path
 * @return      Active Low Validation Status
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FileHEX_Validate (const std::string& Filename) {
  std::ifstream File(Filename);
  if (!File.is_open()) { return 2; }
  std::string Line;
  while (std::getline(File, Line)) {
    if (Line.empty() || Line[0] != ':') { return 1; }
    Line = Line.substr(1);
    if (Line.length() < 10) { return 1; }
    std::vector<uint8_t> Bytes;
    for (size_t i = 0; i < Line.length(); i += 2) {
        std::string ByteStr = Line.substr(i, 2);
        uint8_t Byte = static_cast<uint8_t>(std::stoi(ByteStr, nullptr, 16));
        Bytes.push_back(Byte);
    }
    uint8_t Sum = 0;
    for (size_t i = 0; i < Bytes.size(); i++) { Sum += Bytes[i]; }
    if (Sum != 0) { return 1; }
  } return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FileHEX_CRC16
 * @class       ISO_UDS (Private)
 * @brief       Calculate Standard 16 Bit CRC of .hex files
 * @param [Filename]      File Path
 * @param [Polynomial]    Polynomial 16 Bit
 * @param [InitialValue]  Initial Value Starting 16 Bit
 * @param [FinalValue]    Final Value XOR 16 Bit
 * @param [InputConvert]  Input Reversing of Data Active High
 * @param [OutputConvert] Final Reversing of Data Active High
 * @return      16 Bit CRC
 */
/* ---------------------------------------------------------------------------------------------------- */
uint16_t ISO_UDS::FileHEX_CRC16 (const std::string& Filename, uint16_t Polynomial, uint16_t InitialValue,
    uint16_t FinalValue, uint8_t InputConvert, uint8_t OutputConvert) {
  std::ifstream File(Filename);
  std::string Line;
  uint16_t CRC = InitialValue;

  while (std::getline(File, Line)) {
    if (Line.empty() || Line[0] != ':') continue;
    Line = Line.substr(1);
    if (Line.length() < 10) { continue; }

    uint8_t ByteCount = std::stoi(Line.substr(0, 2), nullptr, 16);
    uint8_t RecordType = std::stoi(Line.substr(6, 2), nullptr, 16);
    if (RecordType != 0x00) { continue; }

    for (size_t i = 0; i < ByteCount; ++i) {
      std::string ByteSTR = Line.substr(8 + i * 2, 2);
      uint8_t Byte = static_cast<uint8_t>(std::stoi(ByteSTR, nullptr, 16));
      if (InputConvert) {
        uint8_t Reversed = 0;
        for (int i = 0; i < 8; ++i) { Reversed <<= 1; Reversed |= (Byte & 1); Byte >>= 1; }
        Byte = Reversed;
      }
      CRC ^= static_cast<uint16_t>(Byte) << 8;
      for (int j = 0; j < 8; ++j) {
        if (CRC & 0x8000) { CRC = (CRC << 1) ^ Polynomial; }
        else { CRC <<= 1; }
      }
    }
  }

  File.close();
  CRC ^= FinalValue;
  if (OutputConvert) {
    uint16_t Reversed = 0;
    for (int i = 0; i < 16; ++i) { Reversed <<= 1; Reversed |= (CRC & 1); CRC >>= 1; }
    CRC = Reversed;
  }
  return CRC;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FileHEX_MemoryInfo
 * @class       ISO_UDS (Private)
 * @brief       Extract Flashing Information From .hex files
 * @param [INFO]          Information Structure
 * @param [Filename]      File Path
 * @param [Polynomial]    Polynomial 16 Bit
 * @param [InitialValue]  Initial Value Starting 16 Bit
 * @param [FinalValue]    Final Value XOR 16 Bit
 * @param [InputConvert]  Input Reversing of Data Active High
 * @param [OutputConvert] Final Reversing of Data Active High
 * @return      16 Bit CRC
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::FileHEX_MemoryInfo (UDSFLASHMEMORYINFO &INFO, const std::string& Filename, uint16_t Polynomial,
    uint16_t InitialValue, uint16_t FinalValue, uint8_t InputConvert, uint8_t OutputConvert) {
  std::ifstream File(Filename);
  if (!File.is_open()) { return 1; }

  std::string Line;
  uint32_t ExtendedLinearAddress = 0;
  uint32_t LowestAddress = std::numeric_limits<uint32_t>::max();
  uint32_t HighestAddress = 0;
  uint32_t TotalDataBytes = 0;

  while (std::getline(File, Line)) {
    if (Line.empty() || Line[0] != ':') { continue; }
    Line = Line.substr(1);
    if (Line.length() < 10) continue;

    uint8_t ByteCount = std::stoi(Line.substr(0, 2), nullptr, 16);
    uint16_t Address = std::stoi(Line.substr(2, 4), nullptr, 16);
    uint8_t RecordType = std::stoi(Line.substr(6, 2), nullptr, 16);

    if (RecordType == 0x04) {
      uint16_t UpperAddress = std::stoi(Line.substr(8, 4), nullptr, 16);
      ExtendedLinearAddress = static_cast<uint32_t>(UpperAddress) << 16;
    } else if (RecordType == 0x00) {
      uint32_t FullAddress = ExtendedLinearAddress | Address;
      if (FullAddress < LowestAddress) { LowestAddress = FullAddress; }
      uint32_t EndOfLineAddress = FullAddress + ByteCount - 1;
      if (EndOfLineAddress > HighestAddress) { HighestAddress = EndOfLineAddress; }
      TotalDataBytes += ByteCount;
    }
  }
  if (LowestAddress > HighestAddress) { return 1; }
  INFO.ADDRESSSTART = LowestAddress;
  INFO.ADDRESSEND = HighestAddress;
  INFO.SIZE = TotalDataBytes;
  File.close();
  uint16_t CRC = FileHEX_CRC16(Filename, Polynomial, InitialValue, FinalValue, InputConvert, OutputConvert);
  INFO.CRC = CRC;
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FileHEX_LoadData
 * @class       ISO_UDS (Private)
 * @brief       Load Data in Vector From .hex files
 * @param [Filename]  File Path
 * @param [Data]      Data Vector Reference
 * @return      16 Bit CRC
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::FileHEX_LoadData (const std::string& Filename, std::vector<std::string>& Data) {
  std::ifstream File(Filename);
  if (!File.is_open()) { return; }
  std::string Line;
  Data.clear();

  while (std::getline(File, Line)) {
    if (Line.empty() || Line[0] != ':') { continue; }
    Line = Line.substr(1);
    if (Line.length() < 10) { continue; }
    uint8_t ByteCount = std::stoi(Line.substr(0, 2), nullptr, 16);
    uint8_t RecordType = std::stoi(Line.substr(6, 2), nullptr, 16);
    if (RecordType == 0x04) {
      continue;
    }
    if (RecordType != 0x00) { continue; }
    for (size_t i = 0; i < ByteCount; i++) {
      std::string ByteSTR = Line.substr(8 + i * 2, 2);
      Data.push_back(ByteSTR);
    }
  }
}
/* ==================================================================================================== */

















// MARK: Initial Page Handling
/* ==================================================================================================== */
/**
 * @name        Application
 * @class       ISO_UDS (Public)
 * @brief       Application that Operates and Renders UI
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::Application (void) {
  if (FLAGS.INIT_PAGE) {
    FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
    HandlerInitialPage();
  }
  else if (FLAGS.DASHBOARD) {
    FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
    HandlerDiagnosticsPage(NORMALDASH);
  }
  else if (FLAGS.PLAYLIST) {
    FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0;
    HandlerPlaylist();
  }
  else if (FLAGS.FLASH) {
    FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 1; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
    HandlerFlashList();
  }
  else if (FLAGS.FAULT) {
    FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 1; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
    RenderWorkInPorgress();
  }
  else if (FLAGS.EXIT) {
    FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 1;
    RenderExitPage();
  }
  else {
    FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
    HandlerInitialPage();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HandlerInitialPage
 * @class       ISO_UDS (Private)
 * @brief       Handle Initial Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerInitialPage (void) {
  int TransitionFlag = 0;
  UI.INIT = CalculatePosition(UI.INIT, ARROWCALIBRATE);
  RenderInitialPage();
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              UI.INIT = CalculatePosition(UI.INIT, ARROWUP);
              RenderInitialPage(); break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              UI.INIT = CalculatePosition(UI.INIT, ARROWDOWN);
              RenderInitialPage(); break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        switch (UI.INIT.SELECTED) {
          case 0 : { // Diagonostics Dashboard
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 1 : { // Flash Dashboard
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 1; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 2 : { // Fault Manager
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 1; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 3 : { // View CAN UDS Traffic
            HandlerViewTraffic();
            FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 4 : { // View CAN UDS Traffic
            HandlerTrace();
            FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 5 : { // Settings
            RenderSettings();
            FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            TransitionFlag = 1; break;
          }
          case 6 : { // Exit
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 1;
            TransitionFlag = 1; break;
          }
          default : { // Rare Condition
            FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            UI.INIT.SELECTED = 0; break;
          }
        }
      }
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (TransitionFlag == 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderInitialPage
 * @class       ISO_UDS (Private)
 * @brief       Render Initial Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderInitialPage (void) {
  UI.INIT.TOTAL = 7; UI.INIT.PAGE = 7; UI.INIT.END = 7; UI.INIT.START = 0;
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n SELECT WHICH MODE TO START :\n";
  ConsoleColor();
  RenderSelector((UI.INIT.SELECTED == 0), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Diagonostics Dashboard";
  RenderSelector((UI.INIT.SELECTED == 1), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Flash ECU Dashboard";
  RenderSelector((UI.INIT.SELECTED == 2), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Fault Manager";
  cout << "\n";
  RenderSelector((UI.INIT.SELECTED == 3), CONSOLECOLOUR_LIGHTAQUA);
  cout << "View UDS Traffic";
  RenderSelector((UI.INIT.SELECTED == 4), CONSOLECOLOUR_LIGHTAQUA);
  cout << "View CAN Trace";
  cout << "\n";
  RenderSelector((UI.INIT.SELECTED == 5), CONSOLECOLOUR_LIGHTAQUA);
  if (CONFIG.LOGGINGMODE) { cout << "Turn OFF Logging Mode"; }
  else { cout << "Turn ON Logging Mode"; }

  RenderSelector((UI.INIT.SELECTED == 6), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Exit";
  cout << "\n\n\n\n";

  ConsoleColor(CONSOLECOLOUR_GRAY);
    cout << "\n KEY BINDING :\n\n ";
  ConsoleColor(CONSOLECOLOUR_BLACK, CONSOLECOLOUR_GRAY);
    cout << " ENTER ";
  ConsoleColor(CONSOLECOLOUR_GRAY);
    cout << "     : SELECT OPTION\n\n ";
  ConsoleColor(CONSOLECOLOUR_BLACK, CONSOLECOLOUR_GRAY);
    cout << " BACKSPACE ";
  ConsoleColor(CONSOLECOLOUR_GRAY);
    cout << " : GO BACK\n\n ";
  ConsoleColor(CONSOLECOLOUR_BLACK, CONSOLECOLOUR_GRAY);
    cout << " ESC ";
  ConsoleColor(CONSOLECOLOUR_GRAY);
    cout << "       : EXIT";
  ConsoleColor();
}
/* ==================================================================================================== */


// MARK: Diagnostics Dashboard Page
/* ==================================================================================================== */
/**
 * @name        HandlerDiagnosticsPage
 * @class       ISO_UDS (Private)
 * @brief       Handle Diagnostics Dashboard Page
 * @param [Add]     Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerDiagnosticsPage (CONFIGATIONS Add) {
  int TransitionFlag = 0;
  UI.DASH = CalculatePosition(UI.DASH, ARROWCALIBRATE);
  RenderDiagnosticsPage(Add);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              UI.DASH = CalculatePosition(UI.DASH, ARROWUP);
              RenderDiagnosticsPage(Add); break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              UI.DASH = CalculatePosition(UI.DASH, ARROWDOWN);
              RenderDiagnosticsPage(Add); break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (UI.DASH.SELECTED == 0) { // PLAYLIST
          FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0;
          FLAGS.PLAYOUT = 1; return;
        }
        else if (UI.DASH.SELECTED == 1) { // SECURITY ACCESS
            HandlerSecurityAccess(Add);
            if (Add) {
              if (FLAGS.PLAYOUT) {
                FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0;
                FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0; return;
              } else { RenderDiagnosticsPage(Add); continue; }
            } else { RenderDiagnosticsPage(Add); continue; }
        }
        else if ((UI.DASH.SELECTED > 1) && (UI.DASH.SELECTED <= (UI.DASH.TOTAL - 4))){ // LIST
            HandlerDiagnosticsServicePage((UI.DASH.SELECTED - 1), Add);
            if (Add) {
              if (FLAGS.PLAYOUT) {
                FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0;
                FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0; return;
              } else { RenderDiagnosticsPage(Add); continue; }
            } else { RenderDiagnosticsPage(Add); continue; }
        }
        else if (UI.DASH.SELECTED == (UI.DASH.TOTAL - 3)) { // RAW SERVICE
            RawServiceBuilder("", "Raw Service", Add, 0);
            if (Add) {
              if (FLAGS.PLAYOUT) {
                FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0;
                FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0; return;
              } else { RenderDiagnosticsPage(Add); continue; }
            } else { RenderDiagnosticsPage(Add); continue; }
        }
        else if (UI.DASH.SELECTED == (UI.DASH.TOTAL - 2)) { // BACK
            if (Add) {
              FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0;
              FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0; return;
            } else {
              FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
              TransitionFlag = 1; return;
            }
        }
        else if (UI.DASH.SELECTED == (UI.DASH.TOTAL - 1)) { RenderExitPage(); return; } // EXIT
        else {  // RARE
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            UI.DASH.SELECTED = 0;
            break;
        }
      }
      else if (KEY == CONBACKSPACE) { // BACKSPACE KEY
        if (Add) {
          FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0;
          FLAGS.PLAYLIST = 1; FLAGS.EXIT = 0; return;
        } else {
          FLAGS.INIT_PAGE = 1; FLAGS.DASHBOARD = 0; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
          TransitionFlag = 1; return;
        }
      }
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (TransitionFlag == 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderDiagnosticsPage
 * @class       ISO_UDS (Private)
 * @brief       Render Diagnostics Dashboard Page
 * @param [Add]         Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderDiagnosticsPage (CONFIGATIONS Add) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n SELECT FROM THE FOLLOWING LIST :\n";
  ConsoleColor();
  RenderSelector((UI.DASH.SELECTED == 0), CONSOLECOLOUR_LIGHTAQUA);
  if (Add) { cout << "BACK TO PLAYLIST"; }
  else { cout << "SHOW PLAYLIST"; }
  cout << "\n";
  int RenderRange = min((UI.DASH.TOTAL - 4), (UI.DASH.PAGE));
  int RenderStart, RenderEnd;
  if (UI.DASH.START < 2) { RenderStart = 1; RenderEnd = RenderStart + RenderRange; }
  else if (UI.DASH.END > (UI.DASH.TOTAL - 3)) { RenderEnd = UI.DASH.TOTAL  - 3; RenderStart = RenderEnd - RenderRange; }
  else { RenderStart = UI.DASH.START; RenderEnd = RenderStart + RenderRange; }
  RenderDebugVariableList (UI.DASH, RenderStart, RenderEnd, RenderRange);
  int SerPOS = 1;
  for (const auto& SER : SERVICEDIRECTORY) {
    if ((SerPOS >= RenderStart) && (SerPOS < RenderEnd)) {
      RenderSelector((UI.DASH.SELECTED == SerPOS), CONSOLECOLOUR_LIGHTAQUA);
      cout << "  " << SER.SID << " : " << SER.SERVICE;
    }
    SerPOS++;
  }
  cout << "\n";
  RenderSelector((UI.DASH.SELECTED == (UI.DASH.TOTAL - 3)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "RAW SERVICE";
  cout << "\n";
  RenderSelector((UI.DASH.SELECTED == (UI.DASH.TOTAL - 2)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  RenderSelector((UI.DASH.SELECTED == (UI.DASH.TOTAL - 1)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "EXIT";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HandlerDiagnosticsServicePage
 * @class       ISO_UDS (Private)
 * @brief       Handle Diagnostics Service Specific Dashboard Page
 * @param [Service]     Service Index
 * @param [Add]         Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerDiagnosticsServicePage (uint8_t Service, CONFIGATIONS Add) {
  int TransitionFlag = 0;
  int RequestSize = 0;
  UDSLIST SF;
  SF.TOTAL = 0; SF.PAGE = 12;
  for (const auto& FUN : SERVICEDIRECTORY[Service].FUNCTIONS) {
    SF.TOTAL++;
    int Length = FUN.REQUEST.length();
    if (Length > RequestSize) { RequestSize = Length; }
  }
  SF.TOTAL += 3; SF.END = SF.TOTAL; SF.START = 0; SF.SELECTED = 0;
  SF = CalculatePosition (SF, ARROWCALIBRATE);
  RenderDiagnosticsServicePage(SF, Service, RequestSize);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              SF = CalculatePosition (SF, ARROWUP);
              RenderDiagnosticsServicePage(SF, Service, RequestSize);
              break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              SF = CalculatePosition (SF, ARROWDOWN);
              RenderDiagnosticsServicePage(SF, Service, RequestSize);
              break;
            }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (SF.SELECTED == 0) { // RAW SUB FUNCTION
          RawServiceBuilder(SERVICEDIRECTORY[Service].SID, SERVICEDIRECTORY[Service].SERVICE, Add, 0);
          if (Add) {
            if (FLAGS.PLAYOUT == 0) {
              RenderDiagnosticsServicePage(SF, Service, RequestSize); continue;
            } else { return; }
          } else { RenderDiagnosticsServicePage(SF, Service, RequestSize); continue; }
        }
        else if ((SF.SELECTED > 0) && (SF.SELECTED <= (SF.TOTAL - 3))){  // LIST
          ServiceExecuter (SERVICEDIRECTORY[Service].FUNCTIONS[SF.SELECTED - 1].REQUEST,
              SERVICEDIRECTORY[Service].FUNCTIONS[SF.SELECTED - 1].FUNCTION, Add);
          if (Add) {
            if (FLAGS.PLAYOUT == 0) {
              RenderDiagnosticsServicePage(SF, Service, RequestSize); continue;
            } else { return; }
          } else { RenderDiagnosticsServicePage(SF, Service, RequestSize); continue; }
        }
        else if (SF.SELECTED == (SF.TOTAL - 2)) { TransitionFlag = 1; return; } // BACK
        else if (SF.SELECTED == (SF.TOTAL - 1)) { RenderExitPage(); return; } // EXIT
        else { SF.SELECTED = 0; break; } // RARE
      }
      else if (KEY == CONBACKSPACE) { TransitionFlag = 1; return; } // BACKSPACE KEY
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (TransitionFlag == 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderDiagnosticsServicePage
 * @class       ISO_UDS (Private)
 * @brief       Render Diagnostics Service Specific Page
 * @param [DIAG]          Diagnostics UI List Structure
 * @param [SID]           Service Index
 * @param [RequestSize]   Mazimum Request String Size in List
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderDiagnosticsServicePage (UDSLIST DIAG, uint8_t SID, uint8_t RequestSize) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n SELECT FROM THE FOLLOWING LIST :\n";
  ConsoleColor();
  RenderSelector((DIAG.SELECTED == 0), CONSOLECOLOUR_LIGHTAQUA);
  cout << "RAW SUB-FUNCTION";
  cout << "\n";
  int RenderRange = min((DIAG.TOTAL - 3), (DIAG.PAGE));
  int RenderStart, RenderEnd;
  if (DIAG.START < 2) { RenderStart = 1; RenderEnd = RenderStart + RenderRange; }
  else if (DIAG.END > (DIAG.TOTAL - 2)) { RenderEnd = DIAG.TOTAL - 2; RenderStart = RenderEnd - RenderRange; }
  else { RenderStart = DIAG.START; RenderEnd = RenderStart + RenderRange; }
  RenderDebugVariableList (DIAG, RenderStart, RenderEnd, RenderRange);
  int SerPOS = 1;
  for (const auto& FUN : SERVICEDIRECTORY[SID].FUNCTIONS) {
    if ((SerPOS >= RenderStart) && (SerPOS < RenderEnd)) {
      RenderSelector((DIAG.SELECTED == SerPOS), CONSOLECOLOUR_LIGHTAQUA);
      cout << "  " << FUN.REQUEST;
      int Length = FUN.REQUEST.length();
      for (int I = 0; I < (RequestSize - Length); I++) { cout << " "; }
      cout << " : " << FUN.FUNCTION;
    }
    SerPOS++;
  }
  cout << "\n";
  RenderSelector((DIAG.SELECTED == SerPOS), CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  SerPOS++;
  RenderSelector((DIAG.SELECTED == SerPOS), CONSOLECOLOUR_LIGHTAQUA);
  cout << "EXIT";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HandlerSecurityAccess
 * @class       ISO_UDS (Private)
 * @brief       Handle Security Access Dashboard Page
 * @param [Add]     Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerSecurityAccess (CONFIGATIONS Add) {
  int TransitionFlag = 0;
  UDSLIST SEC;
  SEC.PAGE = 10; SEC.TOTAL = 9; SEC.END = SEC.TOTAL; SEC.START = 0; SEC.SELECTED = 0;
  SEC = CalculatePosition (SEC, ARROWCALIBRATE);
  RenderSecurityAccess(SEC);
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
          case CONARROWUP : { // ARROW UP KEY
            SEC = CalculatePosition (SEC, ARROWUP); RenderSecurityAccess(SEC); break;
          }
          case CONARROWDOWN : { // ARROW DOWN KEY
            SEC = CalculatePosition (SEC, ARROWDOWN); RenderSecurityAccess(SEC); break;
          }
        }
      }
      else if (KEY == CONENTER) { // ENTER KEY
        if (SEC.SELECTED == 0) { // RAW SUB FUNCTION
          RawServiceBuilder("27", "Security Access Raw", Add, 0);
          if (Add) {
            if (FLAGS.PLAYOUT == 0) {
              RenderSecurityAccess(SEC); continue;
            } else { return; }
          } else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 1) { // Programming Seed & Key
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Programming", 0, 1, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 2) { // Enhanced Seed & Key
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Enhanced", 0, 3, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 3) { // Programming Seed
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Programming Request Seed", 1, 1, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 4) { // Programming Key
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Programming Send Key", 2, 2, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 5) { // Enhanced Seed
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Enhanced Request Seed", 1, 3, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 6) { // Enhanced Key
          ExecuteSecurityAccess (CONFIG.DLLPATH, "Enhanced Send Key", 2, 4, Add);
          if (Add) { if (FLAGS.PLAYOUT == 0) { RenderSecurityAccess(SEC); continue; } else { return; } }
          else { RenderSecurityAccess(SEC); continue; }
        }
        else if (SEC.SELECTED == 7) { TransitionFlag = 1; return; } // BACK
        else if (SEC.SELECTED == 8) { RenderExitPage(); return; } // EXIT
        else { SEC.SELECTED = 0; break; } // RARE
      }
      else if (KEY == CONBACKSPACE) { TransitionFlag = 1; return; } // BACKSPACE KEY
      else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
    }
  } while (TransitionFlag == 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderSecurityAccess
 * @class       ISO_UDS (Private)
 * @brief       Render Security Access Dashboard Page
 * @param [SEC]   Diagnostics UI List Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderSecurityAccess (UDSLIST SEC) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n SELECT FROM THE FOLLOWING LIST :\n";
  ConsoleColor();
  RenderSelector((SEC.SELECTED == 0), CONSOLECOLOUR_LIGHTAQUA);
  cout << "RAW SUB-FUNCTION";
  cout << "\n";
  RenderSelector((SEC.SELECTED == 1), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Programming Seed & Key (1:2)";
  RenderSelector((SEC.SELECTED == 2), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Enhanced Seed & Key (3:4)";
  cout << "\n";
  RenderSelector((SEC.SELECTED == 3), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Programming Request Seed (1)";
  RenderSelector((SEC.SELECTED == 4), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Programming Send Key (2)";
  RenderSelector((SEC.SELECTED == 5), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Enhanced Request Seed (3)";
  RenderSelector((SEC.SELECTED == 6), CONSOLECOLOUR_LIGHTAQUA);
  cout << "Enhanced Send Key (4)";
  cout << "\n";
  RenderSelector((SEC.SELECTED == 7), CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  RenderSelector((SEC.SELECTED == 8), CONSOLECOLOUR_LIGHTAQUA);
  cout << "EXIT";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ServiceExecuter
 * @class       ISO_UDS (Private)
 * @brief       Service Executer or Passing To Save Playlist
 * @param [REQUEST]     Request Message String
 * @param [SERVICE]     Service Description String
 * @param [Add]         Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ServiceExecuter (const std::string& REQUEST, const std::string& SERVICE, uint8_t Add) {
  if (Add) { SaveToPlaylist (REQUEST, SERVICE, 0); }
  else {
    ClearScreen();
    RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
    RenderHeader();
    int Length = 0, SID = 0, KEY = 0;
    std::istringstream StreamREQUEST(REQUEST);
    std::string ByteString;
    uint8_t Data = 0;
    UDSREQRES REQ;
    while (StreamREQUEST >> ByteString) {
      StringToHEX(Data, ByteString);
      MESSAGE.DATA[Length] = Data;
      REQ.DATA.push_back(Data);
      Length++;
    }
    MESSAGE.LEN = Length;
    REQ.LEN = Length;
    SID = MESSAGE.DATA[0];

    cout << "\n\n REQUEST EXECUTOR : ";
    cout << "\n\n SERVICE : " << SERVICE;
    cout << "\n\n REQUEST MESSAGE :";
    RequestRender(REQ, 4); ConsoleColor();
    DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_PHY);
    std::string ErrorLine;
    int Result = Executor(1, ErrorLine);
    if (Result) {
      cout << "\n     " << ErrorLine;
      ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK";
      ConsoleColor();
      do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
    } else {
      if (MESSAGE.DATA[0] == (SID + 0x40)) {
        cout << "\n\n RESPONSE : ";
        ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "POSITIVE";
        ConsoleColor();
      } else if (MESSAGE.DATA[0] == 0x7F) {
        cout << "\n\n RESPONSE : ";
        ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "NEGATIVE" << NegativeResponse(MESSAGE.DATA[2]);
        ConsoleColor();
      } else {
        cout << "\n\n RESPONSE : ";
        ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "UPEXPECTED";
        ConsoleColor();
      }
      cout << "\n";
      HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
      GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
      COORD ResPoint = ConsoleBuffer.dwCursorPosition;
      DWORD CellsToClear = (ConsoleBuffer.dwSize.Y - ResPoint.X) * ConsoleBuffer.dwSize.X;
      UDSLIST RUI; UDSREQRES RES; DWORD CellsCleared;
      if (MESSAGE.LEN % UI.REQRESWIDHTH) { RUI.TOTAL = (MESSAGE.LEN / UI.REQRESWIDHTH) + 1; }
      else { RUI.TOTAL = (MESSAGE.LEN / UI.REQRESWIDHTH); }
      RUI.PAGE = UI.RESHEIGHT; RUI.START = 0; RUI.END = RUI.PAGE; RUI.SELECTED = 0;
      RUI = ListCalculatePosition(RUI, ARROWCALIBRATE);
      for (int I = 0; I < MESSAGE.LEN; I++) { RES.DATA.push_back(MESSAGE.DATA[I]); }
      RES.LEN = MESSAGE.LEN;
      FillConsoleOutputCharacter(ConsoleHandle, ' ', CellsToClear, ResPoint, &CellsCleared);
      FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, CellsToClear, ResPoint, &CellsCleared);
      ResponseRender(RES, RUI, 4); ConsoleColor();
      ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK";
      ConsoleColor();
      do {
        if(_kbhit()) {
          KEY = _getch();
          if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
            KEY = _getch();
            switch(KEY) {
                case CONARROWUP : { // ARROW UP KEY
                  RUI = ListCalculatePosition(RUI, ARROWUP);
                  SetConsoleCursorPosition(ConsoleHandle, ResPoint);
                  FillConsoleOutputCharacter(ConsoleHandle, ' ', CellsToClear, ResPoint, &CellsCleared);
                  FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, CellsToClear,
                    ResPoint, &CellsCleared);
                  ResponseRender(RES, RUI, 4); ConsoleColor();
                  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK";
                  ConsoleColor(); break;
                }
                case CONARROWDOWN : { // ARROW DOWN KEY
                  RUI = ListCalculatePosition(RUI, ARROWDOWN);
                  SetConsoleCursorPosition(ConsoleHandle, ResPoint);
                  FillConsoleOutputCharacter(ConsoleHandle, ' ', CellsToClear, ResPoint, &CellsCleared);
                  FillConsoleOutputAttribute(ConsoleHandle, ConsoleBuffer.wAttributes, CellsToClear,
                    ResPoint, &CellsCleared);
                  ResponseRender(RES, RUI, 4); ConsoleColor();
                  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK";
                  ConsoleColor(); break;
                }
            }
          }
        }
      } while (KEY != CONENTER);
    }
  }
}
/* ==================================================================================================== */


// MARK: Playlist
/* ==================================================================================================== */
/**
 * @name        HandlerPlaylist
 * @class       ISO_UDS (Private)
 * @brief       Handle Playlist Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerPlaylist (void) {
  int TransitionFlag = 0;
  UI.PLAY.START = 0; UI.PLAY.END = 0; UI.PLAY.TOTAL = PLAYLIST.size() + 4;
  UI.PLAY = CalculatePosition(UI.PLAY, ARROWCALIBRATE);
  RenderPlaylistPage();
  do {
    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == 224 || KEY == 0) {  // Special Key Prefix
        KEY = _getch();
        switch(KEY) {
            case 72 : {  // UP ARROW KEY
              UI.PLAY = CalculatePosition(UI.PLAY, ARROWUP);
              RenderPlaylistPage();
              break;
            }
            case 80 : {  // DOWN ARROW KEY
              UI.PLAY = CalculatePosition(UI.PLAY, ARROWDOWN);
              RenderPlaylistPage();
              break;
            }
        }
      }
      else if (KEY == 13) {  // ENTER KEY
          if (UI.PLAY.SELECTED < (UI.PLAY.TOTAL - 4)) {  // PLAYLIST ITEMS

          }
          else if (UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 4)) {  // ADD SERVICES
            FLAGS.PLAYOUT = 0;
            HandlerDiagnosticsPage(PLAYLISTDASH);
            TransitionFlag = 1;
            return;
          }
          else if (UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 3)) {  // PLAY

          }
          else if (UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 2)) {  // BACK
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            FLAGS.PLAYOUT = 0; TransitionFlag = 1;
          }
          else if (UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 1)) {  // EXIT
            RenderExitPage(); return;
          }
          else {  // RARE
            FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
            UI.PLAY.SELECTED = 0; FLAGS.PLAYOUT = 0; break;
          }
      }
      else if (KEY == 8) {  // BACKSPACE KEY
        FLAGS.INIT_PAGE = 0; FLAGS.DASHBOARD = 1; FLAGS.FLASH = 0; FLAGS.FAULT = 0; FLAGS.PLAYLIST = 0; FLAGS.EXIT = 0;
        FLAGS.PLAYOUT = 0; TransitionFlag = 1;
      }
      else if (KEY == 27) {  // ESCAPE KEY
        RenderExitPage(); return;
      }
    }
  } while (TransitionFlag == 0);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderPlaylistPage
 * @class       ISO_UDS (Private)
 * @brief       Render Playlist Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderPlaylistPage (void) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n SERVICE PLAYLIST :\n";
  cout << "\n     REQUEST              : DESCRIPTION                    : ADD : SPR : RESPONSE CHECK\n";
  ConsoleColor();

  int RenderRange = min((UI.PLAY.TOTAL - 4), (UI.PLAY.PAGE));
  int RenderStart, RenderEnd;
  if (UI.PLAY.END > (UI.PLAY.TOTAL - 4)) { RenderEnd = UI.PLAY.TOTAL  - 4; RenderStart = RenderEnd - RenderRange; }
  else { RenderStart = UI.PLAY.START; RenderEnd = RenderStart + RenderRange; }
  RenderDebugVariableList (UI.PLAY, RenderStart, RenderEnd, RenderRange);

  int SerPOS = 0;
  for (const auto& PLAY : PLAYLIST) {
    if ((SerPOS >= RenderStart) && (SerPOS < RenderEnd)) {
      RenderSelector((UI.PLAY.SELECTED == SerPOS), CONSOLECOLOUR_LIGHTAQUA);
      cout << PlayListStrings(PLAY.REQUEST, 20) << " : ";
      cout << PlayListStrings(PLAY.FUNCTION, 30) << " : ";
      cout << ((PLAY.ADDRESS == "P") ? "PHY" : "FUN") << " : ";
      cout << PLAY.SUPPRESS << "   : ";
      cout << PlayListStrings(PLAY.RESPONSE, 20);
    }
    SerPOS++;
  }

  cout << "\n";
  RenderSelector((UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 4)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "ADD SERVICE";
  cout << "\n";
  RenderSelector((UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 3)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "PLAY";
  cout << "\n";
  RenderSelector((UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 2)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  RenderSelector((UI.PLAY.SELECTED == (UI.PLAY.TOTAL - 1)), CONSOLECOLOUR_LIGHTAQUA);
  cout << "EXIT";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        PlayListStrings
 * @class       ISO_UDS (Private)
 * @brief       Standardise Strings to Perticular Length or Trim to that Length
 * @param [Input]     String
 * @param [Length]    Length of Standardization
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
std::string ISO_UDS::PlayListStrings (const std::string& Input, size_t Length) {
  if (Input.length() > Length) {
    return Input.substr(0, Length - 2) + "..";
  } else {
    return Input + std::string(Length - Input.length(), ' ');
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SaveToPlaylist
 * @class       ISO_UDS (Private)
 * @brief       Saves The Item In The Playlist Vector
 * @param [REQUEST]     Request Message String
 * @param [SERVICE]     Service Description String
 * @param [Issue]       Issue Detector for Recursion
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::SaveToPlaylist (const std::string& REQUEST, const std::string& SERVICE, uint8_t Issue) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  int ENTER = 0, PRESS = 0, ADD = 0, CHECK = 0, SUPPRESS = 0, FIRST = 0, SAVE = 0;
  std::string RESPONSE;

  cout << "\n\n SAVE TO PLAYLIST : ";
  cout << "\n\n SERVICE : " << SERVICE;
  cout << "\n REQUEST : " << REQUEST;

  if (Issue) {
    ConsoleColor(CONSOLECOLOUR_LIGHTRED);
    cout << "\n\n ERROR : Request or Response Format Issue or Invalid Value\n";
    ConsoleColor();
  }

  cout << "\n ADDRESING : ";
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  ENTER = 0;
  ADD = 0;
  do {
    SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
    (ADD == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "PHYSICAL"; cout << "  ";
    (ADD == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "FUNCTIONAL";
    PRESS = 0;
    do {
      if(_kbhit()) {
        int KEY = _getch();
        if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
          KEY = _getch();
          switch(KEY) {
            case CONARROWLEFT  : { ADD = 0; PRESS = 1; break; } // ARROW LEFT KEY
            case CONARROWRIGHT : { ADD = 1; PRESS = 1; break; } // ARROW RIGHT KEY
          }
        }
        else if (KEY == CONENTER) { PRESS = 1; ENTER = 1; } // ENTER KEY
        else if (KEY == CONBACKSPACE) { return; }  // BACKSPACE KEY
        else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
      }
    } while (PRESS == 0);
  } while (ENTER == 0);
  ConsoleColor();

  cout << "\n RESPONSE SUPPRESS : ";
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  ENTER = 0;
  SUPPRESS = 0;
  do {
    SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
    (SUPPRESS == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "YES"; cout << "  ";
    (SUPPRESS == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "NO";
    PRESS = 0;
    do {
      if(_kbhit()) {
        int KEY = _getch();
        if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
          KEY = _getch();
          switch(KEY) {
            case CONARROWLEFT  : { SUPPRESS = 1; PRESS = 1; break; } // ARROW LEFT KEY
            case CONARROWRIGHT : { SUPPRESS = 0; PRESS = 1; break; } // ARROW RIGHT KEY
          }
        }
        else if (KEY == CONENTER) { PRESS = 1; ENTER = 1; } // ENTER KEY
        else if (KEY == CONBACKSPACE) { return; }  // BACKSPACE KEY
        else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
      }
    } while (PRESS == 0);
  } while (ENTER == 0);
  ConsoleColor();

  if (!SUPPRESS) {
    cout << "\n RESPONSE CHECK : ";
      GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
      OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
    ENTER = 0;
    CHECK = 0;
    do {
      SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
      (CHECK == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
      cout << "YES"; cout << "  ";
      (CHECK == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
      cout << "NO";
      PRESS = 0;
      do {
        if(_kbhit()) {
          int KEY = _getch();
          if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
            KEY = _getch();
            switch(KEY) {
              case CONARROWLEFT  : { CHECK = 1; PRESS = 1; break; } // ARROW LEFT KEY
              case CONARROWRIGHT : { CHECK = 0; PRESS = 1; break; } // ARROW RIGHT KEY
            }
          }
          else if (CONENTER == 13) { PRESS = 1; ENTER = 1; } // ENTER KEY
          else if (CONBACKSPACE == 8) { return; }  // BACKSPACE KEY
          else if (CONESCAPE == 27) { RenderExitPage(); return; } // ESCAPE KEY
        }
      } while (PRESS == 0);
    } while (ENTER == 0);
    ConsoleColor();

    if (CHECK) {
      std::istringstream StreamSID(REQUEST);
      std::string ByteString;
      StreamSID >> ByteString;
      uint8_t SID = 0;
      StringToHEX(SID, ByteString);
      if (SID >= 0xC0) {
        SaveToPlaylist(REQUEST, SERVICE, 1); return;
      }
      cout << "\n\n HEXADECIMAL FORMAT -> HH HH HH HH .. .. ";
      cout << "\n To EXIT Enter 'X' Character";
      cout << "\n\n ENTER THE RESPONSE CHECK (IN HEX) :\n ";
      SID = SID + 0x40;
      ConsoleCursor(1);
      cout << HEXToString(SID) << " ";
      std::string Input;
      std::getline(std::cin, Input);
      Input  = HEXToString(SID) + " " + Input;
      ConsoleCursor(0);
      uint8_t OK = ValidRequestString(Input);
      if (OK == 2) { return; }
      else if (OK == 0) { RESPONSE = Input; }
      else { SaveToPlaylist(REQUEST, SERVICE, 1); return; }
    }
  }

  cout << " \n\nProcessing : " << REQUEST;
  std::istringstream StreamREQUEST(REQUEST);
  std::string REQUEST_BYTE;
  FIRST = 0;
  std::string PROCESS_REQUEST;
  while (StreamREQUEST >> REQUEST_BYTE) {
    uint8_t Data = 0;
    uint8_t Result = StringToHEX(Data, REQUEST_BYTE);
    if (Result) {
      SaveToPlaylist(REQUEST, SERVICE, 1); return;
    }
    if (FIRST == 0) {
      PROCESS_REQUEST = HEXToString(Data);
    } else {
      PROCESS_REQUEST += " " + HEXToString(Data);
    }
    FIRST++;
  }

  cout << " \n\nProcessing : " << RESPONSE;
  std::string PROCESS_RESPONSE;
  if (CHECK) {
    std::istringstream StreamRESPONSE(RESPONSE);
    std::string RESPONSE_BYTE;
    FIRST = 0;
    while (StreamRESPONSE >> RESPONSE_BYTE) {
      uint8_t Data = 0;
      uint8_t Result = StringToHEX(Data, RESPONSE_BYTE);
      if (Result) {
        SaveToPlaylist(REQUEST, SERVICE, 1); return;
      }
      if (FIRST == 0) {
        PROCESS_RESPONSE = HEXToString(Data);
      } else {
        PROCESS_RESPONSE += " " + HEXToString(Data);
      }
      FIRST++;
    }
  } else {
    PROCESS_RESPONSE = "";
  }

  UDSPLAYLIST Play;
  Play.FUNCTION = SERVICE;
    Play.REQUEST = PROCESS_REQUEST;
    Play.RESPONSE = PROCESS_RESPONSE;
  Play.ADDRESS = (ADD) ? "P" : "F"; Play.SUPPRESS = (SUPPRESS) ? "Y" : "N";

  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor();
  cout << "\n\n SAVE TO PLAYLIST : ";
  cout << "\n\n SERVICE : " << Play.FUNCTION;
  cout << "\n REQUEST : " << Play.REQUEST;
  cout << "\n ADDRESING : ";
    (ADD == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "PHYSICAL";
    cout << "  ";
    (ADD == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "FUNCTIONAL";
  ConsoleColor();
  cout << "\n RESPONSE SUPPRESS : ";
    (SUPPRESS == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "YES";
    cout << "  ";
    (SUPPRESS == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "NO";
  ConsoleColor();
  cout << "\n RESPONSE CHECK : ";
    (CHECK == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "YES";
    cout << "  ";
    (CHECK == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "NO";
  ConsoleColor();
  cout << "\n RESPONSE : " << Play.RESPONSE;

  cout << "\n\n SAVE TO PLAYLIST : ";
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  ENTER = 0;
  SAVE = 0;
  do {
    SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
    (SAVE == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "YES"; cout << "  ";
    (SAVE == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
    cout << "NO";
    PRESS = 0;
    do {
      if(_kbhit()) {
        int KEY = _getch();
        if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
          KEY = _getch();
          switch(KEY) {
            case CONARROWLEFT  : { SAVE = 0; PRESS = 1; break; } // ARROW LEFT KEY
            case CONARROWRIGHT : { SAVE = 1; PRESS = 1; break; } // ARROW RIGHT KEY
          }
        }
        else if (KEY == CONENTER) { PRESS = 1; ENTER = 1; } // ENTER KEY
        else if (KEY == CONBACKSPACE) { return; }  // BACKSPACE KEY
        else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
      }
    } while (PRESS == 0);
  } while (ENTER == 0);
  ConsoleColor();

  if (SAVE) {
    FLAGS.PLAYOUT = 1; return;
  } else {
    ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "\n\n SAVED TO PLAYLIST";
    ConsoleColor();
    if (FLAGS.MIDADD) {
      PLAYLIST.insert(PLAYLIST.begin() + FLAGS.MIDINDEX, Play);
    } else {
      PLAYLIST.push_back(Play);
    }
    FLAGS.PLAYOUT = 1;
    Sleep(750); return;
  }
}
/* ==================================================================================================== */


// MARK: View UDS Traffic
/* ==================================================================================================== */
/**
 * @name        HandlerViewTraffic
 * @class       ISO_UDS (Private)
 * @brief       Handles View UDS Traffic Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerViewTraffic (void) {
  std::deque<std::string> RawLOGS;
  UDSLIST TRAF; uint8_t QUEUESIZE = 50; uint32_t TesterPresent = 0;
  TRAF.TOTAL = 0; TRAF.PAGE = 15; TRAF.START = 0; TRAF.END = 0; TRAF.SELECTED = 0;
  ofstream ViewFile;
  if (CONFIG.LOGGINGMODE) {
    std::string Path = CONFIG.LOGPATH + "\\";
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&now_c);
    std::ostringstream FileName;
    FileName << Path << std::put_time(localTime, "%Y%m%d-%H%M") << "-MONITOR.log";
    ViewFile.open (FileName.str(), std::ios::app);
    ViewFile << "ISO 14429 LOGGING STARTED\nUnified Diagnostic Services\nUDS Traffic Monitoring" << "\nSTART" << std::endl;
  }
  uint32_t STARTID = 0, ENDID = 0;
  STARTID = std::min(CONFIG.REQID, std::min(CONFIG.RESID, CONFIG.FUNID));
  ENDID = std::max(CONFIG.REQID, std::max(CONFIG.RESID, CONFIG.FUNID));
  UDS.DoCAN.PCAN.Filter(STARTID, ENDID, PCAN_MESSAGE_STANDARD);

  TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
  RenderViewTraffic (RawLOGS, TRAF, TesterPresent);
  uint8_t OutLoop = 1;
  TPCANMsg MSG;
  do {
    if (UDS.DoCAN.PCAN.Read(MSG) == PCAN_ERROR_OK) {
      if ((MSG.LEN == 8) && ((MSG.ID == CONFIG.REQID) || (MSG.ID == CONFIG.RESID) || (MSG.ID == CONFIG.FUNID))) {
        std::stringstream Stream;
        string LogLine;
        Stream << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << static_cast<int>(MSG.ID) << " : ";
        if (((MSG.DATA[0] & 0xF0) == 0x00) || ((MSG.DATA[0] & 0xF0) == 0x20)) {
          Stream << "[" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[0]) << "]";
          for (int I = 1; I < 8; I++) {
            Stream << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[I]);
          }
        } else if ((MSG.DATA[0] & 0xF0) == 0x30) {
          Stream << "[" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[0]);
          for (int I = 1; I < 8; I++) {
            Stream << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[I]);
          }
          Stream << "]";
        } else if((MSG.DATA[0] & 0xF0) == 0x10) {
          Stream << "[" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[0]) << " " << static_cast<int>(MSG.DATA[1]) << "]";
          for (int I = 2; I < 8; I++) {
            Stream << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[I]);
          }
        } else {
          Stream << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[0]);
          for (int I = 1; I < 8; I++) {
            Stream << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
              << static_cast<int>(MSG.DATA[I]);
          }
        }
        LogLine = Stream.str();
        if ((MSG.DATA[0] == 0x02) && ((MSG.DATA[1] == 0x3E) || (MSG.DATA[1] == 0x7E))) {
          if ((MSG.DATA[1] == 0x3E)) { TesterPresent++; }
        } else {
          if (RawLOGS.size() >= QUEUESIZE) { RawLOGS.pop_front(); }
          RawLOGS.push_back(LogLine);
          TRAF.TOTAL = RawLOGS.size();
          if (TRAF.TOTAL >= TRAF.PAGE) { TRAF.END = TRAF.TOTAL; TRAF.START = TRAF.END - TRAF.PAGE; }
          TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
          RenderViewTraffic (RawLOGS, TRAF, TesterPresent);
        }
        if (CONFIG.LOGGINGMODE) { ViewFile << LogLine << std::endl; }
      }
    }

    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              TRAF = ListCalculatePosition(TRAF, ARROWUP);
              RenderViewTraffic (RawLOGS, TRAF, TesterPresent); break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              TRAF = ListCalculatePosition(TRAF, ARROWDOWN);
              RenderViewTraffic (RawLOGS, TRAF, TesterPresent); break;
            }
        }
      }
      else if (KEY == CONENTER) { OutLoop = 0; FLAGS.INIT_PAGE = 1; break; } // ENTER KEY
      else if (KEY == CONBACKSPACE) { // BACKSPCAE KEY
        RawLOGS.clear(); TRAF.TOTAL = 0; TRAF.START = 0; TRAF.END = 0;
        TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
        RenderViewTraffic (RawLOGS, TRAF, TesterPresent);
      }
      else if (KEY == CONESCAPE) { // ESCAPE KEY
        if (CONFIG.LOGGINGMODE) { ViewFile << "\nSTOPPED" << std::endl; ViewFile.close(); }
        RenderExitPage();
      }
    }
  } while (OutLoop);
  if (CONFIG.LOGGINGMODE) { ViewFile << "\nSTOPPED" << std::endl; ViewFile.close(); }
  UDS.DoCAN.PCAN.Filter(CONFIG.RESID);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderViewTraffic
 * @class       ISO_UDS (Private)
 * @brief       Renders UDS Traffic View
 * @param [Data]      Data Vector
 * @param [TRAF]      Traffic UI List Structure
 * @param [Tester]    Number of Tester Present Received
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderViewTraffic (const std::deque<std::string>& Data, UDSLIST TRAF, uint32_t Tester) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n View UDS CAN Traffic";
  ConsoleColor();
  cout << "\n Use BACKSPACE to Clear Screen, ENTER to go back, ESCAPE to Exit, and, ARROW KEYS to Navigate Traffic\n";
  for (int I = TRAF.START; I < TRAF.END; I++) {
    std::string STR = Data[I];
    if (STR.find('[') != std::string::npos && STR.find(']') != std::string::npos) {
      if ((STR[7] == '0') || (STR[7] == '2')) {
        std::string STARTSTR = STR.substr(0, 6);
        std::string PCISTR = STR.substr(6, 4);
        std::string RESTSTR = STR.substr(10);
        cout << "\n       ";
        ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
        cout << STARTSTR;
        ConsoleColor(CONSOLECOLOUR_GRAY);
        cout << PCISTR;
        ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
        cout << RESTSTR;
      } else if (STR[7] == '1') {
        std::string STARTSTR = STR.substr(0, 6);
        std::string PCISTR = STR.substr(6, 7);
        std::string RESTSTR = STR.substr(13);
        cout << "\n       ";
        ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
        cout << STARTSTR;
        ConsoleColor(CONSOLECOLOUR_GRAY);
        cout << PCISTR;
        ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
        cout << RESTSTR;
      } else {
        ConsoleColor();
        cout << "\n       " << STR;
      }
    } else {
      ConsoleColor(CONSOLECOLOUR_GRAY);
      cout << "\n       " << STR;
    }
  }
  ConsoleColor();
  cout << "\n\n Tester Present : " << Tester;
  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA);
  cout << "\n BACK";
  ConsoleColor();
  RenderDebugVariableList (TRAF, TRAF.START, TRAF.END, TRAF.PAGE);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HandlerTrace
 * @class       ISO_UDS (Private)
 * @brief       Handles CAN Trace View Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::HandlerTrace (void) {
  std::deque<std::string> RawLOGS;
  UDSLIST TRAF; uint8_t QUEUESIZE = 50;
  TRAF.TOTAL = 0; TRAF.PAGE = 15; TRAF.START = 0; TRAF.END = 0; TRAF.SELECTED = 0;
  ofstream ViewFile;
  if (CONFIG.LOGGINGMODE) {
    std::string Path = CONFIG.LOGPATH + "\\";
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&now_c);
    std::ostringstream FileName;
    FileName << Path << std::put_time(localTime, "%Y%m%d-%H%M") << "-TRACE.asc";
    ViewFile.open (FileName.str(), std::ios::app);
    ViewFile << "base hex timestamps absolute\n// version 1.0:0" << std::endl;
  }
  uint32_t STARTID = 0, ENDID = 0;
  STARTID = std::min(CONFIG.REQID, std::min(CONFIG.RESID, CONFIG.FUNID));
  ENDID = std::max(CONFIG.REQID, std::max(CONFIG.RESID, CONFIG.FUNID));
  UDS.DoCAN.PCAN.Filter(STARTID, ENDID, PCAN_MESSAGE_STANDARD);

  TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
  RenderTrace (RawLOGS, TRAF);
  static auto start = chrono::steady_clock::now();
  uint8_t OutLoop = 1;
  TPCANMsg MSG;
  do {
    if (UDS.DoCAN.PCAN.Read(MSG) == PCAN_ERROR_OK) {
      std::string TimeStamp;
        auto delta = chrono::steady_clock::now() - start;
        double sec = chrono::duration_cast<chrono::duration<double>>(delta).count();
        std::ostringstream os;
        os << std::fixed << std::setprecision(6) << sec;
      TimeStamp = os.str();
      std::stringstream Stream;
      Stream << TimeStamp << " 1 ";
      Stream << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << (int)(MSG.ID) << " Rx ";
      if (MSG.LEN) {
        Stream << " d " << (int)(MSG.LEN);
        for (int I = 0; I < MSG.LEN; I++) {
          Stream << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)(MSG.DATA[0]);
        }
      } else { Stream << " r"; }
      std::string LogLine = Stream.str();
      if (RawLOGS.size() >= QUEUESIZE) { RawLOGS.pop_front(); }
      RawLOGS.push_back(LogLine);
      TRAF.TOTAL = RawLOGS.size();
      if (TRAF.TOTAL >= TRAF.PAGE) { TRAF.END = TRAF.TOTAL; TRAF.START = TRAF.END - TRAF.PAGE; }
      TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
      RenderTrace (RawLOGS, TRAF);
      if (CONFIG.LOGGINGMODE) { ViewFile << LogLine << std::endl; }
    }

    if(_kbhit()) {
      int KEY = _getch();
      if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
        KEY = _getch();
        switch(KEY) {
            case CONARROWUP : { // ARROW UP KEY
              TRAF = ListCalculatePosition(TRAF, ARROWUP);
              RenderTrace (RawLOGS, TRAF); break;
            }
            case CONARROWDOWN : { // ARROW DOWN KEY
              TRAF = ListCalculatePosition(TRAF, ARROWDOWN);
              RenderTrace (RawLOGS, TRAF); break;
            }
        }
      }
      else if (KEY == CONENTER) { OutLoop = 0; FLAGS.INIT_PAGE = 1; break; } // ENTER KEY
      else if (KEY == CONBACKSPACE) { // BACKSPCAE KEY
        RawLOGS.clear(); TRAF.TOTAL = 0; TRAF.START = 0; TRAF.END = 0;
        TRAF = ListCalculatePosition(TRAF, ARROWCALIBRATE);
        RenderTrace (RawLOGS, TRAF);
      }
      else if (KEY == CONESCAPE) { // ESCAPE KEY
        if (CONFIG.LOGGINGMODE) { ViewFile << "\nEnd Triggerblock" << std::endl; ViewFile.close(); }
        RenderExitPage();
      }
    }
  } while (OutLoop);
  if (CONFIG.LOGGINGMODE) { ViewFile << "\nEnd Triggerblock" << std::endl; ViewFile.close(); }
  UDS.DoCAN.PCAN.Filter(CONFIG.RESID);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderTrace
 * @class       ISO_UDS (Private)
 * @brief       Renders CAN Trace View
 * @param [Data]      Data Vector
 * @param [TRAF]      Trace UI List Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderTrace (const std::deque<std::string>& Data, UDSLIST TRAF) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n View CAN TRACE";
  ConsoleColor();
  cout << "\n Use BACKSPACE to Clear Screen, ENTER to go back, ESCAPE to Exit, and, ARROW KEYS to Navigate Traffic\n";
  for (int I = TRAF.START; I < TRAF.END; I++) {
    std::string STR = Data[I]; cout << "\n       " << STR;
  }
  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA);
  cout << "\n\n BACK";
  ConsoleColor();
  RenderDebugVariableList (TRAF, TRAF.START, TRAF.END, TRAF.PAGE);
}
/* ==================================================================================================== */


// MARK: Misc Pages
/* ==================================================================================================== */
/**
 * @name        RenderSettings
 * @class       ISO_UDS (Private)
 * @brief       Renders Settings Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderSettings (void) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE);
  cout << "\n\n LOGGING MODE\n";
  ConsoleColor();
  cout << "\n Path for Logs are ./logs/";
  if (CONFIG.LOGGINGMODE) {
    CONFIG.LOGGINGMODE = 0;
    LoggingMode(CONFIG.LOGGINGMODE);
  } else {
    CONFIG.LOGGINGMODE = 1;
    LoggingMode(CONFIG.LOGGINGMODE);
  }
  ConsoleColor(CONSOLECOLOUR_LIGHTAQUA);
  cout << "\n\n BACK";
  ConsoleColor();
  int KEY = 0;
  do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
  FLAGS.INIT_PAGE = 1;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderWorkInPorgress
 * @class       ISO_UDS (Private)
 * @brief       Renders Work In Progress Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderWorkInPorgress (void) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();
  cout << "\n\n WORK IN PROGRESS!\n";
  RenderSelector(1, CONSOLECOLOUR_LIGHTAQUA);
  cout << "BACK";
  int KEY = 0;
  do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
  FLAGS.INIT_PAGE = 1;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderExitPage
 * @class       ISO_UDS (Private)
 * @brief       Renders Exit Page
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderExitPage (void) {
  Sleep(100); ClearScreen();
  RenderLOGO (30,5);
    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD Coordinates = { 0, 15};
    SetConsoleCursorPosition(ConsoleHandle, Coordinates);
  ConsoleColor();
  DoCAN.Stop(); Stop();
  cout << "\n PCAN Disconnected ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
  cout << "\n DoCAN Logging Stopped ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
  cout << "\n UDS Logging Stopped ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
  cout << "\n DoCAN Driver Stopped ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
  cout << "\n UDS Driver Stopped ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
  cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);

  cout << "\n\n PRESS ENTER TO EXIT\n";
  int KEY = 0;
  do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER);
  FLAGS.INIT_PAGE = 1;
  Sleep(100);
  exit(0);
}
/* ==================================================================================================== */


// MARK: Render Calculations
/* ==================================================================================================== */
/**
 * @name        RenderDebugVariableList
 * @class       ISO_UDS (Private)
 * @brief       Render Debuging Values for List Renderings
 * @param [LUI]   List Handling Structure
 * @param [STR]   Visible List Start Point
 * @param [END]   Visible List End Point
 * @param [RAN]   Visible List Range
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderDebugVariableList (UDSLIST LUI, int STR, int END, int RAN) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
    COORD Coordinates = {1, 28};
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
  ConsoleColor(CONSOLECOLOUR_LIGHTPURPLE);
    cout << std::dec;
    cout << "RENDER : {  STR: " << STR << "  END: " << END << "  RAN: " << RAN << "  }";
    cout << "   LIST  {  TOT: " << LUI.TOTAL << "  PAG: " << LUI.PAGE << "  STR: " << LUI.START
        << "  END: " << LUI.END << "  SEL: " << LUI.SELECTED << "  }";
  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
  ConsoleColor();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderSelector
 * @class       ISO_UDS (Private)
 * @brief       Renders Active or Deactive Selector
 * @param [Selector]      Active HIGH Selected Flag
 * @param [Colour]        Text Colur (Enum)
 * @param [Background]    Background Colour (Enum)
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderSelector (uint8_t Selected, CONSOLECOLOURS Colour, CONSOLECOLOURS Background) {
  if (Selected) {
    ConsoleColor (Colour, Background);
    cout << "\n  >  ";
  } else {
    ConsoleColor();
    cout << "\n     ";
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RequestRender
 * @class       ISO_UDS (Private)
 * @brief       Renders Request Message
 * @param [REQS]      Request Data Structure
 * @param [Offset]    Offset from Left Side Screen
 * @param [Colour]    Console Text Colour (Enum)
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RequestRender (UDSREQRES REQS, uint8_t Offset, CONSOLECOLOURS Colour) {
  ConsoleColor(Colour);
  if (REQS.DATA.size() != REQS.LEN) { return; }
  int OverFlow = ((UI.REQRESWIDHTH * UI.REQHEIGHT) < REQS.LEN) ? 1 : 0;
  int C = 0;
  for (int I = 0; I < UI.REQHEIGHT; I++) {
    cout << "\n"; for (int K = 0; K < Offset; K++) { cout << " "; }
    for (int J = 0; J < UI.REQRESWIDHTH; J++) {
      if (C >= REQS.LEN) { return; }
      if (C == (UI.REQRESWIDHTH * UI.REQHEIGHT - 1) && (OverFlow)) { cout << " .."; }
      else if (C < REQS.LEN) {
        cout << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)REQS.DATA[C];
      }
      C++;
    }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ResponseRender
 * @class       ISO_UDS (Private)
 * @brief       Renders Response Message
 * @param [REQS]      Response Data Structure
 * @param [RUI]       Response UI List Structure
 * @param [Offset]    Offset from Left Side Screen
 * @param [Colour]    Console Text Colour (Enum)
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ResponseRender (UDSREQRES REQS, UDSLIST RUI, uint8_t Offset, CONSOLECOLOURS Colour) {
  uint8_t Index = 0;
  if (REQS.LEN >= (UI.REQRESWIDHTH * UI.RESHEIGHT)) { Index = 1; }
  int Start = RUI.START * UI.REQRESWIDHTH;
  int End = RUI.END * UI.REQRESWIDHTH;
  ConsoleColor(Colour);
  int I = Start;
  while (I < End) {
    if ((I % UI.REQRESWIDHTH) == 0) {
      cout << "\n"; for (int K = 0; K < Offset; K++) { cout << " "; }
      if (Index) {
        ConsoleColor(CONSOLECOLOUR_GRAY);
        cout << " " << std::dec << std::setw(4) << setfill('0') << (int)(I);
        ConsoleColor(Colour);
      }
    }
    if (I < REQS.LEN) {
      cout << " " << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)REQS.DATA[I];
    }
    I++;
  }
}
/* ==================================================================================================== */


// MARK: Execute DoCAN
/* ==================================================================================================== */
/**
 * @name        Executor
 * @class       ISO_UDS (Private)
 * @brief       Service Executer
 * @param [Suppress]  Suppress Response Active Low
 * @param [Line]      Error Code Line
 * @return      Active Low Status
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::Executor (uint8_t Suppress, std::string& Line) {
  int Wait = 0, Result = 0;
  Result = DoCAN.Write();
  if (Result != DoCAN_TX_COMPLETE) {
    ConsoleColor(CONSOLECOLOUR_LIGHTRED);
    Line = "ERROR: TX " + ErrorCodeDoCAN();
    ConsoleColor();
    return 1;
  }
  if (Suppress) {
    do {
      Result = DoCAN.Receive();
      if (Result != DoCAN_RX_COMPLETE) {
        ConsoleColor(CONSOLECOLOUR_LIGHTRED);
        Line = "ERROR: RX " + ErrorCodeDoCAN();
        ConsoleColor();
        return 1;
      } else {
        if ((MESSAGE.DATA[0] == 0x7F) && (MESSAGE.DATA[2] == 0x78)) {
          Wait++; continue;
        } else { return 0;}
      }
    } while (Wait < 17);
    ConsoleColor(CONSOLECOLOUR_LIGHTRED);
    Line = "ERROR: RX TOO MANY WAITS";
    ConsoleColor();
    return 1;
  } return 0;
}
/* ==================================================================================================== */


// MARK: Misc But Important Functions
/* ==================================================================================================== */
/**
 * @name        RawServiceBuilder
 * @class       ISO_UDS (Private)
 * @brief       Raw Service Request String Builder
 * @param [SID]       SID String
 * @param [SERVICE]   Function Name
 * @param [Add]       Playlist Add Flag Active High
 * @param [Issue]     Recurrive Call Indentifier of Error Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RawServiceBuilder (const std::string& SID, const std::string& SERVICE, CONFIGATIONS Add, uint8_t Issue) {
  ClearScreen();
  RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
  RenderHeader();

  cout << "\n\n RAW SERVICE OR SUB FUNCTION";
  cout << "\n\n HEXADECIMAL FORMAT -> HH HH HH HH .. .. ";
  cout << "\n To EXIT Enter 'X' Character";

  if (Issue) {
    ConsoleColor(CONSOLECOLOUR_LIGHTRED);
    cout << "\n\n Incorrect Format or Wrong Values";
    ConsoleColor();
  }

  cout << "\n\n ENTER THE MESSAGE (IN HEX) :\n ";
  ConsoleCursor(1);
  cout << SID << " ";
  std::string Input;
  std::getline(std::cin, Input);
  Input  = SID + " " + Input;
  ConsoleCursor(0);

  uint8_t OK = ValidRequestString(Input);
  if (OK == 2) { return; }
  else if (OK == 0) { ServiceExecuter(Input, SERVICE, Add); }
  else { RawServiceBuilder(SID, SERVICE, Add, 1); return; }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        LoggingMode
 * @class       ISO_UDS (Private)
 * @brief       Validate .hex files for Corruption or Tampering
 * @param [Mode]      1 means Logging ON else Logging OFF
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::LoggingMode (uint8_t Mode) {
  std::string Path = CONFIG.LOGPATH + "\\";
  if (Mode) { // LOGGING MODE ON
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&now_c);
    std::ostringstream FileName;
    FileName << Path << std::put_time(localTime, "%Y%m%d-%H%M") << "-UDS.log";
    auto Now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(Now);
    std::tm* local_tm = std::localtime(&now_time_t);
      TIME.YY = local_tm->tm_year + 1900;
      TIME.MM = local_tm->tm_mon + 1;
      TIME.DD = local_tm->tm_mday; TIME.HH = local_tm->tm_hour;
      TIME.mm = local_tm->tm_min; TIME.SS = local_tm->tm_sec;
    std::ostringstream TimeStamp;
    TimeStamp << "[ "
      << std::setw(4) << std::setfill('0') << TIME.YY << "."
      << std::setw(2) << std::setfill('0') << TIME.MM << "."
      << std::setw(2) << std::setfill('0') << TIME.DD << " ";
      int displayHour = (TIME.HH == 0) ? 12 : (TIME.HH > 12 ? TIME.HH - 12 : TIME.HH);
      const char* ampm = (TIME.HH < 12 || TIME.HH == 0) ? "AM" : "PM";
    TimeStamp << std::setw(2) << std::setfill('0') << displayHour << ":"
      << std::setw(2) << std::setfill('0') << TIME.mm << ":"
      << std::setw(2) << std::setfill('0') << TIME.SS << " "
      << ampm << " ]";
    LogFile.open (FileName.str(), std::ios::app);
    if (!LogFile.is_open()) {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      cout << "\n Logging ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      ConsoleColor(CONSOLECOLOUR_RED);
      cout << "\n ERROR: .\\logs Folder not Found"; ConsoleColor();
      DoCAN.LoggingMode(ISO_DoCAN::DoCAN_LOGOFF, Path);
      CONFIG.LOGGINGMODE = 0;
      return;
    }
    LogFile << "ISO 14429 LOGGING STARTED\nUnified Diagnostic Services\nUDSonCAN" << "\nTIME : " << TimeStamp.str()
        << "\nSTART" << std::endl;
    cout << "\n Logging Started ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    DoCAN.LoggingMode(ISO_DoCAN::DoCAN_LOGON, Path);
    CONFIG.LOGGINGMODE = 1;
  }
  else { // LOGGING MODE OFF
    if (LogFile.is_open()) {
      LogFile << "STOPPED\nUDS SAFELY CLOSING . . .\nISO UDS LOGGING CLOSED" << std::endl;
      LogFile.close();
      cout << "\n Logging Stoped ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
      cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    }
    DoCAN.LoggingMode(ISO_DoCAN::DoCAN_LOGOFF, Path);
    CONFIG.LOGGINGMODE = 0;
  }
}
/* ==================================================================================================== */


// MARK: Start
/* ==================================================================================================== */
/**
 * @name        Start
 * @class       ISO_UDS (Public)
 * @brief       Start UDS Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::Start (void) {
  CONFIG.ERRORCODE = UDS_ERR_ALLOK;
  if (FLAGS.CONSOLE == 0) {
    string ReSize = "mode con: cols=" + to_string(UI.WINDOWWIDTH) + " lines=" + to_string(UI.WINDOWHEIGHT);
    system(ReSize.c_str());
    system("title Sutrika:Universal-UDS");
    HWND ConsoleWindow = GetConsoleWindow();
    int WindowWidth, WindowHeight, WindowSizeFlag = 0;
    if (ConsoleWindow) {
      RECT r;
      GetWindowRect(ConsoleWindow, &r);
      SetWindowLong(ConsoleWindow, GWL_STYLE,
          GetWindowLong(ConsoleWindow, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
      if (GetConsoleScreenBufferInfo(hConsole, &ConsoleBuffer)) {
        WindowWidth = ConsoleBuffer.srWindow.Right - ConsoleBuffer.srWindow.Left + 1;
        WindowHeight = ConsoleBuffer.srWindow.Bottom - ConsoleBuffer.srWindow.Top + 1;
        if (WindowWidth != UI.WINDOWWIDTH || WindowHeight != UI.WINDOWHEIGHT) { WindowSizeFlag = 1; }
        else { WindowSizeFlag = 0; }
      } else { WindowSizeFlag = 1; }
    }
    else { WindowSizeFlag = 1; }

    SetConsoleOutputCP(CP_UTF8);
    ConsoleColor();
    ConsoleCursor(0);

    ClearScreen();
    RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
    RenderHeader();
    cout << "\n\n";
    cout << "\n Copyright © 2025\n Alakshendra Singh";
    cout << "\n\n";

    if (WindowSizeFlag) {
      cout << "\n Console Rendering ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      return;
    } else {
      cout << "\n Console Rendering ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
      cout << "OK"; ConsoleColor(); cout << "]";
      Sleep(250);
    }

    cout << "\n Checking Console Colour Rendering ...";
    Sleep(100);
    ConsoleColor(); cout <<  "\n  ";
      ConsoleColor(CONSOLECOLOUR_BLACK); cout << "██";
      ConsoleColor(CONSOLECOLOUR_BLUE); cout << "██";
      ConsoleColor(CONSOLECOLOUR_GREEN); cout << "██";
      ConsoleColor(CONSOLECOLOUR_AQUA); cout << "██";
      ConsoleColor(CONSOLECOLOUR_RED); cout << "██";
      ConsoleColor(CONSOLECOLOUR_PURPLE); cout << "██";
      ConsoleColor(CONSOLECOLOUR_YELLOW); cout << "██";
      ConsoleColor(CONSOLECOLOUR_WHITE); cout << "██";
    ConsoleColor(); cout <<  "\n  ";
      ConsoleColor(CONSOLECOLOUR_GRAY); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTBLUE); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTPURPLE); cout << "██";
      ConsoleColor(CONSOLECOLOUR_LIGHTYELLOW); cout << "██";
      ConsoleColor(CONSOLECOLOUR_BRIGHTWHITE); cout << "██";
    ConsoleColor();
    cout << "\n Console Colour Rendering ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(200);
    FLAGS.CONSOLE = 1;

    cout << "\n Peak PCAN Driver Loaded ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    cout << "\n DoCAN Driver Loaded ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    cout << "\n UDS Driver Loaded ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
  }

  if (FLAGS.LOG == 0) {
    LoggingMode(CONFIG.LOGGINGMODE);
    std::string Path = CONFIG.LOGPATH;
    struct stat InfoFS;
    int Access = stat(Path.c_str(), &InfoFS);
    if ((InfoFS.st_mode & S_IFDIR) && (Access == 0)) {
      cout << "\n Logging Path ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
      cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    } else {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      cout << "\n Logging ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      ConsoleColor(CONSOLECOLOUR_RED);
      cout << "\n ERROR: .\\logs Folder not Found"; ConsoleColor();
      return;
    }
    FLAGS.LOG = 1;
  }

  if (FLAGS.CONFIGURATION == 0) {
    int Result = ReadDashboard();
    if (Result) {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      cout << "\n Configuration File Read ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      if (Result == 1) {
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: .\\config\\dashboard.json File not Found"; ConsoleColor();
      } else {
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: dashboard.json has Invalid or Corrupt Data"; ConsoleColor();
      }
      return;
    }
    Result = ValidateDashboard();
    if (Result) {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      cout << "\n Configuration File Read ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      ConsoleColor(CONSOLECOLOUR_RED);
      cout << "\n ERROR: dashboard.json has Invalid or Corrupt Data"; ConsoleColor();
      return;
    }
    Result = ReadConfiguration();
    if (Result) {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      cout << "\n Configuration File Read ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
      cout << "ERROR"; ConsoleColor(); cout << "]";
      if (Result == 1) {
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: .\\config\\config.json File not Found"; ConsoleColor();
      } else {
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: config.json has Invalid or Corrupt Data"; ConsoleColor();
      }
      return;
    }
    cout << "\n Configuration File Read ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    FLAGS.CONFIGURATION = 1;
  }

  if (FLAGS.DRIVER == 0) {
    DoCAN.SetBuffer (&MESSAGE.ID, &MESSAGE.LEN, MESSAGE.DATA);
    DoCAN.SetUDSParameter (CONFIG.PADDING, CONFIG.STMIN, CONFIG.BLOCKS, 4095, 500);
    DoCAN.SetTiming (CONFIG.P2, CONFIG.P2STAR);
    DoCAN.SetCANID (CONFIG.REQID, CONFIG.RESID, CONFIG.FUNID);
    DoCAN.Start();

#ifdef DEVICE_UNAVAILBLE_ONLY_DEVELOPMENT
    int DoCAN_ERR = DoCAN_ERR_FRAMEOK;
#else
    int DoCAN_ERR = DoCAN.GetErrorCode();
#endif
    if (DoCAN_ERR != DoCAN_ERR_FRAMEOK) {
      if (DoCAN_ERR == DoCAN_ERR_EXTERNALCONFIGATION) {
        cout << "\n DoCAN Initialization ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
        cout << "ERROR"; ConsoleColor(); cout << "]";
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: Internal Fault"; ConsoleColor();
      } else if (DoCAN_ERR == DoCAN_ERR_DRIVERFAILER) {
        cout << "\n PCAN Connection ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
        cout << "ERROR"; ConsoleColor(); cout << "]";
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: Connect PCAN Device"; ConsoleColor();
      } else if (DoCAN_ERR == DoCAN_ERR_LOGGINGISSUE) {
        cout << "\n DoCAN Logging ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
        cout << "ERROR"; ConsoleColor(); cout << "]";
        ConsoleColor(CONSOLECOLOUR_RED);
        cout << "\n ERROR: .\\logs Folder not Found"; ConsoleColor();
      } else {
        cout << "\n DoCAN Internal Issue ["; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
        cout << "ERROR"; ConsoleColor(); cout << "]";
        cout << "\n ERROR: Internal Fault"; ConsoleColor();
      }
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      return;
    }
    cout << "\n DoCAN Initialization ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    cout << "\n DoCAN Logging ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    cout << "\n PCAN Connected ["; ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
    cout << "OK"; ConsoleColor(); cout << "]"; Sleep(100);
    cout << "\n";
    Sleep(250);
    FLAGS.DRIVER = 1;
  }

  if ((FLAGS.DRIVER) && (FLAGS.CONFIGURATION) && (FLAGS.LOG) && (FLAGS.CONSOLE)) {
    Sleep(1500); ClearScreen();
      RenderLOGO (16,5);
    Sleep (750); ClearScreen();
    Sleep (250); ClearScreen();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Stop
 * @class       ISO_UDS (Public)
 * @brief       Stop UDS Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::Stop (void) {
  if (LogFile.is_open()) {
    LogFile << "STOPPED\nUDS SAFELY CLOSING . . .\nISO UDS LOGGING CLOSED" << std::endl;
    LogFile.close();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Kill
 * @class       ISO_UDS (Public)
 * @brief       Kill UDS Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::Kill (void) {
  if (LogFile.is_open()) {
    LogFile << "STOPPED\nPROCESS KILLED\nISO UDS LOGGING CLOSED" << std::endl;
    LogFile.close();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        GetERROR
 * @class       ISO_UDS (Public)
 * @brief       Get Error Code of UDS
 * @param []    Nothing
 * @return      Error Code of UDS
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::GetERROR (void) {
  return CONFIG.ERRORCODE;
}
/* ==================================================================================================== */


// MARK: Security DLL
/* ==================================================================================================== */
/**
 * @name        ValidateSecurityDll
 * @class       ISO_UDS (Private)
 * @brief       Validate the Security Access DLL and Executes it using Dummy Seed Values
 * @param [DLLPath]   DLL File Path
 * @return      Status Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ValidateSecurityDll (const std::string& DLLPath) {
  HMODULE sDLL = LoadLibraryA(DLLPath.c_str());
  if (!sDLL) { return 1; }
  auto generateKeyEx = (GenerateKeyExFunc) GetProcAddress (sDLL, "GenerateKeyEx");
  if (!generateKeyEx) { FreeLibrary(sDLL); return 2; }

  unsigned char Seed[16] =
      { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
  unsigned char Key[16] = {0};
  unsigned int KeySize = 0;
  int Result = generateKeyEx( Seed, 16, 1, "TestVariant", Key, sizeof(Key), KeySize );

  if (Result != 0) { FreeLibrary(sDLL); return 2; }
  FreeLibrary(sDLL);
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SecurityDLL
 * @class       ISO_UDS (Private)
 * @brief       Executes Security Access DLL and Returns Key for the Seed
 * @param [DLLPath]   DLL File Path
 * @param [SEED]      Security Access Structure for SEED
 * @param [KEY]       Security Access Structure for KEY
 * @return      Status Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::SecurityDLL (const std::string& DLLPath, UDSAECURITYACCESS& SEED, UDSAECURITYACCESS& KEY) {
  HMODULE sDLL = LoadLibraryA(DLLPath.c_str());
  if (!sDLL) { return 1; }
  auto generateKeyEx = (GenerateKeyExFunc) GetProcAddress (sDLL, "GenerateKeyEx");
  if (!generateKeyEx) { FreeLibrary(sDLL); return 2; }

  int Result = generateKeyEx ( SEED.DATA, SEED.LENGTH, SEED.LEVEL, "Universal-UDS",
      KEY.DATA, sizeof(KEY.DATA), KEY.LENGTH );
  KEY.LEVEL = SEED.LEVEL + 1;
  if (Result != 0) { FreeLibrary(sDLL); return 2; }
  FreeLibrary(sDLL);
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ExecuteSecurityAccess
 * @class       ISO_UDS (Private)
 * @brief       Executes Security Access
 * @param [DLLPath]   DLL File Path
 * @param [SERVICE]   Service Name
 * @param [Type]      1 for Seed Only, 2 for Key only, else for Seed & Key Pair
 * @param [Level]     Level for Security Access
 * @param [Add]       Playlist Add Flag Active High
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ExecuteSecurityAccess (const std::string& DLLPath, const std::string& SERVICE, uint8_t Type,
    uint8_t Level, uint8_t Add) {
  if (Add) {
    UDSPLAYLIST PLAY;
    PLAY.SUPPRESS = "N";
    PLAY.ADDRESS = "P";
    PLAY.FUNCTION = "Security Access";
    if (Type == 1) {
      PLAY.REQUEST = "Seed For Level " + to_string(Level);
    } else if ( Type == 2) {
      PLAY.REQUEST = "Key For Level " + to_string(Level);
    } else {
      PLAY.REQUEST = "Seed & Key For Level " + to_string(Level);
    }
    PLAY.RESPONSE = DLLPath;

    ClearScreen();
    RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
    RenderHeader();
    ConsoleColor();
    cout << "\n\n SAVE TO PLAYLIST : ";
    cout << "\n\n SERVICE : " << PLAY.FUNCTION;
    cout << "\n REQUEST : " << PLAY.REQUEST;
    cout << "\n ADDRESING : ";
    ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "PHYSICAL"; cout << "  ";
    ConsoleColor(); cout << "FUNCTIONAL";
    cout << "\n RESPONSE SUPPRESS : "; cout << "YES"; cout << "  ";
    ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "NO"; ConsoleColor();
    cout << "\n RESPONSE DLL : " << PLAY.RESPONSE;
    cout << "\n\n SAVE TO PLAYLIST : ";

    HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
    GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
    COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
    int ENTER = 0, SAVE = 0;
    do {
      SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
      (SAVE == 0) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
      cout << "YES"; cout << "  ";
      (SAVE == 1) ? ConsoleColor(CONSOLECOLOUR_LIGHTAQUA) : ConsoleColor();
      cout << "NO";
      int PRESS = 0;
      do {
        if(_kbhit()) {
          int KEY = _getch();
          if (KEY == CONARROWPREFIX1 || KEY == CONARROWPREFIX2) {
            KEY = _getch();
            switch(KEY) {
              case CONARROWLEFT  : { SAVE = 0; PRESS = 1; break; } // ARROW LEFT KEY
              case CONARROWRIGHT : { SAVE = 1; PRESS = 1; break; } // ARROW RIGHT KEY
            }
          }
          else if (KEY == CONENTER) { PRESS = 1; ENTER = 1; } // ENTER KEY
          else if (KEY == CONBACKSPACE) { return; }  // BACKSPACE KEY
          else if (KEY == CONESCAPE) { RenderExitPage(); return; } // ESCAPE KEY
        }
      } while (PRESS == 0);
    } while (ENTER == 0);
    ConsoleColor();

    if (SAVE) {
      FLAGS.PLAYOUT = 1; return;
    } else {
      ConsoleColor(CONSOLECOLOUR_LIGHTGREEN);
      cout << "\n\n SAVED TO PLAYLIST";
      ConsoleColor();
      if (FLAGS.MIDADD) {
        PLAYLIST.insert(PLAYLIST.begin() + FLAGS.MIDINDEX, PLAY);
      } else {
        PLAYLIST.push_back(PLAY);
      }
      FLAGS.PLAYOUT = 1;
      Sleep(750); return;
    }
  }
  else {
    ClearScreen();
    RenderSHORTLOGO(UI.LOGO_X, UI.LOGO_Y);
    RenderHeader();
    int KEY = 0;
    if ((Type == 1) || (Type == 2)) { // SEED OR KEY ONLY
      UDSREQRES REQ;
      if (Type == 1) { // SEED ONLY
        MESSAGE.DATA[0] = 0x27;  REQ.DATA.push_back(MESSAGE.DATA[0]);
        MESSAGE.DATA[1] = Level; REQ.DATA.push_back(MESSAGE.DATA[1]);
        MESSAGE.LEN = 2; REQ.LEN = MESSAGE.LEN;
      } else { // KEY ONLY
        UDSAECURITYACCESS sKEY;
        int ResultDLL = SecurityDLL (DLLPath, SECURE, sKEY);
        if (ResultDLL) {
          cout << "\n\n REQUEST EXECUTOR : "; cout << "\n\n SERVICE : " << SERVICE;
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n   ERROR In Executing DLL"; ConsoleColor();
          do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
        }
        else {
          MESSAGE.DATA[0] = 0x27;  REQ.DATA.push_back(MESSAGE.DATA[0]);
          MESSAGE.DATA[1] = Level; REQ.DATA.push_back(MESSAGE.DATA[1]);
          for (uint16_t I = 0; I < sKEY.LENGTH; I++) {
            MESSAGE.DATA[I + 2] = sKEY.DATA[I]; REQ.DATA.push_back(MESSAGE.DATA[I + 2]);
          } MESSAGE.LEN = sKEY.LENGTH + 2; REQ.LEN = MESSAGE.LEN;
        }
      }
      cout << "\n\n REQUEST EXECUTOR : ";
      cout << "\n\n SERVICE : " << SERVICE;
      cout << "\n\n REQUEST MESSAGE :";
      RequestRender(REQ, 4); ConsoleColor();
      DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_PHY);
      std::string ErrorLine;
      int Result = Executor(1, ErrorLine);
      if (Result) {
        cout << "\n     " << ErrorLine;
        ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
        do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
      }
      else {
        if (MESSAGE.DATA[0] == 0x67) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "POSITIVE";
          ConsoleColor();
        } else if (MESSAGE.DATA[0] == 0x7F) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "NEGATIVE" << NegativeResponse(MESSAGE.DATA[2]);
          ConsoleColor();
        } else {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "UPEXPECTED";
          ConsoleColor();
        }
      }
      UDSREQRES RES;
      for (int I = 0; I < MESSAGE.LEN; I++) { RES.DATA.push_back(MESSAGE.DATA[I]); }
      RequestRender(RES, 4);
      if (Type == 1) {
        if ((MESSAGE.LEN <= 18) && (MESSAGE.LEN > 2)) {
          for (int I = 2; I < MESSAGE.LEN; I++) { SECURE.DATA[I - 2] = MESSAGE.DATA[I]; }
          SECURE.LENGTH = MESSAGE.LEN - 2; SECURE.LEVEL = Level;
        } else {
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n   Seed Should be Less Than Equal To 16";
          ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
        }
      }
      do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
    }
    else { // BOTH SEED AND KEY
      UDSREQRES REQ1;
      MESSAGE.DATA[0] = 0x27;  REQ1.DATA.push_back(MESSAGE.DATA[0]);
      MESSAGE.DATA[1] = Level; REQ1.DATA.push_back(MESSAGE.DATA[1]);
      MESSAGE.LEN = 2; REQ1.LEN = MESSAGE.LEN;
      cout << "\n\n REQUEST EXECUTOR : ";
      cout << "\n\n SERVICE : " << SERVICE << " Seed";
      cout << "\n\n REQUEST MESSAGE :";
      RequestRender(REQ1, 4); ConsoleColor();
      DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_PHY);
      std::string ErrorLine;
      int Result = Executor(1, ErrorLine);
      if (Result) {
        cout << "\n     " << ErrorLine;
        ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
        do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
      }
      else {
        if (MESSAGE.DATA[0] == 0x67) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "POSITIVE";
          ConsoleColor();
        } else if (MESSAGE.DATA[0] == 0x7F) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "NEGATIVE" << NegativeResponse(MESSAGE.DATA[2]);
          ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
          do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
        } else {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "UPEXPECTED";
          ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
          do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
        }
      }
      UDSREQRES RES1;
      for (int I = 0; I < MESSAGE.LEN; I++) { RES1.DATA.push_back(MESSAGE.DATA[I]); }
      RequestRender(RES1, 4);
      if ((MESSAGE.LEN <= 18) && (MESSAGE.LEN > 2)) {
        for (int I = 2; I < MESSAGE.LEN; I++) { SECURE.DATA[I - 2] = MESSAGE.DATA[I]; }
        SECURE.LENGTH = MESSAGE.LEN - 2; SECURE.LEVEL = Level;
      } else {
        ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n   Seed Should be Less Than Equal To 16";
        ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
        do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
      }

      UDSAECURITYACCESS sKEY;
      UDSREQRES REQ2;
      int ResultDLL = SecurityDLL (DLLPath, SECURE, sKEY);
      if (ResultDLL) {
        cout << "\n\n SERVICE : " << SERVICE << " Key";
        ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "\n   ERROR In Executing DLL"; ConsoleColor();
        do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
      }
      else {
        MESSAGE.DATA[0] = 0x27;         REQ2.DATA.push_back(MESSAGE.DATA[0]);
        MESSAGE.DATA[1] = SECURE.LEVEL; REQ2.DATA.push_back(MESSAGE.DATA[1]);
        for (uint16_t I = 0; I < sKEY.LENGTH; I++) {
          MESSAGE.DATA[I + 2] = sKEY.DATA[I]; REQ2.DATA.push_back(MESSAGE.DATA[I + 2]);
        } MESSAGE.LEN = sKEY.LENGTH + 2; REQ2.LEN = MESSAGE.LEN;
      }
      cout << "\n\n SERVICE : " << SERVICE << " Key";
      cout << "\n\n REQUEST MESSAGE :";
      RequestRender(REQ2, 4); ConsoleColor();
      DoCAN.SetAddressingMode(ISO_DoCAN::DoCAN_PHY);
      Result = Executor(1, ErrorLine);
      if (Result) {
        cout << "\n     " << ErrorLine;
        ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
        do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
      }
      else {
        if (MESSAGE.DATA[0] == 0x67) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTGREEN); cout << "POSITIVE";
          ConsoleColor();
        } else if (MESSAGE.DATA[0] == 0x7F) {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "NEGATIVE" << NegativeResponse(MESSAGE.DATA[2]);
          ConsoleColor();
        } else {
          cout << "\n\n RESPONSE : ";
          ConsoleColor(CONSOLECOLOUR_LIGHTRED); cout << "UPEXPECTED";
          ConsoleColor();
        }
      }
      UDSREQRES RES2;
      for (int I = 0; I < MESSAGE.LEN; I++) { RES2.DATA.push_back(MESSAGE.DATA[I]); }
      RequestRender(RES2, 4);
      ConsoleColor(CONSOLECOLOUR_LIGHTAQUA); cout << "\n\n BACK"; ConsoleColor();
      do { if(_kbhit()) { KEY = _getch(); } } while (KEY != CONENTER); return;
    }
  }
}
/* ==================================================================================================== */


// MARK: String HEX Calculation
/* ==================================================================================================== */
/**
 * @name        StringToHEX
 * @class       ISO_UDS (Priavte)
 * @brief       String Hexadecimal To Number
 * @param [Number]      Number Variable Pointer
 * @param [NumberSTR]   Number String
 * @return      Validity Check Active LOW
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::StringToHEX (uint8_t &Number ,const std::string& NumberSTR) {
  if (NumberSTR.length() != 2) { return 1; }
  for (char NumberCHAR : NumberSTR) {
    if (!isxdigit(NumberCHAR)) { return 1; }
  }
  uint8_t NumberExtract = static_cast<uint8_t>(std::stoi(NumberSTR, nullptr, 16));
  Number = NumberExtract;
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        HEXToString
 * @class       ISO_UDS (Priavte)
 * @brief       Hexadecimal to String
 * @param [Number]      Number
 * @return      Hexadecimal ASCII String
 */
/* ---------------------------------------------------------------------------------------------------- */
std::string ISO_UDS::HEXToString (uint8_t Data) {
  std::stringstream Steam;
  Steam << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(Data);
  return Steam.str();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ValidRequestString
 * @class       ISO_UDS (Priavte)
 * @brief       Validate Request & Response String Format
 * @param [Input]      Request or Response String
 * @return      Validity Check OK is 0 and Abort is 2
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ValidRequestString (std::string& Input) {
  size_t StartPoint = Input.find_first_not_of(" \t\n\r");
  size_t EndPoint   = Input.find_last_not_of(" \t\n\r");
  if (StartPoint == std::string::npos || EndPoint == std::string::npos || EndPoint < StartPoint) {
    Input = ""; return 1;
  } else {
    Input = Input.substr(StartPoint, EndPoint - StartPoint + 1);
  }
  if (Input.empty()) { return 1; }
  if ((Input.find('X') != std::string::npos) || (Input.find('x') != std::string::npos)) {
    return 2;
  }
  std::istringstream InputStream(Input);
  std::string ByteString;
  while (InputStream >> ByteString) {
    if (ByteString.length() != 2) { return 1; }
    for (char C : ByteString) { if (!isxdigit(C)) { return 1; } }
  }
  return 0;
}
/* ==================================================================================================== */


// MARK: Error Codes
/* ==================================================================================================== */
/**
 * @name        ErrorCodeDoCAN
 * @class       ISO_UDS (Private)
 * @brief       Get DoCAN Issue
 * @param []    Nothing
 * @return      DoCAN Issue String
 */
/* ---------------------------------------------------------------------------------------------------- */
std::string ISO_UDS::ErrorCodeDoCAN (void) {
  int ERR = DoCAN.GetErrorCode();
  switch (ERR) {
    case 0 : { return "00 ALL OK"; }
    case 1 : { return "01 FRAME ISSUE"; }
    case 2 : { return "02 FRAME OVER FLOW"; }
    case 3 : { return "03 WRONG CAN ID"; }
    case 4 : { return "04 WRONG FRAME COUNTER"; }
    case 5 : { return "05 DRIVER FAILER"; }
    case 6 : { return "06 INTERNAL ISSUE"; }
    case 7 : { return "07 SEND CAN ISSUE"; }
    case 8 : { return "08 WAIT TIMEOUT"; }
    case 9 : { return "09 EXTERNAL CONFIGATION"; }
    case 10 : { return "10 LOGGING ISSUE"; }
    case 11 : { return "11 TIMEOUT"; }
    default : { return "XX INTERNAL ISSUE"; }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        NegativeResponse
 * @class       ISO_UDS (Private)
 * @brief       Get Negative Response String
 * @param [NR]    NRC Code
 * @return      Negative Response String
 */
/* ---------------------------------------------------------------------------------------------------- */
std::string ISO_UDS::NegativeResponse (uint8_t NR) {
  switch (NR) {
    case 0x10 : { return "10 General Reject"; }
    case 0x11 : { return "11 Service Not Supported"; }
    case 0x12 : { return "12 Sub Function Not Supported"; }
    case 0x13 : { return "13 Incorrect Message Length or Invalid Format"; }
    case 0x14 : { return "14 Response Too Long"; }
    case 0x21 : { return "21 Busy Repeat Request"; }
    case 0x22 : { return "22 Conditions Not Correct"; }
    case 0x24 : { return "24 Request Sequence Error"; }
    case 0x31 : { return "31 Request Out of Range"; }
    case 0x33 : { return "33 Security Access Denied"; }
    case 0x34 : { return "34 Authentication Required"; }
    case 0x35 : { return "35 Invalid Key"; }
    case 0x36 : { return "36 Exceed Number of Attempts"; }
    case 0x37 : { return "37 Required Time Delay Not Expired"; }
    case 0x70 : { return "70 Upload Download Not Accept"; }
    case 0x71 : { return "71 Transfer Data Suspended"; }
    case 0x72 : { return "72 General Programming Failure"; }
    case 0x73 : { return "73 Wrong Block Sequence Counter"; }
    case 0x78 : { return "78 Request Correctly Received Response Pending"; }
    case 0x7E : { return "7E Sub Function Not Supported In Active Session"; }
    case 0x7F : { return "7F Service Not Supported In Active Session"; }
    case 0x86 : { return "86 Temperature Too High"; }
    case 0x87 : { return "87 Temperature Too Low"; }
    case 0x92 : { return "92 Voltage Too High"; }
    case 0x93 : { return "93 Voltage Too Low"; }
    default : { return "XX Unknown";}
  }
}
/* ==================================================================================================== */


// MARK: UI List Calculations
/* ==================================================================================================== */
/**
 * @name        ListCalculatePosition
 * @class       ISO_UDS (Priavte)
 * @brief       List Navigation Calculation
 * @param [LUI]           List UI Structure
 * @param [Direction]     Direction for Arrow Key (Enum)
 * @return      Calculated List UI Structure
 */
/* ---------------------------------------------------------------------------------------------------- */
ISO_UDS::UDSLIST ISO_UDS::ListCalculatePosition (UDSLIST LUI, CONFIGATIONS Direction) {
  int VARIABLE = 0;
  if (LUI.PAGE >= LUI.TOTAL) {
    VARIABLE = 0; LUI.START = 0; LUI.END = LUI.TOTAL;
  } else {
    VARIABLE = 1;
  }

  if (LUI.START < 0) { LUI.START = 0; }
  if (LUI.END > LUI.TOTAL) { LUI.END = LUI.TOTAL; }
  if (VARIABLE) {
    if (LUI.END - LUI.START != LUI.PAGE) {
      LUI.END = LUI.START + LUI.PAGE;
      if (LUI.END > LUI.TOTAL) {
        LUI.END = LUI.TOTAL;
        LUI.START = LUI.END - LUI.PAGE;
      }
    }
  } else { LUI.START = 0; LUI.END = LUI.TOTAL; }
  if (Direction == 2) { return LUI; } // CALIBRATION CALCULATION

  if (Direction == 1) { // UP ARROW
    if(VARIABLE) {
      if (LUI.START > 0) {
        LUI.START--; LUI.END--;
      }
    }
  }
  else if (Direction == 0) { // DOWN ARROW
    if(VARIABLE) {
      if (LUI.END < LUI.TOTAL) {
        LUI.START++; LUI.END++;
      }
    }
  }
  return LUI;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        CalculatePosition
 * @class       ISO_UDS (Priavte)
 * @brief       Variable List Navigation Calculation
 * @param [LUI]           List UI Structure
 * @param [Direction]     Direction for Arrow Key (Enum)
 * @return      Calculated List UI Structure
 */
/* ---------------------------------------------------------------------------------------------------- */
ISO_UDS::UDSLIST ISO_UDS::CalculatePosition (UDSLIST LUI, CONFIGATIONS Direction) {
  int VARIABLE = 0;
  if (LUI.PAGE >= LUI.TOTAL) {
    VARIABLE = 0; LUI.START = 0; LUI.END = LUI.TOTAL;
  } else {
    VARIABLE = 1;
  }

  if (LUI.START < 0) { LUI.START = 0; }
  if (LUI.END > LUI.TOTAL) { LUI.END = LUI.TOTAL; }
  if (VARIABLE) {
    if (LUI.END - LUI.START != LUI.PAGE) {
      LUI.END = LUI.START + LUI.PAGE;
      if (LUI.END > LUI.TOTAL) {
        LUI.END = LUI.TOTAL;
        LUI.START = LUI.END - LUI.PAGE;
      }
    }
  } else { LUI.START = 0; LUI.END = LUI.TOTAL; }
  if ( LUI.SELECTED < LUI.START) { LUI.SELECTED = LUI.START; }
  if ( LUI.SELECTED >= LUI.END) { LUI.SELECTED = (LUI.END - 1); }

  if (Direction == 2) { return LUI; } // CALIBRATION CALCULATION

  if (Direction == 1) { // UP ARROW
    if(VARIABLE) {
      if (LUI.SELECTED > 0) {
        LUI.SELECTED--;
        if (LUI.SELECTED < LUI.START) {
          LUI.START--; LUI.END--;
        }
      }
    } else { LUI.SELECTED = ((LUI.SELECTED == 0 ) ? (LUI.TOTAL - 1) : (LUI.SELECTED - 1)); }
  }
  else if (Direction == 0) { // DOWN ARROW
    if(VARIABLE) {
      if (LUI.SELECTED < LUI.TOTAL - 1) {
        LUI.SELECTED++;
        if (LUI.SELECTED >= LUI.END) {
          LUI.START++; LUI.END++;
        }
      }
    } else { LUI.SELECTED = (LUI.SELECTED + 1) % LUI.TOTAL; }
  }
  return LUI;
}
/* ==================================================================================================== */


// MARK: Console Handlers
/* ==================================================================================================== */
/**
 * @name        ConsoleCursor
 * @class       ISO_UDS (Private)
 * @brief       Shows or Hide Cursor
 * @param [Show]    Show Cursor Active HIGH
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ConsoleCursor (uint8_t Show) {
  if (Show) {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
  } else {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ConsoleColor
 * @class       ISO_UDS (Private)
 * @brief       Sets Colour for Console
 * @param [Colour]        Text Colur (Enum)
 * @param [Background]    Background Colur (Enum)
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ConsoleColor (CONSOLECOLOURS Colour, CONSOLECOLOURS Background) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  int SafeColour = (uint8_t)(Colour & 0x0F);
  int SafeBackground = (uint8_t)(Background & 0x0F);
  int ConsoleColour = (SafeColour) | (SafeBackground << 4);
  SetConsoleTextAttribute(ConsoleHandle, ConsoleColour);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ClearScreen
 * @class       ISO_UDS (Private)
 * @brief       Clear Screen
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ClearScreen (void) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  WORD DefaultSettings = ConsoleBuffer.wAttributes;
  SetConsoleTextAttribute(ConsoleHandle, DefaultSettings);
  DWORD ConsoleSize = ConsoleBuffer.dwSize.X * ConsoleBuffer.dwSize.Y;
  COORD Origin = {0, 0};
  DWORD CharsWritten;
  FillConsoleOutputCharacter(ConsoleHandle, ' ', ConsoleSize, Origin, &CharsWritten);
  FillConsoleOutputAttribute(ConsoleHandle, DefaultSettings, ConsoleSize, Origin, &CharsWritten);
  SetConsoleCursorPosition(ConsoleHandle, Origin);
}
/* ==================================================================================================== */


// MARK: Dashboard JSON File Handler
/* ==================================================================================================== */
/**
 * @name        ValidateDashboard
 * @class       ISO_UDS (Private)
 * @brief       Validate Dashboard Contents
 * @param []    Nothing
 * @return      Error Code Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ValidateDashboard (void) {
  int OK = 0; ConsoleColor(CONSOLECOLOUR_LIGHTRED);
  for (const auto& SER : SERVICEDIRECTORY) {
    std::istringstream StreamSID(SER.SID);
    std::string ByteString;
    uint8_t Data = 0;
    while (StreamSID >> ByteString) {
      if (StringToHEX(Data, ByteString)) {
        cout << "\n ERROR : Format Rendering Error in \"" << SER.SID << "\"";
        OK = 1; break;
      }
    } if (OK) { break; }
    for (const auto& FUN : SER.FUNCTIONS) {
      std::istringstream StreamRequest(FUN.REQUEST);
      while (StreamRequest >> ByteString) {
        if (StringToHEX(Data, ByteString)) {
          cout << "\n ERROR : Format Rendering Error in \"" << FUN.REQUEST << "\"";
          OK = 1; break;
        }
      } if (OK) { break; }
    } if (OK) { break; }
  } ConsoleColor(); return OK;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ShowDashboard
 * @class       ISO_UDS (Private)
 * @brief       Show the DashBoard Tree
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::ShowDashboard (void) {
  cout << "\n[";
  for (const auto& SER : SERVICEDIRECTORY) {
    cout << "\n  {";
    cout << "\n    SERVICE : " << SER.SERVICE;
    cout << "\n    SID : " << SER.SID;
    cout << "\n    FUNCTIONS : [";
    for (const auto& FUN : SER.FUNCTIONS) {
      cout << "\n      {";
      cout << "\n          FUNCTION : " << FUN.FUNCTION;
      cout << "\n          REQUEST : " << FUN.REQUEST;
      cout << "\n      }";
    }
    cout << "\n    ]";
    cout << "\n  }";
  }
  cout << "\n]";
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ReadDashboard
 * @class       ISO_UDS (Private)
 * @brief       Read & Make Dashboard Tree
 * @param []    Nothing
 * @return      Error Code Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ReadDashboard (void) {
  ifstream Dashboard(CONFIG.DASHBOARDPATH);
  if (!Dashboard) {
    CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
    return 1;
  }
  SERVICEDIRECTORY.clear();

  UDSSERVICES SA; UDSFUNCTION SF;
  SF.FUNCTION = "Programming Security"; SF.REQUEST = "27 01 02";
  SA.SERVICE = "Security Access"; SA.SID = "27";
  SA.FUNCTIONS.push_back(SF);
  SERVICEDIRECTORY.push_back(SA);

  json JSON;
  Dashboard >> JSON;
  UI.DASH.TOTAL = 0;

  for (const auto& Service : JSON) {
    if (!Service.contains("SERVICE") || !Service["SERVICE"].is_string() ||
        !Service.contains("SID")     || !Service["SID"].is_string()     ||
        !Service.contains("FUNCTION")|| !Service["FUNCTION"].is_string()||
        !Service.contains("REQUEST") || !Service["REQUEST"].is_string()) {
      CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
      return 2;
    }
    std::string SERVICE = Service["SERVICE"];
    std::string SID = Service["SID"];
    std::string FUNCTION = Service["FUNCTION"];
    std::string REQUEST = Service["REQUEST"];

    int ServiceIndex = 0;
    int FoundService = 0;
    int Unique = 0;
    for (const auto& SER : SERVICEDIRECTORY) {
      if (SER.SERVICE == SERVICE) {
        FoundService = 1;
        for (const auto& FUN : SER.FUNCTIONS) {
          if (FUN.REQUEST == REQUEST) {
            Unique = 1; break;
          }
        }
        break;
      }
      ServiceIndex++;
    }
    if (Unique == 0) {
      UDSFUNCTION UDS_FUN;
      UDS_FUN.FUNCTION = FUNCTION;
      UDS_FUN.REQUEST = REQUEST;
      if (FoundService) {
        SERVICEDIRECTORY[ServiceIndex].FUNCTIONS.push_back(UDS_FUN);
      }
      else {
        UDSSERVICES UDS_SER;
        UDS_SER.SERVICE = SERVICE;
        UDS_SER.SID = SID;
        UDS_SER.FUNCTIONS.push_back(UDS_FUN);
        SERVICEDIRECTORY.push_back(UDS_SER);
        UI.DASH.TOTAL++;
      }
    }
  }
  UI.DASH.TOTAL += 4;
  Dashboard.close();
  return 0;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ReadConfiguration
 * @class       ISO_UDS (Private)
 * @brief       Read and Load Configuration File
 * @param []    Nothing
 * @return      Error Code Active Low
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_UDS::ReadConfiguration (void) {
  ifstream Config(CONFIG.CONFIGPATH);
  if (!Config) {
    CONFIG.ERRORCODE = UDS_ERR_INTERNALISSUE;
    return 1;
  }
  json JSON;
  try { Config >> JSON; } catch (const json::parse_error& E) { return 2; }
  if (!JSON.contains("REQ-ID")  || !JSON["REQ-ID"].is_string())          { return 2; }
  if (!JSON.contains("RES-ID")  || !JSON["RES-ID"].is_string())          { return 2; }
  if (!JSON.contains("FUN-ID")  || !JSON["FUN-ID"].is_string())          { return 2; }
  if (!JSON.contains("STMIN")   || !JSON["STMIN"].is_number_integer())   { return 2; }
  if (!JSON.contains("BLOCK")   || !JSON["BLOCK"].is_number_integer())   { return 2; }
  if (!JSON.contains("PADDING") || !JSON["PADDING"].is_number_integer()) { return 2; }
  if (!JSON.contains("P2")      || !JSON["P2"].is_number_integer())      { return 2; }
  if (!JSON.contains("P2*")     || !JSON["P2*"].is_number_integer())     { return 2; }
  std::string REQs = JSON["REQ-ID"];
  std::string RESs = JSON["RES-ID"];
  std::string FUNs = JSON["FUN-ID"];
  uint32_t REQh, RESh, FUNh;
  try {
    REQh = std::stoul(REQs, nullptr, 16);
    RESh = std::stoul(RESs, nullptr, 16);
    FUNh = std::stoul(FUNs, nullptr, 16);
  }
  catch (const std::invalid_argument& E)  { return 2; }
  catch (const std::out_of_range& E)      { return 2; }

  CONFIG.PADDING = JSON["PADDING"];
  CONFIG.STMIN = JSON["STMIN"];
  CONFIG.BLOCKS = JSON["BLOCK"];
  CONFIG.P2STAR = JSON["P2*"];
  CONFIG.P2 = JSON["P2"];
  CONFIG.REQID = REQh;
  CONFIG.RESID = RESh;
  CONFIG.FUNID = FUNh;
  return 0;
}
/* ==================================================================================================== */


// MARK: Logo Rendering
/* ==================================================================================================== */
/**
 * @name        RenderHeader
 * @class       ISO_UDS (Private)
 * @brief       Render Header
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderHeader (void) {
  ConsoleColor(CONSOLECOLOUR_LIGHTYELLOW);
  cout << "\n UNIFIED DIAGONOSTICS SERVICES";
  cout << "\n ISO 14229 - 2022";
  cout << "\n UNIVERSAL-UDS";
  cout << "\n v2.7:0";
  ConsoleColor();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderLOGO
 * @class       ISO_UDS (Private)
 * @brief       Render Company Logo
 * @param [X]     Console X Coordinate
 * @param [Y]     Console Y Coordinate
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderLOGO (int X, int Y) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  COORD Coordinates;
  Coordinates.X = X;
  Coordinates.Y = Y;
  ConsoleColor(CONSOLECOLOUR_LIGHTBLUE);

  SetConsoleOutputCP(CP_UTF8);

  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                                    ████████████████                                        ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                               ████                  ████                                   ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                             ██                        ███████                              ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                           ██                                 ██                            ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                          █                                      ██                         ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                         █                                         ███                      ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                        ██                                            █████                 ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                        ██                                                 ████             ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                        ███            ██                         █        █    ███         ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                        ██████          ██          █  █           █               ██████   ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                 █████████████          ██          █ ██          █     █        █     ███  ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "              ███        █             ████       █████       █████  █  █               ██  ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "            █                        ████████████                █   █  █             █  █  ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "         ████     ███            ███████                          ███   █     ███████████   ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "         ███████████████████████████████          ███       ████████████████████            ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "                               ███████████████████████████████████                          ";
  Coordinates.Y++;
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "      .____       _____  _______________.___. _____________________   _____ __________ ";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "      |    |     /  _  \\ \\____    /\\__  |   | \\______   \\_   _____/  /  _  \\\\______   \\";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "      |    |    /  /_\\  \\  /     /  /   |   |  |    |  _/|    __)_  /  /_\\  \\|       _/";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "      |    |___/    |    \\/     /_  \\____   |  |    |   \\|        \\/    |    \\    |   \\";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "      |_______ \\____|__  /_______ \\ / ______|  |______  /_______  /\\____|__  /____|_  /";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
std::cout << "              \\/       \\/        \\/ \\/                \\/        \\/         \\/       \\/ ";
  ConsoleColor();
  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        RenderSHORTLOGO
 * @class       ISO_UDS (Private)
 * @brief       Render Short Company Logo
 * @param [X]     Console X Coordinate
 * @param [Y]     Console Y Coordinate
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_UDS::RenderSHORTLOGO (int X, int Y) {
  HANDLE ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO ConsoleBuffer;
  GetConsoleScreenBufferInfo(ConsoleHandle, &ConsoleBuffer);
  COORD OriginalCoordinates = ConsoleBuffer.dwCursorPosition;
  COORD Coordinates;
  Coordinates.X = X;
  Coordinates.Y = Y;
  ConsoleColor(CONSOLECOLOUR_AQUA);

  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
    cout << "© ALAKSHENDRA";
  Coordinates.Y++;
  SetConsoleCursorPosition(ConsoleHandle, Coordinates);
    cout << "   LAZY BEARS";
  ConsoleColor();
  SetConsoleCursorPosition(ConsoleHandle, OriginalCoordinates);
}
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @end       End of File UDS.h
 */
/* ---------------------------------------------------------------------------------------------------- */
#endif  // _UDS
/* ==================================================================================================== */