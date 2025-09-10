---
title: "Experiment 00: Blink"
header: "Experiment 00: Blink"
author: M. MAD
pic: img/00-blink.png
name: "آزمایش 00: راه‌اندازی‌های اولیه و تست ماژول‌های آردوینو"
manufacturing_date: 2025
category: آزمایش
manufacturer_name: اپتیک‌نیرو - <span class="en">Optic Niroo</span>
manufacturer_country: ایران
goals:
  - مروری بر اصول و اسلوب برنامه‌نویسی سامانه‌های نهفته و خانواده بردهای توسعه آردوینو
  - >
    معرفی و کار با نرم‌افزار
    <span class="en">Arduino IDE</span>
  - >
    شبیه‌سازی آزمایش‌ها و پروژه‌های مبتنی‌بر خانواده بردهای توسعه آردوینو با استفاده از نرم‌افزار
    <span class="en">Proteus</span>
  - کار با خروجی‌های دیجیتال
  - >
    یادگیری زمان‌بندی انسدادی
    <span class="en">Blocking Timing</span>
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
is_simulation_step_depend_on_prerequisites: true
additional_implementation_uplaud_step: >
  یا از طریق کنش «<span class="en">Blink</span>» زیرمنوی «<span class="en">01.Basics</span>» در «مثال‌ها (<span class="en">Examples</span>)» در منوی «پرونده (<span class="en">File</span>)»
---
<p>
برای کسب اطلاعات درباره ریزپردازنده، ریزمهارگر، بردهای توسعه به پیوست 1 مراجعه
کنید.
</p>
<p>
برای درک و انجام این آزمایش پیوست 2 (مروری بر اصول و اسلوب برنامه‌نویسی) و
پیوست 3 (آموزش کار با نرم‌افزارهای مورد نیاز) را باید بخوانید.
</p>
<p>
کد کامل این آزمایش و آزمایش‌های دیگر نیز همگی در پیوست 4 آمده‌اند (البته کد این
آزمایش استثنائا از طریق کنش
«<span class="en">Blink</span>»
زیرمنوی
«<span class="en">01.Basics</span>»
در «مثال‌ها
(<span class="en">Examples</span>)»
در منوی «پرونده
(<span class="en">File</span>)»
در نرم‌افزار
<span class="en">Arduino IDE</span>
نیز در دسترس است). در ادامه، کد این آزمایش به صورت تکه تکه توضیح داده خواهد شد.

