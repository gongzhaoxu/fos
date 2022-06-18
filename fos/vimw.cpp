#include"vimw.h"
using namespace std;

map<int, pair<char, char>> key_map = {
	{0x30,{'0',')'}},{0x31,{'1','!'}},{0x32,{'2','@'}},{0x33,{'3','#'}},{0x34,{'4','$'}},{0x35,{'5','%'}},{0x36,{'6','^'}},{0x37,{'7','&'}},{0x38,{'8','('}},{0x39,{'9','('}},//数字键区
	{0x41,{'a','A'}},{0x42,{'b','B'}},{0x43,{'c','C'}},{0x44,{'d','D'}},{0x45,{'e','E'}},{0x46,{'f','F'}},{0x47,{'g','G'}},{0x48,{'h','H'}},{0x49,{'i','I'}},{0x4A,{'j','J'}},{0x4B,{'k','K'}},{0x4C,{'l','L'}},{0x4D,{'m','M'}},{0x4E,{'n','N'}},{0x4F,{'o','O'}},{0x50,{'p','P'}},
	{0x51,{'q','Q'}},{0x52,{'r','R'}},{0x53,{'s','S'}},{0x54,{'t','T'}},{0x55,{'u','U'}},{0x56,{'v','V'}},{0x57,{'w','W'}},{0x58,{'x','X'}},{0x59,{'y','Y'}},{0x5A,{'z','Z'}},//字母区
	{VK_RETURN,{'\n','\n'}},{VK_OEM_1,{';',':'}},{VK_OEM_2,{'/','?'}},{VK_OEM_3,{'`','~'}},{VK_OEM_4,{'[','{'}},{VK_OEM_5,{'\\','|'}},{VK_OEM_6,{']','}'}},{VK_OEM_7,{'\'','\"'}},{VK_OEM_MINUS,{'-','_'}},{VK_OEM_PLUS,{'=','+'}},{VK_OEM_COMMA,{',','<'}},{VK_OEM_PERIOD,{'.','>'}},{VK_SPACE,{' ',' '}},//字符区
	{0x60,{'0','0'}},{0x61,{'1','1'}},{0x62,{'2','2'}},{0x63,{'3','3'}},{0x64,{'4','4'}},{0x65,{'5','5'}},{0x66,{'6','6'}},{0x67,{'7','7'}},{0x68,{'8','8'}},{0x69,{'9','9'}},//小键盘区
};

bool all_key_status[256] = { false };

void vimw::showOptionInfo() {
	system("cls");
	printf_s("FILE EDITOR\n");
	printf_s("___________________________________________________________________________________________\n\n");
	printf_s("ctrl + s - 保存文件并退出                    ||                  ctrl + z - 放弃编辑并退出\n");
	printf_s("___________________________________________________________________________________________\n");
	printf_s("%s:\n%s", filename.c_str(), new_content.c_str());//todo
}

int vimw::setContent(string& input_str)
{
	ori_content = input_str;
	new_content = input_str;
	return 0;
}

bool vimw::setFileName(string file_name)
{
	filename = file_name;
	return true;
}

string vimw::getResult()
{
	return result;
}

bool vimw::checkSaveAndDoReturn(int vk_code)
{
	if (KEY_DOWN(VK_CONTROL))
	{
		if (vk_code == 0x53)//s
		{
			result = new_content;
			return true;
		}
		else if (vk_code == 0x5A)//z
		{
			result = ori_content;
			return true;
		}
	}
	return false;
}
void vimw::strInputFun(int vk_code)
{
	if ((!shift) && (!caps_lock))
	{
		new_content.push_back(key_map[vk_code].first);
	}
	else if ((shift) && (!caps_lock))
	{

		new_content.push_back(key_map[vk_code].second);
	}
	else if ((!shift) && (caps_lock))
	{
		if (vk_code >= 0x41 && vk_code <= 0x5A)
		{
			new_content.push_back(key_map[vk_code].second);
		}
		else
		{
			new_content.push_back(key_map[vk_code].first);
		}
	}
	else if ((shift) && (caps_lock))
	{
		if (vk_code >= 0x41 && vk_code <= 0x5A)
		{
			new_content.push_back(key_map[vk_code].first);
		}
		else
		{
			new_content.push_back(key_map[vk_code].second);
		}
	}
}
void vimw::infoLoop()
{
	while (true)
	{
		SLEEPINST;
		if (KEY_DOWN(VK_BACK))
		{
			if (!all_key_status[VK_BACK])
			{
				all_key_status[VK_BACK] = true;
				if (!new_content.empty())
				{
					new_content.pop_back();
					showOptionInfo();
				}
			}
		}
		else
		{
			all_key_status[VK_BACK] = false;
		}

		for (const auto& i : key_map)
		{
			if (KEY_DOWN(i.first))
			{
				if (!all_key_status[i.first])
				{
					all_key_status[i.first] = true;
					ctrl = KEY_DOWN(VK_CONTROL);
					caps_lock = KEY_TOGGLE(VK_CAPITAL);
					shift = KEY_DOWN(VK_SHIFT);
					if (checkSaveAndDoReturn(i.first))
					{
						system("cls");
						return;
					}
					if (lock_status)
					{
						strInputFun(i.first);
					}
					showOptionInfo();
				}
			}
			else
			{
				all_key_status[i.first] = false;
			}
		}
	}
}
int vimw::runProg() {
	//初始化阶段
	showOptionInfo();
	infoLoop();
	return 0;
}

bool vimw::editable(bool ex_lock)
{
	lock_status = ex_lock;
	return true;
}
