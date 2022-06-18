#include<iostream>
#include<string>
#include<Windows.h>
#include<winuser.h>
#include<map>
#include<tuple>
//GetKeyState函数定义判断按键状态 宏
#define KEY_DOWN(vKey) ((GetKeyState(vKey) & 0x8000) ? 1:0)//GetKeyState函数 KEY_DOWN
#define KEY_TOGGLE(vKey) ((GetKeyState(vKey) & 0x0001) ? 1:0)

#define SLEEPINST Sleep(30)

using namespace std;

/*
* a simulated windows editor
*/

extern map<int, pair<char, char>> key_map;

extern bool all_key_status[256];

class vimw {
private:
	char temp_ch;
	string ori_content;
	string new_content;
	string result;
	string filename;
	bool lock_status; //函数输入锁状态
	bool caps_lock, num_lock, shift, ctrl;//CapsLock键、NumLock键和Shift键的检测标记 

public:

	int setContent(string& input_str);//get input string to the private variable "content"
	void showOptionInfo();//
	void infoLoop();
	bool editable(bool ex_lock);
	bool setFileName(string file_name);
	void strInputFun(int vk_code);
	bool checkSaveAndDoReturn(int vk_code);
	string getResult();
	int runProg();// run the main edition loop
};



