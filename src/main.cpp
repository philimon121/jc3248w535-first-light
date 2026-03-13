#include <Arduino.h>
#include <JC3248W535.h>

// Touch first-light:
// - display enabled
// - touch enabled
// - landscape orientation locked to match current known-good board hold
// - one-time screen setup, then only draw touch markers

static constexpr bool kEnableTouch = true;
static constexpr uint8_t kDisplayRotation = ROTATION_270;
static constexpr uint16_t kTouchPollDelayMs = 15;
static constexpr uint16_t kMoveThresholdPx = 4;

JC3248W535_Display display;
JC3248W535_Touch touch;

static bool gTouchReady = false;
static bool gWasTouched = false;
static TouchPoint gLastPoint = {0, 0, false};

static void haltWithMessage(const char *message)
{
  Serial.println(message);
  while (true)
  {
    delay(1000);
  }
}

static void drawCross(Arduino_Canvas *gfx, int16_t x, int16_t y, uint16_t color)
{
  constexpr int16_t kArm = 10;
  constexpr int16_t kHalfBox = 2;

  gfx->drawLine(x - kArm, y - kArm, x + kArm, y + kArm, color);
  gfx->drawLine(x - kArm, y + kArm, x + kArm, y - kArm, color);
  gfx->drawRect(x - kHalfBox, y - kHalfBox, (kHalfBox * 2) + 1, (kHalfBox * 2) + 1, color);
}

static void drawTouchUi(Arduino_Canvas *gfx, int16_t w, int16_t h)
{
  gfx->fillScreen(WHITE);
  gfx->drawRect(0, 0, w, h, BLACK);
  gfx->drawRect(1, 1, w - 2, h - 2, BLACK);

  gfx->fillRect(0, 0, 24, 24, RED);
  gfx->fillRect(w - 24, 0, 24, 24, GREEN);
  gfx->fillRect(0, h - 24, 24, 24, BLUE);
  gfx->fillRect(w - 24, h - 24, 24, 24, YELLOW);

  gfx->setTextColor(BLACK);
  gfx->setTextSize(2);
  gfx->setCursor(16, 18);
  gfx->print("JC3248W535 touch test");

  gfx->setCursor(16, 44);
  gfx->print("Touch the screen to draw X marks");

  gfx->drawFastHLine(0, 64, w, BLACK);
}

static bool pointMovedEnough(const TouchPoint &a, const TouchPoint &b)
{
  return (abs((int)a.x - (int)b.x) >= kMoveThresholdPx) ||
         (abs((int)a.y - (int)b.y) >= kMoveThresholdPx);
}

void setup()
{
  Serial.begin(115200);
  delay(250);

  Serial.println();
  Serial.println("JC3248W535 touch first-light");
  Serial.println("Display: enabled");
  Serial.println("Touch: enabled");
  Serial.println("Rotation: landscape (ROTATION_270)");

  if (!display.begin())
  {
    haltWithMessage("Display init failed!");
  }

  display.setRotation(kDisplayRotation);

  Arduino_Canvas *gfx = display.getCanvas();
  if (gfx == nullptr)
  {
    haltWithMessage("Canvas unavailable");
  }

  const int16_t w = display.width();
  const int16_t h = display.height();

  drawTouchUi(gfx, w, h);
  display.flush();

  if (kEnableTouch)
  {
    if (touch.begin())
    {
      display.setTouchRotation(&touch);
      gTouchReady = true;
      Serial.println("Touch init OK");
    }
    else
    {
      Serial.println("Touch init failed");
    }
  }
  else
  {
    Serial.println("Touch disabled");
  }

  Serial.print("Display size: ");
  Serial.print(w);
  Serial.print(" x ");
  Serial.println(h);
  Serial.println("Touch first-light ready");
}

void loop()
{
  if (!gTouchReady)
  {
    delay(250);
    return;
  }

  TouchPoint point;
  const bool touched = touch.read(point);

  if (touched && point.touched)
  {
    const bool shouldDraw = (!gWasTouched) || pointMovedEnough(point, gLastPoint);

    if (shouldDraw)
    {
      Arduino_Canvas *gfx = display.getCanvas();
      if (gfx != nullptr)
      {
        drawCross(gfx, point.x, point.y, RED);
        display.flush();
      }

      Serial.print("Touch: x=");
      Serial.print(point.x);
      Serial.print(", y=");
      Serial.println(point.y);

      gLastPoint = point;
    }

    gWasTouched = true;
  }
  else
  {
    if (gWasTouched)
    {
      Serial.println("Touch released");
    }
    gWasTouched = false;
  }

  delay(kTouchPollDelayMs);
}
