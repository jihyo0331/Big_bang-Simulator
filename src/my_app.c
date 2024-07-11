#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// 창 크기 설정
const int screenWidth = 1200;
const int screenHeight = 720;

bool enterPress = false;  // StartApp 엔터 입력 여부
bool gameOver = false; // 게임 종료 여부
bool showText = true; // 텍스트 표시 여부
bool mouseClicked = false;  // 마우스 클릭 여부
bool timerRunning = true;  // 타이머 실행 여부
bool showExplanation = false; // 사각형 표시 여부

float gameTimer = 0.00f; // 타이머 전역변수
float blinkTimer = 0.0f; // 텍스트 깜빡이는 타이머
float blinkInterval = 0.5f; // 깜빡임 간격 (초)

// 함수 원형
void UpdateTimers();
void PrintTimer(float seconds, char* buffer);
void StartSimualtor();
void StartApp();
void Explanation();

int main(void)
{
    // 윈도우 초기화
    InitWindow(screenWidth, screenHeight, "BIGBANG_Simulator");
    SetTargetFPS(30); // 목표 FPS를 30으로 설정

    // 메인 게임 루프
    while (!WindowShouldClose()) // ESC 키 또는 창 닫기 버튼을 누를 때까지 실행
    {
        BeginDrawing();
        StartApp();
        EndDrawing();
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

void Explanation(){
    if(showExplanation){
        if(gameTimer <= 3){
            DrawRectangle(200, 300, 200, 100, Fade(RAYWHITE, 0.5f));
            //DrawRectangleLines(200, 300, 200, 100, GRAY);
            DrawRectangleLinesEx((Rectangle){200, 300, 205, 105}, 5, DARKGRAY);  //테두리 추가
        }
        
    }
}

// 시뮬레이터 스타트 함수
void StartSimualtor(){
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) mouseClicked = true;
    if (mouseClicked) {
        if (IsKeyPressed(KEY_SPACE)) {
            timerRunning = !timerRunning; // 스페이스바 눌러 타이머 토글
            showExplanation = !showExplanation; // 스페이스바 눌러 사각형 표시 여부 토글
        }
        UpdateTimers();
        ClearBackground(BLACK);
        char timerText[10]; // 타이머 문자열 버퍼
        PrintTimer(gameTimer, timerText);
        DrawText(timerText, 1000, 27, 40, GRAY);

        Explanation(); // 사각형 그리기
    }
    else {
        ClearBackground(BLACK); // 배경색을 RAYWHITE로 설정
        DrawText("Click to start BIGBANG", screenWidth / 2 - MeasureText("Click to start BIGBANG", 40) / 2, screenHeight / 2 - 20, 40, LIGHTGRAY);
    }
}

// 앱 시작 함수
void StartApp(){
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
        StartSimualtor();
    }
}
