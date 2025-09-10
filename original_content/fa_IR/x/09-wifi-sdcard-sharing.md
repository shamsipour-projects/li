---
title: "Experiment 09: WiFi SD Card sharing"
header: "Experiment 09: WiFi SD Card sharing"
author: M. MAD
pic: img/09-wifi-sdcard-sharing.png
name: "آزمایش 07: اشتراک‌گذاری پرونده‌ها از طریق WiFi Hot Spot"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="en">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - >
    خوانش اطلاعات از 
    <span class="en">SD Card / MicroSD Card</span>
  - >
    راه‌اندازی اتصال
    <span class="en">WiFi</span>
    از طریق ماژول
    <span class="en">NodeMCU</span>
    و میزبانی یک
    <span class="en">Hot Spot</span>
  - >
    مدیریت و کنترل ماژول
    <span class="en">NodeMCU</span>
    توسط ماژول آردوینو اونو یا ماژول آردوینو مگا و انتقال پرونده‌ها میان آنها
  - >
    به‌اشتراک‌گذاری اطلاعات کارت حافظه از طریق میزبانی یک وبگاه ساده با دستگاه‌هایی که به
    <span class="en">WiFi Hot Spot</span>
    متصل شده‌اند
ingredients:
  - یک دستگاه رایانه
  - >
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
    <a href="../h/p/06-sensors-IV.html">تابلوی حسگرهای چهارم</a>
    برای ماژول خواندن از کارت حافظه
  - >
    <a href="../h/p/08-telecom.html">تابلوی مخابراتی</a>
    (برای ماژول
    <span class="en">NodeMCU</span>)
---
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند. در ادامه، کد
این آزمایش به صورت تکه تکه توضیح داده خواهد شد.
</p>
<h4>
توضیح کد آزمایش
</h4>
<pre>
برای انجام این آزمایش هر دو ماژول آردوینو (اونو یا مگا) و
<span class="en">NodeMCU</span>
را برنامه‌ریزی می‌کنیم تا با یکدیگر کار کنند. البته می‌توان این آزمایش را تماما
با به‌کارگیری ماژول
<span class="en">NodeMCU</span>
و بدون استفاده از ماژول‌های آردوینو نیز انجام داد اما ما در این آزمایش قصد
داشتیم تا ارتباط دو ریزمهارگر با یکدیگر و مدیریت همکاری میان آنها را نیز
بیاوریم و به شما نشان دهیم.
</pre>
<pre>
از ماژول
<span class="en">NodeMCU</span>
برای راه‌اندازی قابلیت وایفای و از ماژول آردوینو برای خواندن پرونده از کارت
حافظه و تحویل آن به ماژول
<span class="en">NodeMCU</span>
استفاده کرده‌ایم. ماژول آردوینو مگا را به این دلیل به کار برده‌ایم که حافظه
بیشتری دارد و می‌تواند پرونده‌های بزرگ‌تری را در حافظه خود نگه دارد و آنها را
راحت‌تر و سریع‌تر انتقال دهد.
</pre>
<pre>
کدهای مربوط به ماژول آردوینو مگا و ماژول
<span class="en">NodeMCU</span>
را به‌صورت جداگانه در ادامه بررسی خواهیم نمود.
</pre>
<h5>
کد آردوینو مگا
</h5>
<pre>
در کد زیر (سربرگ‌ها و تعریف‌ها):
•	#include <SPI.h> رابط سخت‌افزاری SPI را فعال می‌کند.
•	#include <SD.h> کتابخانه کار با کارت SD را فراهم می‌کند.
•	chipSelect = 53 پین Chip-Select برای ماژول SD روی Arduino Mega تعریف شده — این مقدار باید با سخت‌افزار (ماژول SD) مطابقت داشته باشد.
</pre>
<code lang="arduino">
#include <SPI.h>
#include <SD.h>

