# Wisam Programming Language (لغة وسام)

<p align="center">
  <img src="https://img.shields.io/badge/version-1.0.0-blue.svg" alt="Version">
  <img src="https://img.shields.io/badge/language-C-orange.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-green.svg" alt="License">
  <img src="https://github.com/droy-go/wisam/workflows/Build%20Wisam/badge.svg" alt="Build Status">
  <img src="https://github.com/droy-go/wisam/workflows/Code%20Quality/badge.svg" alt="Code Quality">
</p>

<p align="center">
  <b>أول لغة برمجة عربية كاملة مكتوبة بلغة C</b><br>
  <b>The First Complete Arabic Programming Language Written in C</b>
</p>

---

## 📖 نظرة عامة / Overview

**وسام (Wisam)** هي لغة برمجة عربية حديثة، سهلة التعلم، وقوية في الأداء. تم تصميمها لتكون بديلاً عربياً للغات البرمجة العالمية مثل Python، مع الحفاظ على البساطة والقوة.

**Wisam** is a modern Arabic programming language, easy to learn and powerful in performance. It is designed to be an Arabic alternative to global programming languages like Python, while maintaining simplicity and power.

### ✨ المميزات / Features

- 🔤 **كلمات مفتاحية عربية** - Arabic keywords
- 🚀 **مترجم سريع** - Fast interpreter written in C
- 📦 **مكتبة قياسية غنية** - Rich standard library
- 🤖 **دعم الذكاء الاصطناعي** - AI support built-in
- 🌐 **دعم الشبكات** - Network support
- 💾 **تخزين البيانات** - Data storage
- 🎨 **واجهات المستخدم** - User interfaces
- 📱 **وسائط متعددة** - Multimedia support

---

## 🚀 التثبيت / Installation

### المتطلبات / Requirements

- GCC أو أي مترجم C / GCC or any C compiler
- نظام Linux أو Windows / Linux or Windows OS
- دعم UTF-8 / UTF-8 support

### البناء من المصدر / Build from Source

```bash
# استنساخ المستودع / Clone the repository
git clone https://github.com/wisam-lang/wisam.git
cd wisam-lang

# البناء / Build
make

# التثبيت / Install (Linux/Mac)
sudo make install
```

### على Windows / On Windows

```cmd
# استخدام MinGW أو Cygwin / Using MinGW or Cygwin
mingw32-make

# أو / Or
make
```

---

## 📝 الاستخدام / Usage

### تشغيل ملف وسام / Run a Wisam File

```bash
wisam program.wsm
```

### الوضع التفاعلي / Interactive Mode

```bash
wisam -i
```

### خيارات سطر الأوامر / Command Line Options

```bash
wisam -h              # عرض المساعدة / Show help
wisam -v              # عرض الإصدار / Show version
wisam -t program.wsm  # عرض الرموز / Show tokens
wisam -a program.wsm  # عرض شجرة التحليل / Show AST
```

---

## 💻 أمثلة / Examples

### مثال: مرحبا بالعالم / Hello World

```wsm
# مرحبا بك في لغة وسام
ليكن اسم = "وسام"
اكتب "مرحبا بك في " + اسم
```

### مثال: المتغيرات والثوابت / Variables and Constants

```wsm
ليكن اسم = "أحمد"
ليكن عمر = 25
ثابت PI = 3.14159

اكتب "الاسم: " + اسم
اكتب "العمر: " + عمر
```

### مثال: الشروط / Conditions

```wsm
ليكن رقم = 10

إذا رقم > 5 إذن
    اكتب "الرقم أكبر من خمسة"
وإلا إذا رقم == 5 إذن
    اكتب "الرقم يساوي خمسة"
وإلا
    اكتب "الرقم أصغر من خمسة"
انتهى
```

### مثال: الحلقات / Loops

```wsm
# حلقة لكل
لكل عدد من 1 إلى 10
    اكتب "العدد: " + عدد
انتهى

# حلقة طالما
ليكن عداد = 0
طالما عداد < 5
    اكتب "العداد: " + عداد
    عداد = عداد + 1
انتهى
```

### مثال: الدوال / Functions

