---
title: "Experiment 04: Toy car with respect to traffic light"
header: "Experiment 04: Toy car with respect to traffic light"
author: M. MAD
pic: img/04-toy-car-with-respect-to-traffic-light.png
name: "آزمایش 04: پیاده‌سازی خودروی اسباب‌بازی با احترام به چراغ راهنمایی"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="en">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - >
    خوانش ورودی‌های آنالوگ و دیجیتال از
    <span class="en">Joystick</span>
  - >
    راه‌اندازی و ارسال فرامین و پالس‌های
    <span class="en">PWM</span>
    به درایور موتور
    <span class="en">L298N</span>
    (پل
    <span class="en">H</span>)
  - کنترل همزمان دو موتور الکتریکی با دو روش متفاوت
  - >
    تشخیص رنگ با حسگر
    <span class="en">TCS3200</span>
    و احترام به چراغ راهنمایی
ingredients:
  - یک دستگاه رایانه
  - >
    <a href="../h/p/01-arduino-uno.html">تابلوی آردوینو اونو</a>
    (برای پیاده‌سازی چراغ راهنمایی با
    <a href="../h/m/01-arduino-uno.html">برد توسعه آردوینو اونو</a>)
    و
    <a href="../h/p/02-arduino-mega.html">تابلوی آردوینو مگا</a>
    (برای پیاده‌سازی پروژه اصلی با
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
    (برای ماژول
    <span class="en">Joystick</span>
    و حسگر
    <span class="en">TCS3200</span>)
  - >
    <a href="../h/p/05-sensors-III.html">تابلوی حسگرهای سوم</a>
    (برای موتور و درایور موتور
    <span class="en">L298N</span>)
---
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند. در ادامه، کد
این آزمایش به صورت تکه تکه توضیح داده خواهد شد.
</p>
<h4>
توضیح کد آزمایش
</h4>
<pre>
در کد زیر (سربرگ و کلان‌دستورهای دیباگ):
•  ماکروی ENABLE_DEBUG تعیین می‌کند آیا امکانات دیباگ فعال شوند یا نه. مقدار فعلی 0 است؛ بنابراین تمام ماکروهای DEBUG_* به تعاریف خالی نگاشته می‌شوند و هیچ‌کد دیباگی اجرا نخواهد شد.
•  زمانی که ENABLE_DEBUG برابر 1 شود:
•	DEBUG_SETUP(baud) پین DEBUG_PIN را با INPUT_PULLUP و LED داخلی (DEBUG_LED) را با OUTPUT پیکربندی و سریال را با باودریت baud راه‌اندازی می‌کند.
•	DEBUG_PRINT و DEBUG_PRINTLN فراخوانی‌های سریال را همراه با Serial.flush() انجام می‌دهند.
•	ماکروی DEBUG(x) تنها وقتی عملیات داخل x را اجرا می‌کند که پین DEBUG_PIN در وضعیت فعال (LOW به دلیل pull-up) باشد؛ در طول اجرای x LED داخلی روشن می‌شود تا نشانگر فیزیکیِ چاپِ سریال قابل مشاهده باشد.
</pre>
<code lang="arduino">
#define ENABLE_DEBUG 0
#if ENABLE_DEBUG                                                                                                                                    
  #define DEBUG_PIN 12
  #define DEBUG_LED LED_BUILTIN
  #define DEBUG_SETUP(baud) do { \
    pinMode(DEBUG_PIN, INPUT_PULLUP); \
    pinMode(DEBUG_LED, OUTPUT); \
    Serial.begin(baud); \
  } while(0)

  #define DEBUG_PRINT(x) do {Serial.print(x); Serial.flush();} while(0)
  #define DEBUG_PRINTLN(x) do {Serial.println(x); Serial.flush();} while(0)
  #define DEBUG_FLUSH() Serial.flush()
  #define DEBUG(x) do { \
    if (!digitalRead(DEBUG_PIN)) { \
      digitalWrite(DEBUG_LED, HIGH); \
      x; \
      Serial.flush(); \
      digitalWrite(DEBUG_LED, LOW); \
    } \
  } while(0)
