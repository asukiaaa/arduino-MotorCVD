#ifndef _MOTOR_CVD_ASUKIAAA_HPP_
#define _MOTOR_CVD_ASUKIAAA_HPP_

#include <Arduino.h>
#include <rs485_asukiaaa.h>

namespace MotorCVD_asukiaaa {

const int SPEED_MAX = 5000;

enum Address {
  CurrentPosition = 0x00c6,
  TargetPosition = 0x1802,
  Speed = 0x0480,
  DriveDataNumber = 0x007b,
  DriverCommand = 0x007d,
  Alarm = 0x0a00,
};

enum CommandBit {
  StartR = 0x8,
  Forward = 0x4000,
  Reverse = 0x8000,
};

class Driver {
 public:
  Driver(HardwareSerial* serial, int16_t pinDe, int16_t pinRe, uint8_t address)
      : modbus(serial, pinDe, pinRe), address(address) {}

  void begin(int baudrate, int config) {
    modbus.begin(baudrate, config);
    delay(50);
    setDriveDataNumber(0);
  }

  int setDriveDataNumber(int32_t number) {
    uint32_t dataNumber = number;
    return modbus.writeRegistersBy32t(address, Address::Speed, &dataNumber, 1);
  }

  int readAlarm(uint16_t* alarm) {
    return modbus.readRegistersBy16t(address, Address::Alarm + 1, alarm, 1);
  }

  int readPosition(int32_t* position) {
    uint32_t v;
    int result =
        modbus.readRegistersBy32t(address, Address::CurrentPosition, &v, 1);
    if (result != 0) return result;
    *position = v;
    return 0;
  }

  int writeSpeed(int32_t speed) {
    bool isReverse = speed < 0;
    auto absSpeed = normalizeSpeed(speed);
    int result =
        modbus.writeRegistersBy32t(address, Address::Speed, &absSpeed, 1);
    if (result != 0) return result;
    return modbus.writeRegisterBy16t(address, Address::DriverCommand,
                                     absSpeed == 0 ? 0
                                     : isReverse   ? CommandBit::Reverse
                                                   : CommandBit::Forward);
  }

  int writeStep(int32_t speed, int32_t step) {
    auto absSpeed = normalizeSpeed(speed);
    uint32_t buffs[2];
    buffs[0] = step;
    buffs[1] = absSpeed;
    int result =
        modbus.writeRegistersBy32t(address, Address::TargetPosition, buffs, 2);
    if (result != 0) return result;
    // TODO write start
    return modbus.writeRegisterBy16t(address, Address::DriverCommand,
                                     CommandBit::StartR);
  }

  rs485_asukiaaa::ModbusRtu::Central modbus;

 private:
  const uint8_t address;
  uint32_t normalizeSpeed(int32_t speed) {
    uint32_t absSpeed = abs(speed);
    if (absSpeed > SPEED_MAX) absSpeed = SPEED_MAX;
    return absSpeed;
  }
};

}  // namespace MotorCVD_asukiaaa

#endif