const int chipSelect = 53;
</code>
<pre>
در کد زیر (تابع راه‌اندازی):
•	Serial1.begin(115200) پورت سریال سخت‌افزاری شماره 1 را با نرخ 115200bps برای ارتباط با NodeMCU (ESP) راه‌اندازی می‌کند.
•	while (!Serial1); منتظر برقرار شدن اتصال سریال می‌ماند — رفتار این شرط روی Mega ممکن است غیرضروری یا مسدودکننده باشد؛ توضیح و پیشنهاد در بخش نکات.
•	Serial.begin(9600) برای لاگ محلی (USB) به کامپیوتر فعال می‌شود.
•	SD.begin(chipSelect) اقدام به مقداردهی ماژول SD می‌کند؛ در صورت شکست پیام خطا "ERROR:SD_FAIL" از طریق Serial1 (به ESP) ارسال می‌شود و تابع setup() بازمی‌گردد.
•	در صورت موفقیت پیام "READY" روی Serial1 ارسال می‌شود تا ESP از آماده‌بودن Mega آگاه شود.
نکته مهم: ارسال "READY" راهکار مناسبی برای handshaking است — ESP می‌تواند منتظر "READY" بماند و پس از آن REQUEST_FILE بفرستد.
</pre>
<code lang="arduino">
void setup() {
  Serial1.begin(115200); // Communication with NodeMCU
  while (!Serial1); // Wait for serial connection
  
  Serial.begin(9600); // For debugging
  Serial.println("Initializing SD card...");
  
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    Serial1.println("ERROR:SD_FAIL");
    return;
  }
  Serial.println("SD card initialized.");
  Serial1.println("READY");
}
</code>
<pre>
در کد زیر (تابع حلقه):
•	Serial1.available() بررسی می‌کند آیا بایت‌های جدیدی از NodeMCU رسیده‌اند.
•	readStringUntil('\n') یک خط متنی را تا newline می‌خواند و در command (از نوع String) قرار می‌دهد؛ trim() فضای اضافی/CR/LF را حذف می‌کند.
•	اگر command == "REQUEST_FILE" باشد، تابع sendFileToNodeMCU("data.txt") فراخوانی می‌شود تا فایل data.txt خوانده و به ESP ارسال شود.
</pre>
<code lang="arduino">
void loop() {
  // Check if NodeMCU is requesting a file
  if (Serial1.available()) {
    String command = Serial1.readStringUntil(‘\n’);
    command.trim();
    
    if (command == “REQUEST_FILE”) {
      sendFileToNodeMCU(“data.txt”); // Change to your filename
    }
  }
}
</code>
<pre>
در کد زیر (تابع باز کردن و ارسال پرونده):
•	SD.open(filename) تلاش می‌کند فایل مشخص شده را باز کند؛ اگر باز نشود (!file) پیام خطا "ERROR:FILE_NOT_FOUND" روی Serial1 ارسال شده و تابع خاتمه می‌یابد.
•	در صورت باز بودن فایل، ابتدا "FILE_START" ارسال می‌شود.
•	حلقه while (file.available()) تا انتهای فایل تکرار شده، هر بار یک خط با readStringUntil('\n') خوانده و با Serial1.println(line) فرستاده می‌شود.
•	در پایان "FILE_END" ارسال و فایل بسته می‌شود؛ لاگ روی Serial چاپ می‌شود.
</pre>
<code lang="arduino">
void sendFileToNodeMCU(String filename) {
  File file = SD.open(filename);
  if (!file) {
    Serial1.println("ERROR:FILE_NOT_FOUND");
    Serial.println("Error opening file: " + filename);
    return;
  }
  
  Serial1.println("FILE_START");
  while (file.available()) {
    String line = file.readStringUntil('\n');
    Serial1.println(line);
  }
  Serial1.println("FILE_END");
  
  file.close();
  Serial.println("File sent to NodeMCU");
}
</code>
<h5>
کد
<span class="en">NodeMCU</span>
</h5>
<pre>
در کد زیر (سربرگ‌ها، متغیرها و شیء سرور):
•	ESP8266WiFi.h و ESP8266WebServer.h برای ایجاد Access Point و وب‌سرور استفاده می‌شوند.
•	server(80) یک وب‌سرور HTTP روی پورت 80 می‌سازد.
•	fileContent رشته‌ای است که محتوای دریافتی از Arduino را نگه‌می‌دارد.
•	ssid و password نام و رمز شبکه AP ای که ESP ایجاد می‌کند هستند.
</pre>
<code lang="arduino">
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
String fileContent = “”;