</p>
<h4>
توضیح کد آزمایش
</h4>
<p>
همانگونه که در پیوست 2 (مروری بر اصول و اسلوب برنامه‌نویسی) گفته شد، برای راحتی
و سرعت در خوانش و فهم کدهای از پیش نوشته‌شده، بهتر است که از نظرات توضیحی
ابتدای پرونده‌های کد منبع (که درباره چیستی و چرایی وجود آن پرونده، نویسنده یا
نویسندگان آن و مجوزهای استفاده و انتشار توضیح می‌دهند و معمولا برای فهم آن کد
ضروری نیستند) و تعریف متغیرهای جهانی و توابع دیگر گذر کنیم و مستقیما به جایی که
اجرای کد ما از آنها آغاز می‌شود برویم؛ یعنی تابع «راه‌اندازی آغازین (setup)» و
پس از آن هم تابع «حلقه اصلی
(<var class="v-func">loop</var>)».
شایان ذکر است که برای اطلاعات بیشتر درباره موارد استفاده‌شده در کد (توابع
«<var class="v-func">pinMode</var>»،
«<var class="v-func">digitalWrite</var>»
و
«<var class="v-func">delay</var>»
و ثابت‌های
«<var class="v-const">LED_BUILTIN</var>»،
«<var class="v-const">OUTPUT</var>»،
«<var class="v-const">HIGH</var>»
و
«<var class="v-const">LOW</var>»)
می‌توانید به پیوست 6 مراجعه کنید.
</p>
<h5>
تابع راه‌اندازی آغازین
</h5>
<p>
تکه کد مربوط به تابع راه‌اندازی آغازین در ادامه آورده شده است:
</p>
<code lang="arduino">
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}
</code>
<p>
اگر باز هم در اینجا از نظرات صرف نظر کنیم، کد این تابع عملا دو خط است، در خط
یکم آن (خط دوم کد)، تابعی بدون خروجی (از نوع
«<var class="v-type">void</var>»)
به نام
«<var class="v-func">setup</var>»
اعلان و مقداردهی اولیه شده که بلوک حکم‌های آن تنها دربردارنده یک حکم می‌شود،
فراخوانی تابع
«<var class="v-func">pinMode</var>»
با مقادیر ثابت‌های
«<var class="v-const">LED_BUILTIN</var>»
و
«<var class="v-const">OUTPUT</var>»
به‌ترتیب به‌عنوان آرگومان‌های یکم و دوم آن.
</p>
<p>
تابع
«<var class="v-func">pinMode</var>»
پین مورد نظر را در قالب یک عدد صحیح بی‌علامت 8 بیتی (نوع
«<var class="v-type">unsigned char</var>»)
به‌عنوان آرگومان یکم (ثابت
«<var class="v-const">LED_BUILTIN</var>»
با مقدار «13» در صورت گزینش برد توسعه «آردوینو اونو
(<span class="en">Arduino Uno</span>)»
و «آردوینو مگا
(<span class="en">Arduino Mega</span>)»
در زیرمنوی «برد
(<span class="en">Board</span>)»
در منوی «ابزارها
(<span class="en">Tools</span>)»
در بخش منوهای نرم‌افزار
<span class="en">Arduino IDE</span>،
این مقدار می‌تواند در صورت گزینش بردهای دیگر تغییر کند) و حالت مطلوب را باز هم
در قالب یک عدد صحیح بی‌علامت 8 بیتی (نوع
«<var class="v-type">unsigned char</var>»)
به‌عنوان آرگومان دوم (ثابت
«<var class="v-const">OUTPUT</var>»
با مقدار
«<var class="v-literal v-num v-int v-hexnum">0x01</var>»)
می‌گیرد و حالت داده‌شده را روی پین مورد نظر اعمال می‌کند.
</p>
<h5>
تابع حلقه اصلی
</h5>
<p>
تکه کد مربوط به تابع حلقه اصلی نیز در ادامه آورده شده است:
</p>
<code lang="arduino">
// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on
  delay(1000);                      // wait for a second
  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
  delay(1000);                      // wait for a second
}
</code>
<p>
در اینجا هم تابعی بدون خروجی (از نوع
«<var class="v-type">void</var>»)
به نام
«<var class="v-func">loop</var>»
در خط 2 (خط 8 کد) اعلان و مقداردهی اولیه شده که بلوک حکم‌های آن دربردارنده 4
حکم است، فراخوانی تابع
«<var class="v-func">digitalWrite</var>»،
سپس فراخوانی تابع
«<var class="v-func">delay</var>»
و دوباره همین روال ولی آرگومان دوم فراخوانی تابع
«<var class="v-func">digitalWrite</var>»
این بار تغییر کرده است.
</p>
<p>
تابع
«<var class="v-func">digitalWrite</var>»
پین مورد نظر را به‌عنوان آرگومان یکم و حالت منطقی (دیجیتال) مطلوب را به‌عنوان
آرگومان دوم خود می‌گیرد و حالت منطقی مطلوب را روی پین مورد نظر اعمال می‌کند.
</p>
<p>
تابع
«<var class="v-func">delay</var>»
یک مقدار عدد صحیح بی‌علامت طولانی (نوع
«<var class="v-type">unsigned long</var>»)
به‌عنوان تنها آرگومان خود می‌گیرد و ریزمهارگر را برای همان مقدار زمان (به واحد
میلی ثانیه) معطل می‌کند، به عبارت دیگر ریزمهارگر از زمان فراخوانی این تابع تا
پایان مدت زمان تعیین‌شده هیچ کاری انجام نمی‌دهد.
</p>
<p>
در اینجا ابتدا با استفاده از تابع
«<var class="v-func">digitalWrite</var>»،
پین
«<var class="v-const">LED_BUILTIN</var>»
در حالت
«<var class="v-const">HIGH</var>»
(«یک» منطقی/5 ولت) قرار گرفته و سپس با استفاده از تابع
«<var class="v-func">delay</var>»
به اندازه 1000 میلی ثانیه (1 ثانیه) صبر شده (یعنی
<span class="en">LED</span>
روی برد توسعه روشن شده و برای ۱ ثانیه همچنان روشن نگه داشته شده)، سپس دوباره با
استفاده از تابع
«<var class="v-func">digitalWrite</var>»،
پین
«<var class="v-const">LED_BUILTIN</var>»
این بار در حالت
«<var class="v-const">LOW</var>»
(«صفر» منطقی/0 ولت) قرار گرفته و سپس با استفاده از تابع
«<var class="v-func">delay</var>»
به اندازه 1000 میلی ثانیه (1 ثانیه) صبر شده است (یعنی
<span class="en">LED</span>
روی برد توسعه خاموش شده و برای 1 ثانیه همچنان خاموش نگه داشته شده). به این
ترتیب، با هر بار تکرار این حلقه، LED روی برد توسعه 1 ثانیه روشن و 1 ثانیه خاموش
می‌شود و به این شکل، یک مدار چشمک‌زن ایجاد می‌شود.
</p>
<h4>
توضیح شبیه‌سازی آزمایش
</h4>
<p>
شبیه‌سازی این آزمایش پیچیدگی و نکته خاصی ندارد، صرفا باید در نظر داشته باشید که
همانگونه که در بخش «نرم‌افزار
<span class="en">Proteus</span>»
پیوست 3 هم ذکر شد، به علت یک باگ برنامه‌نویسی در این نرم‌افزار، LED درون قطعه
مرکب آردوینو با وجود سرخ شدن رنگ مربع دیجیتال سر شاخه مربوطه، چشمک نمی‌زند و
برای دیدن چشمک‌زدن LED، باید حتما یک مدار جداگانه LED به‌همراه مقاومت
محدودکننده جریان را به‌صورت دستی به پایه 13 قطعه مرکب آردوینو وصل کنید، مانند
تصویرهای زیر:
</p>
<figure>
<img src="img/00-blink-sim-off.png" alt="Proteus simulation screenshot with the built-in LED Off">
<img src="img/00-blink-sim-on.png" alt="Proteus simulation screenshot with the built-in LED On">
<figcaption>
شکل 1- شبیه‌سازی مدار آزمایش با «نرم‌افزار
<span class="en">Proteus</span>» -
با
<span class="en">LED</span>
داخلی خاموش (راست) و با
<span class="en">LED</span>
داخلی روشن (چپ)
</figcaption>
</figure>
<h4>
توضیح سیم‌بندی و بستن مدار آزمایش
</h4>
<p>
این آزمایش، استثنائا نیازی به بستن مدار اختصاصی برای کار کردن ندارد و بلافاصله
پس از بارگذاری کد آزمایش بر روی برد توسعه مورد نظر، کد شروع به اجرا خواهد کرد.
</p>
</p>
اما در صورت تمایل برای بستن مدار اختصاصی جداگانه برای آزمایش، می‌توانید برد را
از رایانه جدا کرده و توسط جک بشکه‌ای مادگی روی آن، به یک منبع تغذیه ۱۲ ولتی
وصل کنید تا برد توسعه تغذیه شده و کد آزمایش را اجرا کند.
</p>
</p>
همچنین می‌توانید تابلوی آردوینوی مورد نظر را به برق وصل کرده و روشن کنید، سپس
یک سیم بردارید و یک سر آن را زیر ترمینال «زمین
(<span class="en">GND</span>)
» منبع تغذیه پیچ کرده و سر دیگر آن را به یکی از سه پین هدر مادگی «زمین
(<span class="en">GND</span>)
» روی برد آردوینو (هم در آدروینو اونو و هم در آردوینو مگا، یک پین هدر مادگی
«زمین
(<span class="en">GND</span>)»
در انتهای بخش «دیجیتال
(<span class="en">Digital</span>)»
و دو پین هدر مادگی «زمین
(<span class="en">GND</span>)»
در بخش «قدرت
(<span class="en">Power</span>)
» دارند) وصل کنید (البته توصیه می‌شود که در صورت امکان، هر سه پین هدر مادگی را
به «زمین
(<span class="en">GND</span>)
» منبع تغذیه وصل کنید). یک سیم دیگر هم بردارید و یک سر آن را زیر ترمینال «۵ ولت
(<span class="en">5 V</span>)
» منبع تغذیه پیچ کرده و سر دیگر آن را به پین هدر مادگی «۵ ولت
(<span class="en">5V</span>)»
در بخش «قدرت
(<span class="en">Power</span>)
» روی برد آردوینو وصل کنید. در این صورت هم، برد توسعه دوباره تغذیه شده و کد
آزمایش را اجرا می‌کند.

</p>
