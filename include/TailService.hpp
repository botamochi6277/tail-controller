#ifndef TAIL_SERVICE_HPP
#define TAIL_SERVICE_HPP
#include <ArduinoBLE.h>

namespace botalab {

bool setupBle(BLEService &service) {
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");
    return false;
  }
  Serial.print("Peripheral address: ");
  String ble_address = BLE.address();
  Serial.println(ble_address);

  String localname = "SpermWhale_";
  localname += ble_address;

  // set advertised local name and service UUID:
  BLE.setDeviceName("HackaTail");
  BLE.setLocalName(localname.c_str());
  BLE.setAdvertisedService(service);
  BLE.addService(service);
  BLE.advertise();

  return true;
}

class TailService : public BLEService {
 public:
  BLEUnsignedLongCharacteristic timer_chr;
  BLEUnsignedShortCharacteristic command_chr;

  TailService();
  void init();
  void update();
};

TailService::TailService()
    : BLEService("70882CA0-6C07-426C-93F8-5C9483D06360"),
      timer_chr("70882CA1-6C07-426C-93F8-5C9483D06360", BLERead | BLENotify),
      command_chr("70882CA2-6C07-426C-93F8-5C9483D06360", BLERead | BLEWrite) {
  this->addCharacteristic(this->timer_chr);
  this->addCharacteristic(this->command_chr);

  // descriptors
  BLEDescriptor timer_descriptor("2901", "timer_ms");
  this->timer_chr.addDescriptor(timer_descriptor);
  BLEDescriptor cmd_descriptor("2901", "command");
  this->command_chr.addDescriptor(cmd_descriptor);
}

void TailService::update() {
  BLE.poll();
  this->timer_chr.writeValueLE(millis());
  if (this->command_chr.written()) {
    /* code */
  }
}

}  // namespace botalab

#endif