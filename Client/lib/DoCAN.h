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

#include "DriverPCAN.h"
#include "UDS.h"
#include <chrono>

using Clock = std::chrono::steady_clock;

#define DoCAN_ERR_FRAMEOK                 0
#define DoCAN_ERR_FRAMEISSUE              1
#define DoCAN_ERR_FRAMEOVERFLOW           2


/* ==================================================================================================== */
/**
 * @class       DriverPCAN
 * @brief       PCAN Driver For CAN According To ISO 11898-1
 */
/* ---------------------------------------------------------------------------------------------------- */
class ISO_DoCAN {
  private :
    std::chrono::_V2::steady_clock::time_point StartTime;
    DriverPCAN PCAN;
    uint8_t Error;
    

  
  protected : 
    struct {
      uint32_t ID;
      uint8_t LEN;
      uint8_t TYPE;
      uint8_t DATA[8];
    } MESSAGE;
    enum {
      DoCAN_Idle = 0,
      DoCAN_Receive = 1,
      DoCAN_Transmit = 2,
      DoCAN_Wait = 3,
    } Status;

  private :
    void FrameRX_FC (void);
    void FrameRX_SF (void);
    void FrameRX_FF (void);
    void FrameRX_CF (void);
    
  public :
    ISO_DoCAN (void) {
      StartTime = Clock::now();
      cout << "\nDoCAN Driver Loaded";
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


};
/* ==================================================================================================== */




void ISO_DoCAN::FrameRX_FC (void) {
  if ((Status == DoCAN_Wait)) {
    uint8_t FS = 0;
    FS = MESSAGE.DATA[0] & 0x0F;
    switch (FS) {
      case 0 : { // FS : Continue To Send (CTS)
        Status = DoCAN_Transmit;
      }
      case 1 : { // FS = Wait (WT)
        Status = DoCAN_Wait;
        return ;
      }
      case 2 : { // FS = Overflow Flag (OF)
        Error = DoCAN_ERR_FRAMEOVERFLOW;
        Status = DoCAN_Idle;
        return ;
      }
      default : {
        Error = DoCAN_ERR_FRAMEISSUE;
        Status = DoCAN_Idle;
        return;
      }
    }
 
  }
}


// void TP_RxFrameFC (void) {
//   if ((UDS_Server.Status == UDS_ServerTransmitting) ||                                // Checking if UDS Server is Transmitting Mode and Waiting Mode
//      (UDS_Server.Status == UDS_ServerWaiting) || (UDS_Server.Status == UDS_ServerExtension)) {
//     uint8_t Status = 0;
//     Status = TP_MessageRX.Data[0] & 0x0F;                                             // Extracting Flow Status (FS)
//     switch (Status) {
//       case TP_FSContinueToSend : {                                                    // FS = Continue To Send (CTS)
//         TP_TxControl.FlowStatus = TP_FSContinueToSend;                                // TP Transmit Status To CTS
//         TP_TxControl.BlocksAllowed = TP_MessageRX.Data[1];                            // Extracting Block Size (BS) and Loading in TP
//         if (TP_MessageRX.Data[2] <= 0x7F) {
//           TP_TxControl.SeparationTimeout = TP_MessageRX.Data[2];                      // Loading Separation Time Minimum
//         } else {
//           TP_TxControl.SeparationTimeout = 0;                                         // Loading Separation Time As 0
//         }
//         uint32_t _Time = (TP_MessageRX.Data[2] & 0x7F);                               // Extracting Separation Time Minimum (STMin)
//         TP_TxControl.SeparationTimeout = _Time;                                       // Loading Separation Time Minimum
//         TP_TxControl.ReceivedFC = 1;                                                  // TP Received Flow Control Flag is Set
//         break;
//       }
//       case TP_FSWait : {                                                              // FS = Wait (WT)
//         TP_TxControl.FlowStatus = TP_FSWait;                                          // TP Transmit Status To Wait
//         UDS_Server.Status = UDS_ServerWaiting;                                        // UDS Server Status is Set To Waiting
//         TP_TxControl.ReceivedFC = 1;                                                  // TP Received Flow Control Flag is Set
//         break;
//       }
//       case TP_FSOverflow : {                                                          // FS = Overflow Flag (OF)
//         // Overflow Functions
//         TP_TxControl.FlowStatus = TP_FSOverflow;                                      // TP Transmit Status To Overflow Flag
//         UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status is Set To Free
//         TP_TxControl.ReceivedFC = 1;                                                  // TP Received Flow Control Flag is Set
//         break;
//       }
//       default : {
//         TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1]);                 // NRC : Incorrect Format
//         UDS_Server.Status = UDS_ServerFree;                                           // UDS Server Status is Set To Free
//         break;
//       }
//     }
//   }
// }
// /* ==================================================================================================== */

// /* ==================================================================================================== */
// /**
//  * @name        TP_RxFrameSF
//  * @brief       Receive Single Frame
//  * @param []    Nothing
//  * @return      Nothing
//  * @warning     NOT TO USE DIRECTLY
//  */
// /* ---------------------------------------------------------------------------------------------------- */
// void TP_RxFrameSF (void) {
//   if (UDS_Server.Status == UDS_ServerFree) {                                          // Checking if UDS Server is Free
//     if ((TP_MessageRX.Data[0] < 8) && (TP_MessageRX.Data[0] != 0)) {                  // Checking for Length
//       UDS_Message.CANID = TP_MessageRX.CANID.Raw;                                     // UDS CAN ID Loaded
//       UDS_Message.Length = TP_MessageRX.Data[0];                                      // Frame Length Loaded
//       uint8_t i = 0;
//       while (i < UDS_Message.Length) {                                                // Checking Length
//         UDS_Message.Data[i] = TP_MessageRX.Data[i + 1];                               // Loading Data To UDS
//         i++;
//       }
//       TP_RxControl.Time = TP_Clock();                                                 // TP Rx Time Reset
//       UDS_Server.Status = UDS_ServerBusy;                                             // UDS Server Status Set Busy
//     } else {
//       // NRC : Incorrect Format
//       TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1]);
//       UDS_Server.Status = UDS_ServerFree;
//     }
//   } else {
//     // NRC : Busy, Repeat Request
//     TP_SendNegativeResponse(UDS_NRC_BRR, TP_MessageRX.Data[1]);
//   }
// }
// /* ==================================================================================================== */

// /* ==================================================================================================== */
// /**
//  * @name        TP_RxFrameFF
//  * @brief       Receive First Frame
//  * @param []    Nothing
//  * @return      Nothing
//  * @warning     NOT TO USE DIRECTLY
//  */
// /* ---------------------------------------------------------------------------------------------------- */
// void TP_RxFrameFF (void) {
//   if (UDS_Server.Status == UDS_ServerFree) {                                          // Checking if UDS Server is Free
//     uint16_t Length = 0;
//     Length = (uint16_t)(TP_MessageRX.Data[1] | (uint16_t)((TP_MessageRX.Data[0]       // Extracting Length
//         & 0x0F) << 8));
//     if ((Length > 7) && (Length <= TP_RxBufferSize)) {                                // Checking for Length
//       UDS_Message.CANID = TP_MessageRX.CANID.Raw;                                     // UDS CAN ID Loaded
//       UDS_Message.Length = Length;                                                    // UDS Frame Length Loaded
//       TP_RxControl.TotalLength = Length;                                              // TP Receive Total Payload Loaded
//       uint16_t Frames = 0;
//       Frames = Length - 6;
//       if ((Frames % 7) == 0) {
//         Frames = (Frames / 7) + 1;
//       } else {
//         Frames = (Frames / 7) + 2;
//       }
//       TP_RxControl.TotalFrames = Frames;                                              // TP Receive Total Number of Frames
//       TP_RxControl.FrameCounter = 1;                                                  // TP Receive Frame Counter Set To 1
//       TP_RxControl.FrameIndex = 1;                                                    // TP Receive Frame Index is Zero + 1
//       TP_Status.WaitCount = 0;                                                        // TP Receive Status Wait Count Reset
//       TP_RxControl.DataCounter = 0;                                                   // TP Receive Data Byte Counter Reseted
//       while (i < 8) {
//         UDS_Message.Data[i - 2] = TP_MessageRX.Data[i];                               // TP Received Frames Initially Loaded in UDS
//         TP_RxControl.DataCounter++;                                                   // TP Received Data Bytes Counter Incremented
//         i++;
//       }
//       TP_RxControl.Time = TP_Clock();                                                 // TP Rx Time Reset
//       TP_SendFlowControl(TP_MessageRX.CANID.Raw);                                     // Send Flow Control
//       UDS_Server.Status = UDS_ServerReceiving;                                        // UDS Server in Receiving Mode
//     } else {
//       // NRC : Incorrect Format
//       TP_SendNegativeResponse(UDS_NRC_IMLIF, TP_MessageRX.Data[1]);
//       UDS_Server.Status = UDS_ServerFree;
//     }
//   } else {
//     // NRC : Busy, Repeat Request
//     TP_SendNegativeResponse(UDS_NRC_BRR, TP_MessageRX.Data[1]);
//   }
// }
// /* ==================================================================================================== */

// /* ==================================================================================================== */
// /**
//  * @name        TP_RxFrameCF
//  * @brief       Receive Consecutive Frame
//  * @param []    Nothing
//  * @return      Nothing
//  * @warning     NOT TO USE DIRECTLY
//  */
// /* ---------------------------------------------------------------------------------------------------- */
// void TP_RxFrameCF (void) {
//   if (UDS_Server.Status == UDS_ServerReceiving) {                                     // Checking if UDS Server is Receiving Mode
//     uint8_t FrameIndex = 0x0F & TP_MessageRX.Data[0];                                 // Frame Index Extraction
//     if (TP_RxControl.FrameIndex == FrameIndex) {                                      // Frame Index Checker
//       TP_RxControl.FrameIndex = (TP_RxControl.FrameIndex + 1) % 16;                   // TP Receive Frame Index Incremented with Overflow Check
//       TP_RxControl.FrameCounter++;                                                    // TP Frame Counter Incremented
//       uint8_t i = 1;
//       while (i < 8) {
//         if (TP_RxControl.DataCounter >= TP_RxControl.TotalLength) {                   // TP Data Counter Check for Memory Check
//           break;                                                                      // TP Rare Case Handling
//         }
//         UDS_Message.Data[TP_RxControl.DataCounter] = TP_MessageRX.Data[i];            // TP To UDS Data Loading
//         TP_RxControl.DataCounter++;                                                   // TP Received Data Bytes Counter Incremented
//         i++;
//       }
//       if (TP_RxControl.FrameCounter >= TP_RxControl.TotalFrames) {                    // TP Receiver Check is All Frames Received
//       	if (TP_RxControl.DataCounter >= TP_RxControl.TotalLength) {
//       	  UDS_Server.Status = UDS_ServerBusy;                                         // UDS Server Status is Set To Busy
//       	} else {
//       	  UDS_Server.Status = UDS_ServerFree;                                         // UDS Server Status is Set Free
//       	}
//       }
//       TP_RxControl.Time = TP_Clock();                                                 // TP Rx Time Reset
//     } else {
//         // NRC : Wrong Block Sequence Counter
//         TP_SendNegativeResponse(UDS_NRC_WBSC, TP_MessageRX.Data[1]);
//       }
//   }
// }
// /* ==================================================================================================== */

// /* ==================================================================================================== */
// /**
//  * @name        TP_RxDoCAN
//  * @brief       Receive CAN Frame Handler (Use in Main Loop)
//  * @param []    Nothing
//  * @return      Nothing
//  */
// /* ---------------------------------------------------------------------------------------------------- */
// void TP_RxDoCAN (void) {
//   if (TP_Status.RxFlag == 0x00) {                                                     // Checking for RX Flag
//     return;                                                                           // Nothing Received
//   } else if (TP_Status.RxFlag == 0x01) {                                              // If CAN Message is Received
//     TP_Status.RxFlag = 0;                                                             // RX Flag Reset
//     if (TP_CheckCANID(TP_MessageRX.CANID.Raw)) {                                      // Check CANID To Filter Junk Message
//       return;                                                                         // Ignoring Junk Messages
//     }
//     Number8Bit PCI;
//     PCI.Raw = TP_MessageRX.Data[0];                                                   // Reading PCI

//     switch (PCI.Nibble.N1) {                                                          // Checking The Protocol Control Indicator
        
//       // Receiving Single Frame
//       case 0x00 : {                                                                   // Single Frame Type
//         TP_RxFrameSF();                                                               // Single Frame Processing Function
//         return;
//       }

//       // Receiving First Frame
//       case 0x01 : {                                                                   // First Frame Type
//         TP_RxFrameFF();                                                               // First Frame Processing Function
//         return;
//       }
      
//       // Receiving Consecutive Frame
//       case 0x02 : {                                                                   // Consecutive Frame Type
//         TP_RxFrameCF();                                                               // Consecutive Frame Processing Function
//         return;
//       }
      
//       // Receiving Flow Control Frame
//       case 0x03 : {                                                                   // Flow Control Frame
//         TP_RxFrameFC();                                                               // Flow Control Frame Processing Function
//         return;
//       }
      
//     }
//   } else {
//     TP_Status.RxFlag = 0;                                                             // Rare Exception Handling
//     return;
//   }
// }
// /* ==================================================================================================== */





/* ==================================================================================================== */
/**
 * @name        Clock
 * @scope       ISO_DoCAN (Public)
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
 * @scope       ISO_DoCAN (Public)
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
 * @scope       ISO_DoCAN (Public)
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
 * @scope       ISO_DoCAN (Public)
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
 * @scope       ISO_DoCAN (Public)
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
 * @scope       ISO_DoCAN (Public)
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