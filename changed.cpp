
//**************************************************************//
//          Copyright (c) 2010  SangHyeon Kim					//
//			License : MIT										//
//			Title : 틀린그림 찾기								//
//																//
//**************************************************************//
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include<MMSystem.h>
#pragma comment(lib"Winmm.lib")
#include "item.h"
#include "Digitalv.h"

//
#define speed 8;
//


//플레이어 타일의 크기 26 * 28 //104*112 31*32  124*128 
//피츄 : 22 * 25 // 88*25 (0,128시작) // 숫자 : 6*10 리본 /16*15

//얼굴  : 144*97 (0, 173 시작)

// 전역 변수
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
LPSTR lpszClass = "잠자는 숲속의 피츄";
HWND hWnd;

HBITMAP hBit; // Back Buffer가 될 비트맵헤더.
HBITMAP hBit_map;	// 배경화면을 가리키는 비트맵헤더.
HBITMAP hBit_player;	// 플레이어 타일셋을 가리키는 비트맵 헤더
HBITMAP hBit_mt; //맵 타일(개발용)

MCI_OPEN_PARMS m_mciOpenParms;
MCI_PLAY_PARMS m_mciPlayParms;
DWORD m_dwDeviceID;
MCI_OPEN_PARMS mciOpen;
MCI_PLAY_PARMS mciPlay;

int dwID;
//?? 무슨 변수지?

int gameState = 0;
BOOL KeyBuffer[256];
//키버퍼

BOOL Hint = FALSE;

#define Playtime 1000;
int default_time = 35;
int limit_time = Playtime;
int start_time_point = 0;
//제한시간을 다루는 변수.. 1이상의 초기값을 주어야 돌아간다.

///아래 변수는 추후 변경해야할 변수(삭제 변경 예상)
//모두 맞췄는지 알아보는 변수
int checked_point = 0;

//제한 횟수 5회

int failed_point = 5;

//각각 맞췄는지 알아보는 변수@

bool istrue; //정답인 점을 누르고 있는지 아닌지 구별하는 것.
bool ending; //끝났음을 알림
			 ///위의 변수는 추후 변경해야할 변수(삭제 변경 예상)


			 //맵 블록.
Item gItem;


void calScore() {

	for (int i = 0; i <20; i++)
		for (int j = 0; j < 20; j++) {

			if (mapBlock[j][i] == 6) {
	
				mapBlock[j][i] = 5;
			}
			if (mapBlock[j][i] == 7) {
	
				mapBlock[j][i] = 5;
			}

			if (mapBlock[j][i] == 3) {

				mapBlock[j][i] = 3;
			}

		}


}

int LoadBmp(char* filename, HBITMAP* ArghBit)	// 그림파일을 읽어온다.
{

	HANDLE hFile;
	HBITMAP hBit;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER fh;
	BITMAPINFO * ih;
	PVOID pRaster;

	//파일을 연다.
	hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// 파일 헤더와 정보 구조체 (색상 테이블 포함 ) 를 읽어 들인다.
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwRead, NULL);
	FileSize = fh.bfOffBits - sizeof(BITMAPFILEHEADER);
	ih = (BITMAPINFO *)malloc(FileSize);
	ReadFile(hFile, ih, FileSize, &dwRead, NULL);

	//DIB 섹션을 만들고 버퍼 메모리를 할당한다.
	hBit = CreateDIBSection(NULL, ih, DIB_RGB_COLORS, &pRaster, NULL, 0);
	//래스터 데이터를 읽어들인다.
	ReadFile(hFile, pRaster, fh.bfSize - fh.bfOffBits, &dwRead, NULL);
	free(ih);
	CloseHandle(hFile);

	*ArghBit = hBit;

	return 1;
}



void WarpItem() {
	int l = rand() % 3;//정답 위치가 저장되어있는 곳중 아무곳이나 생성

	if (player[0].curTemp == 1)//1p가 1번 획득시 이동
	{
		player[0].posX = answer[l].posX + 128;//정답좌표로 이동로 이동
		player[0].posY = answer[l].posY + 128;
		gItem.visiableOnGame = false;
		player[0].curTemp = -1;//이동 후 아무것도 얻지 못한 -1상태로 
	}
	if (player[1].curTemp == 1)
	{
		player[1].posX = answer[l].posX + 128;
		player[1].posY = answer[l].posY + 128;
		gItem.visiableOnGame = false;
		player[1].curTemp = -1;
	}
}