#else
  #define DEBUG_SETUP(baud)
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_FLUSH()
  #define DEBUG(x)
#endif
</code>
<pre>
در کد زیر (تعریف نوع رنگ‌ها):
•	enum class Colors نوع شمارشیِ امنِ نام (scoped enum) را تعریف می‌کند که مقادیر Colors::RED، Colors::GREEN و Colors::BLUE را دارد. استفاده از : unsigned char اندازه حافظه را به یک بایت محدود می‌کند.
•	دلیل استفاده از enum class این است که نام اعضا وارد فضای نام سراسری نمی‌شوند و هنگام فراخوانی تابع readColor(Colors color) نام عضو دقیق (Colors::RED) لازم است — خطایابی آسان‌تر و برخورد نام کمتر رخ می‌دهد.
</pre>
<code lang="arduino">
enum class Colors : unsigned char { RED, GREEN, BLUE };
</code>
<pre>
در کد زیر هم •  DEBOUNCE_DELAY مقدار ۲۰ میلی‌ثانیه است که در انتهای حلقه (loop()) برای پایدارسازی خوانش‌ها استفاده می‌شود.
</pre>
<code lang="arduino">
#define DEBOUNCE_DELAY 20  // Small delay to stabilize readings
</code>
<pre>
در کد زیر (جوی‌استیک):
•	آنالوگ JOY_Y برای کنترل رانش (propulsion) و JOY_X برای کنترل فرمان (steering) استفاده می‌شود. پین دیجیتال JOY_BUTTON با INPUT_PULLUP پیکربندی شده است (فعال-پایین — active-low).
•	متغیرهای yVal و xVal مقادیر خوانده شده از مبدل آنالوگ-دیجیتال را نگه می‌دارند. مقدار اولیه 512 نقطه مرکز (نیمِ مقدار دامنه 0..1023) را نمایش می‌دهد.
•	تابع readJoyStick() مقدارهای آنالوگ را در هر تکرار به‌روز می‌کند.
</pre>
<code lang="arduino">
#define JOY_Y A0  // Joystick Y-axis analog input
#define JOY_X A1  // Joystick X-axis analog input
#define JOY_BUTTON 2  // Joystick button (digital input)

int yVal = 512;
int xVal = 512;
int prevX = 512;  // Previous X position (centered)

void joySetup() {
  // Analog inputs do not need any setup
  pinMode(JOY_BUTTON, INPUT_PULLUP);
}

void readJoyStick() {
  yVal = analogRead(JOY_Y);  // Propulsion control
  xVal = analogRead(JOY_X);  // Steering control
}
</code>
<pre>
در کد زیر (سنسور رنگ):
•	پین‌های S0، S1، S2 و S3 مربوط به ماژول TCS3200 هستند و OUT خروجی پالسِ فرکانسِ مربوط به شدت رنگ را دریافت می‌کند.
•	colorSensorSetup() پین‌های S0 و S1 را طوری تنظیم می‌کند که «فریکونسی اسکِیلینگ» روی ۲۰٪ قرار گیرد (نظرِ کد: digitalWrite(S0, HIGH); digitalWrite(S1, LOW);).
•	تابع readColor(Colors color) ابتدا فیلترِ رنگ را با تنظیم S2 و S3 انتخاب می‌کند و سپس با pulseIn(OUT, LOW) زمان پیکسیلی که خروجی LOW می‌ماند را می‌خواند. بر مبنای کامنتِ کد، «مقدار کمترِ بازگشتی = شدت بیشترِ رنگِ مورد نظر».
•	تابع checkRedLight() مقدار قرمز و مقدار سبز را خوانده و سپس طبق قاعده (red < RED_THRESHOLD) && (red < green * 0.7) تصمیم می‌گیرد که آیا چراغ قرمز تشخیص داده شده یا نه. یعنی قرمز باید هم از آستانه مطلق (RED_THRESHOLD) کمتر و هم از 70٪ مقدارِ سبز کمتر باشد — این کار کمک می‌کند از تشخیص اشتباه در شرایط نور محیطی جلوگیری شود.
•	تابع colorCheck() هر COLOR_CHECK_INTERVAL میلی‌ثانیه یک‌بار (۲۰۰ms) فراخوانده می‌شود و در صورت تشخیص قرمز (isRedDetected == true) توابع propulsionStop() و steeringStop() را اجرا می‌کند و در صورت تازه‌بودنِ تشخیص پیام دیباگ چاپ می‌کند.
•	نکته مهم فنی: pulseIn() یک فراخوان blocking است (تا زمانی که پالس دریافت شود یا timeout). بنابراین اگر خواندن رنگ زمان‌بر شود، پاسخ‌دهیِ موتور/فرمان تحت‌تأثیر قرار می‌گیرد.
</pre>
<code lang="arduino">
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define OUT 11

