/* ==================================================================================================== */
/*
 *  DoCAN.h
 *  Diagonostics on CAN (DoCAN)
 *    ISO: 14229 Part 3   - UDSonCAN (DoCAN)
 *  Version: v2.7:0
 *  Developed By: Alakshendra Singh
 *  For Reporting Any Issue Don't Contact Me. Fix Yourself
 */
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @file        DoCAN.h
 * @author      Alakshendra Singh
 * @brief       Diagonstics on CAN (DoCAN)
 * @version     1.0
 *
 * @copyright   Copyright (c) 2025
 */
/* ==================================================================================================== */

#ifndef _DoCAN
#define _DoCAN

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iomanip>
#include <chrono>
#include <bitset>
#include <fstream>
#include <string>

#include "DriverPCAN.hpp"
#include "UDS.hpp"

using Clock = std::chrono::steady_clock;

#define DoCAN_ERR_FRAMEOK                 0
#define DoCAN_ERR_FRAMEISSUE              1
#define DoCAN_ERR_FRAMEOVERFLOW           2
#define DoCAN_ERR_WRONGCANID              3
#define DoCAN_ERR_WRONGFRAMECOUNTER       4
#define DoCAN_ERR_DRIVERFAILER            5
#define DoCAN_ERR_INTERNALISSUE           6
#define DoCAN_ERR_SENDCANISSUE            7
#define DoCAN_ERR_WAITTIMEOUT             8
#define DoCAN_ERR_EXTERNALCONFIGATION     9
#define DoCAN_ERR_LOGGINGISSUE            10
#define DoCAN_ERR_TIMEOUT                 11

#define DoCAN_RX_WORKING                  0
#define DoCAN_RX_IDLE                     1
#define DoCAN_RX_COMPLETE                 2
#define DoCAN_RX_ERROR                    3
#define DoCAN_TX_WORKING                  0
#define DoCAN_TX_IDLE                     1
#define DoCAN_TX_COMPLETE                 2
#define DoCAN_TX_ERROR                    3


/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_DoCAN {

  public :
    DriverPCAN PCAN;
    
    enum DoCANStatus {
      DoCAN_Idle = 0,
      DoCAN_Receive = 1,
      DoCAN_Transmit = 2,
      DoCAN_Wait = 3,
    };

    enum DoCANAddress {
      DoCAN_PHY = 0,
      DoCAN_FUN = 1,
    };

    enum DoCANLogging {
      DoCAN_LOGOFF = 0,
      DoCAN_LOGON = 1,
    };

  protected :

    struct DoCANMESSAGE {
      uint32_t ID;
      uint8_t LEN;
      uint8_t TYPE;
      uint8_t DATA[8];
    }; DoCANMESSAGE MESSAGE;

  private :

    std::chrono::_V2::steady_clock::time_point StartTime;
    ofstream LogFile;

    struct DoCANCONFIGURATION {
      uint32_t CANID_TX;
      uint32_t CANID_RX;
      uint32_t CANID_FN;
      uint16_t SPEED;
      uint8_t PADDING;
      uint8_t STMIN;
      uint8_t BLOCKS;
      uint8_t * DATA;
      uint16_t * LEN;
      uint32_t * ID;
      uint16_t LENGTH;
      uint64_t P2;
      uint64_t P2_STAR;
      uint8_t ERRORCODE;
      DoCANAddress MODE;
      uint8_t INIT;
      DoCANLogging LOGMODE;
    }; DoCANCONFIGURATION CONFIG;

    struct DoCANSETTINGTX {
      uint64_t STMIN;
      uint8_t BLOCKS;
      uint16_t FRAMES;
      uint16_t INDEX;
      uint8_t BLOCKCOUNTER;
      uint8_t WAIT;
      uint8_t TXFLAG;
    }; DoCANSETTINGTX SETTINGS_TX;

    struct DoCANSETTINGRX {
      DoCANStatus STATUS;
      uint64_t TIME;
      uint16_t LENGTH;
      uint16_t FRAMES;
      uint16_t COUNTER;
      uint8_t BLOCKCOUNTER;
      uint16_t INDEX;
      uint8_t RXFLAG;
    }; DoCANSETTINGRX SETTINGS_RX;

    struct DoCANTIME {
      int YY; int MM; int DD;
      int HH; int mm; int SS;
    }; DoCANTIME TIME;


    void ShowFrameCAN (DoCANMESSAGE FRAME);
    void LogMessage (char C = 'R');
    uint8_t SendCAN (void);

    void FrameRX_FC (void);
    void FrameRX_SF (void);
    void FrameRX_FF (void);
    void FrameRX_CF (void);
    void DoCAN_RX (void);

    void FrameTX_FC (uint8_t FS);
    void FrameTX_SF (void);
    void FrameTX_FF (void);
    void FrameTX_CF (void);

  public :

    ISO_DoCAN (void) {
      StartTime = Clock::now();
      CONFIG.PADDING = 0x00; CONFIG.STMIN = 0x00; CONFIG.BLOCKS = 0x00; CONFIG.LENGTH = 4095;
      SETTINGS_RX.INDEX = 0; SETTINGS_RX.BLOCKCOUNTER = 0; SETTINGS_RX.COUNTER = 0; SETTINGS_RX.FRAMES = 0;
      SETTINGS_RX.RXFLAG = 1; SETTINGS_TX.TXFLAG = 1;
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOK;
      CONFIG.MODE = DoCAN_PHY;
      CONFIG.SPEED = 500;
      CONFIG.ERRORCODE = 0;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      CONFIG.INIT = 0x00;
      CONFIG.LOGMODE = DoCAN_LOGOFF;
    }
    ~ISO_DoCAN (void) {
      Stop();
    }

    void SetCANID (uint32_t ID_TX, uint32_t ID_RX, uint32_t ID_FN);
    void SetBuffer (uint32_t * ID, uint16_t * Len, uint8_t * Data);
    void SetTiming (uint64_t P2, uint64_t P2_Star);
    void SetUDSParameter (uint8_t Padding, uint32_t STMin, uint8_t Block, uint16_t Length, uint16_t Speed);
    void SetAddressingMode (DoCANAddress Mode);
    void LoggingMode (DoCANLogging Mode, std::string Path);
    uint8_t GetErrorCode (void);
    void Start (void);
    void Stop (void);
    void Kill (void);
    string ShowParameters (void);

    uint64_t Clock (uint8_t Mode = 0);
    uint64_t MicroClock (void);
    uint64_t MilliClock (void);
    void Delay (uint64_t Time, uint8_t Mode = 0);
    void MilliDelay (uint64_t Time);
    void MicroDelay (uint64_t Time);

    uint8_t Receive (uint8_t Mode = 0, uint8_t Time = 0);
    uint8_t Write (void);

};
/* ==================================================================================================== */


