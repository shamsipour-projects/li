---
title: "آزمایش 10: سامانه امنیتی با قابلیت ارسال پیامک‌های اضطراری"
header: "آزمایش 10: سامانه امنیتی با قابلیت ارسال پیامک‌های اضطراری"
author: M. MAD
pic: img/10-smart-home-emergency-sms.png
name: "آزمایش 10: سامانه امنیتی با قابلیت ارسال پیامک‌های اضطراری"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="en">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - >
    خوانش اطلاعات دیجیتال از
    <span class="en">TTP223</span>
    (حسگر لمس خازنی)
  - >
    خوانش ورودی‌های آنالوگ و دیجیتال از
    <span class="en">Joystick</span>
  - >
    راه‌اندازی و خوانش اطلاعات دما از حسگر
    <span class="en">DHT22</span>
    (حسگر دما و رطوبت محیط)
  - >
    راه‌اندازی و خوانش اطلاعات میزان گاز هیدروژن در هوا از حسگر
    <span class="en">MQ8</span>
    (حسگر گاز هیدروژن)
  - >
    راه‌اندازی و خوانش اطلاعات نور محیط از حسگر
    <span class="en">GY-30</span>
    (حسگر شدت نور، لوکس متر)
  - >
    راه‌اندازی و خوانش اطلاعات وزن/فشار از حسگر
    <span class="en">HX711 + Loadcell</span>
    (حسگر وزن/فشار)
  - >
    راه‌اندازی و خوانش اطلاعات فاصله از حسگر فاصله‌سنج فروسرخی
    <span class="en">GP2Y0E03</span>
  - >
    راه‌اندازی و ارسال پیامک‌های اضطراری با ماژول
    <span class="en">SIM800</span>
  - >
    راه‌اندازی و نمایش اطلاعات روی نمایشگر
    <span class="en">LCD 1602</span>
    ماتریس نقطه
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
    <a href="../h/p/03-sonsors-I.html">تابلوی حسگرهای یکم</a>
    (برای
    <span class="en">TTP223 + Ralay + LED</span>،
    حسگر دما و رطوبت محیط
    <span class="en">DHT22</span>،
    حسگر وزن/فشار
    <span class="en">HX711 + Loadcell</span>
    و حسگر گاز هیدروژن
    <span class="en">MQ8</span>)
  - >
    <a href="../h/p/04-sonsors-II.html">تابلوی حسگرهای دوم</a>
    (برای ماژول
    <span class="en">Joystick</span>
    و حسگر شدت نور
    <span class="en">GY-30</span>)
  - >
    <a href="../h/p/06-sensors-IV.html">تابلوی حسگرهای چهارم</a>
    (برای حسگر فاصله‌سنج فروسرخی
    <span class="en">GP2Y0E03</span>)
  - >
    <a href="../h/p/07-displays.html">تابلوی نمایشگرها</a>
    (برای نمایشگر
    <span class="en">LCD 1602</span>
    ماتریس نقطه)
  - >
    <a href="../h/p/08-telecom.html">تابلوی مخابراتی</a>
    (برای ماژول
    <span class="en">SIM800</span>)
---
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند. در ادامه، کد
این آزمایش به صورت تکه تکه توضیح داده خواهد شد.
</p>
<h4>
توضیح کد آزمایش
</h4>
<pre>
کد این آزمایش برپایه آزمایش 6 است و در اینجا صرفا تغییرات آن را بررسی می‌کنیم.
</pre>
<h5>
افزودن حسگر فاصله‌سنج فروسرخی
<span class="en">GP2Y0E03</span>
</h5>
<pre>
کد زیر تعاریف پین / آدرس I²C را انجام می‌دهد.
</pre>
<code lang="arduino">
#define GP2Y0E03_SDA A4
#define GP2Y0E03_SCL A5
#define GP2Y0E03_ADDRESS 0x40
</code>
<pre>
تابع readGP2Y0E03Distance هم با استفاده از I²C رجیستر 0x5E را می‌نویسد، دو بایت می‌خواند، بایت‌ها را ترکیب و خروجی را بر حسب cm برمی‌گرداند.
متغیرهای جدید برای زمان‌بندیِ چک فاصله، فلاشِ فلگِ ارسال SMS و آستانه هشدار اضافه شده‌اند.
</pre>
<code lang="arduino">
float distance = 0;
bool smsSent = false;
unsigned long lastDistanceCheck = 0;
const unsigned long distanceCheckInterval = 500;
const float DISTANCE_THRESHOLD = 20.0;
</code>
<pre>
کد زیر نیز دربردارنده فراخوانی دوره‌ای خواندن فاصله در loop() و منطق ارسال SMS یک‌بار برای هر رخداد می‌شود.
</pre>
<code lang="arduino">
if (millis() - lastDistanceCheck >= distanceCheckInterval) {
  distance = readGP2Y0E03Distance();
  if (distance > 0 && distance < DISTANCE_THRESHOLD && !smsSent) {
    sendEmergencySMS(distance);
    smsSent = true;
  } else if (distance >= DISTANCE_THRESHOLD) {
    smsSent = false;
  }
}
</code>
<h5>
افزودن ماژول
<span class="en">GSM (SIM800L)</span>
</h5>
<pre>
تعاریف جدید پین و شیء سریال نرم‌افزاری (ارتباط با مودم GSM از طریق SoftwareSerial انجام می‌شود):
</pre>
<code lang="arduino">
#define SIM800_TX_PIN 6
#define SIM800_RX_PIN 7
SoftwareSerial sim800l(SIM800_TX_PIN, SIM800_RX_PIN);
</code>
<pre>
راه‌اندازی مودم در تابع راه‌اندازی و دستورات AT برای فعال شدن حالت ارسال پیامک:
</pre>
<code lang="arduino">
sim800l.begin(9600);
sim800l.println("AT");
sim800l.println("AT+CMGF=1");
sim800l.println("AT+CNMI=1,2,0,0,0");
</code>
<pre>
و افزودن تابع جدید sendEmergencySMS که متن SMS را می‌سازد، آن را از طریق AT+CMGS می‌فرستد و با sim800l.write(26) (Ctrl+Z) پیامک را ارسال می‌کند.
</pre>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<pre>
</pre>