#define RED_THRESHOLD 50    // Calibrate for your environment
#define GREEN_THRESHOLD 50  // Calibrate for your environment
#define COLOR_CHECK_INTERVAL 200  // Check for traffic light every 200ms

bool isRedDetected = false;
bool prevColorDetection = false;
unsigned long lastColorCheck = 0;

void colorSensorSetup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
  
  // Set frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
}

// Read color intensity
int readColor(Colors color) {
  switch(color) {
    case Colors::RED: // Red filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, LOW);
      break;
    case Colors::GREEN: // Green filter
      digitalWrite(S2, HIGH);
      digitalWrite(S3, HIGH);
      break;
    case Colors::BLUE: // Blue filter
      digitalWrite(S2, LOW);
      digitalWrite(S3, HIGH);
      break;
  }
  // Return pulse width (lower value = more color detected)
  return pulseIn(OUT, LOW);
}

// Check for red traffic light
bool checkRedLight() {
  int red = readColor(Colors::RED);
  int green = readColor(Colors::GREEN);
  
  DEBUG_PRINT("R: ");
  DEBUG_PRINT(red);
  DEBUG_PRINT(" G: ");
  DEBUG_PRINTLN(green);
  
  // Red detected when red value is low and significantly lower than green
  return (red < RED_THRESHOLD) && (red < green * 0.7);
}

bool colorCheck() {
  if (millis() - lastColorCheck > COLOR_CHECK_INTERVAL) {
    isRedDetected = checkRedLight();
    
    lastColorCheck = millis();
    if (isRedDetected) {
      propulsionStop();
      steeringStop();
      if (isRedDetected != prevColorDetection) {  // if detection is new
        DEBUG_PRINTLN("RED LIGHT - STOPPED");
        prevColorDetection = isRedDetected;
      }
    }
  }
  return isRedDetected;
}
</code>
<pre>
در کد زیر (موتور پیشران):
•  PM1 و PM2 پین‌های جهتِ موتور پیش‌رانش هستند و PMPWM پین PWM جهت کنترل سرعت است.
•  propulsionMotorSetup() پین‌ها را خروجی می‌کند و propulsionStop() را اجرا می‌کند تا موتور در شروع سیستم قطع باشد.
•  handlePropulsion() مقدار yVal را با map(yVal, 0, 1023, 255, -255) به بازه 255 تا −255 نگاشت می‌دهد و بنابراین محور Y معکوس می‌شود (فشار به بالا یا پایین متنِ فیزیکیِ جوی‌استیک را به جلو/عقب نگاشت می‌کند).
•  آستانه PROP_MIN برای حذف منطقه مرده (dead zone) اعمال می‌شود و اگر مقدار مطلقِ propulsionPWM کمتر از PROP_MIN باشد، propulsionStop() اجرا می‌شود.
•  نکته منطقی مهم و قابل توجه (احتمالاً باگ یا رفتار ناخواسته): شرط بعدی تنها اجازه حرکت رو به جلو را وقتی می‌دهد که propulsionPWM > 0 و !isRedDetected باشد. اما در شاخه else بدون بررسی isRedDetected فراخوانی propulsionBackward(abs(propulsionPWM)) انجام می‌شود. نتیجه عملی:
•	وقتی چراغ قرمز (isRedDetected == true) وجود داشته باشد و کاربر propulsionPWM > 0 (در تلاش برای رفتن به جلو) داشته باشد، شرط propulsionPWM > 0 && !isRedDetected شکست می‌خورد و به ‌طور ناخواسته وارد شاخه else می‌شود و خودرو به عقب حرکت می‌کند (حرکت معکوس) — احتمالاً رفتار نامطلوب است.
</pre>
<code lang="arduino">
#define PM1 3  // Propulsion motor direction 1, L298N IN1 pin
#define PM2 4  // Propulsion motor direction 2, L298N IN2 pin
#define PMPWM 5  // Propulsion motor PWM, L298N ENA pin (PWM)

