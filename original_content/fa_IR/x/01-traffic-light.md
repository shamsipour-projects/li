---
title: "آزمایش 01: پیاده‌سازی چراغ راهنمایی با قابلیت چشمک‌زن"
header: "آزمایش 01: پیاده‌سازی چراغ راهنمایی با قابلیت چشمک‌زن"
author: M. MAD
pic: img/01-traffic-light.png
name: "آزمایش 01: پیاده‌سازی چراغ راهنمایی با قابلیت چشمک‌زن"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="en">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - >
    نصب و استفاده از کتابخانه‌های شخص ثالث خارجی در برنامه
    <span class="en">Arduino IDE</span>
  - بارگذاری برنامه‌های پیش‌ساخته روی خانواده بردهای توسعه آردوینو
  - کار با ورودی‌های دیجیتال دریافتی از صفحه‌کلید خازنی
  - مدریریت حالات اجرای برنامه
  - >
    راه‌اندازی و کار کردن با نمایشگر ماتریس نقطه
    <span class="en">RGB LED (Dot-Matrix)</span>
  - کار با خروجی‌های دیجیتال
  - >
    یادگیری زمان‌بندی غیرانسدادی
    <span class="en">Non-blocking Timing</span>
ingredients:
  - یک دستگاه رایانه
  - >
    <a href="../h/p/01-arduino-uno.html">تابلوی آردوینو اونو</a>
    (برای
    <a href="../h/m/01-arduino-uno.html">برد توسعه آردوینو اونو</a>)
    یا
    <a href="../h/p/02-arduino-mega.html">تابلوی آردوینو مگا</a>
    (برای
    <a href="../h/m/02-arduino-mega.html">برد توسعه آردوینو مگا</a>)
  - >
    کابل تبدیل
    <span class="en">USB Type-B</span>
    (پورت
    <span class="en">USB</span>
    روی بردهای توسعه آردوینو) به
    <span class="en">USB Type-A</span>
    (پورت
    <span class="en">USB</span>
    مرسوم در رایانه‌ها) برای بارگذاری برنامه روی بردهای توسعه آردوینو
  - >
    <a href="../h/p/04-sonsors-II.html">تابلوی حسگرهای دوم</a>
    (برای صفحه‌کلید خازنی)
  - >
    <a href="../h/p/07-displays.html">تابلوی نمایشگرها</a>
    (برای نمایشگر ماتریس نقطه
    <span class="en">RGB LED 4x4</span>)
---
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند. در ادامه، کد
این آزمایش به صورت تکه تکه توضیح داده خواهد شد.
</p>
<h4>
توضیح کد آزمایش
</h4>
<pre>
تکه کد زیر، کتابخانه‌ی FastLED را وارد می‌کند و پارامترهای سخت‌افزاری را تعریف می‌کند: تعداد LEDها (۴×۴ = ۱۶)، پین دیتا، نوع LED (WS2812B) و ترتیب رنگ (GRB). همچنین آرایه‌ی leds که وضعیت هر LED را نگه‌ می‌دارد ساخته می‌شود.
</pre>
<code lang="arduino">
/* Copyright 2025 M. MAD */

#include <FastLED.h>

// --- Configuration for LED Matrix ---
#define NUM_LEDS 16  // 4x4 LED matrix
#define DATA_PIN 6  // Pin connected to the LED data line
#define LED_TYPE WS2812B  // Change if you use a different LED type
#define COLOR_ORDER GRB  // Most WS2812B strips use GRB order

CRGB leds[NUM_LEDS];
</code>
<pre>
اینجا نیز پین‌های ۴ دکمه تعریف شده‌اند (سبز، زرد، قرمز، و دکمه‌ی خاموش/فلاش). متغیرهای زمان برای فلاش (previousMillis و flashInterval) تعریف شده‌اند. سپس یک enum به نام Modes برای حالت‌ها ساخته شده و آرایه‌ی colors هم رنگ متناظر با هر حالت را نگه می‌دارد (حالت MODE_OFF = سیاه).
</pre>
<code lang="arduino">
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
</code>
<pre>
وضعیت فعلی (چه رنگ/حالت فعلی است)، و پرچم‌های مربوط به حالت چشمک‌زن (flashing) و وضعیت داخلی چشمک (flashState) نگهداری می‌شوند.
</pre>
<code lang="arduino">
Modes currentMode = MODE_OFF;

