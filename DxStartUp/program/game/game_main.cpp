#include "DxLib.h"
#include "game_main.h"
#include "../support/Support.h"
#include "../support/vec3.h"
#include <time.h>
#include <math.h>

//=============================================================================
// name... game_main
// work... ゲームのメインループ
// arg.... none
// ret.... [ 正常終了 : 0 ]
//=============================================================================
const int MAPHEI = 9;
const int MAPWID = 12;
const int HABA = 70;
class MapManager {

	int mapData[MAPHEI][MAPWID]{
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 0,0,0,0,0,0,0,0,0,0,0,0 },
		{ 1,1,1,1,0,1,1,1,1,1,1,1 },
		{ 2,2,2,2,0,2,2,2,2,2,2,2 },
		{ 2,2,2,2,0,2,2,2,2,2,2,2 },
		{ 2,2,2,2,0,2,2,2,2,2,2,2 },
	};
	void draw(int sX, int sY, int haba, int gfx) {
		DrawModiGraph(sX, sY, sX + haba, sY, sX + haba, sY + haba, sX, sY + haba, handles[gfx], false);
	}
public:
	int handles[3];
	int getMapNum(int mapX, int mapY) {
		if (0 <= mapX && mapX < MAPWID && 0 <= mapY&&mapY < MAPHEI) {
			return mapData[mapY][mapX];
		}
		return 0;
	}
	bool stopHantei(t2k::vec3 mapPos) {
		return stopHantei(getMapNum(mapPos));
	}
	bool stopHantei(int mapNum) {
		switch (mapNum)
		{
		case 0:
		default:
			return false;
			break;
		case 1:
		case 2:
			return true;
			break;
		}
	}
	int getMapNum(t2k::vec3 mapPos) {
		return mapData[(int)(mapPos.y / HABA)][(int)(mapPos.x / HABA)];
	}
	void mapDraw() {
		for (int h = 0; h < MAPHEI; h++) {
			for (int w = 0; w < MAPWID; w++) {
				draw(w*HABA, h*HABA, HABA, getMapNum(w, h));
			}
		}
	}
};

bool init = false;
int winwid;
int winhei;
int GROUND_UP = 1;
int GROUND = 2;
int SKY = 0;

MapManager mapManager;
class Character {
private:
	int gfx;
	//ブロックに埋まっているかの判断
	void moveMapStopper(int mapX, int mapY, int xBlockSuu, int yBlockSuu) {
		for (int w = 0; w <= xBlockSuu; w++) {
			for (int h = 0; h <= yBlockSuu; h++) {
				int mapnum = mapManager.getMapNum(mapX + w, mapY + h);
				if (mapnum == 1 || mapnum == 2) {
					break;
				}
			}
		}
	}
	t2k::vec3 pos;
public:

	int width;
	int height;

	void setData(t2k::vec3 Pos, int Width, int Height, int Gfx) {
		pos = Pos;
		width = Width;
		height = Height;
		gfx = Gfx;
	}
	//後で分ける関数です
	void playerMove() {

		if (CheckHitKey(KEY_INPUT_D)) {
			move(t2k::vec3(3, 0, 0));
		}
		if (CheckHitKey(KEY_INPUT_A)) {
			move(t2k::vec3(-3, 0, 0));
		}
		move(t2k::vec3(0, 0.98f, 0));

	}
	void moveControll() {
		//playerの動き制御
		//キャラの周りのブロックを見て、動けるか判断。動けなかったらその方向の動きを制限
		//一旦仮で下げてからキャラ底辺のブロックを参照して制御する。ブロック>棘>空白の順番で判定1>2>0
		//playerMove();

		t2k::vec3 preMove(0,0,0);
		if (CheckHitKey(KEY_INPUT_D)) {
			preMove += t2k::vec3(3, 0, 0);
		}
		if (CheckHitKey(KEY_INPUT_A)) {
			preMove += t2k::vec3(-3, 0, 0);
		}
		preMove+= t2k::vec3(0, 0.98f, 0);


		//キャラクタの左下のmap座標
		int mY = (int)((pos.y + height / 2) / HABA);
		int mX = (int)((pos.x - width / 2) / HABA);
		//動きを制御
		const int TOP = 0;
		const int RIGHT = 1;
		const int BOT = 2;
		const int LEFT = 3;
		bool moving[4] = {0};//trueだと通れない
		for (int i = 0; i < width; i += HABA) {
			if (!moving[TOP])moving[TOP] = mapManager.stopHantei(t2k::vec3(mX + i, mY - height - 1, 0));//top
			if (!moving[BOT])moving[BOT] = mapManager.stopHantei(t2k::vec3(mX + i, mY + 1, 0));//bot
		}
		for (int i = 0; i < height; i += HABA) {
			if (!moving[RIGHT])moving[RIGHT] = mapManager.stopHantei(t2k::vec3(mX + width + 1, mY - height + i, 0));//right
			if (!moving[LEFT])moving[LEFT] = mapManager.stopHantei(t2k::vec3(mX - 1, mY - height + i, 0));//Left
		}

		if (moving[TOP] && preMove.y < 0) {
			preMove.y = 0;
		}
		if (moving[BOT] && preMove.y > 0) {
			preMove.y = 0;
		}
		if (moving[LEFT] && preMove.x < 0) {
			preMove.x = 0;
		}
		if (moving[RIGHT] && preMove.x > 0) {
			preMove.x = 0;
		}
		move(preMove);

		int xoffset = (pos.x - width / 2) - mX*HABA;


		//charcterが跨いでるブロック数
		int botblosuu = (xoffset + width) / HABA;
		int playerTrans = 0;
		for (int i = 0; i <= botblosuu; i++) {
			int mapnum = mapManager.getMapNum(mX + i, mY);
			if (mapnum == 1 || mapnum == 2) {
				playerTrans = 1;
				break;
			}
		}
		//座標をブロック上に戻す
		if (playerTrans == 1) {
			pos.y = (-height / 2 + mY*HABA);
		}
	}
	void move(t2k::vec3 moveVec) {
		pos += moveVec;
	}
	void draw() {
		DrawModiGraph(pos.x - width / 2, pos.y - height / 2, pos.x + width / 2, pos.y - height / 2, pos.x + width / 2, pos.y + height / 2, pos.x - width / 2, pos.y + height / 2, gfx, true);
	}
};

Character player;
int game_main()
{

	if (!init) {
		mapManager.handles[SKY] = LoadGraph("graphics/sky.jpg");
		mapManager.handles[GROUND_UP] = LoadGraph("graphics/upGround.jpg");
		mapManager.handles[GROUND] = LoadGraph("graphics/ground.jpg");
		GetWindowSize(&winwid, &winhei);
		player.setData(t2k::vec3(200, 200, 0), 64, 100, LoadGraph("graphics/playerRobo.png"));
		init = true;
	}
	player.moveControll();
	mapManager.mapDraw();

	player.draw();
	return 0;

}

