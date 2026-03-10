# Wisam Code Editor - محرر أكواد وسام

<div align="center">

![Wisam Editor](public/icon-192x192.png)

**محرر أكواد متكامل للغة برمجة وسام**

[![React](https://img.shields.io/badge/React-18-blue)](https://reactjs.org/)
[![TypeScript](https://img.shields.io/badge/TypeScript-5.0-blue)](https://www.typescriptlang.org/)
[![Vite](https://img.shields.io/badge/Vite-5.0-purple)](https://vitejs.dev/)
[![PWA](https://img.shields.io/badge/PWA-Ready-green)](https://web.dev/progressive-web-apps/)

[English](#english) | [العربية](#arabic)

</div>

---

<a name="arabic"></a>
## 🇸🇦 العربية

### المميزات

- ✨ **محرر أكواد متقدم** مع تلوين الصياغة (Syntax Highlighting) للغة وسام
- 🔧 **إكمال تلقائي** للكلمات المفتاحية والدوال
- 📁 **مستكشف ملفات** متكامل
- 🎨 **وضع داكن/فاتح**
- 📱 **تصميم متجاوب** يعمل على جميع الأجهزة
- 💾 **تطبيق ويب تقدمي (PWA)** يمكن تثبيته على الجوال
- 🚀 **تشغيل سريع** للكود

### اللغات المدعومة

- 🇸🇦 **العربية**: دالة، إذا، طالما، طباعة، إدخال، ...
- 🇬🇧 **الإنجليزية**: function, if, while, print, input, ...

### كيفية الاستخدام

#### 1. تشغيل محلياً

```bash
# تثبيت التبعيات
npm install

# تشغيل في وضع التطوير
npm run dev

# بناء للإنتاج
npm run build
```

#### 2. تثبيت كتطبيق ويب (PWA)

1. افتح التطبيق في المتصفح
2. انقر على "Add to Home Screen" أو "تثبيت"
3. استمتع بالتطبيق!

#### 3. بناء APK (Android)

```bash
# تأكد من تثبيت Android SDK
# ثم نفذ:
cd android
./gradlew assembleDebug

# سيجدد APK في:
# android/app/build/outputs/apk/debug/app-debug.apk
```

### متطلبات بناء APK

- [Android Studio](https://developer.android.com/studio)
- [Android SDK](https://developer.android.com/studio/releases/platform-tools)
- [Java JDK 17+](https://adoptium.net/)

---

<a name="english"></a>
## 🇬🇧 English

### Features

- ✨ **Advanced Code Editor** with syntax highlighting for Wisam language
- 🔧 **Auto-completion** for keywords and functions
- 📁 **Integrated File Explorer**
- 🎨 **Dark/Light Mode**
- 📱 **Responsive Design** works on all devices
- 💾 **Progressive Web App (PWA)** installable on mobile
- 🚀 **Fast Code Execution**

### Supported Languages

- 🇸🇦 **Arabic**: دالة، إذا، طالما، طباعة، إدخال، ...
- 🇬🇧 **English**: function, if, while, print, input, ...

### How to Use

#### 1. Run Locally

```bash
# Install dependencies
npm install

# Run in development mode
npm run dev

# Build for production
npm run build
```

#### 2. Install as PWA

1. Open the app in browser
2. Click "Add to Home Screen"
3. Enjoy the app!

#### 3. Build APK (Android)

```bash
# Make sure Android SDK is installed
# Then run:
cd android
./gradlew assembleDebug

# APK will be at:
# android/app/build/outputs/apk/debug/app-debug.apk
```

### APK Build Requirements

- [Android Studio](https://developer.android.com/studio)
- [Android SDK](https://developer.android.com/studio/releases/platform-tools)
- [Java JDK 17+](https://adoptium.net/)

---

## 📁 Project Structure

```
wisam-editor/
├── src/
│   ├── App.tsx          # Main application component
│   ├── App.css          # Custom styles
│   ├── main.tsx         # Entry point
│   └── components/      # UI components
├── public/              # Static assets
├── android/             # Android project (Capacitor)
├── dist/                # Build output
├── index.html           # HTML template
├── vite.config.ts       # Vite configuration
└── capacitor.config.ts  # Capacitor configuration
```

---

## 🛠️ Technologies Used

- [React](https://reactjs.org/) - UI Library
- [TypeScript](https://www.typescriptlang.org/) - Type Safety
- [Vite](https://vitejs.dev/) - Build Tool
- [Tailwind CSS](https://tailwindcss.com/) - Styling
- [shadcn/ui](https://ui.shadcn.com/) - UI Components
- [Monaco Editor](https://microsoft.github.io/monaco-editor/) - Code Editor
- [Capacitor](https://capacitorjs.com/) - Native Runtime
- [Vite PWA](https://vite-pwa-org.netlify.app/) - PWA Plugin

---

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

---

## 📄 License

This project is licensed under the MIT License.

---

<div align="center">

**Made with ❤️ for the Wisam Programming Language**

</div>
