#define  DEVICE_UNAVAILBLE_ONLY_DEVELOPMENT
// #define  DEVICE_DEBUG_MODE
#define  DEVICE_SLOW_FLASHING
#include "lib\DriverPCAN.hpp"
#include "lib\UDS.hpp"
#include <csignal>

void TaskKilled (int Signal) {
  std::cout << "\n\nSignal " << Signal << " : Closing Application . . .";
  UDS.DoCAN.Kill();
  UDS.Kill();
  exit(Signal);
}

std::string HEX2String (uint8_t Data) {
  std::stringstream Steam;
  Steam << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(Data);
  return Steam.str();
}

int main(void) {
  std::signal(SIGINT, TaskKilled);
  std::signal(SIGTERM, TaskKilled);
  int UDS_ERR = 0;
  do {
    UDS.Start();
    UDS_ERR = UDS.GetERROR();
    if (UDS_ERR != UDS_ERR_ALLOK) {
      Sleep(7000);
    }
  } while (UDS_ERR != UDS_ERR_ALLOK);

  do {  UDS.Application();  } while (1);

  return 0;
}


