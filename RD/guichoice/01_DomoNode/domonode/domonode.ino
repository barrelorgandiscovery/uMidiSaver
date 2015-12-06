
/**********************************************************************************************/
/************************************ I N C L U D E S *****************************************/
/**********************************************************************************************/
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

/**********************************************************************************************/
/********************************* D E F I N I T I O N S **************************************/
/**********************************************************************************************/
// LCD Screen
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4
#define BUFFPIXEL 20

// TouchScreen
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A1  // must be an analog pin, use "An" notation!
#define YM 6   // can be a digital pin
#define XP 7   // can be a digital pin

// TouchScreen Limits
#define TS_MINX 95
#define TS_MAXX 940
#define TS_MINY 220
#define TS_MAXY 865
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// SD Card Chip Select
#define SD_CS 10

// In the SD card, place 24 bit color BMP files (be sure they are 24-bit!)
// Common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// KEYS
#define DUMMY 0x55
#define END_DUMMY 0xAA

// EEPROM keys
#define NODE_ID 1
#define TARGET_TEMP 2

#define THE_ID 133

/**********************************************************************************************/
/***************************** G L O B A L   V A R I A B L E S ********************************/
/**********************************************************************************************/

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 400);
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Point p;
byte active_menu = 0;
int count = 0;
byte temperature;

// Loadable variables
byte node_id;
byte target_temp;


/**********************************************************************************************/
/**************************************** S E T U P *******************************************/
/**********************************************************************************************/

void setup() {
  Serial.begin(9600);

  tft.reset();

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    return;
  }

  tft.begin(identifier);

  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    Serial.println(F("failed!"));
  //  return;
  }
  Serial.println(F("OK!"));
  
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  
  node_init();
}


/**********************************************************************************************/
/***************************************** L O O P ********************************************/
/**********************************************************************************************/
void loop() {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
    
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) { // TOUCHED!!
    
      /*Serial.print("x: ");
      Serial.print(p.x);
      Serial.print(" y: ");
      Serial.print(p.y);
      Serial.println("\n\r");*/
      
      if(p.y < 300) { // MENU
        menuOption(); 
      } else {
        touched(); 
      }
    } else { // DIDN'T TOUCH
      count++;
      if(count == 20000) {
        if(Serial.available() > 3) {
          receive_packet();
        }
        Serial.write(DUMMY);        // DUMMY
        Serial.write(node_id);      // NODE_ID
        Serial.write(temperature);  // TEMPERATURE
        Serial.write(target_temp);  // TARGET_TEMPERATURE
        Serial.write(END_DUMMY);    // END_DUMMY
        
        count = 0;
      }
      
    }
}


/**********************************************************************************************/
/************************************ F U N C T I O N S ***************************************/
/**********************************************************************************************/


void receive_packet(void) {
   char rx_dummy = Serial.read();
   char rx_node_id = Serial.read();
   char rx_target_temp = Serial.read();
   char rx_end_dummy = Serial.read();
   
   if(rx_dummy != 'a') return;
   else if(rx_end_dummy != 'f') return;
   else if(rx_node_id != 's') return;
   else {
      if((rx_target_temp > 4) && (rx_target_temp < 31)) {
        if(rx_target_temp != target_temp) {
          target_temp = rx_target_temp;
          save_eeprom();
        }
      }
      if(active_menu == 1) draw_temps();
   }
}

void menuOption() {
  if(p.x < TS_MAXX/7+100) { // Settings (6)
    menu6();
  } else if(p.x < 2*(TS_MAXX/7+40)) { // No function (5)
    menu5();
  } else if(p.x < 3*TS_MAXX/7+40) { // Light (4)
    menu4();
  } else if(p.x < 4*TS_MAXX/7+40) { // Key (3)
    menu3();
  } else if(p.x < 5*TS_MAXX/7+40) { // Blind (2)
    menu2();
  } else if(p.x < 6*TS_MAXX/7+40) { // Heat (1)
    menu1();
  } else {  // Power
        
  }    
}

void touched() {
  if(active_menu == 0) {
    // NO ACTION
  } else if(active_menu == 1) {
    if(p.x < (TS_MAXX-TS_MINX)/2) {
      if(target_temp < 30)
      target_temp++;
    } else {
      if(target_temp > 5)
      target_temp--;
    }
    draw_temps();
  } else if(active_menu == 6) {
    if(p.x < (TS_MAXX-TS_MINX)/2) {
      if(node_id < 255)
      node_id++;
    } else {
      if(node_id > 1)
      node_id--;
    }
    draw_settings();
  } else {
    // NO WAY
  }
}

