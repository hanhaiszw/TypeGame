#include<windows.h>
#pragma comment(lib,"winmm.lib")//需要使用PlaySound函数播放音效
#include<iostream>
#include<vector>//需要在LetterShower类当中使用向量
#include<conio.h>//使用getch()函数
#include<time.h>//使程序中出现的字母具有随机性
using namespace std;
HANDLE handle;//获得输出设备句柄
COORD crd;//用于设置输出坐标
const short GROUND=27;const short SCR_WIDTH=60;
const short HEALTH=5;
class Vitality//管理生命力的变化
{
private:
	short vitality;//剩余血量
public:
	Vitality(){vitality=HEALTH;}
	void ShowVitality()//显示当前血量
	{
		crd.X=SCR_WIDTH*0.6;crd.Y=0;
		SetConsoleCursorPosition(handle,crd);
		SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
		cout<<"HP: ";//hit points血量
		for(short i=0;i<HEALTH;i++)
			cout<<(i<vitality?"■":"__");
		SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);//恢复成黑底白字
	}
	short GetWound(short n=-1)
	{
		vitality+=n;
		ShowVitality();
		return vitality;
	}
	short GetRestore()
	{
		vitality=HEALTH;
		ShowVitality();
		return vitality;
	}
};
Vitality vty;
struct Letter
{
	char letter;//当前字母
	short x;//x轴坐标
	short y;//y轴坐标
};
class LetterShower//管理产生字母，字母下落，掉落地面等行为
{
private:
	vector<Letter> letters[26];//建立26个成员的向量数组，下标0的保存字母A的出现，下落情况，下标25的保存Z的各种情况
	short score;//保存玩家得分
	short delay;//字母掉落时延时的毫秒数
	bool Ground(Letter*l)//判断一个字母已掉落到地面
	{
		return l->y>=GROUND;
	}
public:
	LetterShower(){score=0;}
	void GenerateLetter()//新产生一个字母并进入向量
	{
		Letter l={'A'+rand()%26,rand()%SCR_WIDTH,1};
		letters[l.letter-'A'].push_back(l);
	}
	short Fall()//管理字母的掉落、触地
	{
		short i;
		vector<Letter>::iterator itr;
		for(i=0;i<26;i++)
		{
			for(itr=letters[i].begin();itr!=letters[i].end();)
			{
				crd.X=itr->x;crd.Y=itr->y;//获得字母原来的位置坐标，准备输出空格将其擦除
				SetConsoleCursorPosition(handle,crd);
				cout<<' ';
				if(Ground(itr))//若已触地，则需要处理扣血
				{
					if(vty.GetWound()<=0)//处理Game over的输出信息
					{
						crd.X=13;crd.Y=GROUND+2;
						SetConsoleCursorPosition(handle,crd);
						SetConsoleTextAttribute(handle,BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE);
						cout<<"Game over!";
						SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
						PlaySound(".\\sound\\over.wav",NULL,SND_ASYNC|SND_FILENAME);
						return -1;//返回-1告知本函数的调用块游戏已结束
					}
					letters[i].erase(itr);//将落地的字母从向量当中清除掉
					PlaySound(".\\sound\\wound.wav",NULL,SND_ASYNC|SND_FILENAME);
					continue;
				}
				itr->y++;
				crd.Y=itr->y;
				SetConsoleCursorPosition(handle,crd);
				cout<<itr->letter;
				itr++;
			}
		}
		return 0;//返回0表明尚未Game over
	}
	void ClearAll()
	{
		for(short i=0;i<26;i++)
			while(!letters[i].empty()) letters[i].pop_back();
		score=0;
	}
	void ShowScore()
	{
		crd.X=1;
		crd.Y=GROUND+2;
		SetConsoleCursorPosition(handle,crd);
		cout<<"Score: "<<score<<"   ";
	}
	void SetDelay(short d){delay=d;}
	void Wait(){Sleep(delay);}
	void Rain()//让字母反复执行Fall函数，持续向下掉落，并捕捉用户的按键，消除被按中的字母
	{
		char ch;//保存用户按键信息
		vector<Letter>::iterator itr;
Repeat:
		while(!kbhit())
		{
			if(Fall()==-1) return;
			Wait();
			if(rand()%3==0) GenerateLetter();
		}
		ch=getch();
		if(ch>='a'&&ch<='z')
		{
			if(!letters[ch-'a'].empty())
			{
				itr=letters[ch-'a'].begin();
				crd.X=itr->x;crd.Y=itr->y;
				SetConsoleCursorPosition(handle,crd);
				cout<<' ';
				letters[ch-'a'].erase(itr);
				PlaySound(".\\sound\\erase.wav",NULL,SND_ASYNC|SND_FILENAME);
				score++;
				ShowScore();
			}
		}
		goto Repeat;//回到while(!kbhit())的位置
	}
};
void PrintLevel(short lv)//参数为0,表示EASY, 为1，2则分别表示INTERMEDIATE, HARD
{
	crd.X=4;crd.Y=0;
	SetConsoleCursorPosition(handle,crd);
	switch(lv)
	{
	case 0:cout<<"Level: EASY";break;
	case 1:cout<<"Level: INTERMEDIATE";break;
	default:cout<<"Level: HARD";break;
	}
}
void Welcome()
{
	crd.X=17;crd.Y=10;
	SetConsoleCursorPosition(handle,crd);
	cout<<"Welcome to Type Game!";
	crd.X=17;crd.Y=12;
	SetConsoleCursorPosition(handle,crd);
	cout<<"Press any key to continue...";
}
short ShowMenu()//返回值显示用户所选难度级别。容易0, 中级1, 困难2
{
	system("cls");
	crd.X=10;crd.Y=10;
	SetConsoleCursorPosition(handle,crd);
	cout<<"Which level do you want to try?";
	short result=0;
	char ch=0;
	do
	{
		if(ch==75) result=(result+2)%3;//按下了向左的方向键
		if(ch==77) result=(result+1)%3;//按下了向右的方向键
		crd.X=10;crd.Y=12;
		SetConsoleCursorPosition(handle,crd);
		if(result==0)
			SetConsoleTextAttribute(handle,BACKGROUND_GREEN);
		else
			SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		cout<<" EASY ";
		crd.X=20;crd.Y=12;
		SetConsoleCursorPosition(handle,crd);
		if(result==1)
			SetConsoleTextAttribute(handle,BACKGROUND_GREEN);
		else
			SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		cout<<" INTERMEDIATE ";
		crd.X=38;crd.Y=12;
		SetConsoleCursorPosition(handle,crd);
		if(result==2)
			SetConsoleTextAttribute(handle,BACKGROUND_GREEN);
		else
			SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		cout<<" HARD ";
		ch=getch();
		if(ch==0) ch=getch();
	}while(ch!=VK_RETURN);//只有当按下回车键时才结束循环
	SetConsoleTextAttribute(handle,FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);//恢复成默认的文字以及背景色
	return result;
}
void DrawGround()//绘制地面
{
	crd.X=0;crd.Y=GROUND+1;
	SetConsoleCursorPosition(handle,crd);
	for(short i=0;i<SCR_WIDTH;i++) cout<<'=';
}
int main()
{
	system("mode con cols=60 lines=31");//设置窗体尺寸
	handle=GetStdHandle(STD_OUTPUT_HANDLE);
	Welcome();//显示欢迎界面
	getch();
	srand(time(0));//产生随机种子
	char choice;//由用户输出是否继续游戏
	short level;//接受用户选择的难度级别
	LetterShower ls;
	do
	{
		system("cls");//清除欢迎界面的文字，如果是第二，三……次循环，则清除窗体上已有的字母
		level=ShowMenu();
		system("cls");
		vty.GetRestore();
		vty.ShowVitality();
		PrintLevel(level);
		DrawGround();
		ls.ClearAll();//得分归0，并清空向量
		ls.ShowScore();
		ls.SetDelay(300-level*60);
		ls.Rain();
		cout<<"    Try again?(y/n)";
		cin>>choice;
	}while(choice=='Y'||choice=='y');
	return 0;
}