#define PROP_MIN 60  // Minimum propulsion PWM (dead zone threshold)

void propulsionMotorSetup() {
  pinMode(PM1, OUTPUT);
  pinMode(PM2, OUTPUT);
  pinMode(PMPWM, OUTPUT);
  propulsionStop();
}

// Propulsion motor control functions
void propulsionStop() {
  digitalWrite(PM1, LOW);
  digitalWrite(PM2, LOW);
  analogWrite(PMPWM, 0);  // Ensure the motor got disabled
}

void propulsionForward (unsigned char pwm = 255) {
  digitalWrite(PM1, HIGH);
  digitalWrite(PM2, LOW);
  analogWrite(PMPWM, pwm);
}

void propulsionBackward(unsigned char pwm = 255) {
  digitalWrite(PM1, LOW);
  digitalWrite(PM2, HIGH);
  analogWrite(PMPWM, pwm);
}

void handlePropulsion() {
  // Propulsion Motor Control (Y-axis)
  int propulsionPWM = map(yVal, 0, 1023, 255, -255);  // Invert Y-axis
  if (abs(propulsionPWM) < PROP_MIN) {  // Apply deadzone
    propulsionStop();
    DEBUG_PRINTLN(“PS”);
  } else if (propulsionPWM > 0 && !isRedDetected) {  // Respect the traffic light
    propulsionForward(abs(propulsionPWM));
    DEBUG_PRINT(“F=”);
    DEBUG_PRINTLN(abs(propulsionPWM));
  } else {
    propulsionBackward(abs(propulsionPWM));
    DEBUG_PRINT(“B=”);
    DEBUG_PRINTLN(abs(propulsionPWM));
  }
}
</code>
<pre>
در کد زیر (موتور فرمان):
•  و SM2 جهتِ موتور فرمان و SMPWM کنترل سرعت موتور فرمان هستند.
•  تابع handleSteering() از مشتقِ مقدار محور X استفاده می‌کند: deltaX = xVal - prevX. یعنی فرمان بر اساس سرعت حرکتِ دسته (جوی‌استیک) تولید می‌شود نه موقعیت ثابت دسته. تغییر سریعِ دسته فرمان قوی‌تر، تغییر آرام فرمان ضعیف‌تر.
•  مقدار steeringPWM با ضرب abs(deltaX) * 0.25 مقیاس داده می‌شود و سپس با constrain() در بازه [0, STEER_MAX] محدود می‌شود.
•  اگر steeringPWM کمتر از STEER_MIN باشد، فرمان متوقف (steeringStop()) می‌شود؛ در غیر این صورت جهت فرمان بر اساس علامت deltaX تعیین می‌شود (deltaX > 0 → راست، غیر آن → چپ).
•  نکته عملی: این روش باعث می‌شود اگر کاربر دسته را در یک زاویه ثابت نگه دارد، فرمان ثابتی تولید نشود — مناسب برای کنترلی که انتظار ورودی‌های لحظه‌ای را دارد.
</pre>
<code lang="arduino">
#define SM1 8  // Steering motor direction 1, L298N IN3 pin
#define SM2 7  // Steering motor direction 2, L298N IN4 pin
#define SMPWM 6  // Steering motor PWM, L298N ENB pin (PWM)