```wsm
دالة جمع تأخذ أ و ب
    أعد أ + ب
انتهى

دالة تحية تأخذ اسم
    أعد "مرحبا " + اسم + "!"
انتهى

ليكن النتيجة = جمع(5 ، 3)
اكتب "5 + 3 = " + النتيجة

اكتب تحية("وسام")
```

### مثال: الهياكل / Structs

```wsm
هيكل طالب
    الاسم
    العمر
    المعدل
انتهى

ليكن أحمد = طالب
أحمد.الاسم = "أحمد محمد"
أحمد.العمر = 20
أحمد.المعدل = 3.75

اكتب "اسم الطالب: " + أحمد.الاسم
اكتب "عمر الطالب: " + أحمد.العمر
اكتب "معدل الطالب: " + أحمد.المعدل
```

### مثال: الذكاء الاصطناعي / Artificial Intelligence

```wsm
# إنشاء عقل ذكي
أنشئ عقل باسم مساعد

# تعلم العقل
مساعد.تعلّم
    "مرحبا" = "أهلا بك! 👋"
    "كيف حالك" = "أنا بخير، شكراً!"
    "ما اسمك" = "أنا مساعد وسام الذكي"
انتهى

# استخدام العقل
ليكن رد = مساعد.اسأل("مرحبا")
اكتب رد
```

### مثال: المنظومة الذكية / AI Ecosystem

```wsm
# إنشاء منظومة ذكية
أنشئ منظومة باسم كيان

# إضافة أدمغة
كيان.أضف عقل نصي باسم فهم
كيان.أنشئ ذاكرة طويلة
كيان.تعلّم_ذاتي()

# استخدام المنظومة
ليكن جواب = كيان.استجب("أريد مساعدة في البرمجة")
اكتب جواب

# حفظ المنظومة
كيان.احفظ("my_ai.wsm")
```

---

## 📚 الكلمات المفتاحية / Keywords

| العربية | English | الوظيفة |
|---------|---------|---------|
| `ليكن` | `let` | تعريف متغير |
| `ثابت` | `const` | تعريف ثابت |
| `هو` | `is` | تعيين قيمة |
| `اكتب` | `write` | طباعة نص |
| `إذا` | `if` | شرط |
| `إذن` | `then` | ثم |
| `وإلا` | `else` | وإلا |
| `انتهى` | `end` | نهاية الكتلة |
| `لكل` | `for` | حلقة تكرار |
| `من` | `from` | من |
| `إلى` | `to` | إلى |
| `طالما` | `while` | طالما |
| `دالة` | `function` | دالة |
| `تأخذ` | `takes` | تأخذ |
| `و` | `and` | و |
| `أعد` | `return` | إرجاع |
| `هيكل` | `struct` | هيكل |
| `استورد` | `import` | استيراد |
| `أنشئ` | `create` | إنشاء |
| `عقل` | `mind` | عقل |
| `منظومة` | `ecosystem` | منظومة |
| `تعلّم` | `learn` | تعلم |
| `اسأل` | `ask` | سؤال |

---

## 📦 المكتبة القياسية / Standard Library

### نصوص (Text)

```wsm
نصوص.حوّل_إلى_كبير("hello")     # "HELLO"
نصوص.حوّل_إلى_صغير("HELLO")     # "hello"
نصوص.الطول("مرحبا")              # 10
نصوص.جزء("مرحبا"، 0، 4)         # "مرح"
نصوص.استبدل("مرحبا"، "حب"، "س")  # "مرسا"
نصوص.قسّم("أ،ب،ت"، "،")          # ["أ"، "ب"، "ت"]
نصوص.تقليم("  نص  ")             # "نص"
نصوص.ترجم("مرحبا"، إلى="English") # "Hello"
```

### زمن (Time)

```wsm
زمن.الآن()                      # الطابع الزمني
زمن.التاريخ()                   # "2024-01-15"
زمن.الوقت()                     # "14:30:00"
زمن.السنة()                     # 2024
زمن.الشهر()                     # 1
زمن.اليوم()                     # 15
زمن.نم(5)                       # انتظار 5 ثواني
```

### مخزن (Storage)