void menu1(void) {
  if(active_menu == 1) {
    save_eeprom();
    return;
  }
  clear_main_zone();
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Temperature");  
  
  tft.setCursor(50, 120);
  tft.setTextSize(2);
  tft.print("Actual Temp: ");
  
  tft.setCursor(50, 160);
  tft.setTextSize(2);
  tft.setTextColor(WHITE);
  tft.print("Target Temp: ");
  
  draw_temps();
  
  active_menu = 1;
}

void draw_temps(void) {
  
  tft.fillRect(220, 100, 100, 200, BLACK);
  
  tft.setCursor(220, 115);
  if(temperature > target_temp) tft.setTextColor(RED);
  else tft.setTextColor(BLUE);
  tft.setTextSize(3);
  tft.print(temperature);
  
  tft.setCursor(220, 155);
  tft.setTextSize(3);
  tft.setTextColor(GREEN);
  tft.print(target_temp);
  
  delay(50);
   
}

void menu2(void) {
  if(active_menu == 2) return;
  clear_main_zone();
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Blinds");  
  active_menu = 2;
}

void menu3(void) {
  if(active_menu == 3) return;
  clear_main_zone();
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Alarm");  
  active_menu = 3;
}

void menu4(void) {
  if(active_menu == 4) return;
  clear_main_zone();
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Lights");  
  active_menu = 4;
}

void menu5(void) {
  //clear_main_zone();
  return;
}

void menu6(void) {
  if(active_menu == 6) {
    save_eeprom();
    return;
  }
  clear_main_zone();
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.println("Settings");
  
  tft.setCursor(50, 120);
  tft.setTextSize(2);
  tft.print("Node ID: ");
  
  draw_settings();
  
  active_menu = 6;
}

void draw_settings(void) {
  
  tft.fillRect(180, 100, 100, 200, BLACK);
  
  tft.setCursor(180, 115);
  tft.setTextSize(3);
  tft.setTextColor(GREEN);
  tft.print(node_id);
  
  delay(50);
   
}


void clear_main_zone(void) {
  tft.fillRect(0, 60, 320, 240, BLACK); 
}

void alert_saved() {
  tft.setCursor(258, 220);
  tft.setTextSize(2);
  tft.setTextColor(GREEN);
  tft.print("Saved"); 
  delay(600);
  tft.fillRect(258, 220, 100, 100, BLACK); 
}


/**
 * load_eppron - Loads EEPROM saved variables
 *
 */
void load_eeprom() {
 node_id = EEPROM.read(NODE_ID); 
 //node_id = THE_ID;
 target_temp = EEPROM.read(TARGET_TEMP); 
 //target_temp = 15;
}

/**
 * save_eeprom - Save EEPROM variables
 *
 */
void save_eeprom() {
 EEPROM.write(NODE_ID, node_id);
 EEPROM.write(TARGET_TEMP, target_temp);
 alert_saved();
}









/**
 * bmpDraw - Draws a BMP image in the specified coordinates
 *
 * @param *filename - String with the filename to be drawn.
 * @param x - X coordinate
 * @param y - Y coordinate
 *
 */
void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    (void)read32(bmpFile);
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    // Read DIB header
    (void)read32(bmpFile);
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) {
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

/**
 * read16 - Read 16 bit files
 */
uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

/**
 * read32 - Read 32 bit files
 */
uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


/**
 * node_init - Method to initialize. It calls load_eeprom and draw_menu
 *
 */
void node_init() {
  tft.setCursor(15, 70);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("RaulMerlin Domonode v1.0");  
  tft.setCursor(15, 90);
  tft.setTextSize(2);
  tft.println("Loading Menu... ");
    draw_menu(); //       ---- DRAW MENU ----         //
  tft.setCursor(250, 90);
  tft.setTextColor(GREEN);
  tft.println("DONE");
  
  tft.setCursor(15, 110);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Loading Data... ");
  load_eeprom();
  tft.setCursor(250, 110);
  tft.setTextColor(GREEN);
  tft.println("DONE");
  
  /*tft.setCursor(15, 130);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Loading... ");
    // LOAD ANY DATA
  tft.setCursor(250, 130);
  tft.setTextColor(YELLOW);
  tft.println("WARN");*/
  
  tft.setCursor(100, 185);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.println("Starting... ");
  delay(500);
  
  tft.fillRect(0, 60, 320, 240, BLACK);
  
}



/**
 * draw_menu - Draws main menu icons
 *
 */
void draw_menu() {
  bmpDraw("power.bmp", 10, 15);
  bmpDraw("heating.bmp", 55, 15);
  bmpDraw("blind.bmp", 100, 15);
  bmpDraw("key.bmp", 145, 15);
  bmpDraw("light.bmp", 190, 15);
  bmpDraw("settings.bmp", 280, 15); 
  return;
}

