#include <neopixel.h>

#define PIXEL_PIN D2
#define PIXEL_COUNT 20
#define PIXEL_TYPE WS2812B

#define REFRESH_INTERVAL (1000 * 60 * 30)

#define COLOR_BLANK strip.Color(0, 0, 0)
#define COLOR_UNKNOWN strip.Color(10, 10, 5)
#define COLOR_LIFR strip.Color(15, 0, 7)
#define COLOR_IFR strip.Color(15, 0, 0)
#define COLOR_MVFR strip.Color(0, 0, 15)
#define COLOR_VFR strip.Color(0, 15, 0)

// Airport data
typedef struct { uint16_t pixel; char* code; } airport;
const airport airportMapping[] {
    {0, "KCOS"},
    {1, "K1V6"},
    {2, "KANK"},
    {3, "KGUC"},
    {4, "KASE"},
    {5, "KEGE"},
    {6, "KLXV"},
    {7, "KAPA"},
    {8, "KFTG"},
    {9, "KDEN"},
    {10, "KBJC"},
    {11, "KGNB"},
    {12, "K20V"},
    {13, "KSBS"},
    {14, "K33V"},
    {15, "KFNL"},
    {16, "KGXY"},
    {17, "KCYS"},
    {18, "KLAR"},
    {19, "KSAA"},
};
#define NUM_AIRPORTS (sizeof airportMapping / sizeof airportMapping[0])

// LED setup
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
    
// Setup function
void setup() {
    // Test harness
    pinMode(D1, OUTPUT);
    pinMode(D3, OUTPUT);
    digitalWrite(D1, HIGH);
    digitalWrite(D3, LOW);
    // Hook up Particle integrations
    Particle.subscribe("metar_response", handleWeather, MY_DEVICES);
    // Initialise LEDs by flashing them (helps debug location and colours)
    strip.begin();
    setAllColor(COLOR_VFR);
    delay(1000);
    setAllColor(COLOR_MVFR);
    delay(1000);
    setAllColor(COLOR_IFR);
    delay(1000);
    setAllColor(COLOR_LIFR);
    delay(1000);
    for (uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, COLOR_UNKNOWN);
        strip.show();
        delay(200);
    }
}

// Sets all LEDs to the given color
void setAllColor(uint32_t color) {
    for (uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, color);
    }
    strip.show();
}

// Main loop
unsigned long lastUpdate = -1;
void loop() {
    // See if we should ask for airport updates
    if ((millis() < lastUpdate) || (millis() - lastUpdate > REFRESH_INTERVAL)) {
        // Trigger updates (with phasing to make sure they're not too close together)
        Particle.publish("started_airport_update", String(millis()), PRIVATE);
        for (uint16_t i = 0; i < NUM_AIRPORTS; i++) {
            airport a = airportMapping[i];
            fetchWeather(a.code);
            delay(2000);
        }
        lastUpdate = millis();
    }
    strip.show();
    delay(100);
}

// Fetches the weather for the given station
void fetchWeather(char* code) {
    Particle.publish("get_metar", code, PRIVATE);
}

// Handles weather responses
void handleWeather(const char *event, const char *data) {
    // Grab out the ICAO code
    char icao[5];
    strncpy(icao, data, 4);
    icao[4] = 0;
    // And the conditions
    const char* conditions = (data + 5);
    uint32_t color = COLOR_UNKNOWN;
    if (strcmp("VFR", conditions) == 0) {
        color = COLOR_VFR;
    } else if (strcmp("MVFR", conditions) == 0) {
        color = COLOR_MVFR;
    } else if (strcmp("IFR", conditions) == 0) {
        color = COLOR_IFR;
    } else if (strcmp("LIFR", conditions) == 0) {
        color = COLOR_LIFR;
    }
    // Find the airport and set its pin
    uint16_t pixel = 256;
    for (uint16_t i = 0; i < NUM_AIRPORTS; i++) {
        airport a = airportMapping[i];
        if (strcmp(a.code, icao) == 0) {
            pixel = a.pixel;
        }
    }
    if (pixel < 256) {
        strip.setPixelColor(pixel, color);
        strip.show();
    }
    // // Optional debugging
    // Particle.publish("set_pixel", String(pixel) + "/" + icao + "/" + conditions, PRIVATE);
}