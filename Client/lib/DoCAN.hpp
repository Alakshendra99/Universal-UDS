/* ==================================================================================================== */
/*
 *  DoCAN.h
 *  Diagonostics on CAN (DoCAN)
 *    ISO: 14229 Part 3   - UDSonCAN (DoCAN)
 *  Version: v1.0:0
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

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <cstdio>
#include <iomanip>
#include "DriverPCAN.hpp"
#include "UDS.hpp"
#include <chrono>

using Clock = std::chrono::steady_clock;

#define DoCAN_ERR_FRAMEOK                 0
#define DoCAN_ERR_FRAMEISSUE              1
#define DoCAN_ERR_FRAMEOVERFLOW           2
#define DoCAN_ERR_WRONGCANID              3
#define DoCAN_ERR_WRONGFRAMECOUNTER       4

#define DoCAN_RX_WORKING                  0
#define DoCAN_RX_IDLE                     1
#define DoCAN_RX_COMPLETE                 2
#define DoCAN_RX_ERROR                    3



/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_DoCAN {
  protected : 
    
    std::chrono::_V2::steady_clock::time_point StartTime;
    DriverPCAN PCAN;
    
    struct DoCANMESSAGE {
      uint32_t ID;
      uint8_t LEN;
      uint8_t TYPE;
      uint8_t DATA[8];
    };
    DoCANMESSAGE MESSAGE;

    enum DoCANStatus {
      DoCAN_Idle = 0,
      DoCAN_Receive = 1,
      DoCAN_Transmit = 2,
      DoCAN_Wait = 3,

      DoCAN_PHY = 0,
      DoCAN_FUN = 1,
    };
  
  private :
    

    struct DoCANCONFIGURATION {
      uint32_t CANID_TX;
      uint32_t CANID_RX;
      uint32_t CANID_FN;
      uint8_t PADDING;
      uint8_t STMIN;
      uint8_t BLOCKS;
      uint8_t * DATA;
      uint16_t * LEN;
      uint32_t * ID;
      uint8_t LENGTH;
      uint8_t INIT;
    };
    DoCANCONFIGURATION CONFIG;

    struct DoCANSETTING {
      DoCANStatus STATUS;
      uint8_t ERRORCODE;
      uint64_t TIME;
      uint64_t STMIN;
      uint8_t BLOCKS;
      uint8_t MODE;
      uint16_t LENGTH;
      uint16_t FRAMES;
      uint16_t COUNTER;
      uint8_t BLOCKCOUNTER;
      uint16_t INDEX;
      uint8_t RXFLAG;
      uint8_t TXFLAG;
    };
    DoCANSETTING SETTINGS;

    void FrameTX_FC (uint8_t FS);
    
    void FrameRX_FC (void);
    void FrameRX_SF (void);
    void FrameRX_FF (void);
    void FrameRX_CF (void);
    
  public :
    ISO_DoCAN (void) {
      StartTime = Clock::now();
      cout << "\nDoCAN Driver Loaded";
      CONFIG.PADDING = 0x00;
      CONFIG.STMIN = 0x00;
      CONFIG.BLOCKS = 0x00;
      CONFIG.LENGTH = 4096;
      CONFIG.INIT = 0x00;
      SETTINGS.STATUS = DoCAN_Idle;
      SETTINGS.RXFLAG = 1;
      SETTINGS.TXFLAG = 1;
      SETTINGS.INDEX = 0;
      SETTINGS.ERRORCODE = 0;
      SETTINGS.MODE = 0;
      SETTINGS.BLOCKCOUNTER = 0;
      SETTINGS.COUNTER = 0;
      SETTINGS.FRAMES = 0;
    }
    ~ISO_DoCAN (void) {
      cout << "\nDoCAN Driver Unloaded";
    }

    uint64_t Clock (uint8_t Mode = 0);
    uint64_t MicroClock (void);
    uint64_t MilliClock (void);
    void Delay (uint64_t Time, uint8_t Mode = 0);
    void MilliDelay (uint64_t Time);
    void MicroDelay (uint64_t Time);

    void SetMode_RX (void);
    uint8_t GetMode_RX (void);
    void SetMode_TX (void);
    uint8_t GetMode_TX (void);

    void DoCAN_RX (void);
    void DoCAN_TX (void);

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
  if ((SETTINGS.STATUS == DoCAN_Wait)) {
    uint8_t FS = MESSAGE.DATA[0] & 0x0F;
    switch (FS) {
        case 0 : { // FS : Continue To Send (CTS)
          SETTINGS.STATUS = DoCAN_Transmit;
          break;
        }
        case 1 : { // FS = Wait (WT)
          SETTINGS.STATUS = DoCAN_Wait;
          SETTINGS.TIME = MicroClock();
          return;
        }
        case 2 : { // FS = Overflow Flag (OF)
          SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEOVERFLOW;
          SETTINGS.STATUS = DoCAN_Idle;
          return;
        }
        default : {
          SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
          SETTINGS.STATUS = DoCAN_Idle;
          return;
        }
    }
    SETTINGS.BLOCK = MESSAGE.DATA[1];
    if ( MESSAGE.DATA[2] <= 0x7F ) {
      SETTINGS.STMIN = MESSAGE.DATA[2] * 1000;
      SETTINGS.TIME = MicroClock();
    } else if (( MESSAGE.DATA[2] <= 0xF9 ) && ( MESSAGE.DATA[2] >= 0xF1 )) {
      SETTINGS.STMIN = (MESSAGE.DATA[2] & 0x0F) * 100;
      SETTINGS.TIME = MicroClock();
    } else {
      SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS.STATUS = DoCAN_Idle;
    } return;
  } 
  else {
    SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS.STATUS = DoCAN_Idle;
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
  if (SETTINGS.STATUS == DoCAN_Receive) {
    uint8_t LEN = MESSAGE.DATA[0] & 0x0F;
    if ( LEN <= 7 ) {
      CONFIG.LEN = LEN;
      CONFIG.ID = MESSAGE.ID;
      for (uint8_t I = 0; I < LEN; I++) {
        CONFIG.DATA[I] = MESSAGE.DATA[ I + 1 ];
      }
      SETTINGS.TIME = MicroClock();
      SETTINGS.RXFLAG = DoCAN_RX_COMPLETE;
      SETTINGS.STATUS = DoCAN_Idle;
    }
    else {
      SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS.STATUS = DoCAN_Idle;
      SETTINGS.RXFLAG = DoCAN_RX_ERROR;
      return;
    } 
  }
  else {
    SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS.STATUS = DoCAN_Idle;
    SETTINGS.RXFLAG = DoCAN_RX_ERROR;
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
  if (SETTINGS.STATUS == DoCAN_Receive) {
    uint16_t LEN = 0;
    LEN = (uint16_t)( ((MESSAGE.DATA[0] & 0x0F) << 8) | MESSAGE.DATA[1] );
    if ( (LEN > 7) && (LEN <= CONFIG.LENGTH) ) {
      CONFIG.LEN = LEN;
      CONFIG.ID = MESSAGE.ID;
      SETTINGS.LENGTH = LEN;
      LEN = LEN - 6;
      if ( (LEN % 7) == 0 ) {
        SETTINGS.FRAMES = (LEN / 7);
      } else {
        SETTINGS.FRAMES = (LEN / 7) + 1;
      }
      
      SETTINGS.BLOCKCOUNTER = 0;
      SETTINGS.INDEX = 6;
      FrameTX_FC (0);
      if ( SETTINGS.ERRORCODE = DoCAN_ERR_WRONGCANID ) {
        SETTINGS.STATUS = DoCAN_Idle;
        SETTINGS.RXFLAG = DoCAN_RX_ERROR;
        return;
      }
      SETTINGS.TIME = MicroClock();
    }
    else {
      FrameTX_FC (2);
      SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
      SETTINGS.STATUS = DoCAN_Idle;
      SETTINGS.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS.STATUS = DoCAN_Idle;
    SETTINGS.RXFLAG = DoCAN_RX_ERROR;
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
  if (SETTINGS.STATUS == DoCAN_Receive) {
    if ( (MESSAGE.DATA[0] & 0x0F) == SETTINGS.BLOCKCOUNTER ) {
      SETTINGS.BLOCKCOUNTER = (SETTINGS.BLOCKCOUNTER + 1) % 16;
      for (uint8_t I = 1; I < 8; I++) {
        CONFIG.DATA[SETTINGS.INDEX] = MESSAGE.DATA[I];
        SETTINGS.INDEX++;
        if (SETTINGS.INDEX == SETTINGS.LENGTH) {
          break;
        }
      }
      if (SETTINGS.INDEX == SETTINGS.LENGTH) {
        SETTINGS.RXFLAG = DoCAN_RX_COMPLETE;
        SETTINGS.STATUS = DoCAN_Idle;
      }
      SETTINGS.FRAMES--;
      SETTINGS.COUNTER--;
      if ( SETTINGS.COUNTER == 0 ) {
        FrameTX_FC (0);
        if ( SETTINGS.ERRORCODE = DoCAN_ERR_WRONGCANID ) {
          SETTINGS.STATUS = DoCAN_Idle;
          SETTINGS.RXFLAG = DoCAN_RX_ERROR;
          return;
        }
      } 
    }
    else {
      SETTINGS.ERRORCODE = DoCAN_ERR_WRONGFRAMECOUNTER;
      SETTINGS.STATUS = DoCAN_Idle;
      SETTINGS.RXFLAG = DoCAN_RX_ERROR;
      return;
    }
  }
  else {
    SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
    SETTINGS.STATUS = DoCAN_Idle;
    SETTINGS.RXFLAG = DoCAN_RX_ERROR;
    return;
  }
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
  uint32_t CanID;
  if ( SETTINGS.MODE == DoCAN_PHY ) {
    CanID = CONFIG.CANID_TX;
  } 
  else if  ( SETTINGS.MODE == DoCAN_FUN ) {
    CanID = CONFIG.CANID_FN;
  } 
  else {
    SETTINGS.ERRORCODE = DoCAN_ERR_WRONGCANID;
    SETTINGS.STATUS = DoCAN_Idle;
    return;
  }
  if (FS == 0) {
    uint8_t BS = 0;
    if ( CONFIG.BLOCKS == 0 ) {
      SETTINGS.COUNTER = SETTINGS.FRAMES;
      BS = 0;
    } 
    else {
      if ( SETTINGS.FRAMES <= CONFIG.BLOCKS ) {
        SETTINGS.COUNTER = SETTINGS.FRAMES;
        BS = 0;
      } 
      else {
        SETTINGS.COUNTER = CONFIG.BLOCKS;
        BS = CONFIG.BLOCKS;
      }
    }
    
    uint8_t Data[8];
    Data[0] = 0x30 | (FS & 0x0F);
    Data[1] = BS;
    Data[2] = CONFIG.STMIN;
    Data[3] = CONFIG.PADDING;
    Data[4] = CONFIG.PADDING;
    Data[5] = CONFIG.PADDING;
    Data[6] = CONFIG.PADDING;
    Data[7] = CONFIG.PADDING;
    PCAN.Write (CanID, Data);
  } 
  else {
    uint8_t Data[8];
    Data[0] = 0x32;
    Data[1] = 0x00;
    Data[2] = 0x00;
    Data[3] = CONFIG.PADDING;
    Data[4] = CONFIG.PADDING;
    Data[5] = CONFIG.PADDING;
    Data[6] = CONFIG.PADDING;
    Data[7] = CONFIG.PADDING;
    PCAN.Write (CanID, Data);
  }
}
/* ==================================================================================================== */









void ISO_DoCAN::DoCAN_RX (void) {
  if ( SETTINGS.RXFLAG != DoCAN_RX_WORKING ) {
    SETTINGS.STATUS = DoCAN_Idle;
    return;
  } 
  else {
    if ( SETTINGS.STATUS == DoCAN_Idle ) {
      SETTINGS.STATUS = DoCAN_Idle;
      return;
    } 
    else {
      if ( MESSAGE.ID != CONFIG.CANID_RX ) {
        SETTINGS.ERRORCODE = DoCAN_ERR_WRONGCANID;
        SETTINGS.STATUS = DoCAN_Idle;
        return;
      } 
      else {
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
            SETTINGS.ERRORCODE = DoCAN_ERR_FRAMEISSUE;
            SETTINGS.STATUS = DoCAN_Idle;
            SETTINGS.RXFLAG = DoCAN_RX_ERROR;
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