void setFlaganswer(int x, int y, int input) {
	
	
	if (mapBlock[y][x] == 1) {
		if (mapBlock[y - 1][x - 1] == 1) {
			x = x;
			y = y;
		}
		else if (mapBlock[y + 1][x + 1] == 1) {
				x = x+1;
				y = y+1;
			}		
		else if (mapBlock[y - 1][x + 1] == 1) {
			x = x + 1;
			y = y;
		}
		else if (mapBlock[y + 1][x - 1] == 1) {
			x = x;
			y = y + 1;
		}


		mapBlock[y][x] = input;
		mapBlock[y][x - 1] = input;
		mapBlock[y - 1][x] = input;
		mapBlock[y - 1][x - 1] = input;


	}
	
	for (int i = 0; i < 3; i++) {

		if (answer[i].curTemp == x && answer[i].curStatus == y) {
			answer[i].score = input;
			mapBlock[y][x] = input;
			mapBlock[y][x - 1] = input;
			mapBlock[y - 1][x] = input;
			mapBlock[y - 1][x - 1] = input;

		}

	}

}

void checkScore() {

	for (int i = 0; i < 3; i++) {

		if (answer[i].score == 6) {
			player[0].score += 1;
			player[0].curTemp = 5;
			answer[i].score = 1;

		}
		else if (answer[i].score == 7) {
			player[1].score += 1;
			answer[i].score = 2;
		}
	

	}

	int check = 0;
	for (int i = 0; i < 3; i++) {

		if (flag[0][i].curTemp == 1) check++;
		if (flag[1][i].curTemp == 1) check++;
	}

	if (check > 5) gameState = 1;


}


void charMove(int &p_x, int &p_y, int head) {
	int temp = 1;
	
	if ((detectBlock(p_x, p_y) > 2)) temp = -1;

	switch (head) {

	case 1:
		p_x -= temp * speed;
		break;
	case 3:
		p_x += temp * speed;
		break;
	case 2:
		p_y -= temp * speed;
		break;
	case 0:
		p_y += temp * speed;
		break;

	}

	if (p_x < 40) p_x = 40;
	else if (p_y < 40) p_y = 40;
	else if (p_x > 600) p_x = 600;
	else if (p_y > 600) p_y = 600;

	if ((detectBlock(p_x, p_y) > 2)) charMove(p_x, p_y, head);
}



void loop() {

	if (KeyBuffer[VK_LEFT]) {
		player[0].curStatus = 1;
		charMove(player[0].posX, player[0].posY, player[0].curStatus);
	}
	if (KeyBuffer[VK_RIGHT]) {
		player[0].curStatus = 3;
		charMove(player[0].posX, player[0].posY, player[0].curStatus);
	}
	if (KeyBuffer[VK_UP]) {

		player[0].curStatus = 2;
		charMove(player[0].posX, player[0].posY, player[0].curStatus);
	}
	if (KeyBuffer[VK_DOWN]) {

		player[0].curStatus = 0;
		charMove(player[0].posX, player[0].posY, player[0].curStatus);
	}
	//1P 키보드 입력

	if (KeyBuffer[0x41] && player[1].curTemp != 0) {

		player[1].curStatus = 1;
		charMove(player[1].posX, player[1].posY, player[1].curStatus);
	}
	if (KeyBuffer[0x44] && player[1].curTemp != 0) {

		player[1].curStatus = 3;
		charMove(player[1].posX, player[1].posY, player[1].curStatus);

	}
	if (KeyBuffer[0x57] && player[1].curTemp != 0) {

		player[1].curStatus = 2;
		charMove(player[1].posX, player[1].posY, player[1].curStatus);

	}
	if (KeyBuffer[0x53] && player[1].curTemp != 0) {

		player[1].curStatus = 0;
		charMove(player[1].posX, player[1].posY, player[1].curStatus);
	}
	//2P 키보드 입력


	player[0].curBlock = detectBlock(player[0].posX, player[0].posY);
	player[1].curBlock = detectBlock(player[1].posX, player[1].posY);
	//플레이어가 현재 어떤 맵블록 위에 위치하는지 매번 계산해서 저장한다.



	//아이템 충돌. 어디둬야할지 몰라서 byLee
	if (gItem.visiableOnGame == true) {
		//itM=1p  itN=2p

		//-1,0,1반환//-1=아이템 안얻음  0=상대방 못움직에게 하는 템 얻음  1=정답근처로 워프템 얻음

		if (player[0].curBlock == 2) player[0].curTemp = gItem.getKindOfItem();
		if (player[1].curBlock == 2) player[1].curTemp = gItem.getKindOfItem();

		if (player[0].curTemp == 1 || player[1].curTemp == 1) {
			WarpItem();
		}
		if (player[0].curTemp != -1 || player[1].curTemp != -1) {
			gItem.visiableOnGame = false;
		}
	}


}


