#include <Arduino.h>
#include "WaveShareDemo.h"

// -----------------------------------------------------------------------------
//  Spiral Drawing State
// -----------------------------------------------------------------------------
static bool run = true;         // Whether the spiral should update
static bool draw_delete = true; // true = draw outward, false = erase inward

// Spiral starting center
static int startX = 240;
static int startY = 160;

// Current drawing location
static int xLocation = 240;
static int yLocation = 160;

static int stepSize = 10; // Length of the next segment

// Spiral directions (right → down → left → up)
enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};
Direction direction = RIGHT;

// -----------------------------------------------------------------------------
//  Setup
// -----------------------------------------------------------------------------
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

  Serial.println("LCD Ready...");

  draw_delete = true;
  xLocation = startX;
  yLocation = startY;

  TP_Init(); // Initialize touch panel
}

// Check if coordinates are inside the LCD resolution
bool withinBounds(int x, int y)
{
  return (x >= 0 && x <= 479 && y >= 0 && y <= 319);
}

// -----------------------------------------------------------------------------
//  Draw Spiral Outward
// -----------------------------------------------------------------------------
void DrawSpiral(void)
{
  // Spiral pen color
  static COLOR color = BLUE;

  int x0 = xLocation;
  int y0 = yLocation;
  int x1 = x0;
  int y1 = y0;

  // Compute next segment endpoint
  switch (direction)
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

  // Draw the line segment
  GUI_DrawLine(x0, y0, x1, y1, color, LINE_SOLID, DOT_PIXEL_1X1);

  // Increase next segment size
  stepSize += 4;

  // Update current drawing position
  xLocation = x1;
  yLocation = y1;

  // Stop drawing if we hit the screen boundary
  if (!withinBounds(xLocation, yLocation))
    run = false;
}

// -----------------------------------------------------------------------------
//  Delete Spiral Inward (reverse)
// -----------------------------------------------------------------------------
void DeleteSpiral(void)
{
  // Use background color to erase
  static COLOR color = LCD_BACKGROUND;

  int x0 = xLocation;
  int y0 = yLocation;
  int x1 = x0;
  int y1 = y0;

  // Shorten the step size first
  stepSize -= 4;

  // Compute reverse direction movement
  switch (direction)
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

  // Draw erase line
  GUI_DrawLine(x0, y0, x1, y1, color, LINE_SOLID, DOT_PIXEL_1X1);

  // Update current position
  xLocation = x1;
  yLocation = y1;

  // When we return to the center, stop deleting
  if (stepSize <= 10)
  {
    stepSize = 10;
    run = false;
    xLocation = startX;
    yLocation = startY;
  }
}

// -----------------------------------------------------------------------------
//  Main Loop
// -----------------------------------------------------------------------------
void loop()
{
  // Draw or delete one step per cycle
  if (run)
  {
    Driver_Delay_ms(200);
    if (draw_delete)
      DrawSpiral();
    else
      DeleteSpiral();
  }

  // Handle touch input
  TP_Scan(0);

  if (sTP_DEV.chStatus & TP_PRESSED)
  {
    // Clear pressed flag
    sTP_DEV.chStatus &= ~TP_PRESSED;

    int touchX = sTP_Draw.Xpoint;
    int touchY = sTP_Draw.Ypoint;

    Serial.println("Touch at (" + String(touchX) + "," + String(touchY) + ")");

    const int tolerance = 60;

    // -------------------------------------------------------------
    // 1) If touch is near the center, toggle draw/delete
    // -------------------------------------------------------------
    int distX = abs(touchX - startX);
    int distY = abs(touchY - startY);

    if (distX < tolerance && distY < tolerance)
    {
      draw_delete = !draw_delete;
      run = true;
      Serial.println("Flipped mode at center");
    }
    else
    {
      // ---------------------------------------------------------
      // 2) Touch is inside spiral bounding rectangle
      // ---------------------------------------------------------
      int spiralRadiusX = abs(xLocation - startX);
      int spiralRadiusY = abs(yLocation - startY);

      int distFromStartX = abs(touchX - startX);
      int distFromStartY = abs(touchY - startY);

      Serial.println(
          "Spiral radius (" + String(spiralRadiusX) + "," +
          String(spiralRadiusY) + "), Dist (" +
          String(distFromStartX) + "," + String(distFromStartY) + ")");

      // Basic rectangle check
      if (distFromStartX <= spiralRadiusX &&
          distFromStartY <= spiralRadiusY)
      {
        draw_delete = !draw_delete;
        run = true;
        Serial.println("Flipped mode inside spiral bounds");
      }
    }
    }

    delay(200);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
