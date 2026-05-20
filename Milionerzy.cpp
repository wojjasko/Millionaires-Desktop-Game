//dolacza WINDOWS API - wszystkie funkcje do tworzenia okien, rysowania, obslugi zdarzen
#include <windows.h>
#include <cstdio>
#include <mmsystem.h>
#include <ctime>
#pragma comment(lib, "winmm.lib")

// obsluguje wszystkie komunikaty okna
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

// --- ustawienia okna ---
const int WINDOW_WIDTH = 1366;
const int WINDOW_HEIGHT = 768;
const int RIGHT_PANEL_WIDTH = 300;

// --- tlo ---
HBITMAP hBackgroundBmp = nullptr;

// --- czcionki --- 
HFONT hFontQuestion = nullptr;
HFONT hFontButton = nullptr;

// --- struktura przycisku ---
struct Button {
    RECT rect;
    const wchar_t* text;
};

// przyciski
Button answerButtons[4];
Button lifelines[3];

// --- struktura pytania ---
struct Question
{
    const wchar_t* text;          // tresc pytania (wskaznik na tekst pytania)
    const wchar_t* answers[4];    // odpowiedzi A B C D
    int correct;                  // indeks poprawnej odpowiedzi
};

// stan gry
int currentQuestion = 0;
int selectedAnswer = -1;
int blinkingCount = 0;
bool isBlinkingYellow = false;
UINT_PTR gameTimer = 0;
bool animationActive = false;

// stany kol ratunkowych
bool lifeline5050Used = false;
bool lifelinePhoneUsed = false;
bool lifelineAudienceUsed = false;

// wizualne glosowanie publicznosci
bool showAudienceGraph = false;
int audienceResults[4] = { 0, 0, 0, 0 };

// kolo ratunkowe 50/50
bool answerBlocked[4] = { false, false, false, false };

