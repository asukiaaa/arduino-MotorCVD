# arduino-MotorCVD (MotorCVD_asukiaaa)

A library to use oriental motor CVD2B-KR or CVD5B-KR series series in RS485 modbus.

## Seftware setup

### Arduino IDE

1. Open library manager
2. Install `MotorCVD_asukiaaa`

### PlatformIO

Add `MotorCVD_asukiaaa` to lib_deps of platformio.ini.

## Hardware setup

### SW1: Motor type

Change switch by your motor.

SW1|Motor|Current(A)
---|-----|---
2|PKP213D05\*|0.5
3|PKP214D06\*|0.6
4|PKP24\*D08\*2|0.85
5|PKP26\*D14\*2|1.4
6|PKP22\*D15\*<br />PKP22\*D15\*2<br />PKP22\*MD15\*<br />PKP23\*D15\*<br />PKP262FD15A|1.5
7|PKP24\*D15\*2<br />PKP24\*MD15\*2|1.5
8|PKP23\*D23\* 2.3
9|PKP24\*D23\*2 2.3
A|PKP25\*D28\*A2<br />PKP26\*D28\*2<br />PKP26\*MD28\*2|2.8

### SW2: End resistor for RS485

Both `ON` to activate. If you don't need them, set both `OFF`.

## Usage

See [an example](examples/forwardAndReverse/forwardAndReverse.ino).

## License

MIT

## References

- [CVD2B-KR](https://www.orientalmotor.co.jp/products/detail.action?hinmei=CVD2B-KR)
- [CVD2BR-KR](https://www.orientalmotor.co.jp/products/detail.action?hinmei=CVD2BR-KR)
- [CVD5B-KR](https://www.orientalmotor.co.jp/products/detail.action?hinmei=CVD5B-KR)
- [CVD5BR-KR](https://www.orientalmotor.co.jp/products/detail.action?hinmei=CVD5BR-KR)