void stateChecker() {


	if (player[0].score > player[1].score) {
		player[0].curTemp = 5;
		player[1].curTemp = 6;
	}
	else if (player[0].score < player[1].score) {
		player[0].curTemp = 6;
		player[1].curTemp = 5;
	}
	else {	
		player[0].curTemp = 4;
		player[1].curTemp = 4;
	}

	
}



//현재 그림 상황
void Draw()
{
	
	if(gameState == 1) stateChecker();

	RECT crt;	// 현재 창의 정보를 읽어올 변수.
	HDC hdc, hMemDC, hMemDC2;	// 임시로 추가한 hMemDC2변수
	GetClientRect(hWnd, &crt);
	hdc = GetDC(hWnd);

	if (hBit == NULL) {
		hBit = CreateCompatibleBitmap(hdc, crt.right, crt.bottom);
	}


	HDC MemDCBack = CreateCompatibleDC(hdc);

	HBITMAP OldBit_Back = (HBITMAP)SelectObject(MemDCBack, hBit);

	hMemDC = CreateCompatibleDC(MemDCBack);
	hMemDC2 = CreateCompatibleDC(MemDCBack);



	// hMemDC를 캐릭터 비트맵 헤더에 물린다.
	SelectObject(hMemDC2, hBit_player);





	// 배경화면의 비트맵 헤더인 hBit_map을 아까 구한 hMemDC에 연결한다.
	HBITMAP OldBit_Obj = (HBITMAP)SelectObject(hMemDC, hBit_map);//hBit_map);


	BitBlt(MemDCBack, 0, 0, 640, 750, hMemDC, 0, 0, SRCCOPY);

	static int frame = 0;
	frame++;
	//주인공 움직임 주는 부분. 개선해야함


	static int b = 0;
	if (frame % 10 == 0) {
		b++;
		b = b % 4;
	}
	TransparentBlt(MemDCBack, player[0].posX - 26, player[0].posY - 28, 52, 56, hMemDC2, 26 * b, 28 * player[0].curStatus, 26, 28, RGB(100, 0, 100));
	//1p
	TransparentBlt(MemDCBack, player[1].posX - 26, player[1].posY - 28, 62, 64, hMemDC2, 104 + 31 * b, 32 * player[1].curStatus, 31, 32, RGB(100, 0, 100));
	//2p
	//주인공 움직임 주는 부분. 개선해야함


	// 22 * 25 // 88*25 (0,128시작) 29*20

	if (limit_time > 30) {
		for (int i = 0; i < 3; i++) {
			if (mapBlock[answer[i].curStatus][answer[i].curTemp] == 1) TransparentBlt(MemDCBack, 32 * answer[i].curTemp - 16  , 32 * answer[i].curStatus - 32, 44, 50, hMemDC2, b * 22, 128, 22, 25, RGB(100, 0, 100));
		}
	

	}

	//깃발 그리기 시작
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 3; j++) {
			if (flag[i][j].curTemp != 0) {
				if (flag[i][j].curStatus == 1) {
					TransparentBlt(MemDCBack, flag[i][j].posX, flag[i][j].posY - 48, 22, 48, hMemDC2, 167, 139, 22, 48, RGB(100, 0, 100));
					TransparentBlt(MemDCBack, flag[i][j].posX - 29, flag[i][j].posY - 20, 58, 40, hMemDC2, (b % 2) * 29, 153, 29, 20, RGB(100, 0, 100));
				}
				else TransparentBlt(MemDCBack, flag[i][j].posX, flag[i][j].posY - 48, 22, 48, hMemDC2, 145, 139, 22, 48, RGB(100, 0, 100));
			}
		}
	//깃발 그리기 끝


	//하단 표시 영역

	if (limit_time < 31 && limit_time>-1 && gameState == 0) {
		TransparentBlt(MemDCBack, 292, 670, 28, 40, hMemDC2, 87 + 6 * int(limit_time / 10), 128, 7, 10, RGB(100, 0, 100)); //129
		TransparentBlt(MemDCBack, 320, 670, 28, 40, hMemDC2, 87 + 6 * int(limit_time % 10), 128, 7, 10, RGB(100, 0, 100)); //129
	}
	else if (gameState == 1) TransparentBlt(MemDCBack, 272, 670, 100, 36, hMemDC2, 145, 216, 82, 31, RGB(100, 0, 100));
	else TransparentBlt(MemDCBack, 292, 670, 60, 39, hMemDC2, 145, 187, 45, 29, RGB(100, 0, 100)); //준비
	
	
	
	//숫자 표시


	//얼굴 표시


	if (player[0].curTemp == 5) {
		TransparentBlt(MemDCBack, 0, 640, 100, 100, hMemDC2, 48, 173, 48, 48, RGB(100, 0, 100));
		if (b % 40 == 0 && gameState == 0) player[0].curTemp = -1;
	}
	else if(player[0].curTemp == 6) TransparentBlt(MemDCBack, 0, 640, 100, 100, hMemDC2, 96, 173, 48, 48, RGB(100, 0, 100));
	else TransparentBlt(MemDCBack, 0, 640, 100, 100, hMemDC2, 0, 173, 48, 48, RGB(100, 0, 100));
	//피카츄


	

	if (player[1].curTemp == 5) {
		TransparentBlt(MemDCBack, 540, 640, 100, 100, hMemDC2, 48, 221, 48, 48, RGB(100, 0, 100));
		if (b % 40 == 0 && gameState == 0) player[1].curTemp = -1;
	}
	else if (player[1].curTemp == 6) TransparentBlt(MemDCBack, 540, 640, 100, 100, hMemDC2, 96, 221, 48, 48, RGB(100, 0, 100));
	else TransparentBlt(MemDCBack, 540, 640, 100, 100, hMemDC2, 0, 221, 48, 48, RGB(100, 0, 100));


	//라이츄
	//얼굴 표시


	for (int i = 0; i < player[0].score; i++) {
		TransparentBlt(MemDCBack, 120 + 42 * i, 675, 32, 30, hMemDC2, 113, 139, 16, 15, RGB(100, 0, 100));
	}
	for (int i = 0; i < player[1].score; i++) {
		TransparentBlt(MemDCBack, 490 - 42 * i, 675, 32, 30, hMemDC2, 129, 139, 16, 15, RGB(100, 0, 100));
	}
			
	
	// 리본표시


	//하단 표시 영역
	
	
	//선물상자 표시
	if (gItem.visiableOnGame == true) {

		TransparentBlt(MemDCBack, gItem.posX * 32 -9, gItem.posY * 32 - 8, 50, 48, hMemDC2, 88, 139, 25, 24, RGB(100, 0, 100));
	}

	//물체표시


	// hMemDC에 물려있던 비트맵 헤더를 초기값으로 되돌리고 소멸시킨다.
	SelectObject(hMemDC, OldBit_Obj);

	DeleteDC(hMemDC);
	DeleteDC(hMemDC2);


	// 백버퍼 DC도 원래 초기값으로 되돌리고 소멸시킨다.
	SelectObject(MemDCBack, OldBit_Back);
	DeleteDC(MemDCBack);

	// 아버지뻘 되는 dc였던 hdc를 해제시킨다.
	ReleaseDC(hWnd, hdc);
	//InvalidateRect(hWndMain,NULL,FALSE);

}