// baza pytan
Question questionPool[12][3] = {
    // 1. 500 zl
    {
        { L"Które z tych zwierząt jest ssakiem?", {L"Rekin", L"Wieloryb", L"Tuńczyk", L"Karp"}, 1 },
        { L"Ile to jest 15 - 6?", {L"7", L"8", L"9", L"10"}, 2 },
        { L"Co jest stolicą Francji?", {L"Madryt", L"Rzym", L"Berlin", L"Paryż"}, 3 }
    },
    // 2. 1000 zl
    {
        { L"Który kolor powstanie z połączenia żółtego i niebieskiego?", {L"Zielony", L"Pomarańczowy", L"Fioletowy", L"Szary"}, 0 },
        { L"Jak nazywa się młody koń?", {L"Cielę", L"Źrebię", L"Szczenię", L"Prosię"}, 1 },
        { L"Ile minut ma półtorej godziny?", {L"60", L"75", L"90", L"120"}, 2 }
    },
    // 3. 2000 zl
    {
        { L"Który ocean jest największy?", {L"Atlantycki", L"Indyjski", L"Arktyczny", L"Spokojny"}, 3 },
        { L"W którym roku Polska odzyskała niepodległość?", {L"1410", L"1918", L"1939", L"1945"}, 1 },
        { L"Który instrument ma czarno-białe klawisze?", {L"Gitara", L"Flet", L"Pianino", L"Trąbka"}, 2 }
    },
    // 4. 5000 zl
    {
        { L"Kto napisał 'Pan Tadeusz'?", {L"Adam Mickiewicz", L"Juliusz Słowacki", L"Henryk Sienkiewicz", L"Bolesław Prus"}, 0 },
        { L"Która planeta jest najbliżej Słońca?", {L"Wenus", L"Merkury", L"Mars", L"Ziemia"}, 1 },
        { L"Z jakiego kraju pochodzi pizza?", {L"Hiszpania", L"Grecja", L"Włochy", L"Turcja"}, 2 }
    },
    // 5. 10 000 złl
    {
        { L"Ile wynosi pierwiastek z 144?", {L"10", L"12", L"14", L"16"}, 1 },
        { L"Co jest symbolem chemicznym złota?", {L"Ag", L"Fe", L"Au", L"Cu"}, 2 },
        { L"W którym mieście znajduje się Wawel?", {L"Warszawa", L"Gdańsk", L"Wrocław", L"Kraków"}, 3 }
    },
    // 6. 20 000 zl
    {
        { L"Kto namalował 'Mona Lisę'?", {L"Vincent van Gogh", L"Leonardo da Vinci", L"Pablo Picasso", L"Salvador Dali"}, 1 },
        { L"Ile kości ma dorosły człowiek (około)?", {L"106", L"206", L"306", L"406"}, 1 },
        { L"Które państwo nie leży w Europie?", {L"Albania", L"Estonia", L"Etiopia", L"Andora"}, 2 }
    },
    // 7. 40 000 zl (prog gwarantowany)
    {
        { L"Jak nazywa się najwyższy szczyt Tatr?", {L"Giewont", L"Rysy", L"Kasprowy Wierch", L"Gerlach"}, 3 },
        { L"Ile zawodników liczy drużyna piłki ręcznej na boisku?", {L"5", L"6", L"7", L"11"}, 2 },
        { L"W którym roku człowiek po raz pierwszy stanął na Księżycu?", {L"1961", L"1969", L"1972", L"1975"}, 1 }
    },
    // 8. 75 000 zl
    {
        { L"Co oznacza łacińska sentencja 'Veni, vidi, vici'?", {L"Chwytaj dzień", L"Pamiętaj o śmierci", L"Przyszedłem, zobaczyłem, zwyciężyłem", L"Wiem, że nic nie wiem"}, 2 },
        { L"Który gaz stanowi większość ziemskiej atmosfery?", {L"Tlen", L"Azot", L"Dwutlenek węgla", L"Hel"}, 1 },
        { L"W którym państwie leży miasto Barcelona?", {L"Portugalia", L"Włochy", L"Francja", L"Hiszpania"}, 3 }
    },
    // 9. 125 000 zl
    {
        { L"Jak nazywa się największy mięsień w ciele człowieka?", {L"Dwugłowy ramienia", L"Najszerszy grzbietu", L"Pośladkowy wielki", L"Czworogłowy uda"}, 2 },
        { L"Która rzeka jest najdłuższa na świecie (według większości źródeł)?", {L"Nil", L"Amazonka", L"Jangcy", L"Mississippi"}, 1 },
        { L"Kto odkrył polon i rad?", {L"Maria Skłodowska-Curie", L"Mikołaj Kopernik", L"Albert Einstein", L"Alfred Nobel"}, 0 }
    },
    // 10. 250 000 zl
    {
        { L"Ile bitów składa się na jeden bajt?", {L"4", L"8", L"16", L"32"}, 1 },
        { L"W którym roku odbył się chrzest Polski?", {L"966", L"1000", L"1025", L"1410"}, 0 },
        { L"Która z tych wysp należy do Włoch?", {L"Majorka", L"Korsyka", L"Sycylia", L"Kreta"}, 2 }
    },
    // 11. 500 000 zl
    {
        { L"Jak nazywa się proces przechodzenia substancji ze stanu stałego w gazowy?", {L"Kondensacja", L"Parowanie", L"Resublimacja", L"Sublimacja"}, 3 },
        { L"Kto jest autorem obrazu 'Bitwa pod Grunwaldem'?", {L"Jan Matejko", L"Józef Chełmoński", L"Julian Fałat", L"Stanisław Wyspiański"}, 0 },
        { L"Ile wynosi stała prędkość światła w próżni (ok.)?", {L"100 tys. km/s", L"200 tys. km/s", L"300 tys. km/s", L"400 tys. km/s"}, 2 }
    },
    // 12. 1 000 000 zl
    {
        { L"Który z tych języków programowania powstał najwcześniej?", {L"Python", L"C++", L"Java", L"JavaScript"}, 1 },
        { L"W którym roku zburzono mur berliński?", {L"1981", L"1985", L"1989", L"1991"}, 2 },
        { L"Jak nazywa się najgłębszy rów oceaniczny na Ziemi?", {L"Rów Atakamski", L"Rów Portorykański", L"Rów Jawajski", L"Rów Mariański"}, 3 }
    }
};

// zmienna przechowujaca wylosowane pytania na biezaca gre
Question currentSessionQuestions[12];

// baza wygranych
const wchar_t* winnings[] = {
    L"1 000 000", L"500 000", L"250 000", L"125 000", L"75 000",
    L"40 000", L"20 000", L"10 000", L"5 000", L"2 000",
    L"1000", L"500", L"0"
};
const int levelsCount = sizeof(winnings) / sizeof(winnings[0]);

// --- deklaracje ---
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool IsInside(const RECT&, int, int);
void InitButtons();
void LoadQuestion(int);
void DrawButton(HDC hdc, const Button& btn, COLORREF color);
void DrawWinnings(HDC);