bool flashing = false; // true if the current mode is flashing
bool flashState = false; // internal state to toggle between on and off for flash
</code>
<pre>
FastLED را مقداردهی می‌کند، پین‌های دکمه را به صورت INPUT_PULLUP تنظیم می‌کند (یعنی دکمه‌ها فعال-پایین / active-low هستند) و در ابتدا همه LEDها را خاموش (Black) می‌گذارد.
</pre>
<code lang="arduino">
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
</code>
<pre>
در حلقه‌ی اصلی، هر بار وضعیت چهار دکمه خوانده می‌شود (فعّال وقتی سطح LOW است). با فشردن هر دکمه، تابع handleModeButtonPress با حالت مربوط صدا زده می‌شود و ۲۰۰ میلی‌ثانیه تاخیربرای ضدنوسان (debounce) اعمال می‌شود. بعد از بررسی دکمه‌ها، اگر حالت چشمک‌زن فعال باشد و حالت فعلی MODE_OFF نباشد، updateFlashing() اجرا می‌شود تا وضعیت چشمک را مدیریت کند؛ در غیر این صورت فقط FastLED.show() اجرا می‌شود.
</pre>
<code lang="arduino">
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
</code>
<pre>
منطق رفتار هنگام فشردن دکمه:
•	اگر دکمه‌ی همان حالت فعلی زده شود → حالت چشمک‌زن بین روشن/خاموش تغییر (toggle) می‌شود: اگر قبلاً چشمک نبود، شروع به چشمک می‌کند (و با flashState = true از حالت روشن آغاز می‌کند)، و اگر قبلاً چشمک می‌زد، چشمک را متوقف و رنگ ثابت را نمایش می‌دهد.
•	اگر دکمه‌ی حالت دیگری زده شود → وضعیت (currentMode) به آن حالت جدید تغییر می‌کند و نمایش بصورت ثابت (غیر چشمک‌زن) روی رنگ مربوطه قرار می‌گیرد.
نکته‌ریزی: اگر pressedMode == MODE_OFF باشد، colors[MODE_OFF] برابر CRGB::Black است؛ یعنی با زدن دکمه‌ی OFF، همه‌ی LEDها خاموش می‌شوند. همچنین اگر حالت فعلی MODE_OFF باشد و همان دکمه OFF را دوباره بزنید، تابع سعی می‌کند حالت flashing را فعال کند اما در حلقه‌ی اصلی (loop) شرط اجرای updateFlashing() فقط وقتی currentMode != MODE_OFF است اجرا خواهد شد — بنابراین چشمک‌زنی در حالت OFF عملاً اتفاقی نخواهد افتاد (جز اینکه flashing=true شود ولی به‌روزرسانی انجام نشود).
</pre>
<code lang="arduino">
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
</code>
<pre>
این تابع با استفاده از millis() و previousMillis هر flashInterval میلی‌ثانیه (500ms) وضعیت flashState را معکوس می‌کند و بسته به آن یا رنگ حالت را نمایش می‌دهد یا همه را خاموش می‌کند — یعنی مسئول چشمک‌زدن است.
</pre>
<code lang="arduino">
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
</code>
<pre>
یک تابع کمکی ساده که همه‌ی LEDها را به یک رنگ مشخص تنظیم کرده و سپس خروجی را با FastLED.show() می‌فرستد.
</pre>
<code lang="arduino">
// ---------------------------------------------------------------
// setAllLeds: Sets all LEDs in the matrix to the specified color.
void setAllLeds(const CRGB &color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();
}
</code>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<pre>
</pre>
