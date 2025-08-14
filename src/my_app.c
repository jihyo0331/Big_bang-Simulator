#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

// 창 크기 설정
const int screenWidth = 1200;
const int screenHeight = 720;

bool enterPress = false;  // StartApp 엔터 입력 여부
bool gameOver = false; // 게임 종료 여부
bool showText = true; // 텍스트 표시 여부
bool mouseClicked = false;  // 마우스 클릭 여부
bool timerRunning = true;  // 타이머 실행 여부
bool showExplanation = false; // 사각형 표시 여부
bool spacePress = false;   // Space KEY Press

float gameTimer = 0.0f; // 타이머 전역변수
float blinkTimer = 0.0f; // 텍스트 깜빡이는 타이머
float blinkInterval = 0.5f; // 깜빡임 간격 (초)

typedef struct {
    Vector2 position;
    Vector2 speed;
    Vector2 initialSpeed;
    Color color;
} Quark;

typedef struct {
    Vector2 position;
    Vector2 speed;
    Vector2 initialSpeed;
    Color color;
} Electron;

typedef struct {
    Vector2 position;
    Vector2 speed;
    Color color;
} Nucleons;

typedef struct {
    Vector2 position;
    Vector2 speed;
    Color color;
} He;

typedef struct {
    Vector2 position;
    float theta;  // 각도 (라디안)
    float radiusX; // 타원의 반지름 (X축)
    float radiusY; // 타원의 반지름 (Y축)
    Vector2* center; // 타원의 중심 (포인터로 수정)
    Color color;
} OrbitingCircle;

Color red = (Color) { 222, 78, 78, 255 };
Color gb = (Color) { 101, 184, 199, 255 };

Quark quarks[100];     // 쿼크 최대 생성수
int numQuarks = 100;
Electron electrons[100];     // 전자 최대 생성수
Nucleons nucleons[40];    // 양성자와 중성자 최대 생성수
He he[5];      //헬륨 최대 생성수
OrbitingCircle orbitingCircles[90]; // 모든 궤도를 따라 움직이는 원의 배열
int numHe = 5;
int numNucleons = 40;
int numElectrons = 100;
int numOrbitingCircles = 0; // 궤도를 따라 움직이는 원의 개수
float slowdownRate = 0.98f;  // 감소 비율 (천천히 느려지도록)
float minSpeed = 1.0f;  // 최소 속도 (초기 속도가 더 빠르므로 증가)
bool quarksInitialized = false; // 쿼크 초기화 여부
bool electronsInitialized = false;   // 전자 초기화 여부
bool nucleonsInitialized = false;    // 양성자, 중성자 초기화 여부
bool heInitialized = false;    // 헬륨 원자핵 초기화 여부
bool heElectronsOrbitInitialized = false;    //헬륨 원자 궤도 초기화 여부
bool hElctronsOrbitInitialized =false;      //수소 원자 궤도 초기화 여부
bool decreaseNucleonsInitialized = false;    //양성자, 중성자 감소 여부
bool orbitingCircleInitialized = false;    // 궤도 원 초기화 여부

// 함수 원형
void UpdateTimers();
void PrintTimer(float seconds, char* buffer);
void StartSimulator();     // 시뮬레이터 시작 함수
void StartApp();        // 메인 화면 함수
void Explanation();     // 스페이스 누르면 나오는 설명 그리는 함수
void BigbangAnimation();    // 빅뱅
void InitializeQuarks(); // 쿼크 초기화
void DrawQuarks();     // 쿼크 그리는 함수
void UpdateQuarkSpeeds(); // 쿼크 속도 업데이트
void InitializeElectrons(); // 전자 초기화
void DrawElectrons();     // 전자 그리는 함수
void UpdateElectronSpeeds(); // 전자 속도 업데이트
void DecreaseQuarkCount(); // 쿼크 개수 줄이기
void InitializeNucleons();   // 양성자와 중성자 초기화 함수
void DrawNucleons();    // 양성자와 중성자 그리는 함수
void DecreaseNucleonsCount();   // 양성자와 중성자 개수 줄이기 함수
void InitializeHe();//헬륨 원자핵 초기화
void DrawHe(); // 헬륨 원자핵 그리기 함수
void InitializeOrbitingCircles(); // 궤도 원 초기화 함수
void UpdateOrbitingCircles(); // 궤도 원 업데이트 함수
void DrawOrbitingCircles(); // 궤도 원 그리기 함수