// --- funkcje ---

// funkcja przygotowujaaca nowa gre
void PrepareNewGameSet() {
    for (int i = 0; i < 12; i++) {
        int randomIndex = rand() % 3; // Losuj 0, 1 lub 2
        currentSessionQuestions[i] = questionPool[i][randomIndex];
    }
}

// funkcja sprawdzajaca klikniecie
bool IsInside(const RECT& r, int x, int y)
{
    return x >= r.left && x <= r.right &&
        y >= r.top && y <= r.bottom;
}

// funkcja inicjalizujaca przyciski
void InitButtons()
{
    int leftAreaWidth = WINDOW_WIDTH - RIGHT_PANEL_WIDTH;

    // A B (gorny rzad odpowiedzi)
    answerButtons[0] = {
        { 50, WINDOW_HEIGHT - 180, leftAreaWidth / 2 - 20, WINDOW_HEIGHT - 120 },
        L"A: Odpowiedź A"
    };

    answerButtons[1] = {
        { leftAreaWidth / 2 + 20, WINDOW_HEIGHT - 180, leftAreaWidth - 50, WINDOW_HEIGHT - 120 },
        L"B: Odpowiedź B"
    };

    // C D (dolny rzad odpowiedzi)
    answerButtons[2] = {
        { 50, WINDOW_HEIGHT - 110, leftAreaWidth / 2 - 20, WINDOW_HEIGHT - 50 },
        L"C: Odpowiedź C"
    };

    answerButtons[3] = {
        { leftAreaWidth / 2 + 20, WINDOW_HEIGHT - 110, leftAreaWidth - 50, WINDOW_HEIGHT - 50 },
        L"D: Odpowiedź D"
    };

    // kola ratunkowe (prawa gora)
    lifelines[0] = { { leftAreaWidth + 30, 30, WINDOW_WIDTH - 30, 80 }, L"50/50" };
    lifelines[1] = { { leftAreaWidth + 30, 90, WINDOW_WIDTH - 30, 140 }, L"Telefon" };
    lifelines[2] = { { leftAreaWidth + 30, 150, WINDOW_WIDTH - 30, 200 }, L"Publiczność" };
}

// zaladowuje pytanie
void LoadQuestion(int index)
{
    // tablica pomocnicza z literami
    const wchar_t* letters[] = { L"A: ", L"B: ", L"C: ", L"D: " };

    for (int i = 0; i < 4; i++)
    {
        // tekst z odpowiedziami - dynamicznie
        static wchar_t buffer[4][256];
        swprintf_s(buffer[i], 256, L"%s%s", letters[i], currentSessionQuestions[index].answers[i]);
        answerButtons[i].text = buffer[i];
    }
}

// rysowanie przycisku
void DrawButton(HDC hdc, const Button& btn, COLORREF color)
{
    // ustawienie koloru tla
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

    // ramka przycisku
    Rectangle(hdc, btn.rect.left, btn.rect.top, btn.rect.right, btn.rect.bottom);

    // kolor tekstu (czarny)
    SetTextColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, TRANSPARENT);

    DrawTextW(hdc, btn.text, -1, (LPRECT)&btn.rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrush);
}

