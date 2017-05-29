
//###### 아이템(상자)에 관련한 클래스 및 클래스함수 ############
#pragma once
#include <windows.h>
#include <stdlib.h>
#include <time.h>


class Item
{//private 과 public 으로 나누었지만 실제로 별 의미는 없음.
private:
	int kindOfItem;//아이템 종류//0=상대방 얼음 템   1=나 워프템
public:
	int posX, posY;//상자 위치(왼쪽상단)
	int imgSize;//이미지크기
	int imgCentX, imgCentY;//이미지 중심위치
	bool visiableOnGame;//게임에 나타남 true, 안나타남 false
	HBITMAP img;//상자 이미지

	Item();//생성자
	int getKindOfItem();//아이템 종류 반환//현재 쓰이지는 않는듯
	void setNewItem();//아이템 위치 및 종류를 재 설정하고, 게임에 나타나게 함
	int getThisItem(int px, int py);//플레이어 한명과 충돌처리 검사// 충돌아니면 -1, 충돌시 아이템의 종류(kindOfItem)반환

};

Item::Item()
{
	srand(time(NULL));

	int sx=0, ex=540, sy=0, ey=540;//화면 폭의 시작점, 끝점, 높이의 시작점, 끝점
	int items=2;//아이템 종류 수

	posX = rand() % ((ex + 1) - sx ) + sx;//sx~ex, sy~ey 사이에서 생기도록 함
	posY = rand() % ((ey + 1) - sy) + sy;

	imgSize = 64;

	imgCentX = posX + (imgSize / 2);
	imgCentY = posY + (imgSize / 2);

	kindOfItem = 1;//rand() % items;

	visiableOnGame = false;

	img = NULL;

}
int Item::getKindOfItem()
{
	return kindOfItem;
}
void Item::setNewItem()
{
	int items = 2;//아이템 종류 수

	posX = 2 + rand() % 16;
	posY = 2 + rand() % 16;
	//아이템이 위치하게 될 배열의 위치를 생성.

	imgCentX = imgSize / 2;
	imgCentY = imgSize / 2;

	kindOfItem = rand() % items;

	visiableOnGame = true;
}
int Item::getThisItem(int px, int py)
{
	float d;

	d = ((px - imgCentX)*(px - imgCentX)) + ((py - imgCentY)*(py - imgCentY));

	if (d < (imgSize/2)*(imgSize/2))//충돌이면
		return kindOfItem;//아이템 종류 반환

	return -1;
}

struct FdObjects {
	int curBlock;
	int curTemp;
	int curStatus;
	int posX;
	int posY;
	int score;
};//넘버, 상태(표시 / 비표시), 좌표
  //플레이어 구조체 


#define blockSize 32;
int CalBlock(int inPoint) {

	
	inPoint = inPoint / blockSize;

	return inPoint;
}


#define flag_count 3
struct FdObjects player[2];
struct FdObjects flag[2][flag_count];
struct FdObjects answer[3];

int mapBlock[20][20];

int detectBlock(int x, int y) {
	return mapBlock[CalBlock(y)][CalBlock(x)];
}

// 현재 캐릭터가 위치하는 곳의 블록 좌표 상태를 계속 체크한다.