int main(void)
{
    // 윈도우 초기화
    InitWindow(screenWidth, screenHeight, "BIGBANG_Simulator");
    SetTargetFPS(60); // 목표 FPS를 60으로 설정

    // 메인 게임 루프
    while (!WindowShouldClose()) // ESC 키 또는 창 닫기 버튼을 누를 때까지 실행
    {
        if (gameTimer > 0.2f && IsKeyPressed(KEY_LEFT)) {     // 왼쪽 화살표를 누르면 타이머 뒤로감기
            gameTimer -= 0.2f;     // 0.2초씩 감소
        }
        else if (IsKeyPressed(KEY_RIGHT)) {      // 오른쪽 화살표 누르면 타이머 앞으로 감기
            gameTimer += 0.2f;     // 0.2초씩 증가
        }
        BeginDrawing();    // 메인 루프 내 그리기 시작 함수
        StartApp();
        EndDrawing();     // 그리기 종료
    }

    // 윈도우 종료
    CloseWindow();

    return 0;
}

// 타이머 업데이트 함수
void UpdateTimers() {
    if (timerRunning) {
        float deltaTime = GetFrameTime();
        gameTimer += deltaTime;
    }
}

// 타이머 문자열 생성 함수
void PrintTimer(float seconds, char* buffer) {
    int minutes = (int)seconds / 60;
    int secs = (int)seconds % 60;
    int hundredths = (int)((seconds - (minutes * 60) - secs) * 100);
    sprintf(buffer, "%02d:%02d:%02d", minutes, secs, hundredths);
}

