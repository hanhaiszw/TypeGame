// TypeGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include <conio.h>
#include <vector>
#include <time.h>


// PlaySound需要
#include <Windows.h>
#include<Mmsystem.h>
#pragma comment(lib,"winmm.lib")


using namespace std;
HANDLE handle;  // 获得设备句柄
COORD crd;      // 用于设置输出坐标
const short GROUND = 27; // 地面
const short SCR_WIDTH = 60;  // 行数

const short HEALTH = 5; // 生命值
class Vitality {  //生命力管理类
private:
	short vitality;  //剩余血量
public:
	Vitality() {
		vitality = HEALTH;
	}
	// 显示当前血量
	void ShowVitality(){
		crd.X = SCR_WIDTH * 0.6;
		crd.Y = 0;
		SetConsoleCursorPosition(handle, crd);
		// 黄色
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN|FOREGROUND_INTENSITY);

		cout << "HP: ";// hit points 血量
		for (short i = 0; i < HEALTH; i++) {
			cout << (i < vitality ? "■" : "__");
		}
		// 恢复黑背景白字
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN|FOREGROUND_BLUE);
	}
	short GetWound(short n = -1) {
		vitality += n;
		ShowVitality();
		return vitality;
	}
	// 恢复血量
	short GetRestore() {
		vitality = HEALTH;
		ShowVitality();
		return vitality;
	}
};