// rysowanie drabinki wygranych
void DrawWinnings(HDC hdc)
{
    HFONT oldFont = (HFONT)SelectObject(hdc, hFontButton);
    SetTextColor(hdc, RGB(255, 215, 0)); // zloty kolor
    SetBkMode(hdc, TRANSPARENT);

    int startY = 250; // wspolrzedne poczatek drabinki (pod kolami ratunkowymi)
    int step = 35;    // odstep między poziomami
    int panelX = WINDOW_WIDTH - RIGHT_PANEL_WIDTH + 10;
    int panelWidth = WINDOW_WIDTH - 20;

    for (int i = 0; i < levelsCount; i++)
    {
        int levelIndex = levelsCount - 1 - i; // odwrocenie kolejności
        RECT r = { panelX, startY + i * step, 
            panelWidth, startY + i * step + 30 };

        if (levelIndex == currentQuestion) // podswietla obecny poziom
            SetTextColor(hdc, RGB(0, 255, 0));
        else
            if (levelIndex == 2 || levelIndex == 7 || levelIndex == 12) {
                SetTextColor(hdc, RGB(255, 255, 255));
            }
            else {
                SetTextColor(hdc, RGB(255, 215, 0));
            }
            

        DrawTextW(hdc, winnings[i], -1, &r, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
    }

    SelectObject(hdc, oldFont);
}

// inicjalizacja kola ratunkowego - 50/50%
void Use5050()
{
    if (lifeline5050Used) return;
    lifeline5050Used = true;

    int correct = currentSessionQuestions[currentQuestion].correct;

    int removed = 0;
    for (int i = 0; i < 4 && removed < 2; i++)
    {
        if (i != correct && !answerBlocked[i])
        {
            answerBlocked[i] = true;
            removed++;
        }
    }
}

// inicjalizacja kola ratunkowego - telefon do przyjaciela
void UsePhone(HWND hWnd)
{
    if (lifelinePhoneUsed) return;
    lifelinePhoneUsed = true;

    int correct = currentSessionQuestions[currentQuestion].correct;

    int suggestion;
    if (rand() % 100 < 70)
        suggestion = correct;
    else
        suggestion = rand() % 4;

    wchar_t msg[128];
    swprintf_s(msg, 128,
        L"Myślę, że poprawna odpowiedź to: %c",
        L'A' + suggestion);

    MessageBox(hWnd, msg, L"Telefon do przyjaciela", MB_OK);
}

// inicjalizacja kola ratunkowego - publicznosc
void UseAudience()
{
    if (lifelineAudienceUsed) return;
    lifelineAudienceUsed = true;

    int correct = currentSessionQuestions[currentQuestion].correct;

    // losujemy poprawna odpowiedz w szerokim zakresie (np. 45-85%)
    audienceResults[correct] = 45 + (rand() % 41);

    int remaining = 100 - audienceResults[correct];
    int count = 0;

    for (int i = 0; i < 4; i++) {
        if (i != correct) {
            if (count < 2) {
                // losuje wartosc dla dwoch pierwszych blednych
                int val = (remaining > 0) ? (rand() % (remaining + 1)) : 0;
                // aby nie było zbyt "pusto", dziele to troche bardziej naturalnie
                if (remaining > 10) val = rand() % (remaining - 5);

                audienceResults[i] = val;
                remaining -= val;
            }
            else {
                // ostatnia odpowiedz dostaje reszte procentow
                audienceResults[i] = remaining;
                remaining = 0;
            }
            count++;
        }
    }
    showAudienceGraph = true;
}

// funkcja resetujaca runde
void ResetGame() {
    PrepareNewGameSet(); // losujemy nowy zestaw pytań
    currentQuestion = 0;
    selectedAnswer = -1;
    lifeline5050Used = false;
    lifelinePhoneUsed = false;
    lifelineAudienceUsed = false;
    showAudienceGraph = false;
    for (int i = 0; i < 4; i++) answerBlocked[i] = false;
    LoadQuestion(0);
}

// --- punkt startowy - zamiennik funkcji main() --- 
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) 
{
    srand((unsigned int)time(NULL)); 
    PrepareNewGameSet();
    WNDCLASS wc = {}; // tworzenie struktury okna
    wc.lpfnWndProc = WndProc; // kazdy komunikat (np. klikniecie) bedzie kierowany do funkcji WndProc
    wc.hInstance = hInstance; // przekazujemy uchwyt biezacej instancji programu
    wc.lpszClassName = L"Milionerzy"; // nazwa klasy okna
    RegisterClass(&wc); // rejestruje klase okna w systemie Windows (bez tego nie da sie tworzyc wiec musi byc)

    HWND hWnd = CreateWindowEx( // tworzymy glone okno programu
        0, // brak styli rozszerzonych
        L"Milionerzy", // klasa okna
        L"Milionerzy", // tytul okna
        WS_OVERLAPPEDWINDOW, // standardowe okno Windows z paskiem, minimalizuj/maxymalizuj, ramką
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, // pozycja i rozmiar okna
        nullptr, nullptr, hInstance, nullptr);
    ShowWindow(hWnd, nCmdShow); // wyswietla okno na ekranie

    // ladowanie zdjęcia z pliku
    hBackgroundBmp = (HBITMAP)LoadImage(NULL, L"milionerzy.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    // TEST: Jeśli wyskoczy to okienko, to znaczy że ścieżka do pliku jest zla
    if (hBackgroundBmp == NULL) {
        MessageBox(hWnd, L"Błąd: Nie można załadować pliku milionerzy.bmp!", L"Problem z plikiem", MB_OK | MB_ICONERROR);
    }


    // Odtwarzanie muzyki w pętli (async, loop = w tle i zapetlona)
    PlaySound(L"audio.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

    // inicjalizacja przycisków
    InitButtons();

    // czcionka pytania
    hFontQuestion = CreateFontW(
        32,                 // wysokosc w pikselach
        0,                  // szerokosc – 0 = automatyczna
        0, 0,               // escapement, orientation
        FW_BOLD,            // pogrubienie
        FALSE, FALSE, FALSE,// italic, underline, strikeout
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Arial"
    );

    // czcionka przycisków
    hFontButton = CreateFontW(
        28,
        0,
        0, 0,
        FW_NORMAL,
        FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Arial"
    );

    // zaladowanie pytania
    LoadQuestion(currentQuestion);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) { // pobiera komunikat z kolejki (np. kliknięcie, naciśnięcie klawisza)
        TranslateMessage(&msg); // tlumaczy komunikaty klawiatury na odpowiedni format
        DispatchMessage(&msg); // wysyła komunikat do WndProc
    }
    return (int)msg.wParam; // wraca kod wyjścia aplikacji
}

// Funkcja obsluguje wszystkie komunikaty okna
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // tlo
        if (hBackgroundBmp) {
            HDC hMemDC = CreateCompatibleDC(hdc);
            HGDIOBJ oldBmp = SelectObject(hMemDC, hBackgroundBmp);

            BITMAP bitmap;
            GetObject(hBackgroundBmp, sizeof(bitmap), &bitmap);

            // rozciągamy zdjecie w tle do rozmiarow okna
            StretchBlt(hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, hMemDC, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

            SelectObject(hMemDC, oldBmp);
            DeleteDC(hMemDC);
        }
        else {
            // Jesli nie uda sie zaladowac zdjęcia, dajemy zapasowy kolor
            HBRUSH bg = CreateSolidBrush(RGB(0, 0, 80));
            FillRect(hdc, &ps.rcPaint, bg);
            DeleteObject(bg);
        }

        // ustawienie czcionki pytania
        HFONT oldFont = (HFONT)SelectObject(hdc, hFontQuestion);
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);

        // przygotowanie tekstu: "Pytanie za [kwota] zl: [tresc]"
        // winnings[levelsCount - 2 - currentQuestion] pobiera kwote z drabinki
        wchar_t questionFullText[512];
        swprintf_s(questionFullText, 512, L"Pytanie za %s zł:\n\n%s",
            winnings[levelsCount - 2 - currentQuestion],
            currentSessionQuestions[currentQuestion].text);

        RECT questionRect = {
            50,
            50,
            WINDOW_WIDTH - RIGHT_PANEL_WIDTH - 50,
            200
        };

        DrawTextW(
            hdc,
            questionFullText,
            -1,
            &questionRect,
            DT_WORDBREAK | DT_CENTER
        );

        // rysuje tu przyciski odpowiedzi
        for (int i = 0; i < 4; i++)
        {
            if (answerBlocked[i]) continue;

            COLORREF col = RGB(200, 200, 200); // default szary

            // logika kolorowania wybranego przycisku
            if (animationActive && i == selectedAnswer) {
                // faza 1: miganie na zolto po kliknieciu
                col = isBlinkingYellow ? RGB(255, 255, 0) : RGB(200, 200, 200);
            }
            else if (!animationActive && selectedAnswer != -1) {
                // faza 2: pokazywanie wynikiw (gdy miganie sie skonczy)
                int correctIdx = currentSessionQuestions[currentQuestion].correct;

                if (i == correctIdx) {
                    // poprawna ZAWSZE na zielono po zakonczeniu migania
                    col = RGB(0, 255, 0);
                }
                else if (i == selectedAnswer && selectedAnswer != correctIdx) {
                    // jesli gracz wybral zle, zaznaczamy jego wybor na czerwono
                    col = RGB(255, 0, 0);
                }
            }

            // WYWOŁANIE: Muszą być 3 argumenty! (hdc, struktura przycisku, kolor)
            DrawButton(hdc, answerButtons[i], col);
        }

        // Rysuj kola ratunkowe (bez zaznaczenia)
        for (int i = 0; i < 3; i++)
        {
            COLORREF color = RGB(200, 200, 200); // Standardowy szary

            if (i == 0 && lifeline5050Used) color = RGB(255, 0, 0);     // Czerwony dla 50/50
            if (i == 1 && lifelinePhoneUsed) color = RGB(255, 0, 0);    // Czerwony dla Telefonu
            if (i == 2 && lifelineAudienceUsed) color = RGB(255, 0, 0); // Czerwony dla Publiczności

            DrawButton(hdc, lifelines[i], color);
        }

        // Rysowanie drabinki wygranych
        DrawWinnings(hdc);
        // --- RYSOWANIE SŁUPKÓW PUBLICZNOŚCI ---
        if (showAudienceGraph) {
            int graphX = 60; 
            int graphY = 400;
            int barWidth = 30; 
            int maxBarHeight = 120;

            for (int i = 0; i < 4; i++) {
                int barHeight = (audienceResults[i] * maxBarHeight) / 100;
                RECT barRect = { graphX + (i * 100), graphY - barHeight, graphX + (i * 100) + barWidth, graphY };

                HBRUSH hBarBrush = CreateSolidBrush(RGB(255, 215, 0));
                FillRect(hdc, &barRect, hBarBrush);
                DeleteObject(hBarBrush);

                wchar_t label[16];
                swprintf_s(label, 16, L"%c:%d%%", L'A' + i, audienceResults[i]);
                TextOut(hdc, graphX + (i * 100), graphY + 5, label, lstrlenW(label));
            }
        }

        SelectObject(hdc, oldFont);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_LBUTTONDOWN:
    {
        if (animationActive) return 0; // blokada, jesli trwa animacja

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        for (int i = 0; i < 4; i++)
        {
            if (IsInside(answerButtons[i].rect, x, y) && !answerBlocked[i])
            {
                // 50/50
                if (answerBlocked[i]) {
                    return 0; // NIE DA SIĘ KLIKNĄĆ
                }

                selectedAnswer = i;       // zaznaczenie
                animationActive = true;
                blinkingCount = 8; // 8 zmian kolorów
                isBlinkingYellow = true;

                // uruchamiamy timer
                SetTimer(hWnd, 1, 400, NULL);
                InvalidateRect(hWnd, nullptr, TRUE);
                return 0;
            }
        }

        for (int i = 0; i < 3; i++)
        {
            if (IsInside(lifelines[0].rect, x, y) && !lifeline5050Used)
            {
                Use5050();
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            else if (IsInside(lifelines[1].rect, x, y) && !lifelinePhoneUsed)
            {
                UsePhone(hWnd);
                InvalidateRect(hWnd, nullptr, TRUE);
            }
            else if (IsInside(lifelines[2].rect, x, y) && !lifelineAudienceUsed)
            {
                UseAudience();
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
        break;
    }
    case WM_TIMER:
    {
        if (wParam == 1) {
            if (blinkingCount > 0) {
                isBlinkingYellow = !isBlinkingYellow;
                blinkingCount--;
                InvalidateRect(hWnd, NULL, TRUE);
            }
            else {
                KillTimer(hWnd, 1);
                // wylaczam tryb animacji migania (żółty)
                animationActive = false;

                // oodswiezenie (WM_PAINT teraz użyje logiki zielony/czerwony)
                InvalidateRect(hWnd, NULL, TRUE);

                // wymuszenie rysowania (bez tego Sleep zablokuje zmiany)
                UpdateWindow(hWnd);
                Sleep(2000);

                // sprawdzamy wynik
                if (selectedAnswer == currentSessionQuestions[currentQuestion].correct) {
                    MessageBox(hWnd, L"Dobrze!", L"Wynik", MB_OK);
                    currentQuestion++;
                    if (currentQuestion >= 12) {
                        MessageBox(hWnd, L"GRATULACJE! Zostałeś Milionerem!", L"Wygrana", MB_OK);
                        ResetGame();
                    }
                }
                else {
                    const wchar_t* finalPrize = L"0";
                    if (currentQuestion >= 7) finalPrize = L"40 000";
                    else if (currentQuestion >= 2) finalPrize = L"1 000";

                    wchar_t loseMsg[128];
                    swprintf_s(loseMsg, 128, L"Źle! Koniec gry. Twoja wygrana to: %s zł.", finalPrize);
                    MessageBox(hWnd, loseMsg, L"Przegrana", MB_OK);
                    ResetGame();
                }

                // Reset stanu gry pod nowe pytanie
                showAudienceGraph = false;
                for (int i = 0; i < 4; i++) answerBlocked[i] = false;
                selectedAnswer = -1;
                LoadQuestion(currentQuestion);
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}