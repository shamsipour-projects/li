---
title: "Experiment 03: Toy car"
header: "Experiment 03: Toy car"
author: M. MAD
pic: img/p/07-displays.png
name: "آزمایش 03: پیاده‌سازی خودروی اسباب‌بازی ساده"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="english-text">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - >
    نصب و استفاده از کتابخانه‌های شخص ثالث خارجی در برنامه
    <span class="english-text">Arduino IDE</span>
  - بارگذاری برنامه‌های پیش‌ساخته روی خانواده بردهای توسعه آردوینو
  - کار با ورودی‌های دیجیتال دریافتی از صفحه‌کلید خازنی
  - مدریریت حالات اجرای برنامه
  - >
    راه‌اندازی و کار کردن با نمایشگر ماتریس نقطه
    <span class="english-text">RGB LED (Dot-Matrix)</span>
  - کار با خروجی‌های دیجیتال
  - >
    یادگیری زمان‌بندی غیرانسدادی
    <span class="english-text">Non-blocking Timing</span>
ingredients:
  - یک دستگاه رایانه
  - تابلوی آردوینو اونو (برای برد توسعه آردوینو اونو) یا تابلوی آردوینو مگا (برای برد توسعه آردوینو مگا)
  - >
    کابل تبدیل
    <span class="english-text">USB Type-B</span>
    (پورت
    <span class="english-text">USB</span>
    روی بردهای توسعه آردوینو) به
    <span class="english-text">USB Type-A</span>
    (پورت
    <span class="english-text">USB</span>
    مرسوم در رایانه‌ها) برای بارگذاری برنامه روی بردهای توسعه آردوینو
  - تابلوی حسگرهای دوم (برای صفحه‌کلید خازنی)
  - >
    تابلوی نمایشگرها (برای نمایشگر ماتریس نقطه
    <span class="english-text">RGB LED 4x4</span>)
---
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند. در ادامه، کد
این آزمایش به صورت تکه تکه توضیح داده خواهد شد.
</p>
<h4>
توضیح کد آزمایش
</h4>
<p>
تکه کد زیر، کتابخانه‌ی FastLED را وارد می‌کند و پارامترهای سخت‌افزاری را تعریف می‌کند: تعداد LEDها (۴×۴ = ۱۶)، پین دیتا، نوع LED (WS2812B) و ترتیب رنگ (GRB). همچنین آرایه‌ی leds که وضعیت هر LED را نگه‌ می‌دارد ساخته می‌شود.
</p>
<div class="code">
/* Copyright 2025 M. MAD */

#include <FastLED.h>

// --- Configuration for LED Matrix ---
#define NUM_LEDS 16  // 4x4 LED matrix
#define DATA_PIN 6  // Pin connected to the LED data line
#define LED_TYPE WS2812B  // Change if you use a different LED type
#define COLOR_ORDER GRB  // Most WS2812B strips use GRB order

CRGB leds[NUM_LEDS];
</div>
<p>
اینجا نیز پین‌های ۴ دکمه تعریف شده‌اند (سبز، زرد، قرمز، و دکمه‌ی خاموش/فلاش). متغیرهای زمان برای فلاش (previousMillis و flashInterval) تعریف شده‌اند. سپس یک enum به نام Modes برای حالت‌ها ساخته شده و آرایه‌ی colors هم رنگ متناظر با هر حالت را نگه می‌دارد (حالت MODE_OFF = سیاه).
</p>
<div class="code">
// --- Button Pin Definitions ---
#define BUTTON_GREEN_PIN 2  // Button for green (all leds green)
#define BUTTON_YELLOW_PIN 3  // Button for yellow (all leds yellow)
#define BUTTON_RED_PIN 4  // Button for red (all leds red)
#define BUTTON_OFF_PIN 5  // Button for flashing mode

// --- Timing for flashing mode ---
unsigned long previousMillis = 0;
const unsigned long flashInterval = 500; // 500ms flashing interval

// --- Modes ---
enum Modes {
  MODE_GREEN = 0,
  MODE_YELLOW,
  MODE_RED,
  MODE_OFF,
  MODE_COUNT  // Helper to get number of normal modes
};

const CRGB colors[MODE_COUNT] = {
  CRGB::Green,  // MODE_GREEN
  CRGB::Yellow, // MODE_YELLOW
  CRGB::Red,    // MODE_RED
  CRGB::Black   // MODE_OFF
};
</div>
<p>
وضعیت فعلی (چه رنگ/حالت فعلی است)، و پرچم‌های مربوط به حالت چشمک‌زن (flashing) و وضعیت داخلی چشمک (flashState) نگهداری می‌شوند.
</p>
<div class="code">
Modes currentMode = MODE_OFF;

