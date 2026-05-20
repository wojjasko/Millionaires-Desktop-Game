# 💰 Milionerzy – Win32 API Desktop Quiz Game
Kompleksowa aplikacja okienkowa typu quiz, wzorowana na popularnym programie telewizyjnym „Milionerzy”. Gra została zaprojektowana i zaimplementowana w języku C++ przy użyciu czystego, niskopoziomowego środowiska **Windows API (Win32)**. Aplikacja łączy zaawansowaną logikę biznesową teleturnieju z asynchronicznym przetwarzaniem multimediów i dynamicznym renderowaniem interfejsu graficznego.

## 🌟 Kluczowe Funkcjonalności

### 📊 System Zarządzania i Losowania Pytań
* **Dynamiczna sesja gry:** Baza danych podzielona jest na 12 odrębnych poziomów trudności (od 500 zł do 1 000 000 zł). Na każdą nową grę system generuje unikalny zestaw pytań poprzez pseudolosowy wybór z niezależnych pul.
* **Drabinka Wygranych Pro:** Panel boczny wyświetla pełną hierarchię nagród finansowych. System na bieżąco analizuje stan gry, podświetla aktualny poziom oraz precyzyjnie obsługuje progi gwarantowane (**1 000 zł** na poziomie 2. oraz **40 000 zł** na poziomie 7.).

### 🔄 Algorytmy Kół Ratunkowych (Business Logic)
* **Koło 50/50:** Algorytm analizuje dostępne warianty, zachowuje poprawną odpowiedź i losowo eliminuje dokładnie dwie błędne opcje, nakładając na nie trwałą blokadę interakcji.
* **Telefon do Przyjaciela:** Moduł symulacyjny oparty na rozkładzie prawdopodobieństwa. Przyjaciel posiada 70% szans na wskazanie poprawnej odpowiedzi oraz 30% szans na wybór losowej opcji.
* **Podpowiedź Publiczności (Wykresy GDI):** Dynamiczny moduł generujący zbalansowane, losowe wyniki procentowe głosowania widowni, wyświetlane w formie graficznych słupków bezpośrednio w oknie aplikacji.

### 🎬 System Animacji i Multitaskingu
* **Wielofazowy Feedback:** Po wybraniu odpowiedzi interfejs uruchamia asynchroniczny proces migania na żółto (sekwencja 8 zmian kolorów kontrolowana przez timer systemowy).
* **Data Consistency & Security:** Na czas trwania animacji weryfikacyjnej nakładana jest całkowita blokada obsługi komunikatów myszy, uniemożliwiająca użytkownikowi kliknięcie innych odpowiedzi przed werdyktem.
* **Audio Engine:** Zintegrowany moduł dźwiękowy odtwarzający muzykę budującą napięcie w tle za pomocą asynchronicznej pętli systemowej.

## 🛠 Stack Techniczny
* **Język programowania:** C++ (standard C++17 / C++20)
* **Interfejs graficzny:** Windows API (Win32 User & GDI - Graphics Device Interface)
* **Obsługa audio:** Waveform Audio (Biblioteka systemowa `winmm.lib` / `mmsystem.h`)
* **Środowisko programistyczne:** Visual Studio (Kompilator MSVC)

## 🚀 Kompilacja i Uruchomienie

### ⚠️ BARDZO WAŻNE: Konfiguracja Zasobów Multimedialnych
System operacyjny Windows ładuje zasoby graficzne i dźwiękowe relatywnie do aktualnego katalogu roboczego uruchomionego procesu. 

Aby gra działała prawidłowo i nie wyrzucała błędu krytycznego o braku plików, **po skompilowaniu projektu należy ręcznie skopiować pliki multimedialne** do odpowiednich katalogów wyjściowych (tam gdzie kompilator tworzy plik `.exe`):

1. Skopiować pliki `milionerzy.bmp` oraz `audio.wav` z głównego katalogu projektu.
2. Wkleić je bezpośrednio do folderów:
   * `[Katalog_Projektu]/Debug/`
   * `[Katalog_Projektu]/x64/Debug/` (lub odpowiednio `Release`, zależnie od konfiguracji w Visual Studio).

### ⚙️ Kroki uruchomienia w Visual Studio:
1. Otworzyć plik rozwiązania `Milionerzy.sln` w środowisku Microsoft Visual Studio.
2. Upewnić się, że w konfiguracji projektu załączona jest biblioteka multimediów za pomocą dyrektywy `#pragma comment(lib, "winmm.lib")` lub we właściwościach Linkera.
3. Skompilować i uruchomić projekt za pomocą przycisku **Local Windows Debugger** (zielona strzałka).

## 📜 Przebieg Rozgrywki (Instrukcja Użytkownika)
1. **Początek gry:** Po włączeniu aplikacji okno generuje dynamiczną strukturę. Na środku widzisz pytanie i 4 przyciski. Po prawej stronie znajduje się interaktywny panel wygranych i koła ratunkowe.
2. **Wybór odpowiedzi:** Kliknięcie przycisku uruchamia proces sprawdzania. Przycisk miga na żółto, po czym właściwa odpowiedź podświetla się na zielono (błędna na czerwono).
3. **Koniec gry i restart:** W przypadku porażki aplikacja wyświetla natywny komunikat `MessageBox` z informacją o zdobytej kwocie (z uwzględnieniem progów gwarantowanych). Zatwierdzenie komunikatu automatycznie resetuje stan kół, losuje nową pulę pytań i rozpoczyna nową sesję.
