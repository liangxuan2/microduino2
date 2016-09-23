#include <IRremote.h>
#include <Adafruit_NeoPixel.h>

#define PIN 6
#define RECV_PIN 10

//遥控器按键编码定义
#define INCREASE 0x1FEF807    //增加+
#define DECREASE 0x1FE708F   //减少-
#define NUM_0 0xFF6897      //数字0
#define NUM_1 0x1FE807F      //数字1
#define NUM_2 0x1FE40BF      //数字2
#define NUM_3 0x1FEC03F      //数字3
#define NUM_4 0x1FE20DF      //数字4
#define NUM_5 0x1FEA05F      //数字5
#define NUM_6 0x1FE609F      //数字6
#define NUM_7 0x1FEE01F      //数字7
#define NUM_8 0x1FE10EF      //数字8
#define NUM_9 0x1FE906F      //数字9

Adafruit_NeoPixel strip = Adafruit_NeoPixel(6, PIN, NEO_GRB + NEO_KHZ800);

IRrecv irrecv(RECV_PIN);
decode_results results;

#define val_max 255
#define val_min 0

int pos = 8, color = 100;

uint32_t color_n[9] =
{
  strip.Color(255, 0, 0),
  strip.Color(248, 141, 30),
  strip.Color(255, 255, 0),
  strip.Color(0, 255, 0),
  strip.Color(0, 127, 255),
  strip.Color(0, 0, 255),
  strip.Color(139, 0, 255),
  strip.Color(255, 255, 255),
  strip.Color(0, 0, 0)
};
uint32_t color_m[9][3] =
{
  {0, 255, 255},
  {255, 0, 0},
  {248, 141, 30},
  {255, 255, 0},
  {0, 255, 0},
  {0, 127, 255},
  {0, 0, 255},
  {139, 0, 255},
  {255, 255, 255}
};

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  irrecv.enableIRIn(); // Start the receiver
  for (int i = 0; i < 9; i++)
  {
    colorSetall(color_n[i]);
    delay(300);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (irrecv.decode(&results))
  {
    Serial.println(results.value, HEX);
    switch (results.value)
    {
      case INCREASE:
//        color += 50;
//        if (color > 250)
//          color = 250;
        color = 100;
        pos = 0;
        break;
      case DECREASE:
//        color -= 50;
//        if (color < 0)
//          color = 0;
        color = 0;
        pos = 0;
        break;
      case NUM_1:
        color = 100;
        pos = 0;
        break;
      case NUM_2:
        pos = 1;
        color = 100;
        break;
      case NUM_3:
        pos = 2;
        color = 100;
        break;
      case NUM_4:
        pos = 3;
        color = 100;
        break;
      case NUM_5:
        pos = 4;
        color = 100;
        break;
      case NUM_6:
        pos = 5;
        color = 100;
        break;
      case NUM_7:
        pos = 6;
        color = 100;
        break;
      case NUM_8:
        pos = 7;
        color = 100;
        break;
      case NUM_9:
        pos = 8;
        color = 100;
        break;
      default:
        break;
    }
    irrecv.resume(); // Receive the next value
    colorSetall(strip.Color(map(color, val_min, val_max, 0, color_m[pos][0]),
                            map(color, val_min, val_max, 0, color_m[pos][1]),
                            map(color, val_min, val_max, 0, color_m[pos][2])));
  }
}

void colorSetall(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}