bool flashing = false; // true if the current mode is flashing
bool flashState = false; // internal state to toggle between on and off for flash
</div>
<p>
FastLED را مقداردهی می‌کند، پین‌های دکمه را به صورت INPUT_PULLUP تنظیم می‌کند (یعنی دکمه‌ها فعال-پایین / active-low هستند) و در ابتدا همه LEDها را خاموش (Black) می‌گذارد.
</p>
<div class="code">
void setup() {
  // Initialize serial monitor for debugging.
  //Serial.begin(9600);
  
  // Initialize LED library
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();
  FastLED.show();
  
  // Configure button pins as inputs with internal pull-up resistors.
  pinMode(BUTTON_GREEN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RED_PIN, INPUT_PULLUP);
  pinMode(BUTTON_OFF_PIN, INPUT_PULLUP);
  
  // Start with off mode
  setAllLeds(CRGB::Black);
}
</div>
<p>
در حلقه‌ی اصلی، هر بار وضعیت چهار دکمه خوانده می‌شود (فعّال وقتی سطح LOW است). با فشردن هر دکمه، تابع handleModeButtonPress با حالت مربوط صدا زده می‌شود و ۲۰۰ میلی‌ثانیه تاخیربرای ضدنوسان (debounce) اعمال می‌شود. بعد از بررسی دکمه‌ها، اگر حالت چشمک‌زن فعال باشد و حالت فعلی MODE_OFF نباشد، updateFlashing() اجرا می‌شود تا وضعیت چشمک را مدیریت کند؛ در غیر این صورت فقط FastLED.show() اجرا می‌شود.
</p>
<div class="code">
void loop() {
  // Check buttons (active low; adjust if your capacitive setup acts differently)
  if (digitalRead(BUTTON_GREEN_PIN) == LOW) {
    handleModeButtonPress(MODE_GREEN);
    delay(200); // Debounce delay
  }
  else if (digitalRead(BUTTON_YELLOW_PIN) == LOW) {
    handleModeButtonPress(MODE_YELLOW);
    delay(200);
  }
  else if (digitalRead(BUTTON_RED_PIN) == LOW) {
    handleModeButtonPress(MODE_RED);
    delay(200);
  }
  else if (digitalRead(BUTTON_OFF_PIN) == LOW) {
    handleModeButtonPress(MODE_OFF);
    delay(200);
  }

  // Update flashing if needed.
  if (flashing && currentMode != MODE_OFF) {
    updateFlashing();
  }
  else {
    FastLED.show();
  }
}
</div>
<p>
منطق رفتار هنگام فشردن دکمه:
•	اگر دکمه‌ی همان حالت فعلی زده شود → حالت چشمک‌زن بین روشن/خاموش تغییر (toggle) می‌شود: اگر قبلاً چشمک نبود، شروع به چشمک می‌کند (و با flashState = true از حالت روشن آغاز می‌کند)، و اگر قبلاً چشمک می‌زد، چشمک را متوقف و رنگ ثابت را نمایش می‌دهد.
•	اگر دکمه‌ی حالت دیگری زده شود → وضعیت (currentMode) به آن حالت جدید تغییر می‌کند و نمایش بصورت ثابت (غیر چشمک‌زن) روی رنگ مربوطه قرار می‌گیرد.
نکته‌ریزی: اگر pressedMode == MODE_OFF باشد، colors[MODE_OFF] برابر CRGB::Black است؛ یعنی با زدن دکمه‌ی OFF، همه‌ی LEDها خاموش می‌شوند. همچنین اگر حالت فعلی MODE_OFF باشد و همان دکمه OFF را دوباره بزنید، تابع سعی می‌کند حالت flashing را فعال کند اما در حلقه‌ی اصلی (loop) شرط اجرای updateFlashing() فقط وقتی currentMode != MODE_OFF است اجرا خواهد شد — بنابراین چشمک‌زنی در حالت OFF عملاً اتفاقی نخواهد افتاد (جز اینکه flashing=true شود ولی به‌روزرسانی انجام نشود).
</p>
<div class="code">
void handleModeButtonPress(Modes pressedMode) {
  if (currentMode == pressedMode) {
    // The same mode button is pressed.
    if (!flashing) {
      // If not already flashing, start flashing.
      flashing = true;
      flashState = true; // Start with the color on
      previousMillis = millis();
    } else {
      // If it's already flashing, stop flashing and revert to solid.
      flashing = false;
      setAllLeds(colors[pressedMode]);
    }
  } else {
    // A different color button is pressed: update mode to that color, solid.
    currentMode = pressedMode;
    flashing = false;
    setAllLeds(colors[pressedMode]);
  }
}
</div>
<p>
این تابع با استفاده از millis() و previousMillis هر flashInterval میلی‌ثانیه (500ms) وضعیت flashState را معکوس می‌کند و بسته به آن یا رنگ حالت را نمایش می‌دهد یا همه را خاموش می‌کند — یعنی مسئول چشمک‌زدن است.
</p>
<div class="code">
// updateFlashing: toggle between the mode color and off.
void updateFlashing() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= flashInterval) {
    previousMillis = currentMillis;
    flashState = !flashState;
    if (flashState) {
      setAllLeds(colors[currentMode]);
    } else {
      setAllLeds(CRGB::Black);
    }
  }
}
</div>
<p>
یک تابع کمکی ساده که همه‌ی LEDها را به یک رنگ مشخص تنظیم کرده و سپس خروجی را با FastLED.show() می‌فرستد.
</p>
<div class="code">
// ---------------------------------------------------------------
// setAllLeds: Sets all LEDs in the matrix to the specified color.
void setAllLeds(const CRGB &color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}
</div>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<p>
</p>
