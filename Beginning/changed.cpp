
#include <windows.h>

// 전역 변수
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
HINSTANCE g_hInst;
LPSTR lpszClass="바뀐그림찾기 Made By. 김상현";
HWND hWnd;
//djfiqej
HBITMAP hBit; // Back Buffer가 될 비트맵헤더.
HBITMAP hBit_map;	// 배경화면을 가리키는 비트맵헤더.
HBITMAP hBit_circle;	// 동그라미를 가리키는 비트맵 헤더,

//각각 마우스의 좌표
int m_x;
int m_y;

//모두 맞췄는지 알아보는 변수
int checked_point = 0;

//제한 횟수 5회

int failed_point = 5;

//각각 맞췄는지 알아보는 변수
bool checked[3]; //0 : 자블라니, 1 : Sonata Arctica, 2 : 레스폴
bool istrue; //정답인 점을 누르고 있는지 아닌지 구별하는 것.
bool ending; //끝났음을 알림

int limit_time = 10;
//제한시간을 다루는 변수.. 1이상의 초기값을 주어야 돌아간다.

int start_time_point = timeGetTime();
//막 시작할때 시간 측정...

int temp_time;//(timeGetTime()/1000) - (start_time_point/1000) 를 다루는 변수... 그냥 시작으로 부터 몇초 흘렀는지만 알려준다...

//각각의 좌표 입력

int point1[4] = { 507, 268, 552, 312 };//자블라니

int point2[4] = { 499, 336, 701, 385 }; //Sonata Arctica

int point3[4] = { 696, 152, 740, 276 }; //레스폴


// 클릭 여부를 가리키는 변수.
DWORD clicked = 0;