/* ==================================================================================================== */
/**
 * @name        FrameRX_FC
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Flow Control Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_FC (void) {
  if ((SETTINGS_RX.STATUS == DoCAN_Wait)) {
    uint8_t FS = MESSAGE.DATA[0] & 0x0F;
    switch (FS) {
        case 0 : { // FS : Continue To Send (CTS)
          SETTINGS_RX.STATUS = DoCAN_Transmit;
          break;
        }
        case 1 : { // FS = Wait (WT)
          SETTINGS_RX.STATUS = DoCAN_Wait;
          SETTINGS_RX.TIME = MicroClock();
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_TX.WAIT = 1;
          SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
          return;
        }
        case 2 : { // FS = Overflow Flag (OF)
          CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOVERFLOW;
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
        default : {
          CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
    }
    CONFIG.BLOCKS = MESSAGE.DATA[1];
    if ( MESSAGE.DATA[2] <= 0x7F ) {
      SETTINGS_TX.STMIN = MESSAGE.DATA[2] * 1000;
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
    } else if (( MESSAGE.DATA[2] <= 0xF9 ) && ( MESSAGE.DATA[2] >= 0xF1 )) {
      SETTINGS_TX.STMIN = (MESSAGE.DATA[2] & 0x0F) * 100;
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
    } else {
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
    return;
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_SF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Single Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_SF (void) {
  if ((SETTINGS_RX.STATUS == DoCAN_Receive)) {
    uint8_t LEN = MESSAGE.DATA[0] & 0x0F;
    if ( LEN <= 7 ) {
      *CONFIG.LEN = LEN;
      *CONFIG.ID = MESSAGE.ID;
      for (uint8_t I = 0; I < LEN; I++) {
        CONFIG.DATA[I] = MESSAGE.DATA[ I + 1 ];
      }
      SETTINGS_RX.TIME = MicroClock();
      SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
    }
    else {
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_FF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive First Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_FF (void) {
  if (SETTINGS_RX.STATUS == DoCAN_Receive) {
    uint16_t LEN = 0;
    LEN = (uint16_t)( ((MESSAGE.DATA[0] & 0x0F) << 8) | MESSAGE.DATA[1] );
    if ( (LEN > 7) && (LEN <= CONFIG.LENGTH) ) {
      *CONFIG.LEN = LEN;
      *CONFIG.ID = MESSAGE.ID;
      SETTINGS_RX.LENGTH = LEN;
      for (int I = 0; I < 6; I++) {
        CONFIG.DATA[I] = MESSAGE.DATA[I + 2];
      }
      LEN = LEN - 6;
      if ( (LEN % 7) == 0 ) {
        SETTINGS_RX.FRAMES = (LEN / 7);
      } else {
        SETTINGS_RX.FRAMES = (LEN / 7) + 1;
      }

      SETTINGS_RX.BLOCKCOUNTER = 1;
      SETTINGS_RX.INDEX = 6;
      
      FrameTX_FC (0);
      if ( CONFIG.ERRORCODE == DoCAN_ERR_WRONGCANID ) {
        SETTINGS_RX.STATUS = DoCAN_Idle;
        SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
        return;
      }
      SETTINGS_RX.TIME = MicroClock();
    }
    else {
      FrameTX_FC (2);
      CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameRX_CF
 * @class       ISO_DoCAN (Private)
 * @brief       Receive Consecutive Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameRX_CF (void) {
  if (SETTINGS_RX.STATUS == DoCAN_Receive) {
    if ( (MESSAGE.DATA[0] & 0x0F) == SETTINGS_RX.BLOCKCOUNTER ) {
      SETTINGS_RX.BLOCKCOUNTER = (SETTINGS_RX.BLOCKCOUNTER + 1) % 16;
      for (uint8_t I = 1; I < 8; I++) {
        CONFIG.DATA[SETTINGS_RX.INDEX] = MESSAGE.DATA[I];
        SETTINGS_RX.INDEX++;
        if (SETTINGS_RX.INDEX == SETTINGS_RX.LENGTH) {
          break;
        }
      }
      if (SETTINGS_RX.INDEX == SETTINGS_RX.LENGTH) {
        SETTINGS_RX.RXFLAG = DoCAN_RX_COMPLETE;
        SETTINGS_RX.STATUS = DoCAN_Idle;
        return;
      }
      SETTINGS_RX.FRAMES--;
      SETTINGS_RX.COUNTER--;
      if ( SETTINGS_RX.COUNTER == 0 ) {
        FrameTX_FC (0);
        if ( CONFIG.ERRORCODE == DoCAN_ERR_WRONGCANID ) {
          SETTINGS_RX.STATUS = DoCAN_Idle;
          SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
      }
    }
    else {
      CONFIG.ERRORCODE = DoCAN_ERR_WRONGFRAMECOUNTER;
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        DoCAN_RX
 * @class       ISO_DoCAN (Private)
 * @brief       Process Receive DoCAN Messages
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::DoCAN_RX (void) {
  if ( SETTINGS_RX.RXFLAG != DoCAN_RX_WORKING ) {
    SETTINGS_RX.STATUS = DoCAN_Idle;
    return;
  }
  else {
    if ( SETTINGS_RX.STATUS == DoCAN_Idle ) {
      SETTINGS_RX.STATUS = DoCAN_Idle;
      return;
    }
    else {
      if ( MESSAGE.ID != CONFIG.CANID_RX ) {
        CONFIG.ERRORCODE = DoCAN_ERR_WRONGCANID;
        SETTINGS_RX.STATUS = DoCAN_Idle;
        return;
      }
      else {
        LogMessage('R');
        uint8_t PCI = 0;
        PCI = (MESSAGE.DATA[0] & 0xF0) >> 4;
        switch (PCI) {
          case 0x00 : { // Receiving SF
            FrameRX_SF();
            return;
          }
          case 0x01 : { // Receiving FF
            FrameRX_FF();
            return;
          }
          case 0x02 : { // Receiving CF
            FrameRX_CF();
            return;
          }
          case 0x03 : { // Receiving FC
            FrameRX_FC();
            return;
          }
          default : {
            CONFIG.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
            SETTINGS_RX.STATUS = DoCAN_Idle;
            SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
            return;
          }
        }
      }
    }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Receive
 * @class       ISO_DoCAN (Public)
 * @brief       Receive DoCAN Frame
 * @param [Mode]    Mode Where 0 is for Normal Messages and 1 is for Flow Control
 * @param [Mode]    Timeout Where 0 is for P2* and 1 is for P2
 * @return      DoCAN Receive Status (Enum)
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_DoCAN::Receive (uint8_t Mode, uint8_t Time) {
  SETTINGS_RX.TIME = MicroClock();
  CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOK;
  SETTINGS_RX.RXFLAG = DoCAN_RX_WORKING;
  SETTINGS_RX.STATUS = ((Mode) ? DoCAN_Wait : DoCAN_Receive);
  uint64_t TIMEOUT = (Time) ? CONFIG.P2 : CONFIG.P2_STAR;
  uint8_t fTIMEOUT = 0;

  do {
    if (PCAN.Read(PCAN.MESSAGE) == PCAN_ERROR_OK) {
      if (PCAN.MESSAGE.LEN == 8) {
        MESSAGE.TYPE = PCAN.MESSAGE.MSGTYPE;
        MESSAGE.LEN = PCAN.MESSAGE.LEN;
        MESSAGE.ID = PCAN.MESSAGE.ID;
        for (int I = 0; I < 8; I++) {
          MESSAGE.DATA[I] = PCAN.MESSAGE.DATA[I];
        }
        DoCAN_RX();
      }
    }
    fTIMEOUT = (((MicroClock() - SETTINGS_RX.TIME) <= TIMEOUT) ? 1 : 0);
  } while ((SETTINGS_RX.RXFLAG == DoCAN_RX_WORKING) && (fTIMEOUT));
  if (fTIMEOUT == 0) { 
    CONFIG.ERRORCODE = DoCAN_ERR_TIMEOUT;
    SETTINGS_RX.STATUS = DoCAN_Idle;
    SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
  }
  return SETTINGS_RX.RXFLAG;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameTX_FC
 * @class       ISO_DoCAN (Private)
 * @brief       Transmit Flow Control Frame
 * @param [CanID]     CAN ID
 * @param [FS]        Flow Status
 * @param [BS]        Block Size
 * @param [STMin]     ST Min Time
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameTX_FC (uint8_t FS) {
  if (FS == 0) {
    uint8_t BS = 0;
    if ( CONFIG.BLOCKS == 0 ) {
      SETTINGS_RX.COUNTER = SETTINGS_RX.FRAMES;
      BS = 0;
    }
    else {
      if ( SETTINGS_RX.FRAMES <= CONFIG.BLOCKS ) {
        SETTINGS_RX.COUNTER = SETTINGS_RX.FRAMES;
        BS = 0;
      }
      else {
        SETTINGS_RX.COUNTER = CONFIG.BLOCKS;
        BS = CONFIG.BLOCKS;
      }
    }

    MESSAGE.DATA[0] = 0x30 | (FS & 0x0F);
    MESSAGE.DATA[1] = BS;
    MESSAGE.DATA[2] = CONFIG.STMIN;
    MESSAGE.DATA[3] = CONFIG.PADDING;
    MESSAGE.DATA[4] = CONFIG.PADDING;
    MESSAGE.DATA[5] = CONFIG.PADDING;
    MESSAGE.DATA[6] = CONFIG.PADDING;
    MESSAGE.DATA[7] = CONFIG.PADDING;
    if (SendCAN()) {
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
    LogMessage('T');
  }
  else {
    MESSAGE.DATA[0] = 0x32;
    MESSAGE.DATA[1] = 0x00;
    MESSAGE.DATA[2] = 0x00;
    MESSAGE.DATA[3] = CONFIG.PADDING;
    MESSAGE.DATA[4] = CONFIG.PADDING;
    MESSAGE.DATA[5] = CONFIG.PADDING;
    MESSAGE.DATA[6] = CONFIG.PADDING;
    MESSAGE.DATA[7] = CONFIG.PADDING;
    if (SendCAN()) {
      SETTINGS_RX.STATUS = DoCAN_Idle;
      SETTINGS_RX.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
    LogMessage('T');
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameTX_SF
 * @class       ISO_DoCAN (Private)
 * @brief       Transmit Single Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameTX_SF (void) {
  if ((*CONFIG.LEN < 8) && (*CONFIG.LEN != 0)) {
    MESSAGE.DATA[0] = 0x0F & (*CONFIG.LEN);
    int I = 0; int J = 1;
    while (I < *CONFIG.LEN) {
      MESSAGE.DATA[J] = CONFIG.DATA[I];
      I++; J++;
    }
    while (J < 8) {
      MESSAGE.DATA[J] = CONFIG.PADDING;
      I++; J++;
    }
    if (SendCAN()) {
      SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
    }
    LogMessage('T');
    SETTINGS_TX.TXFLAG = DoCAN_TX_COMPLETE;
  } else {
    CONFIG.ERRORCODE = DoCAN_ERR_INTERNALISSUE;
    SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameTX_FF
 * @class       ISO_DoCAN (Private)
 * @brief       Transmit First Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameTX_FF (void) {
  MESSAGE.DATA[0] = ((*CONFIG.LEN >> 8) & 0x0F) | 0x10;
  MESSAGE.DATA[1] = (uint8_t)(*CONFIG.LEN & 0x00FF);
  SETTINGS_TX.INDEX = 0;
  int I = 2;
  while (I < 8) {
    MESSAGE.DATA[I] = CONFIG.DATA[SETTINGS_TX.INDEX];
    SETTINGS_TX.INDEX++;
    I++;
  }
  SETTINGS_TX.FRAMES = 0;
  SETTINGS_TX.BLOCKCOUNTER = 1;
  if (SendCAN()) {
    SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
  }
  LogMessage('T');
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        FrameTX_CF
 * @class       ISO_DoCAN (Private)
 * @brief       Transmit Consecutive Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::FrameTX_CF (void) {
  MESSAGE.DATA[0] = 0x20 | SETTINGS_TX.BLOCKCOUNTER;
  SETTINGS_TX.BLOCKCOUNTER = (SETTINGS_TX.BLOCKCOUNTER + 1) % 16;
  int I = 1;
  while (I < 8) {
    MESSAGE.DATA[I] = CONFIG.DATA[SETTINGS_TX.INDEX];
    SETTINGS_TX.INDEX++;
    I++;
    if (SETTINGS_TX.INDEX >= (*CONFIG.LEN)) {
      SETTINGS_TX.TXFLAG = DoCAN_TX_COMPLETE;
      break;
    }
  }
  while (I < 8) {
    MESSAGE.DATA[I] = CONFIG.PADDING;
    I++;
  }
  if (SendCAN()) {
    SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
  }
  LogMessage('T');
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Write
 * @class       ISO_DoCAN (Public)
 * @brief       Write DoCAN Frame
 * @param []    Nothing
 * @return      DoCAN Transmit Status (Enum)
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_DoCAN::Write (void) {
  SETTINGS_TX.TXFLAG = DoCAN_TX_WORKING;
  CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOK;

  if ((*CONFIG.LEN) < 8) {
    FrameTX_SF();
    return SETTINGS_TX.TXFLAG;
  }
  else {
    SETTINGS_TX.STMIN = 0;
    SETTINGS_TX.BLOCKS = 0;
    FrameTX_FF();
    if (CONFIG.ERRORCODE != DoCAN_ERR_FRAMEOK) {
      return SETTINGS_TX.TXFLAG;
    }
  }

  SETTINGS_TX.WAIT = 0;
  uint16_t WaitCount = 0;
  do {
    Receive(1);
    if (CONFIG.ERRORCODE != DoCAN_ERR_FRAMEOK) {
      SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
      return SETTINGS_TX.TXFLAG;
    }
    if (SETTINGS_TX.WAIT) {
      WaitCount++;
      if (WaitCount >= 600) {
        SETTINGS_TX.TXFLAG = DoCAN_TX_ERROR;
        CONFIG.ERRORCODE = DoCAN_ERR_WAITTIMEOUT;
        return SETTINGS_TX.TXFLAG;
      }
    }
    else {
      SETTINGS_TX.WAIT = 0;
      SETTINGS_TX.FRAMES = 0;
      uint16_t Length = 0;
      uint16_t Frames = 0;
      Length = (*CONFIG.LEN) - SETTINGS_TX.INDEX;
      if ((Length % 7) == 0) {
        Frames = (Length / 7);
      } else {
        Frames = (Length / 7) + 1;
      }
      if (CONFIG.BLOCKS == 0) {
        SETTINGS_TX.FRAMES = Frames;
      } else {
        if (CONFIG.BLOCKS >= Frames) {
          SETTINGS_TX.FRAMES = Frames;
        } else {
          SETTINGS_TX.FRAMES = CONFIG.BLOCKS;
        }
      }
      while (SETTINGS_TX.FRAMES) {
        MicroDelay(SETTINGS_TX.STMIN);
        FrameTX_CF();
        SETTINGS_TX.FRAMES--;
      }
    }
  } while (SETTINGS_TX.TXFLAG == DoCAN_TX_WORKING);
  return SETTINGS_TX.TXFLAG;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        LogMessage
 * @class       ISO_DoCAN (Private)
 * @brief       Show DoCAN Parameters JSON
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_DoCAN::SendCAN (void) {
  uint32_t CanID;
  if ( CONFIG.MODE == DoCAN_PHY ) {
    CanID = CONFIG.CANID_TX;
  }
  else if  ( CONFIG.MODE == DoCAN_FUN ) {
    CanID = CONFIG.CANID_FN;
  }
  else {
    CONFIG.ERRORCODE = DoCAN_ERR_WRONGCANID;
    return DoCAN_ERR_WRONGCANID;
  }
  MESSAGE.ID = CanID;
  if (PCAN.Write (MESSAGE.ID, MESSAGE.DATA)) {
    CONFIG.ERRORCODE = DoCAN_ERR_SENDCANISSUE;
    return DoCAN_ERR_SENDCANISSUE;
  }
  return DoCAN_ERR_FRAMEOK;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ShowFrameCAN
 * @class       ISO_DoCAN (Private)
 * @brief       Show CAN Frame on Console for Debug Purpose
 * @param [FRAME]     CAN Frame Structure
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::ShowFrameCAN (DoCANMESSAGE FRAME) {
  cout << "\nCAN FRAME:  ID: ";
  cout << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << FRAME.ID;
  cout << "  DATA: ";
  for (int I = 0; I < 8; I++) {
    cout << " ";
    cout  << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)FRAME.DATA[I];
  }
  cout << std::dec;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        LogMessage
 * @class       ISO_DoCAN (Private)
 * @brief       Saves Log of CAN Frame
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::LogMessage (char C) {
  if (CONFIG.LOGMODE == DoCAN_LOGON) {
    std::ostringstream TimeStamp;
      uint64_t HH_Cal = (MicroClock() / 3600000000) % 24;
      uint64_t MM_Cal = (MicroClock() / 60000000) % 60;
      uint64_t SS_Cal = (MicroClock() / 1000000) % 60;
      uint64_t MS = (MicroClock() / 1000) % 1000;
      uint64_t US = (MicroClock() % 1000);
      int HH = (TIME.HH + HH_Cal) % 24;
      int MM = (TIME.mm + MM_Cal) % 60;
      int SS = (TIME.SS + SS_Cal) % 60;
    TimeStamp << "[ "
      << std::setw(4) << std::setfill('0') << TIME.YY << "."
      << std::setw(2) << std::setfill('0') << TIME.MM << "."
      << std::setw(2) << std::setfill('0') << TIME.DD << " ";
      int displayHour = (HH == 0) ? 12 : (HH > 12 ? HH - 12 : HH);
      const char* ampm = (HH < 12 || HH == 0) ? "AM" : "PM";
    TimeStamp << std::setw(2) << std::setfill('0') << displayHour << ":"
      << std::setw(2) << std::setfill('0') << MM << ":"
      << std::setw(2) << std::setfill('0') << SS << ":"
      << std::setw(3) << std::setfill('0') << MS << ":"
      << std::setw(3) << std::setfill('0') << US << ":"
      << ampm << " ]";

    std::ostringstream Message;
    Message << TimeStamp.str() << "  ";
    Message << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << MESSAGE.ID << "  ";
    if ((C == 'T') || (C == 't')) {
      Message << "Tx ";
    } else {
      Message << "Rx ";
    }
    for (int I = 0; I < 8; I++) {
      Message << " ";
      Message << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)MESSAGE.DATA[I];
    }
    if (LogFile.is_open()) {
      LogFile << Message.str() << std::endl;
    }
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        ShowParameters
 * @class       ISO_DoCAN (Public)
 * @brief       Show DoCAN Parameters JSON
 * @param []    Nothing
 * @return      JSON String
 */