void Flip()
{

	HDC hdc = GetDC(hWnd);
	HDC hMemDC = CreateCompatibleDC(hdc);

	RECT crt;
	GetClientRect(hWnd, &crt);


	HBITMAP OldBit = (HBITMAP)SelectObject(hMemDC, hBit);

	BitBlt(hdc, 0, 0, crt.right, crt.bottom, hMemDC, 0, 0, SRCCOPY);

	// 여기는 그냥 해제하는 부분.
	SelectObject(hMemDC, OldBit);

	DeleteDC(hMemDC);
	ReleaseDC(hWnd, hdc);
}




void process()
{	
	if (player[0].score >1 || player[1].score > 1 || limit_time < 1) gameState = 1;
	
	calScore();
	if ((limit_time<31 && limit_time > -1) && gameState == 0) loop();
	Draw();	// 실제적으로 그리는 부분.
	Flip();	// Draw에서 Back Buffer로 그림들을 그렸기 때문에, 여기서 화면과 Back Buffer를 바꿔치기(Flip)하는 부분이다.
}


void init_game() {//게임을 시작할 때 각 요소를 초기화 한다.

	gameState = 0;
	default_time = 31;
	start_time_point = timeGetTime();

	player[0] = {0, };
	player[1] = {0, };
	

	player[0].posX = 290;
	player[0].posY = 605;
	player[0].curStatus = 2;
	player[0].curTemp = -1;

	player[1].posX = 350;
	player[1].posY = 600;
	player[1].curStatus = 2;
	player[1].curTemp = -1;

	//1P 2P시작 위치(추후 재 설정), 애니메이션 시작 초기화.



	for (int i = 0; i < flag_count; i++) {

		flag[0][i].curTemp = 0;
		flag[0][i].posX = 0;
		flag[0][i].posY = 0;
		flag[0][i].curStatus = 0;


		flag[1][i].curTemp = 0;
		flag[1][i].posX = 0;
		flag[1][i].posY = 0;
		flag[1][i].curStatus = 0;

	}

	//깃발 내역 초기화


	for (int i = 0; i < 20; i++)
		for (int j = 0; j < 20; j++) {
			if ((i < 8 || i>11) && j == 0)  mapBlock[j][i] = 5;
			else if ((i < 2 || i>17) && (j < 5 || j >13)) mapBlock[j][i] = 5;
			else if ((i > 17) && (j > 4 && j < 7)) mapBlock[j][i] = 5;
			else if (((i > 1 && i < 6) || (i > 13 && i < 18)) && j > 17) mapBlock[j][i] = 5;
			else mapBlock[j][i] = 0;
		}
	//

	//맵타일 초기화 
	srand(unsigned int(time(NULL)));

	static int m_nNum[16];

	for (int i = 0; i < 16; i++) {
		if (i < 3) m_nNum[i] = 1;
		else m_nNum[i] = 0;
	}

	int nDust, nSour, nTemp;

	for (int i = 0; i < 32; i++) {

		nDust = rand() % 16;
		nSour = rand() % 16;

		nTemp = m_nNum[nDust];
		m_nNum[nDust] = m_nNum[nSour];
		m_nNum[nSour] = nTemp;

	}
	int temp = 2;
	for (int i = 0; i < 16; i++) {

		if (m_nNum[i] == 1) {
			int x, y;
			x = int(i / 4);
			y = int(i % 4);
			answer[temp].posX = 128 * x;
			answer[temp].posY = 128 * y;

			x = 4 + 4 * x;
			y = 4 + 4 * y;

			answer[temp].curTemp = x;
			answer[temp].curStatus = y;

			mapBlock[y][x] = 1;
			mapBlock[y][x-1] = 1;
			mapBlock[y - 1][x] = 1;
			mapBlock[y - 1][x - 1] = 1;



			temp--;
		}
	}

	//보물의 위치를 랜덤으로 섞는 부분

	gItem.setNewItem();
	if (mapBlock[gItem.posY][gItem.posX] != 0) {
		gItem.posX += 2;
		gItem.posY += 2;
	}
	mapBlock[gItem.posY][gItem.posX] = 2;


	//아이템을 맵에 배치

}