#define STEER_MIN 15  // Minimum steering PWM (dead zone threshold)
#define STEER_MAX 200  // Maximum steering PWM (prevent overdrive)

void steeringMotorSetup() {
  pinMode(SM1, OUTPUT);
  pinMode(SM2, OUTPUT);
  pinMode(SMPWM, OUTPUT);
  steeringStop();
}

// Steering motor control functions
void steeringStop() {
  digitalWrite(SM1, LOW);
  digitalWrite(SM2, LOW);
  analogWrite(SMPWM, 0);  // Ensure the motor got disabled
}

void steeringRight (unsigned char pwm = 255) {
  digitalWrite(SM1, HIGH);
  digitalWrite(SM2, LOW);
  analogWrite(SMPWM, pwm);
}

void steeringLeft(unsigned char pwm = 255) {
  digitalWrite(SM1, LOW);
  digitalWrite(SM2, HIGH);
  analogWrite(SMPWM, pwm);
}

void handleSteering() {
  // Steering Motor Control (X-axis derivative)
  int deltaX = xVal – prevX;  // Calculate position change
  prevX = xVal;  // Store current position
  
  int steeringPWM = abs(deltaX) * 0.25;  // Scale derivative to PWM
  steeringPWM = constrain(steeringPWM, 0, STEER_MAX);  // Limit PWM range
  
  // Apply steering deadzone and direction
  if (steeringPWM < STEER_MIN) {
    steeringStop();
    DEBUG_PRINTLN(“SS”);
  } else if (deltaX > 0) {
    steeringRight(steeringPWM);
    DEBUG_PRINT(“R=”);
    DEBUG_PRINTLN(steeringPWM);
  } else {
    steeringLeft(steeringPWM);
    DEBUG_PRINT(“L=”);
    DEBUG_PRINTLN(steeringPWM);
  }
}
</code>
<pre>
در کد زیر (توابع راه‌اندازی و حلقه):
•	ترتیب راه‌اندازی در setup():
1.	پیکربندی جوی‌استیک (joySetup()).
2.	پیکربندی موتورهای پیش‌رانش و فرمان (propulsionMotorSetup() و steeringMotorSetup()).
3.	پیکربندی ماژول رنگ (colorSensorSetup()).
4.	فعال‌سازی سریال و پین‌های دیباگ در صورت فعال بودن ENABLE_DEBUG با DEBUG_SETUP(9600).
•	جریان اصلی در loop():
1.	colorCheck() هر COLOR_CHECK_INTERVAL میلی‌ثانیه وضعیت چراغ را بررسی می‌کند و در صورت تشخیص قرمز توابع propulsionStop() و steeringStop() را اجرا می‌کند.
2.	readJoyStick() مقادیر آنالوگ محورهای جوی‌استیک را به‌روزرسانی می‌کند.
3.	handlePropulsion() و handleSteering() بر اساس مقادیر yVal و xVal فرمان‌های موتور را اجرا می‌کنند.
4.	delay(DEBOUNCE_DELAY) تاخیر ۲۰ms برای پایدارسازی خوانش‌ها اعمال می‌شود.
</pre>
<code lang="arduino">
void setup() {
  joySetup();

  propulsionMotorSetup();
  steeringMotorSetup();

  colorSensorSetup();

  DEBUG_SETUP(9600);
}

void loop() {
  colorCheck();
  readJoyStick();

  handlePropulsion();
  handleSteering();

  // Small delay to stabilize readings
  delay(DEBOUNCE_DELAY);
}
</code>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<pre>
</pre>