int LoadBmp( char* filename , HBITMAP* ArghBit)	// 그림파일을 읽어온다.
{


	HANDLE hFile;
	HBITMAP hBit;
	DWORD FileSize, dwRead;
	BITMAPFILEHEADER fh;
	BITMAPINFO * ih;
	PVOID pRaster;

	//파일을 연다.
	hFile = CreateFile( filename,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
		NULL);
	if( hFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	// 파일 헤더와 정보 구조체 (색상 테이블 포함 ) 를 읽어 들인다.
	ReadFile(hFile , &fh , sizeof(BITMAPFILEHEADER) , &dwRead, NULL);
	FileSize = fh.bfOffBits - sizeof(BITMAPFILEHEADER);
	ih = (BITMAPINFO *)malloc(FileSize);
	ReadFile(hFile, ih, FileSize, &dwRead, NULL);

	//DIB 섹션을 만들고 버퍼 메모리를 할당한다.
	hBit = CreateDIBSection(NULL,ih,DIB_RGB_COLORS, &pRaster, NULL , 0);
	//래스터 데이터를 읽어들인다.
	ReadFile(hFile , pRaster ,fh.bfSize - fh.bfOffBits, &dwRead , NULL);
	free(ih);
	CloseHandle(hFile);

	*ArghBit = hBit;

	return 1;
}

void Draw()
{
	
	char n, buf[1000];
	int temp_time = (timeGetTime()/1000) - (start_time_point/1000);
	
	limit_time = 10 - ((timeGetTime()/1000) - (start_time_point/1000));

	RECT crt;	// 현재 창의 정보를 읽어올 변수.
	HDC hdc,hMemDC;

	GetClientRect(hWnd,&crt);	
	hdc=GetDC(hWnd);	


	if (hBit==NULL) {
		hBit=CreateCompatibleBitmap(hdc,crt.right,crt.bottom);
	}


	HDC MemDCBack = CreateCompatibleDC( hdc );


	HBITMAP OldBit_Back = (HBITMAP)SelectObject(MemDCBack, hBit);

	hMemDC=CreateCompatibleDC(MemDCBack);

	// 배경화면의 비트맵 헤더인 hBit_map을 아까 구한 hMemDC에 연결한다.
	HBITMAP OldBit_Obj = (HBITMAP)SelectObject(hMemDC,hBit_map);

	BitBlt( MemDCBack , 0 , 0 , 800 , 600 , hMemDC , 0 , 0 , SRCCOPY);

	n = wsprintf(buf, "맞춘 개수 : %d / 3    Life : %d / 5                                                                                                                     남은 시간 : %d초       ", checked_point, failed_point, limit_time);
	TextOut( MemDCBack, 0, 400, buf, strlen(buf));

	// hMemDC를 캐릭터 비트맵 헤더에 물린다.
	SelectObject(hMemDC, hBit_circle );
	

	
	if ( checked[0] == true ) //자블라니에 동그라미 표시
	{
		
		TransparentBlt( MemDCBack , 500, 260, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
		TransparentBlt( MemDCBack , 100, 260, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
	}
	if ( checked[1] == true ) //Sonata Arctica에 동그라미 표시
	{

		TransparentBlt( MemDCBack , 565, 333, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
		TransparentBlt( MemDCBack , 165, 333, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
	}
	if ( checked[2] == true ) //레스폴에 동그라미 표시
	{
		TransparentBlt( MemDCBack , 690, 202, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
		TransparentBlt( MemDCBack , 290, 202, 64, 64, hMemDC, 0, 0, 64, 64, RGB(100,0,100) );
	}


	//limit_time = ((timeGetTime()/1000) - (start_time_point/1000));



	// hMemDC에 물려있던 비트맵 헤더를 초기값으로 되돌리고 소멸시킨다.
	SelectObject(hMemDC,OldBit_Obj);
	DeleteDC(hMemDC);

	// 백버퍼 DC도 원래 초기값으로 되돌리고 소멸시킨다.
	SelectObject(MemDCBack,OldBit_Back);
	DeleteDC(MemDCBack);

	// 아버지뻘 되는 dc였던 hdc를 해제시킨다.
	ReleaseDC(hWnd,hdc);
	//InvalidateRect(hWndMain,NULL,FALSE);
}


void Flip()
{

	HDC hdc = GetDC( hWnd );
	HDC hMemDC=CreateCompatibleDC(hdc);

	RECT crt;
	GetClientRect(hWnd,&crt);	


    HBITMAP OldBit=(HBITMAP)SelectObject(hMemDC, hBit);

	BitBlt(hdc,0,0,crt.right,crt.bottom,hMemDC,0,0,SRCCOPY);

	// 여기는 그냥 해제하는 부분.
    SelectObject(hMemDC, OldBit);

	DeleteDC(hMemDC);
	ReleaseDC( hWnd, hdc );
}


void process()
{
	Draw();	// 실제적으로 그리는 부분.
	Flip();	// Draw에서 Back Buffer로 그림들을 그렸기 때문에, 여기서 화면과 Back Buffer를 바꿔치기(Flip)하는 부분이다.
}


int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance
		  ,LPSTR lpszCmdParam,int nCmdShow)
{
	// 괜히 되도않는 윈도우 구조체 선언부분이다. 양이 많다고 쫄지 마라. 걍 무시하고 넘어가면 된다.

	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance,IDI_APPLICATION);
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=(WNDPROC)WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=NULL;
	WndClass.style=CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd=CreateWindow(lpszClass,lpszClass,WS_SYSMENU,
		  CW_USEDEFAULT,CW_USEDEFAULT,800,450,
		  NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);

	// 그림파일을 읽어온다. 이를테면 "bg.bmp"라는 파일을 hBit_map이라는 비트맵 헤더에 집어넣는다는 의미이다.

	LoadBmp( "background.bmp", &hBit_map );
	LoadBmp( "circle.bmp", &hBit_circle );



	// 기본 메시지 루프입니다.

	while( 1 )
	{
		// 운영체제로부터 뭔가 메시지가 들어오면 그걸 처리하고,
		if( PeekMessage(&Message , NULL , 0 , 0 , PM_NOREMOVE) )
		{
			if(!GetMessage(&Message, NULL, 0, 0)) break ;
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
		// 딱히 없다면 그림을 그리는 등의 딴짓을 한다.
		else
		{
			process();
		}
		if ( checked_point == 3 ) // 다 맞추면 프로그램 종료
		{
			ending = true;
		}

		if ( failed_point == 0)// 다 틀려도 프로그램 종료
		{
			ending = true;
		}

		if ( ending == true && checked_point == 3 )
		{
			MessageBox(hWnd, TEXT("축하합니다. 모두 맞추셨습니다."),
			TEXT("Cleared!!"), MB_OK);
			break;
		}
		else if ( ending == true && failed_point == 0 )
		{
			MessageBox(hWnd, TEXT("Oh My Shoulder!"),
			TEXT("Game Over"), MB_OK);
			break;
		}

		if ( limit_time == 0 )
		{
			MessageBox(hWnd, TEXT("장비를 정지합니다."),
			TEXT("Time Over!"), MB_OK);
			break;
		}

	}

	return Message.wParam;
}

void check_click_point()
{
	if ( m_x >= point1[0] && m_x <= point1[2] && m_y >= point1[1] && m_y <= point1[3])
	{	
		if ( checked[0] != true )
		{
			checked_point ++;
			checked[0] = true;
		}
		istrue = true;
	}//자블라니 찾을때 - 다른 그림에서

	else if ( m_x >= point2[0] && m_x <= point2[2] && m_y >= point2[1] && m_y <= point2[3] )
	{
		if ( checked[1] != true )
		{
			checked_point ++;
			checked[1] = true;
		}
		istrue = true;
	}//Sonata Arctica 찾을때 - 다른 그림에서

	else if ( m_x >= point3[0] && m_x <= point3[2] && m_y >= point3[1] && m_y <= point3[3] )
	{
		if ( checked[2] != true )
		{
			checked_point ++;
			checked[2] = true;
		}
		istrue = true;
	}//레스폴 찾을때 - 다른 그림에서
	
	else if ( m_x >= point1[0] - 400 && m_x <= point1[2] - 400 && m_y >= point1[1] && m_y <= point1[3] )
	{
		if ( checked[0] != true )
		{
			checked_point ++;
			checked[0] = true;
		}
		istrue = true;
	}//자블라니 찾을때 - 원본 그림에서

	else if ( m_x >= point2[0] - 400 && m_x <= point2[2] - 400 && m_y >= point2[1] && m_y <= point2[3] )
	{
		if ( checked[1] != true )
		{
			checked_point ++;
			checked[1] = true;
		}
		istrue = true;
	}//Sonata Arctica 찾을때 - 원본 그림에서

	else if ( m_x >= point3[0] - 400 && m_x <= point3[2] - 400 && m_y >= point3[1] && m_y <= point3[3] )
	{
		if ( checked[2] != true )
		{
			checked_point ++;
			checked[2] = true;
		}
		istrue = true;
	}//레스폴 찾을때 - 원본 그림에서

	else
	{
		failed_point --;
		istrue = false;
	}


}

LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage) {

	
	case WM_MOUSEMOVE:
		m_x = LOWORD(lParam);
		m_y = HIWORD(lParam);
		return 0;

	case WM_LBUTTONDOWN:
		clicked = timeGetTime();
		check_click_point();
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}
