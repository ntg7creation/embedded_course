#include <Arduino.h>

#include "WaveShareDemo.h"

// void setup()
// {
//   Wvshr_Init();

//   Serial.println("3.5inch TFT Touch Shield LCD Show...");
//   Serial.println("LCD Init...");
//   LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;
//   LCD_Init(Lcd_ScanDir, 200);

//   Serial.println("LCD_Clear...");
//   LCD_Clear(LCD_BACKGROUND);

//   Serial.println("LCD_Show...");
//   GUI_Show();

//   TP_Init(); // Init the TouchPad

//   //  ======  Draw button to moveon to time display ======
//   GUI_DrawRectangle(200, 170, 360, 170 + 35, YELLOW, DRAW_FULL, DOT_PIXEL_1X1);
//   GUI_DisString_EN(205, 180, "Touch for Time", &Font16, YELLOW, BLUE);

//   while (1)
//   {
//     TP_Scan(0);
//     if (sTP_DEV.chStatus & TP_PRESSED)
//     {
//       sTP_DEV.chStatus &= ~TP_PRESSED;
//       if (sTP_Draw.Xpoint > 200 && sTP_Draw.Xpoint < 360 && sTP_Draw.Ypoint > 170 && sTP_Draw.Ypoint < 170 + 35)
//         break;
//     }
//     delay(100);
//   }

//   GUI_DrawRectangle(200, 170, 360, 170 + 35, WHITE, DRAW_FULL, DOT_PIXEL_1X1);

//   //  ======  Prepare button to moveon to Touch-Draw demonstration in loop() ======
//   GUI_DrawRectangle(360, 170, 444, 170 + 35, BLUE, DRAW_FULL, DOT_PIXEL_1X1);
//   GUI_DisString_EN(362, 176, "Next=>", &Font20, BLUE, WHITE);
// }

bool run = true;
bool draw_delete = true;
static int startX = 240;
static int startY = 160;
int xLocation = 240;
int yLocation = 160;
int stepSize = 10;
enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

Direction direction = RIGHT;

void setup()
{
  Wvshr_Init();

  Serial.begin(115200);
  Serial.println("3.5inch TFT Touch Shield LCD Show...");
  Serial.println("LCD Init...");
  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;
  LCD_Init(Lcd_ScanDir, 200);

  Serial.println("LCD_Clear...");
  LCD_Clear(LCD_BACKGROUND);

  Serial.println("LCD_Show...");

  draw_delete = true;
  xLocation = 240;
  yLocation = 160;
  TP_Init(); // Init the TouchPad
}

bool withinBounds(int x, int y)
{
  return (x >= 0 && x <= 479 && y >= 0 && y <= 319);
}

void DrawSpiral(void)
{
  // Spiral drawing state (persist between calls)

  static int segmentsSinceIncrease = 0;
  static COLOR color = BLUE;

  // start point is the globals xLocation/yLocation

  int x0 = xLocation;
  int y0 = yLocation;
  int x1 = x0;
  int y1 = y0;
  Direction dir = direction;
  // compute next end point based on current direction and stepSize
  switch (dir)
  {
  case RIGHT:
    x1 = x0 + stepSize;
    direction = DOWN;
    break;
  case DOWN:
    y1 = y0 + stepSize;
    direction = LEFT;
    break;
  case LEFT:
    x1 = x0 - stepSize;
    direction = UP;
    break;
  case UP:
    y1 = y0 - stepSize;
    direction = RIGHT;
    break;
  }

  GUI_DrawLine(x0, y0, x1, y1, color, LINE_SOLID, DOT_PIXEL_1X1);
  // update current location globals
  xLocation = x1;
  yLocation = y1;

  // Serial.println("Drew line now at (" + String(xLocation) + "," + String(yLocation) + ")");

  // track when to increase step size (increase every two segments)
  segmentsSinceIncrease++;
  if (segmentsSinceIncrease >= 2)
  {
    segmentsSinceIncrease = 0;
    stepSize += 5; // increase step by 5 pixels (matches previous increments)
  }

  // rotate color a bit for visual variety (wrap-around depends on COLOR type)
  // color = (COLOR)(color + 1);
  if (!withinBounds(xLocation, yLocation))
  {
    run = false;
  }
}