void check_click_point(int x, int y) {

	if ((x > 280 && x < 360) && (y > 680 && y < 720)) init_game();

}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	// 윈도우 구조체 선언부분

	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;

	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);


	
	mciOpen.lpstrElementName = "bgm.mp3"; // 파일 경로 입력
	mciOpen.lpstrDeviceType = "mpegvideo";

	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
	(DWORD)(LPVOID)&mciOpen);

	dwID = mciOpen.wDeviceID;
	mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, // play & repeat
	(DWORD)(LPVOID)&m_mciPlayParms);
	// 사운드 부분


	hWnd = CreateWindow(lpszClass, lpszClass, WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 656, 776,
		NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	// 그림파일을 읽어온다. 이를테면 "bg.bmp"라는 파일을 hBit_map이라는 비트맵 헤더에 집어넣는다는 의미이다.

	LoadBmp("map.bmp", &hBit_map);
	LoadBmp("player.bmp", &hBit_player);
	LoadBmp("map_tile.bmp", &hBit_mt);

	//보물 표시

	init_game();

	DWORD dwStartTick;
	DWORD dwDelay;
	DWORD dwInterval = 1000 / 30;




	// 기본 메시지 루프입니다.

	while (1)
	{


		limit_time = default_time - ((timeGetTime() - start_time_point) / 1000);
		//제한시간을 표시 100(기본 주어지는 시간)) - ((현재시간을 받아오는 함수  - 시작시간))

		dwStartTick = GetTickCount();
		dwDelay = dwInterval - (GetTickCount() - dwStartTick);

		//프레임값 ( 현재 시간 - 프로그램 시작했을때);

		
		// 운영체제로부터 뭔가 메시지가 들어오면 그걸 처리하고,
		if (PeekMessage(&Message, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&Message, NULL, 0, 0)) break;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		// 딱히 없다면 그림을 그리는 등의 딴짓을 한다.
		else
		{						
				process();
				Sleep(dwDelay);		
			//렌더링 하는 부분.
		}
	}
	return Message.wParam;
}