void Explanation() {
    if (showExplanation) {
        if (gameTimer <= 3) {
            DrawRectangle(200, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            DrawRectangleLinesEx((Rectangle) { 200, 300, 205, 105 }, 5, DARKGRAY);  // 테두리 추가
            DrawText("BIG BANG!!!\nThe birth of fundamental\nparticles such as quarks\nand electrons", 212, 315, 15, BLACK);
        }
        else if (gameTimer > 3 && gameTimer <= 6.6) {
            DrawRectangle(400, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            DrawRectangleLinesEx((Rectangle) { 400, 300, 205, 105 }, 5, DARKGRAY);  // 테두리 추가
            DrawText("The speed of electrons\nand quarks is gradually\ndecreasing.", 412, 315, 15, BLACK);
        }
        else if(gameTimer >= 5.5f && gameTimer < 7.5f){
            DrawRectangle(600, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            DrawRectangleLinesEx((Rectangle) { 600, 300, 205, 105 }, 5, DARKGRAY);  // 테두리 추가
            DrawText("Quarks combine\nto form protons\nand neutrons.", 612, 315, 15, BLACK);
        }
        else if(gameTimer >= 7.5f && gameTimer < 11.0f){
            DrawRectangle(500, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            DrawRectangleLinesEx((Rectangle) { 500, 300, 205, 105 }, 5, DARKGRAY);  // 테두리 추가
            DrawText("Protons and neutrons\ncombine to form\na helium nucleus.", 512, 315, 15, BLACK);
        }
        else if(gameTimer >= 11.0f){
            DrawRectangle(700, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            DrawRectangleLinesEx((Rectangle) { 700, 300, 205, 105 }, 5, DARKGRAY);  // 테두리 추가
            DrawText("Helium and hydrogen\nuclei become atoms!", 712, 315, 15, BLACK);
        }

    }
}

// 쿼크 초기화 함수
void InitializeQuarks() {
    if (!spacePress) {
        srand(time(NULL));
        for (int i = 0; i < 100; i++) {
            quarks[i].position = (Vector2){ rand() % screenWidth, rand() % screenHeight };
            quarks[i].initialSpeed = (Vector2){ (rand() % 2 == 0 ? 1 : -1) * 20, (rand() % 2 == 0 ? 1 : -1) * 20 }; // 초기 빠른 속도
            quarks[i].speed = quarks[i].initialSpeed;
            quarks[i].color = (rand() % 2) == 0 ? GREEN : PURPLE;
        }
        quarksInitialized = true;
    }
}

// 쿼크 속도 업데이트 함수
void UpdateQuarkSpeeds() {
    if (!spacePress) {
        if (gameTimer >= 1.7f && quarksInitialized) {
            for (int i = 0; i < numQuarks; i++) {
                quarks[i].speed.x *= slowdownRate;
                quarks[i].speed.y *= slowdownRate;

                if (fabs(quarks[i].speed.x) < minSpeed) quarks[i].speed.x = (quarks[i].speed.x < 0 ? -1 : 1) * minSpeed;
                if (fabs(quarks[i].speed.y) < minSpeed) quarks[i].speed.y = (quarks[i].speed.y < 0 ? -1 : 1) * minSpeed;
            }
        }
    }
}

// 쿼크 그리기 및 업데이트 함수
void DrawQuarks() {
    if (gameTimer >= 1.7f && quarksInitialized) {
        for (int i = 0; i < numQuarks; i++) {
            if (!spacePress) {
                quarks[i].position.x += quarks[i].speed.x;
                quarks[i].position.y += quarks[i].speed.y;

                // 화면 경계에 도달하면 반대 방향으로 이동
                if (quarks[i].position.x <= 0 || quarks[i].position.x >= screenWidth) quarks[i].speed.x *= -1;
                if (quarks[i].position.y <= 0 || quarks[i].position.y >= screenHeight) quarks[i].speed.y *= -1;
            }
            DrawCircleV(quarks[i].position, 4, quarks[i].color);
        }
    }
}

// 전자 초기화 함수
void InitializeElectrons() {
    if (!spacePress) {
        srand(time(NULL));
        for (int i = 0; i < 100; i++) {
            electrons[i].position = (Vector2){ rand() % screenWidth, rand() % screenHeight };
            electrons[i].initialSpeed = (Vector2){ (rand() % 2 == 0 ? 1 : -1) * 20, (rand() % 2 == 0 ? 1 : -1) * 20 }; // 초기 빠른 속도
            electrons[i].speed = electrons[i].initialSpeed;
            electrons[i].color = YELLOW;
        }
        electronsInitialized = true;
    }
}

// 전자 속도 업데이트 함수
void UpdateElectronSpeeds() {
    if (!spacePress) {
        if (gameTimer >= 1.7f && electronsInitialized) {
            for (int i = 0; i < numElectrons; i++) {
                electrons[i].speed.x *= slowdownRate;
                electrons[i].speed.y *= slowdownRate;

                if (fabs(electrons[i].speed.x) < minSpeed) electrons[i].speed.x = (electrons[i].speed.x < 0 ? -1 : 1) * minSpeed;
                if (fabs(electrons[i].speed.y) < minSpeed) electrons[i].speed.y = (electrons[i].speed.y < 0 ? -1 : 1) * minSpeed;
            }
        }
    }
}

// 전자 그리기 및 업데이트 함수
void DrawElectrons() {
    if (gameTimer >= 1.7f && electronsInitialized) {
        for (int i = 0; i < numElectrons; i++) {
            if (!spacePress) {
                electrons[i].position.x += electrons[i].speed.x;
                electrons[i].position.y += electrons[i].speed.y;

                // 화면 경계에 도달하면 반대 방향으로 이동
                if (electrons[i].position.x <= 0 || electrons[i].position.x >= screenWidth) electrons[i].speed.x *= -1;
                if (electrons[i].position.y <= 0 || electrons[i].position.y >= screenHeight) electrons[i].speed.y *= -1;
            }
            DrawCircleV(electrons[i].position, 5, electrons[i].color);
        }
    }
}

// 양성자, 중성자 초기화 함수
void InitializeNucleons() {
    if (!spacePress) {
        srand(time(NULL));
        for (int i = 0; i < 40; i++) {
            nucleons[i].position = (Vector2){ rand() % screenWidth, rand() % screenHeight };
            nucleons[i].speed = (Vector2){ 1.0f,1.0f, };
            nucleons[i].color = (rand() % 2) == 0 ?  red : gb;
        }
        nucleonsInitialized = true;
    }
}

// 양성자, 중성자 그리기 및 업데이트 함수
void DrawNucleons() {
    if (gameTimer >= 5.5f && nucleonsInitialized) {
        for (int i = 0; i < numNucleons; i++) {
            if (!spacePress) {
                nucleons[i].position.x += nucleons[i].speed.x;
                nucleons[i].position.y += nucleons[i].speed.y;

                // 화면 경계에 도달하면 반대 방향으로 이동
                if (nucleons[i].position.x <= 0 || nucleons[i].position.x >= screenWidth) nucleons[i].speed.x *= -1;
                if (nucleons[i].position.y <= 0 || nucleons[i].position.y >= screenHeight) nucleons[i].speed.y *= -1;
            }
            DrawCircleV(nucleons[i].position, 12, Fade(nucleons[i].color, 0.2f + (gameTimer - 5.2f)));
            if (decreaseNucleonsInitialized){
                if(nucleons[i].color.r == 222 && gameTimer > 11.0f){
                    DrawEllipseLines(nucleons[i].position.x,nucleons[i].position.y, 60,30,WHITE);
                }
            }
        }
    }
}

// 양성자, 중성자 감소 함수
void DecreaseNucleonsCount() {
    int minNucleons = 5;  // 최소 남겨둘 양성자 및 중성자 수
    int particlesToDeactivate = (int)((gameTimer - 6.6f) / (7.5f - 6.6f) * (40 - minNucleons));  // 비활성화할 양성자, 중성자 수 계산

    if (numNucleons > minNucleons + particlesToDeactivate) {
        numNucleons -= particlesToDeactivate;
    }
    else {
        numNucleons = minNucleons;
    }
    decreaseNucleonsInitialized = true;
}

// 헬륨원자핵 초기화 함수
void InitializeHe() {
    if (!heInitialized) {
        srand(time(NULL));
        for (int i = 0; i < 5; i++) {
            he[i].position = (Vector2){ rand() % screenWidth, rand() % screenHeight };
            he[i].speed = (Vector2){ (rand() % 2 == 0 ? 1 : -1) * 50, (rand() % 2 == 0 ? 1 : -1) * 50 };
            he[i].color = (rand() % 2) == 0 ? (Color) { 222, 78, 78, 255 } : (Color) { 101, 184, 199, 255 };
        }
        heInitialized = true;
    }
}

// 헬륨원자핵 그리기 및 업데이트 함수
void DrawHe() {
    if (gameTimer >= 7.5f && heInitialized) {
        for (int i = 0; i < 5; i++) {
            if (!spacePress) {
                he[i].position.x += he[i].speed.x * GetFrameTime();
                he[i].position.y += he[i].speed.y * GetFrameTime();

                // 화면 경계에 도달하면 반대 방향으로 이동
                if (he[i].position.x <= 0 || he[i].position.x >= screenWidth) he[i].speed.x *= -1;
                if (he[i].position.y <= 0 || he[i].position.y >= screenHeight) he[i].speed.y *= -1;
            }

            Vector2 offset1 = { he[i].position.x + 10, he[i].position.y };
            Vector2 offset2 = { he[i].position.x - 10, he[i].position.y };
            Vector2 offset3 = { he[i].position.x, he[i].position.y + 10 };
            Vector2 offset4 = { he[i].position.x, he[i].position.y - 10 };

            if (he[i].color.r == 222) { // 빨간색일 때
                DrawCircleV(he[i].position, 12, he[i].color);
                DrawCircleV(offset1, 12, (Color) { 101, 184, 199, 255 });
                DrawCircleV(offset2, 12, (Color) { 101, 184, 199, 255 });
                DrawCircleV(offset3, 12, he[i].color);
                DrawCircleV(offset4, 12, he[i].color);
            }
            else { // 파란색일 때
                DrawCircleV(offset1, 12, he[i].color);
                DrawCircleV(offset2, 12, he[i].color);
                DrawCircleV(offset3, 12, (Color) { 222, 78, 78, 255 });
                DrawCircleV(offset4, 12, (Color) { 222, 78, 78, 255 });
            }
            if(gameTimer > 11.0f){
                DrawEllipseLines(he[i].position.x,he[i].position.y, 60,30,WHITE);
                DrawEllipseLines(he[i].position.x,he[i].position.y, 30,60,WHITE);
            }
        }
    }
}

// 궤도 전자 초기화 함수
void InitializeOrbitingCircles() {
    if (!orbitingCircleInitialized) {
        for (int i = 0; i < 5; i++) {
            orbitingCircles[numOrbitingCircles].center = &he[i].position;
            orbitingCircles[numOrbitingCircles].radiusX = 60;
            orbitingCircles[numOrbitingCircles].radiusY = 30;
            orbitingCircles[numOrbitingCircles].theta = 0.0f;
            orbitingCircles[numOrbitingCircles].color = YELLOW;
            numOrbitingCircles++;

            orbitingCircles[numOrbitingCircles].center = &he[i].position;
            orbitingCircles[numOrbitingCircles].radiusX = 30;
            orbitingCircles[numOrbitingCircles].radiusY = 60;
            orbitingCircles[numOrbitingCircles].theta = 0.0f;
            orbitingCircles[numOrbitingCircles].color = YELLOW;
            numOrbitingCircles++;
        }
        for (int i = 0; i < 40; i++) {
            if (nucleons[i].color.r == 222) { // 빨간색일 때만 궤도 추가
                orbitingCircles[numOrbitingCircles].center = &nucleons[i].position;
                orbitingCircles[numOrbitingCircles].radiusX = 60;
                orbitingCircles[numOrbitingCircles].radiusY = 30;
                orbitingCircles[numOrbitingCircles].theta = 0.0f;
                orbitingCircles[numOrbitingCircles].color = YELLOW;
                numOrbitingCircles++;
            }
        }
        orbitingCircleInitialized = true;
    }
}

// 궤도 전자 업데이트 함수
void UpdateOrbitingCircles() {
    if (!spacePress) {
        if (orbitingCircleInitialized) {
            for (int i = 0; i < numOrbitingCircles; i++) {
                orbitingCircles[i].theta += GetFrameTime(); // 각도를 프레임 시간에 따라 증가
                orbitingCircles[i].position.x = orbitingCircles[i].center->x + orbitingCircles[i].radiusX * cos(orbitingCircles[i].theta);
                orbitingCircles[i].position.y = orbitingCircles[i].center->y + orbitingCircles[i].radiusY * sin(orbitingCircles[i].theta);
            }
        }
    }
}

// 궤도 전자 그리기 함수
void DrawOrbitingCircles() {
    if (orbitingCircleInitialized) {
        for (int i = 0; i < numOrbitingCircles; i++) {
            DrawCircleV(orbitingCircles[i].position, 5, orbitingCircles[i].color);
        }
    }
}

// 빅뱅 애니메이션 시작
void BigbangAnimation() {
    float radius = 10 * powf(gameTimer, 4);
    float fade = gameTimer >= 1.5 ? 3 : 0;
    int color = 0;
    DrawCircle(100, 300, 10 * radius * 10, Fade(RAYWHITE, 2.0f - gameTimer));
    DrawCircle(100, 300, 10 * radius, Fade(color == 0 ? DARKPURPLE : RAYWHITE, 2.0f - gameTimer));
    DrawCircleLines(100, 300, 10 * radius, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 10 * radius + 1, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 10 * radius + 2, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 10 * radius + 3, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 10 * radius + 4, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 10 * radius + 5, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 9.5 * radius, Fade(DARKBLUE, fade - gameTimer));
    DrawCircleLines(100, 300, 8.5 * radius, Fade(PURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 7.5 * radius, Fade(DARKBLUE, fade - gameTimer));
    DrawCircleLines(100, 300, 11.5 * radius, Fade(PURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 6.5 * radius, Fade(RAYWHITE, fade - gameTimer));
    DrawCircleLines(100, 300, 5.5 * radius, Fade(ORANGE, fade - gameTimer));
    DrawCircleLines(100, 300, 4.5 * radius, Fade(BLUE, fade - gameTimer));
    DrawCircleLines(100, 300, 3.5 * radius, Fade(DARKBLUE, fade - gameTimer));
    DrawCircleLines(100, 300, 3.3 * radius, Fade(DARKPURPLE, fade - gameTimer));
    DrawCircleLines(100, 300, 3 * radius, Fade(RAYWHITE, fade - gameTimer));
}

// 쿼크 개수 줄이기 함수
void DecreaseQuarkCount() {
    int particlesToDeactivate = (int)((gameTimer - 6.6f) / (7.5f - 6.6f) * 100);  // 비활성화할 쿼크 수 계산

    if (numQuarks > particlesToDeactivate) {
        numQuarks -= particlesToDeactivate;
    }
    else {
        numQuarks = 0;
    }
}

// 시뮬레이터 스타트 함수
void StartSimulator() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mouseClicked = true;
    if (mouseClicked) {
        if (IsKeyPressed(KEY_SPACE)) {
            timerRunning = !timerRunning; // 스페이스바 눌러 타이머 토글
            showExplanation = !showExplanation; // 스페이스바 눌러 사각형 표시 여부 토글
            spacePress = !spacePress;
        }
        UpdateTimers();
        if (timerRunning && gameTimer >= 1.7f) {
            if (!quarksInitialized) InitializeQuarks();
            if (!electronsInitialized) InitializeElectrons();
            if (!nucleonsInitialized) InitializeNucleons();
        }
        if (timerRunning && quarksInitialized) UpdateQuarkSpeeds();
        if (timerRunning && electronsInitialized) UpdateElectronSpeeds();
        if (gameTimer >= 6.6f && gameTimer <= 7.5f) {
            DecreaseQuarkCount();  // 6.6초부터 7.5초 사이에 쿼크 수 줄이기
            DecreaseNucleonsCount();  // 6.6초부터 7.5초 사이에 양성자, 중성자 수 줄이기
        }
        if (7.5f < gameTimer && !heInitialized) InitializeHe();
        if (11.0f < gameTimer && !orbitingCircleInitialized) InitializeOrbitingCircles();
        if (orbitingCircleInitialized) UpdateOrbitingCircles();

        ClearBackground(BLACK);
        char timerText[10]; // 타이머 문자열 버퍼
        PrintTimer(gameTimer, timerText);
        DrawText(timerText, 1000, 27, 40, GRAY);
        BigbangAnimation();
        DrawQuarks(); // 쿼크 그리기
        DrawElectrons(); // 전자 그리기
        DrawNucleons();  // 양성자, 중성자 그리기
        DrawHe();  // 헬륨 원자핵 그리기
        DrawOrbitingCircles(); // 궤도 원 그리기
        Explanation(); // 사각형 그리기
    }
    else {
        ClearBackground(BLACK); // 배경색을 BLACK로 설정
        DrawText("Click to start BIGBANG", screenWidth / 2 - MeasureText("Click to start BIGBANG", 40) / 2, screenHeight / 2 - 20, 40, LIGHTGRAY);
    }
}

// 앱 시작 함수
void StartApp() {
    if (!enterPress) {
        if (IsKeyPressed(KEY_ENTER)) enterPress = true;
        float deltaTime = GetFrameTime();
        blinkTimer += deltaTime;
        if (blinkTimer >= blinkInterval) {
            blinkTimer = 0.0f;
            showText = !showText;
        }
        ClearBackground(BLACK);
        DrawText("BIGBANG Simulator", screenWidth / 2 - MeasureText("BIGBANG Simulator", 40) / 2, screenHeight / 2 - 30, 40, DARKBLUE);
        if (showText) {
            DrawText("Press Enter to start", screenWidth / 2 - MeasureText("Press Enter to start", 30) / 2, 380, 30, GREEN);
        }
    }
    else {
        StartSimulator();
    }
}