// WiFi credentials
const char* ssid = “FileServerAP”;
const char* password = “12345678”;
</code>
<pre>
در کد زیر (تابع راه‌اندازی):
•	Serial.begin(115200) پورت سریال را برای تبادل پیغام با Arduino Mega راه‌اندازی می‌کند.
•	WiFi.softAP(ssid, password) یک Access Point محلی با SSID و پسورد تعیین‌شده می‌سازد؛ WiFi.softAPIP() آدرس آی‌پی AP را چاپ می‌کند.
•	مسیرهای HTTP "/" و "/download" به توابع handleRoot و handleDownload وصل می‌شوند؛ سپس سرور با server.begin() فعال می‌شود.
•	در انتهای setup() تابع requestFileFromArduino() فراخوانی می‌شود که یک پیام REQUEST_FILE روی سریال چاپ می‌کند تا Arduino فایل را بفرستد.
</pre>
<code lang="arduino">
void setup() {
  Serial.begin(115200); // Communication with Arduino Mega
  
  // Create WiFi Access Point
  WiFi.softAP(ssid, password);
  
  Serial.println("");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  // Set up web server endpoints
  server.on("/", handleRoot);
  server.on("/download", handleDownload);
  
  server.begin();
  Serial.println("HTTP server started");
  
  // Request file from Arduino
  requestFileFromArduino();
}
</code>
<pre>
در کد زیر (تابع حلقه اصلی):
•	server.handleClient() اتصالات HTTP ورودی را هندل می‌کند و توابع ثبت‌شده (handleRoot, handleDownload) را برای درخواست‌ها اجرا می‌کند.
•	این loop کوتاه و سریع است؛ خواندن سریال در serialEvent() انجام شده است (بلوک بعدی).
</pre>
<code lang="arduino">
void loop() {
  server.handleClient();
}
</code>
<pre>
در کد زیر (تابع ارسال درخواست به آردوینو):
•	این تابع یک خط متنی "REQUEST_FILE\n" روی پورت سریال ارسال می‌کند؛ Arduino Mega آن را می‌خواند و پاسخ می‌دهد.
•	در طراحی فعلی این فراخوان فقط یک‌بار در setup() اجرا می‌شود.
</pre>
<code lang="arduino">
void requestFileFromArduino() {
  Serial.println("REQUEST_FILE"); // Send request to Arduino
}
</code>
<pre>
در کد زیر (تابع صفحه اصلی کارساز):
•	handleRoot() یک صفحه HTML ساده می‌سازد و با server.send(200, "text/html", html) به مرورگر می‌فرستد. لینک /download برای دانلود فایل قرار داده شده است.
</pre>
<code lang="arduino">
void handleRoot() {
  String html = "<html><head><title>File Server</title></head>";
  html += "<body><h1>File Server</h1>";
  html += "<pre>Click <a href='/download'>here</a> to download the file.</pre>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}
</code>
<pre>
در کد زیر (تابع ارسال محتوا برای دانلود):
•	در مسیر /download محتویات fileContent (که توسط serialEvent() پر می‌شود) با هدر Content-Type: text/plain به پاسخ‌دهنده ارسال می‌گردد.
</pre>
<code lang="arduino">
void handleDownload() {
  server.send(200, "text/plain", fileContent);
}
</code>
<pre>
در کد زیر (تابع رویداد سریال):
•	serialEvent() زمانی که داده سریال موجود باشد اجرا می‌شود (در غالب پیاده‌سازی‌های core بعد از loop() فراخوانی می‌شود).
•	حلقه while (Serial.available()) تا زمانی که بایت در بافر سریال باشد، خطوط را با readStringUntil('\n') خوانده و trim() می‌کند.
•	اگر خط برابر "FILE_START" بود، fileContent پاک می‌شود تا از صفر دوباره جمع‌آوری شود.
•	اگر خط برابر "FILE_END" بود، انتقال فایل تمام شده (در کد فعلی هیچ ACK یی فرستاده نمی‌شود).
•	پیام‌های خطا ERROR:SD_FAIL و ERROR:FILE_NOT_FOUND به متن خطا در fileContent نگاشت می‌شوند.
•	خطوط داده (محتوای فایل) در غیر از موارد فوق، به fileContent اضافه می‌شوند و پس از هر خط \n هم ذخیره می‌شود.
</pre>
<code lang="arduino">
void serialEvent() {
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    
    if (line == "FILE_START") {
      fileContent = "";
    } else if (line == "FILE_END") {
      // File transmission complete
    } else if (line == "ERROR:SD_FAIL") {
      fileContent = "Error: SD card initialization failed";
    } else if (line == "ERROR:FILE_NOT_FOUND") {
      fileContent = "Error: File not found on SD card";
    } else {
      fileContent += line + "\n";
    }
  }
}
</code>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<pre>
</pre>