int m_x, m_y;


//마우스 , 키보드 입력 메시지를 처리하는 부분
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {

	case WM_LBUTTONDOWN:
	{
		if (gameState == 1) check_click_point(m_x, m_y);
	}
	break;
	
	case WM_MOUSEMOVE:
	m_x = LOWORD(lParam);
	m_y = HIWORD(lParam);
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYUP:
		KeyBuffer[wParam] = FALSE;
		break;
	
	case WM_KEYDOWN:
		switch (wParam) {

		case VK_SPACE:
		{
			static int nextflag = 0;
			if (flag[0][nextflag].curTemp != 1) {
				flag[0][nextflag].posX = player[0].posX;
				flag[0][nextflag].posY = player[0].posY;
				flag[0][nextflag].curStatus = player[0].curBlock;
				setFlaganswer(CalBlock(flag[0][nextflag].posX), CalBlock(flag[0][nextflag].posY), 6);
				flag[0][nextflag].curTemp = 1;
				nextflag = (nextflag + 1) % 3;
				checkScore();
			}


		}
		break;

		case 0x47:
		{
			static int nextflag = 0;
			if (flag[1][nextflag].curTemp != 1) {
				flag[1][nextflag].posX = player[1].posX;
				flag[1][nextflag].posY = player[1].posY;
				flag[1][nextflag].curStatus = player[1].curBlock;
				flag[1][nextflag].curTemp = 1;
				setFlaganswer(CalBlock(flag[1][nextflag].posX), CalBlock(flag[1][nextflag].posY), 7);
				nextflag = (nextflag + 1) % 3;
				checkScore();
			}


		}
		break;

		default:
			KeyBuffer[wParam] = TRUE;
			break;
		}
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}