```wsm
مخزن.أنشئ("قاعدة_البيانات")
مخزن.أضف("قاعدة_البيانات"، مفتاح="مستخدم1"، قيمة="أحمد")
ليكن قيمة = مخزن.اقرأ("قاعدة_البيانات"، "مستخدم1")
مخزن.احذف("قاعدة_البيانات"، "مستخدم1")
```

### شبكة (Network)

```wsm
ليكن رد = شبكة.احصل("https://api.example.com")
شبكة.أرسل("https://api.example.com"، "بيانات")
شبكة.حمّل("https://example.com/file.zip"، "file.zip")
```

### وسائط (Media)

```wsm
وسائط.اعرض_صورة("photo.png")
وسائط.شغّل("video.mp4")
وسائط.شغّل_صوت("music.mp3")
```

### واجهات (UI)

```wsm
ليكن نافذة = واجهات.أنشئ_نافذة("تطبيقي")
واجهات.أضف_زر(نافذة، "اضغطني")
واجهات.أضف_تسمية(نافذة، "مرحبا")
واجهات.رسالة("مرحبا بالعالم!")
```

### ذكاء (AI)

```wsm
أنشئ عقل باسم مساعد
مساعد.تعلّم
    "مرحبا" = "أهلا بك!"
انتهى
ليكن رد = مساعد.اسأل("مرحبا")

أنشئ شبكة عصبية باسم شبكة_عميقة
شبكة_عميقة.طبقات = 3
شبكة_عميقة.معدل_تعلم = 0.01
شبكة_عميقة.درّب("data.csv")
```

---

## 🔧 البنية / Architecture

```
wisam-lang/
├── .github/
│   ├── workflows/       # GitHub Actions CI/CD
│   │   ├── build.yml    # Build workflow
│   │   ├── release.yml  # Release workflow
│   │   ├── code-quality.yml  # Code quality checks
│   │   └── docs.yml     # Documentation deployment
│   └── ISSUE_TEMPLATE/  # Issue templates
├── src/
│   ├── main.c           # نقطة الدخول
│   ├── lexer.c          # المحلل اللغوي
│   ├── parser.c         # المحلل النحوي
│   └── interpreter.c    # المفسر
├── include/
│   ├── wisam.h          # التعريفات الرئيسية
│   ├── lexer.h          # تعريفات المحلل اللغوي
│   ├── parser.h         # تعريفات المحلل النحوي
│   └── interpreter.h    # تعريفات المفسر
├── stdlib/
│   ├── text.c           # مكتبة النصوص
│   ├── time.c           # مكتبة الزمن
│   ├── storage.c        # مكتبة التخزين
│   ├── network.c        # مكتبة الشبكة
│   ├── media.c          # مكتبة الوسائط
│   ├── ui.c             # مكتبة الواجهات
│   └── ai.c             # مكتبة الذكاء الاصطناعي
├── examples/
│   ├── hello.wsm        # مثال مرحبا
│   ├── calculator.wsm   # مثال حاسبة
│   ├── loops.wsm        # مثال حلقات
│   └── structs.wsm      # مثال هياكل
├── Makefile
├── README.md
├── LICENSE
├── CONTRIBUTING.md
└── SECURITY.md
```

---

## 🧪 الاختبارات / Testing

```bash
# تشغيل جميع الأمثلة
make test

# تشغيل مثال محدد
make run-hello
make run-calculator
make run-loops
make run-structs

# الوضع التصحيحي
make debug-hello
```

---

## 🤝 المساهمة / Contributing

نرحب بمساهماتكم! يمكنكم المساهمة في:

- 🐛 الإبلاغ عن الأخطاء
- 💡 اقتراح ميزات جديدة
- 📝 تحسين الوثائق
- 🔧 إصلاح الأخطاء
- ✨ إضافة ميزات جديدة

---

## 📄 الترخيص / License

هذا المشروع مرخص بموجب [MIT License](LICENSE).

This project is licensed under the [MIT License](LICENSE).

---

## 🙏 الشكر / Acknowledgments

- شكر خاص لجميع المساهمين
- مستوحى من Python و Lua
- مصمم للعالم العربي

---

<p align="center">
  <b>صُنع ب❤️ للعالم العربي</b><br>
  <b>Made with ❤️ for the Arab World</b>
</p>

<p align="center">
  © 2024 Wisam Programming Language. All rights reserved.
</p>
