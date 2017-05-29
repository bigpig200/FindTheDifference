
//###### ������(����)�� ������ Ŭ���� �� Ŭ�����Լ� ############
#pragma once
#include <windows.h>
#include <stdlib.h>
#include <time.h>


class Item
{//private �� public ���� ���������� ������ �� �ǹ̴� ����.
private:
	int kindOfItem;//������ ����//0=���� ���� ��   1=�� ������
public:
	int posX, posY;//���� ��ġ(���ʻ��)
	int imgSize;//�̹���ũ��
	int imgCentX, imgCentY;//�̹��� �߽���ġ
	bool visiableOnGame;//���ӿ� ��Ÿ�� true, �ȳ�Ÿ�� false
	HBITMAP img;//���� �̹���

	Item();//������
	int getKindOfItem();//������ ���� ��ȯ//���� �������� �ʴµ�
	void setNewItem();//������ ��ġ �� ������ �� �����ϰ�, ���ӿ� ��Ÿ���� ��
	int getThisItem(int px, int py);//�÷��̾� �Ѹ�� �浹ó�� �˻�// �浹�ƴϸ� -1, �浹�� �������� ����(kindOfItem)��ȯ

};

Item::Item()
{
	srand(time(NULL));

	int sx=0, ex=540, sy=0, ey=540;//ȭ�� ���� ������, ����, ������ ������, ����
	int items=2;//������ ���� ��

	posX = rand() % ((ex + 1) - sx ) + sx;//sx~ex, sy~ey ���̿��� ���⵵�� ��
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
	int items = 2;//������ ���� ��

	posX = 2 + rand() % 16;
	posY = 2 + rand() % 16;
	//�������� ��ġ�ϰ� �� �迭�� ��ġ�� ����.

	imgCentX = imgSize / 2;
	imgCentY = imgSize / 2;

	kindOfItem = rand() % items;

	visiableOnGame = true;
}
int Item::getThisItem(int px, int py)
{
	float d;

	d = ((px - imgCentX)*(px - imgCentX)) + ((py - imgCentY)*(py - imgCentY));

	if (d < (imgSize/2)*(imgSize/2))//�浹�̸�
		return kindOfItem;//������ ���� ��ȯ

	return -1;
}

struct FdObjects {
	int curBlock;
	int curTemp;
	int curStatus;
	int posX;
	int posY;
	int score;
};//�ѹ�, ����(ǥ�� / ��ǥ��), ��ǥ
  //�÷��̾� ����ü 


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

// ���� ĳ���Ͱ� ��ġ�ϴ� ���� ��� ��ǥ ���¸� ��� üũ�Ѵ�.