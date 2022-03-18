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
  Information = 0x0a20,
};

enum CommandBit {
  StartR = 0x8,
  Forward = 0x4000,
  Reverse = 0x8000,
};

enum InformationCode : uint32_t {
  DriverTemperature = 0x4,
  OverVoltage = 0x10,
  UnderVoltage = 0x20,
  OperationStartError = 0x200,
  RequestingPreset = 0x800,
  MotorConfigurationError = 0x1000,
  RS485Error = 0x8000,
  ForwardOperationProhibition = 0x10000,
  ReverseOperationProhibition = 0x20000,
  Tripmeter = 0x1 << 5,
  Odometer = 0x2 << 5,
  OperationStartRestrictedMode = 0x1 << 7,
  IOTestMode = 0x2 << 7,
  ConfigurateionRequest = 0x4 << 7,
  RebootRequest = 0x8 << 7,
};

String getLabelsOfInformation(uint32_t info) {
  String strInfo = "";
  String strJoint = ". ";
  if ((info & InformationCode::DriverTemperature) != 0) {
    strInfo += "Driver temperature" + strJoint;
  }
  if ((info & InformationCode::OverVoltage) != 0) {
    strInfo += "Over voltage" + strJoint;
  }
  if ((info & InformationCode::UnderVoltage) != 0) {
    strInfo += "Under voltage" + strJoint;
  }
  if ((info & InformationCode::OperationStartError) != 0) {
    strInfo += "Operation start error" + strJoint;
  }
  if ((info & InformationCode::RequestingPreset) != 0) {
    strInfo += "Requesting preset" + strJoint;
  }
  if ((info & InformationCode::MotorConfigurationError) != 0) {
    strInfo += "Motor configuration error" + strJoint;
  }
  if ((info & InformationCode::RS485Error) != 0) {
    strInfo += "RS485 error" + strInfo;
  }
  if ((info & InformationCode::ForwardOperationProhibition) != 0) {
    strInfo += "Forward operation prohibition" + strJoint;
  }
  if ((info & InformationCode::ReverseOperationProhibition) != 0) {
    strInfo += "Reverse operation prohibition" + strJoint;
  }
  if ((info & InformationCode::Tripmeter) != 0) {
    strInfo += "Tripmeter" + strJoint;
  }
  if ((info & InformationCode::Odometer) != 0) {
    strInfo += "Odometer" + strJoint;
  }
  if ((info & InformationCode::OperationStartRestrictedMode) != 0) {
    strInfo += "Operation start restricted mode" + strJoint;
  }
  if ((info & InformationCode::IOTestMode) != 0) {
    strInfo += "IO test mode" + strJoint;
  }
  if ((info & InformationCode::ConfigurateionRequest) != 0) {
    strInfo += "Configuration request" + strJoint;
  }
  if ((info & InformationCode::RebootRequest) != 0) {
    strInfo += "Reboot request" + strJoint;
  }
  if (strInfo == "") {
    strInfo = "none";
  }
  return strInfo;
}

class Driver {
 public:
  Driver(HardwareSerial* serial, int16_t pinDe, int16_t pinRe,
         uint8_t address = 1)
      : address(address), createdModbus(true) {
    this->modbus = new rs485_asukiaaa::ModbusRtu::Central(serial, pinDe, pinRe);
  }
  Driver(rs485_asukiaaa::ModbusRtu::Central* modbus, uint8_t address = 1)
      : address(address), createdModbus(false) {
    this->modbus = modbus;
  }
  ~Driver() {
    if (createdModbus) {
      delete modbus;
    }
  }

  void beginWithoutModbus() { setDriveDataNumber(0); }

  void begin(int baudrate = 115200, int serialConfig = SERIAL_8E1) {
    modbus->begin(baudrate, serialConfig);
    delay(50);
    beginWithoutModbus();
  }

  int setDriveDataNumber(int32_t number) {
    uint32_t dataNumber = number;
    return modbus->writeRegistersBy32t(address, Address::DriveDataNumber,
                                       &dataNumber, 1);
  }

  int readAlarm(uint16_t* alarm) {
    return modbus->readRegistersBy16t(address, Address::Alarm + 1, alarm, 1);
  }

  int readInformation(uint32_t* info) {
    return modbus->readRegistersBy32t(address, Address::Information, info, 1);
  }

  int readPosition(int32_t* position) {
    uint32_t v;
    int result =
        modbus->readRegistersBy32t(address, Address::CurrentPosition, &v, 1);
    if (result != 0) return result;
    *position = v;
    return 0;
  }

  int writeSpeed(int32_t speed) {
    bool isReverse = speed < 0;
    auto absSpeed = normalizeSpeed(speed);
    int result =
        modbus->writeRegistersBy32t(address, Address::Speed, &absSpeed, 1);
    if (result != 0) return result;
    return modbus->writeRegisterBy16t(address, Address::DriverCommand,
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
        modbus->writeRegistersBy32t(address, Address::TargetPosition, buffs, 2);
    if (result != 0) return result;
    // TODO write start
    return modbus->writeRegisterBy16t(address, Address::DriverCommand,
                                      CommandBit::StartR);
  }

  rs485_asukiaaa::ModbusRtu::Central* modbus;

 private:
  const uint8_t address;
  const boolean createdModbus;
  uint32_t normalizeSpeed(int32_t speed) {
    uint32_t absSpeed = abs(speed);
    if (absSpeed > SPEED_MAX) absSpeed = SPEED_MAX;
    return absSpeed;
  }
};

}  // namespace MotorCVD_asukiaaa

#endif