// 血量实例
Vitality vty;
// 字母
struct Letter {
	char letter;
	short x;
	short y;
};
class LetterShower {
private:
	vector<Letter> letters[26];   // 建立26个成员的向量数组，下标0保存A的出现，下落情况
	short score; // 得分
	short delay;// 字母掉落时延时的毫秒数
	// 判断是否触地
	bool Ground(vector<Letter>::iterator l) {
		return l->y >= GROUND;
	}
public:
	LetterShower() {

		score = 0;
	}
	void GenerateLetter() {
		Letter l = { 'A' + rand() % 26, rand() % SCR_WIDTH,1 };
		letters[l.letter - 'A'].push_back(l);
	}

	
	// 管理字母的掉落、触地
	short Fall() {
		short i;
		vector<Letter>::iterator itr;
		for (i = 0; i < 26; i++) {
			for (itr = letters[i].begin(); itr != letters[i].end();) {
				crd.X = itr->x;
				crd.Y = itr->y;   // 在原来的位置输出空格
				SetConsoleCursorPosition(handle, crd);
				cout << ' ';
				// 已经触地
				if (Ground(itr)) {
					// 扣血
					if (vty.GetWound() <= 0) {
						// 血量玩了
						// Game Over
						crd.X = 13;
						crd.Y = GROUND + 2;
						SetConsoleCursorPosition(handle, crd);
						SetConsoleTextAttribute(handle, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
						cout << "Game Over";
						SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
						// 声音  PlaySound
						PlaySound(L".\\sound\\over.wav",NULL, SND_ASYNC|SND_FILENAME);  // 第一个参数表示立马返回   2表示根据文件名找播放文件
						return -1;  //告知调用处，游戏结束
					}
					itr = letters[i].erase(itr);  // 清除掉
					PlaySound(L".\\sound\\wound.wav", NULL, SND_ASYNC | SND_FILENAME);
					continue;
				}
				// 更新位置
				itr->y++;
				crd.Y = itr->y;
				SetConsoleCursorPosition(handle, crd);
				cout << itr->letter;
				itr++;
			}
		}
		return 0;  // 返回0  表示游戏还没结束
	}

	void ClearAll() {
		for (int i = 0; i < 26; i++) {
			while (!letters[i].empty()) {
				letters[i].pop_back();
			}
		}
		score = 0;
	}

	void ShowScore() {
		crd.X = 1;
		crd.Y = GROUND + 2;
		SetConsoleCursorPosition(handle, crd);
		cout << "Score: " << score << "    ";
	}
	void SetDelay(short d) { delay = d; }
	void Wait() { Sleep(delay); }
	void Rain() {  // 让字母反复执行Fall函数，持续向下掉落，并捕获用户的按键，消除被按中的字母
		char ch;
		vector<Letter>::iterator itr;
Repeat:
		// 如果没有按键事件
		while (!_kbhit()) {
			if (Fall() == -1) {
				return;  // 游戏结束
			}
			Wait(); 
			// 是否产生新字母  1/3概率
			if (rand() % 3 == 0) {
				GenerateLetter();
			}
		}
		ch = _getch();
		// 字母键
		if ((ch >= 'a'&&ch <= 'z')|| (ch >= 'A'&&ch <= 'Z')) {
			int index;
			if (ch >= 'a' && ch <= 'z') {
				index = ch - 'a';
			}	
			else {
				index = ch - 'A';
			}
			if (!letters[index].empty()) {
				itr = letters[index].begin();
				crd.X = itr->x;
				crd.Y = itr->y;
				SetConsoleCursorPosition(handle, crd);
				cout << ' ';
				letters[index].erase(itr);
				PlaySound(L".\\sound\\erase.wav", NULL, SND_ASYNC | SND_FILENAME);
				score++;
				ShowScore();
			}
		}
		goto Repeat;  // 回到while循环这里
	}
};

// lv 0 1 2
// EASY INTERMEDIATE,HARD
void PrintLevel(short lv) {
	crd.X = 4;
	crd.Y = 0;
	SetConsoleCursorPosition(handle, crd);
	switch (lv) {
	case 0:
		cout << "LEVEL: EASY";
		break;
	case 1:
		cout << "LEVEL: INTERMEDIATE";
		break;
	case 2:
		cout << "LEVEL: HARD";
		break;
	default:
		cout << "LEVEL: HARD";
		break;
	}
}

void Welcome() {
	
	crd.X = 17;
	crd.Y = 10;
	SetConsoleCursorPosition(handle, crd);
	cout << "Welcome to Type Game!";
	crd.X = 17;
	crd.Y = 12;
	SetConsoleCursorPosition(handle, crd);
	cout << "Press any key to continue...";

}

// 难度选择菜单
// 简单  0
// 中级  1
// 困难  2
short ShowMenu() {
	system("cls");  // 清屏
	crd.X = 10;
	crd.Y = 10;
	SetConsoleCursorPosition(handle, crd);
	cout << "which level do you want to try?";
	short result = 0;
	char ch = 0;
	do {
		// 左方向键
		if (ch == 75) {
			result = (result + 2) % 3;
		}
		else if (ch == 77) {
			// 有方向键
			result = (result + 1) % 3;
		}

		crd.X = 10; 
		crd.Y = 12;
		SetConsoleCursorPosition(handle, crd);
		if (result == 0) {
			// 设置背景色   前景色默认是黑色
			SetConsoleTextAttribute(handle, BACKGROUND_GREEN);
		}
		else {
			// 设置前景色为白色   背景色默认为黑色
			SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		}
		cout << " EASY ";


		crd.X = 20;
		crd.Y = 12;
		SetConsoleCursorPosition(handle, crd);
		if (result == 1) {
			// 设置背景色   前景色默认是黑色
			SetConsoleTextAttribute(handle, BACKGROUND_GREEN);
		}
		else {
			// 设置前景色为白色   背景色默认为黑色
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
		cout << " INRERMEDIATE ";


		crd.X = 38;
		crd.Y = 12;
		SetConsoleCursorPosition(handle, crd);
		if (result == 2) {
			// 设置背景色为青色   前景色默认是黑色
			SetConsoleTextAttribute(handle, BACKGROUND_GREEN);
		}
		else {
			// 设置前景色为白色   背景色默认为黑色
			SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		}
		cout << " HARD ";

		// 使用getchar() 无法获取到值
		ch = _getch();
		if (ch == 0)
			ch = _getch();

	} while (ch != VK_RETURN);  // 只有按下回车键才结束循环

	// 恢复成默认的前景背景色
	SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return result;
}

// 绘制地面
void DrawGround() {
	crd.X = 0;
	crd.Y = GROUND + 1;
	SetConsoleCursorPosition(handle, crd);
	for (short i = 0; i < SCR_WIDTH; i++) {
		cout << '=';
	}
}
int main()
{
	// 设置控制台窗口的行数和列数
	system("mode con cols=60 lines=31");
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	Welcome();
	_getch();
	// 产生随机种子
	srand(time(0));

	char choice; // 确定是否继续玩游戏
	short level;
	LetterShower ls;
	do {
		// 游戏流程
		system("cls");
		level = ShowMenu();		
		system("cls");
		PrintLevel(level);
		vty.GetRestore();
		vty.ShowVitality();
		DrawGround();

		ls.ClearAll();  // 重置分数
		ls.ShowScore();

		ls.SetDelay((3-level) * 300);
		ls.Rain();


		// 会出现在Game over后面
		cout << "     Try again?(y/n)";
		cin >> choice;
	} while (choice == 'Y' || choice == 'y');

	
	/*_getch();
	_getch();*/

	//getchar();
    return 0;
}

