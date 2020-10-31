#include <Arduino.h>
#include <bluefruit.h>

#define VERSION "cc-client-0.1"

#define BLE_UUID_BAT_SERVICE 0x1235
#define BLE_UUID_BAT_MEASUREMENT_CHAR 0x1236

#define BLE_UUID_PARAM_SERVICE 0x5678
#define BLE_UUID_PARAM_MEASUREMENT_CHAR 0x5679

volatile bool heater_on = false;

const uint8_t LBS_UUID_BASE[16] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00};
const uint8_t LBS_UUID_SERVICE[16] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x23, 0x15, 0x00, 0x00};
const uint8_t LBS_UUID_BUTTON_CHAR[16] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x24, 0x15, 0x00, 0x00};
const uint8_t LBS_UUID_LED_CHAR[16] = {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x25, 0x15, 0x00, 0x00};

// 1. Declare device information service
BLEDis dis;

// 3. Declare LED service
BLEService led_service = BLEService(BLEUuid(LBS_UUID_SERVICE));
BLECharacteristic btn_characteristic = BLECharacteristic(BLEUuid(LBS_UUID_BUTTON_CHAR));
BLECharacteristic led_characteristic = BLECharacteristic(BLEUuid(LBS_UUID_LED_CHAR));

void set_up_services() {
    // 1. Set up device information service
    dis.setManufacturer("Reece");
    dis.begin();

    // 2. Set up LED service
    led_service.begin();
    btn_characteristic.setProperties(CHR_PROPS_NOTIFY | CHR_PROPS_READ);
    btn_characteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
    btn_characteristic.setMaxLen(10);
    btn_characteristic.begin();
}

void start_advertising()
{
  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(160, 160);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode

  Bluefruit.Advertising.addService(dis);

  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void on_disconnect(uint16_t conn_handle, uint8_t reason) {
    reset_state();
}

volatile bool send_notification = false;
uint8_t counter = 0;

void button_isr() {
    send_notification = true;
    digitalToggle(13);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(13, OUTPUT);
  int button_pin = 12;
  pinMode(button_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(button_pin), button_isr, CHANGE);

  digitalWrite(13, LOW);
  Serial.begin(115200);
  /* while ( !Serial ) delay(10);   // for nrf52840 with native usb */

  Bluefruit.begin();

  // Use blue LED as debugging tool to indicate advertising vs. connected states
  Bluefruit.autoConnLed(true);
  Bluefruit.setTxPower(0);    // Check bluefruit.h for supported values
  Bluefruit.setName("candy-chute-client");

  set_up_services();
  start_advertising();

  btn_characteristic.write8(counter);
  counter += 1;
}

bool playback = false;
void reset_state() {
    playback = false;
}

uint8_t data[1] = {0x01};

void loop() {
    if (Bluefruit.connected()) {
        playback = true;
        if (send_notification) {
            btn_characteristic.notify(data, 1);
            send_notification = false;
            delay(1000);
        }
    } else {
        if (playback) {
            // We were playing back a sample, but BLE client disconnected
            Serial.println("err connection-abort");
            reset_state();
            return;
        } else {
            // Still awaiting a connection
            Serial.println("awaiting connection");
            delay(100);
        }
    }
}
