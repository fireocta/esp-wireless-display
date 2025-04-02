// Comment out all other display drivers
#define ST7789_DRIVER

// Define pins for ESP32
#define TFT_MISO -1
#define TFT_MOSI 23  // SDA pin
#define TFT_SCLK 18  // SCL pin
#define TFT_CS   5   // Chip select pin
#define TFT_DC   2   // Data/Command pin
#define TFT_RST  4   // Reset pin

// Rotation settings
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// Color depth
#define SPI_FREQUENCY  40000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000