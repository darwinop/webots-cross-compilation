#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include "LinuxDARwIn.h"

using namespace Robot;
using namespace std;

int main()
{

  //////////////////// Framework Initialize ////////////////////////////
  LinuxCM730 linux_cm730("/dev/ttyUSB0");
  CM730 cm730(&linux_cm730);
  if (cm730.Connect() == false) {
    cerr << "Fail to connect CM-730!" << endl;
    return 0;
  }
  /////////////////////////////////////////////////////////////////////

  int value = 0;
  int thresholdPos = 230; // (20/360)*4096 => 20 degree
  int thresholdAcc = 32;  // 128/4 => 1/4 G
  int position[20] = { 1500, 2517, 1834, 2283, 2380, 1710, 2043, 2033, 2057, 2043, 1277, 2797, 3513, 571, 2843, 1240, 2077, 2037, 2050, 2173};  // Start position of each servos

  if (cm730.ReadByte(JointData::ID_R_SHOULDER_PITCH, MX28::P_VERSION, &value, 0)  != CM730::SUCCESS)
    cout << "MX28 firmware version : " << value << endl;

  // Check legs servos positions
  for (int c=6; c<18; c++) {
    cm730.ReadWord((c+1), MX28::P_PRESENT_POSITION_L, &value, 0);
    if (fabs(value - position[c]) > thresholdPos)
      return 0;
  }
  
  // Check accelerometer => orientation
  cm730.ReadWord(CM730::ID_CM, CM730::P_ACCEL_X_L, &value, 0);
  if (fabs(value - 512) > thresholdAcc)
    return 0;
  cm730.ReadWord(CM730::ID_CM, CM730::P_ACCEL_Y_L, &value, 0);
  if (fabs(value - 512) > thresholdAcc)
    return 0;
  cm730.ReadWord(CM730::ID_CM, CM730::P_ACCEL_Z_L, &value, 0);
  if (fabs(value - 640) > thresholdAcc)
    return 0;

  cout << "Ok" << endl;
  return 0;
}
