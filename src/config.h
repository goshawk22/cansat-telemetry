// Find all these in the schematic
#define GPS_TX_PIN 15
#define GPS_RX_PIN 14
#define GPS_BAUD_RATE 9600

#define SENSOR_SDA_PIN 16
#define SENSOR_SCL_PIN 17

#define SX1262_MISO_PIN 4
#define SX1262_MOSI_PIN 3
#define SX1262_SCK_PIN 6
#define SX1262_CS_PIN 1

#define SX1262_DIO1_PIN 9
#define SX1262_RST_PIN 0
#define SX1262_BUSY_PIN 8

// OFCOM IR2030/1/19
#define SX1262_FREQUENCY 869.525
#define SX1262_BANDWIDTH 125.0
#define SX1262_SPREADING_FACTOR 8
#define SX1262_CODING_RATE 5
#define SX1262_PREAMBLE_LENGTH 8
#define SX1262_SYNC_WORD 0x12
#define SX1262_TX_POWER 22

// Duty cycle is max 10%, but we want to transmit as often as possible
// https://iftnt.github.io/lora-air-time/index.html
// Or use RadioLib's air time calculator
// We don't actually use this but keep it here for reference
#define LORA_PACKET_AIR_TIME 92672 // in microseconds
// From above, airtime is 92.672ms, so since there's processing time
// we set the interval to 930ms
#define LORA_INTERVAL 930 // in milliseconds, so 1 second

// Flash memory interval
#define FLASH_INTERVAL 1000 // in milliseconds, so 1 second

// Battery voltage measurement
// Schottky diode voltage drop is about 0.25V at 100mA
// R1 = 200kOhm, R2 = 100kOhm
// Voltage divider multiplier is (R1 + R2) / R2
#define VOLTAGE_DIVIDER_MULTIPLIER 3.0
#define VSYS_ADC_PIN 29

// Use these for sensor encoding limits. Smaller limits result in higher resolution.
// Shouldn't get this cold or this hot, but we want to be safe.
#define AHT20_TEMPERATURE_MIN -50.0
#define AHT20_TEMPERATURE_MAX 60.0
#define BME280_TEMPERATURE_MIN -50.0
#define BME280_TEMPERATURE_MAX 60.0

// I have no idea what the expected humidity range is, but we can assume 0% to 100% for now
#define AHT20_HUMIDITY_MIN 0.0
#define AHT20_HUMIDITY_MAX 100.0
#define BME280_HUMIDITY_MIN 0.0
#define BME280_HUMIDITY_MAX 100.0

// Estimated from the altitude range
#define BME280_PRESSURE_MIN 400.0
#define BME280_PRESSURE_MAX 1100.0

// Maximum competition altitude is 3000m, but we want to be safe)
// Hopefully it doesn't say -100m altitude because then it's in the sea
#define ALTITUDE_MIN -100.0
#define ALTITUDE_MAX 4900.0

// Speed is in km/h, but we want to encode it in a byte
// Only speed that matters is the drifting speed, but if it gets a fix while
// descending and ascending at speed that would be cool
#define SPEED_MIN 0.0
#define SPEED_MAX 255.0

// we don't really care if we have more than 9 satellites, so we limit it to 10
#define SATELLITES_MIN 0
#define SATELLITES_MAX 9

// lithium battery voltage range, let's hope it doesn't get to 2.0V
#define VOLTAGE_MIN 2.0
#define VOLTAGE_MAX 4.3

// Uncomment to enable debug printing
// #define DEBUG

// Uncomment to enable verbose printing
// #define VERBOSE

#ifdef DEBUG
#define DEBUG_SERIAL Serial
#define DEBUG_PRINTLN(x) DEBUG_SERIAL.println(x)
#define DEBUG_PRINT(x) DEBUG_SERIAL.print(x)
#define DEBUG_PRINTF(...) DEBUG_SERIAL.printf(__VA_ARGS__)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTF(...)
#endif
#ifdef VERBOSE
#ifdef DEBUG
#define VERBOSE_PRINTLN(x) DEBUG_SERIAL.println(x)
#define VERBOSE_PRINT(x) DEBUG_SERIAL.print(x)
#endif
#else
#define VERBOSE_PRINTLN(x)
#define VERBOSE_PRINT(x)
#endif