void DeleteSpiral(void)
{
  // Spiral drawing state (persist between calls)

  static int segmentsSinceIncrease = 0;
  static COLOR color = RED;

  // start point is the globals xLocation/yLocation

  int x0 = xLocation;
  int y0 = yLocation;
  int x1 = x0;
  int y1 = y0;
  Direction dir = direction;
  // compute next end point based on current direction and stepSize
  // this time dir reprasents the direction we should have gone next

  // track when to increase step size (increase every two segments)

  // segmentsSinceIncrease++;
  // if (segmentsSinceIncrease >= 2)
  // {
  //   segmentsSinceIncrease = 0;
  //   stepSize += 5;
  // }

  if (segmentsSinceIncrease <= 0)
  {
    segmentsSinceIncrease = 2;
    stepSize -= 5; // decrease step by 5 pixels (matches previous increments)
  }
  segmentsSinceIncrease--;

  switch (dir)
  {
  case RIGHT:
    y1 = y0 + stepSize;
    direction = UP;
    break;
  case DOWN:
    x1 = x0 - stepSize;
    direction = RIGHT;
    break;
  case LEFT:
    y1 = y0 - stepSize;
    direction = DOWN;
    break;
  case UP:
    x1 = x0 + stepSize;
    direction = LEFT;
    break;
  }

  GUI_DrawLine(x0, y0, x1, y1, color, LINE_SOLID, DOT_PIXEL_1X1);
  Serial.println("Drew line now at (" + String(xLocation) + "," + String(yLocation) + ")");
  // update current location globals
  xLocation = x1;
  yLocation = y1;

  // rotate color a bit for visual variety (wrap-around depends on COLOR type)
  // color = (COLOR)(color + 1);
  if (stepSize < 10)
  {
    segmentsSinceIncrease = 0;
    stepSize = 10;
    run = false;
    xLocation = startX;
    yLocation = startY;
  }
}

void loop()
{
  if (run)
  {
    Driver_Delay_ms(200);
    if (draw_delete)
      DrawSpiral();
    else
      DeleteSpiral();
  }
  TP_Scan(0);
  if (sTP_DEV.chStatus & TP_PRESSED)
  {
    sTP_DEV.chStatus &= ~TP_PRESSED;
    Serial.println("Touch at (" + String(sTP_Draw.Xpoint) + "," + String(sTP_Draw.Ypoint) + ")");

    // Check if touch is within spiral bounds (start location ± tolerance)
    int tolerance = 30;
    // First check if within tolerance of start position
    int distX = abs(sTP_Draw.Xpoint - startX);
    int distY = abs(sTP_Draw.Ypoint - startY);

    if (distX < tolerance && distY < tolerance)
    {
      draw_delete = !draw_delete;
      Serial.println("Flipped mode at start");
      run = true;
    }
    else
    {
      // Check if within spiral bounds
      int spiralRadiusX = abs(xLocation - startX);
      int spiralRadiusY = abs(yLocation - startY);

      int distFromStartX = abs(sTP_Draw.Xpoint - startX);
      int distFromStartY = abs(sTP_Draw.Ypoint - startY);

      if (distFromStartX <= spiralRadiusX + tolerance && distFromStartX >= spiralRadiusX - tolerance &&
          distFromStartY <= spiralRadiusY + tolerance && distFromStartY >= spiralRadiusY - tolerance)
      {
        draw_delete = !draw_delete;
        run = true;
        Serial.println("Flipped mode within spiral");
      }
    }
  }

  delay(200);
}

//   //  ======  Demonstrate updating Time Display ======
//   GUI_Showtime(200, 170, 327, 170 + 47, &sDev_time, RED);
//   Driver_Delay_ms(1000); // Analog clock 1s
//   if (sDev_time.Sec == 60)
//     sDev_time.Sec = 0;

//   TP_Scan(0);
// if (sTP_DEV.chStatus & TP_PRESSED)
//   {
//     sTP_DEV.chStatus &= ~TP_PRESSED;
//     if (sTP_Draw.Xpoint > 360 && sTP_Draw.Xpoint < 444 && sTP_Draw.Ypoint > 170 && sTP_Draw.Ypoint < 170 + 35)
//       break;
//   }
// }

// //  ======  Demonstrate Touch Drawing ======
// TP_Dialog();
// for (;;)
// {
//   TP_DrawBoard();
// }

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