/* ---------------------------------------------------------------------------------------------------- */
string ISO_DoCAN::ShowParameters (void) {
  string Message = "\n{\n  \"MESSAGE\" : \"DoCAN PARAMETERS ISO 14229\",";
  Message = Message + "\n  \"P2* TIME\" : \"" + to_string(CONFIG.P2_STAR) + " uS\",";
  Message = Message + "\n  \"P2 TIME\" : \"" + to_string(CONFIG.P2) + " uS\",";
  Message = Message + "\n  \"BUFFER SIZE\" : \"" + to_string(CONFIG.LENGTH) + " BYTES\",";
  ostringstream BS, ST, PAD;
  BS << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)CONFIG.BLOCKS;
  ST << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)CONFIG.STMIN;
  PAD << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << (int)CONFIG.PADDING;
  Message = Message + "\n  \"BLOCK SIZE\" : \"" + BS.str() + "\",";
  Message = Message + "\n  \"ST MIN\" : \"" + ST.str() + "\",";
  Message = Message + "\n  \"PADDING\" : \"" + PAD.str() + "\",";
  Message = Message + "\n  \"CAN DEVICE\" : \"" + "Peak PCAN\",";
  ostringstream CANRX, CANTX, CANFN;
  CANTX << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << CONFIG.CANID_TX;
  CANRX << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << CONFIG.CANID_RX;
  CANFN << std::uppercase << std::hex << std::setw(3) << std::setfill('0') << CONFIG.CANID_FN;
  Message = Message + "\n  \"CAN IDs\" : {";
  Message = Message + "\n    \"TX\" : \"" + CANTX.str() + "\",";
  Message = Message + "\n    \"RX\" : \"" + CANRX.str() + "\",";
  Message = Message + "\n    \"FUN\" : \"" + CANFN.str() + "\"";
  Message = Message + "\n  }\n}";
  return Message;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetCANID
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [ID_TX]     DoCAN CAN ID For TX
 * @param [ID_RX]     DoCAN CAN ID For RX
 * @param [ID_FN]     DoCAN CAN ID For FUN
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetCANID (uint32_t ID_TX, uint32_t ID_RX, uint32_t ID_FN) {
  CONFIG.CANID_TX = ID_TX;
  CONFIG.CANID_RX = ID_RX;
  CONFIG.CANID_FN = ID_FN;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetBuffer
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [ID]        UDS Data CAN ID Variable Address
 * @param [Len]       UDS Data Length Variable Address
 * @param [Data]      UDS Data Buffer Start Address
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetBuffer (uint32_t * ID, uint16_t * Len, uint8_t * Data) {
  CONFIG.DATA = Data;
  CONFIG.LEN = Len;
  CONFIG.ID = ID;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetTiming
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [P2]          DoCAN P2 Time
 * @param [P2_Star]     DoCAN P2* Time
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetTiming (uint64_t P2, uint64_t P2_Star) {
  CONFIG.P2_STAR = P2_Star;
  CONFIG.P2 = P2;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetUDSParameter
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param [Padding]     DoCAN CAN Padding
 * @param [STMin]       DoCAN ST Min in Microseconds For Flow Control TX
 * @param [Block]       DoCAN Block Size For Flow Control TX
 * @param [Length]      UDS Buffer Length
 * @param [Speed]       CAN Speed In KBPS
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetUDSParameter (uint8_t Padding, uint32_t STMin, uint8_t Block, uint16_t Length, uint16_t Speed) {
  STMin = STMin / 100;
  if (STMin == 0) {
    CONFIG.STMIN = 0x00;
  }
  else if (STMin < 10) {
    CONFIG.STMIN = 0xF0 | (0x0F & STMin);
  }
  else if (STMin <= 1270) {
    STMin = STMin / 10;
    CONFIG.STMIN = STMin & 0x7F;
  } else {
    CONFIG.STMIN = 0x00;
  }
  CONFIG.SPEED = Speed;
  CONFIG.PADDING = Padding;
  CONFIG.BLOCKS = Block;
  CONFIG.LENGTH = Length;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        SetAddressingMode
 * @class       ISO_DoCAN (Public)
 * @brief       Set DoCAN Addressing Mode
 * @param [Mode]    Addressing Mode 0 Means Physical and !0 Means Functional
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::SetAddressingMode (DoCANAddress Mode) {
  if (Mode) {
    CONFIG.MODE = DoCAN_FUN;
  } else {
    CONFIG.MODE = DoCAN_PHY;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        LoggingMode
 * @class       ISO_DoCAN (Public)
 * @brief       Set DoCAN Logging Mode
 * @param [Mode]    1 Means Logging ON else Logging OFF (Enum)
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::LoggingMode (DoCANLogging Mode, std::string Path) {
  if (Mode == DoCAN_LOGON) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&now_c);
    std::ostringstream FileName;
    FileName << Path << std::put_time(localTime, "%Y%m%d-%H%M") << "-DoCAN.log";

    StartTime = Clock::now();
    auto Now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(Now);
    std::tm* local_tm = std::localtime(&now_time_t);
      TIME.YY = local_tm->tm_year + 1900;
      TIME.MM = local_tm->tm_mon + 1;
      TIME.DD = local_tm->tm_mday;
      TIME.HH = local_tm->tm_hour;
      TIME.mm = local_tm->tm_min;
      TIME.SS = local_tm->tm_sec;
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
      CONFIG.ERRORCODE = DoCAN_ERR_INTERNALISSUE;
      CONFIG.LOGMODE = DoCAN_LOGOFF;
      return;
    } else {
      LogFile << "ISO 14429 LOGGING STARTED\nDiagnostics on CAN (DoCAN)\nUDSonCAN" << ShowParameters() << "\nTIME : " << TimeStamp.str()
        << "\nSTART" << std::endl;
    }
    CONFIG.LOGMODE = DoCAN_LOGON;
  } else {
    if (LogFile.is_open()) {
      LogFile << "STOPPED\nDoCAN SAFELY CLOSING . . .\nISO DoCAN LOGGING CLOSED" << std::endl;
      LogFile.close();
    }
    CONFIG.LOGMODE = DoCAN_LOGOFF;
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        GetErrorCode
 * @class       ISO_DoCAN (Public)
 * @brief       Get DoCAN Error Code
 * @param []    Nothing
 * @return      Error Code
 */
/* ---------------------------------------------------------------------------------------------------- */
uint8_t ISO_DoCAN::GetErrorCode (void) {
  return CONFIG.ERRORCODE;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Start
 * @class       ISO_DoCAN (Public)
 * @brief       Start DoCAN and CAN Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Start (void) {
  CONFIG.ERRORCODE = DoCAN_ERR_FRAMEOK;
  SETTINGS_RX.STATUS = DoCAN_Idle;
  CONFIG.MODE = DoCAN_PHY;
    SETTINGS_RX.RXFLAG = 1;
    SETTINGS_TX.TXFLAG = 1;
    SETTINGS_RX.INDEX = 0;
    SETTINGS_RX.BLOCKCOUNTER = 0;
    SETTINGS_RX.COUNTER = 0;
    SETTINGS_RX.FRAMES = 0;
  CONFIG.INIT = 0;

  TPCANStatus Status;
  Status = PCAN.Start(CONFIG.SPEED);
  if (PCAN_ERROR_OK != Status) {
    CONFIG.ERRORCODE = DoCAN_ERR_DRIVERFAILER;
    return;
  }
  Status = PCAN.Filter(CONFIG.CANID_RX);
  if (PCAN_ERROR_OK != Status) {
    CONFIG.ERRORCODE = DoCAN_ERR_DRIVERFAILER;
    return;
  }
  if (CONFIG.DATA == nullptr || CONFIG.LEN == nullptr || CONFIG.ID == nullptr) {
    CONFIG.ERRORCODE = DoCAN_ERR_EXTERNALCONFIGATION;
    return;
  }

  CONFIG.INIT = 1;
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Stop
 * @class       ISO_DoCAN (Public)
 * @brief       Stop DoCAN and CAN Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Stop (void) {
  if (LogFile.is_open()) {
    PCAN.Uninitialize();
    LogFile << "STOPPED\nDoCAN SAFELY CLOSING . . .\nISO DoCAN LOGGING CLOSED" << std::endl;
    LogFile.close();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Kill
 * @class       ISO_DoCAN (Public)
 * @brief       Kill DoCAN and CAN Drivers
 * @param []    Nothing
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Kill (void) {
  if (LogFile.is_open()) {
    PCAN.Uninitialize();
    LogFile << "STOPPED\nPROCESS KILLED\nISO DoCAN LOGGING CLOSED" << std::endl;
    LogFile.close();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Clock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time accoridng to Mode
 * @param [Mode]    Mode of Clock Resolution
 * @return      Time
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::Clock (uint8_t Mode) {
  if (Mode) {
    return MilliClock();
  } else {
    return MicroClock();
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MicroClock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time in MicroSeconds
 * @param []    Nothing
 * @return      Time in MicroSeconds
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::MicroClock (void) {
  return std::chrono::duration_cast<std::chrono::microseconds>(Clock::now() - StartTime).count();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MilliClock
 * @class       ISO_DoCAN (Public)
 * @brief       Get Central Clock Time in MilliSecond
 * @param []    Nothing
 * @return      Time in MilliSecond
 */
/* ---------------------------------------------------------------------------------------------------- */
uint64_t ISO_DoCAN::MilliClock (void) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - StartTime).count();
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        Delay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time accoridng to Mode
 * @param [Time]    Time of Delay
 * @param [Mode]    Mode of Clock Resolution
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::Delay (uint64_t Time, uint8_t Mode) {
  if (Mode) {
    MilliDelay (Time);
  } else {
    MicroDelay (Time);
  }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MicroDelay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time in MicroSeconds
 * @param [Time]    Time of Delay in MicroSeconds
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::MicroDelay (uint64_t Time) {
  uint64_t Entry = MicroClock();
  uint64_t Exit = MicroClock();
  while ((Exit - Entry) < Time) { Exit = MicroClock(); }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @name        MilliDelay
 * @class       ISO_DoCAN (Public)
 * @brief       Delay for Some Time in MilliSecond
 * @param [Time]    Time of Delay in MilliSecond
 * @return      Nothing
 */
/* ---------------------------------------------------------------------------------------------------- */
void ISO_DoCAN::MilliDelay (uint64_t Time) {
  uint64_t Entry = MilliClock();
  uint64_t Exit = MilliClock();
  while ((Exit - Entry) < Time) { Exit = MilliClock(); }
}
/* ==================================================================================================== */

/* ==================================================================================================== */
/**
 * @end       End of File DoCAN.h
 */
/* ---------------------------------------------------------------------------------------------------- */
#endif  // _DoCAN
/* ==================================================================================================== */