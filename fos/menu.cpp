#include "menu.h"
#include "catalog.h"
#include "user.h"
#include "inode.h"
#include<windows.h>
vector<string> Menu::getLine()
{
	char c;
	vector<string>vec;
	string str;
	while (c = getchar()) {
		if (c == '\n') {
			vec.push_back(str);
			return vec;
		}
		else if (c == ' ') {
			vec.push_back(str);
			str = "";
		}
		else {
			str += c;
		}
	}
	return vector<string>();
}

void Menu::debug(bool flag)
{
	DEBUG = flag;
}

void Menu::signin()
{
	string username;
	string passwd, re_passwd;
	cout << "请输入用户名：";
	cin >> username;
	cout << "请输入用户密码：";
	cin >> passwd;
	cout << "请再次输入用户密码：";
	cin >> re_passwd;
	getchar();
	if (passwd != re_passwd) {
		cout << "两次输入的密码不一致" << endl;
	}
	else {
		int status = UserController::signIn(username, passwd);
		if (status == Status::userSignInFailedNameNull) {
			cout << "用户名为空" << endl;
		}
		else if (status == Status::userSignInFailedPassNull) {
			cout << "密码为空" << endl;
		}
		else if (status == Status::userSignInFailedNameOverflow) {
			cout << "用户名过长" << endl;
		}
		else if (status == Status::userSignInFailedPassOverflow) {
			cout << "密码过长" << endl;
		}
		else if (status == Status::userSignInFailedUserExisted) {
			cout << "用户已存在" << endl;
		}
		else if (status == Status::userSignInFailedUserOverflow) {
			cout << "当前用户列表注册已满，请删掉部分用户后重新注册。" << endl;
		}
		else {
			cout << "注册成功" << endl;
		}
	}
}

void Menu::signup()
{
	string username;
	string passwd, re_passwd;
	cout << "请输入用户名：";
	cin >> username;
	cout << "请输入用户密码：";
	cin >> passwd;
	getchar();

	int status = UserController::signUp(username, passwd);
	if (status == Status::userSignUpFailedUserNotFound) {
		cout << "未找到该用户" << endl;
	}
	else if (status == Status::userSignUpFailedPassError) {
		cout << "输入的用户密码错误" << endl;
	}
	else {
		//success
	}


}



void Menu::mkdir(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::mkdir(instruction[1]);
		if (flag == true) {
			cout << "该目录下已存在与 " << instruction[1] << " 同名目录" << endl;
		}
	}
	else {
		cout << "mkdir指令有误" << endl;
	}
}

void Menu::touch(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::touch(instruction[1]);
		if (flag == true) {
			cout << "该目录下已存在与 " << instruction[1] << " 同名文件" << endl;
		}
	}
	else {
		cout << "touch指令有误" << endl;
	}
}


void Menu::cd(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::cd(instruction[1]);
		if (flag) {
			cout << "要进入的目录 " << instruction[1] << " 不存在，请重新输入" << endl;
		}
	}
	else {
		cout << "cd指令有误" << endl;
	}
}

void Menu::ls(vector<string> instruction)
{
	if (instruction.size() == 1) {
		Catalog cata = CatalogController::ls();
		bool dir_exist = false;
		bool ftxt_exist = false;
		bool undefined_exist = false;
		bool link_exist = false;
		HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		//打印目录
		SetConsoleTextAttribute(outHandle, 0x0d);//淡紫色
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == dir) {
				cout << cata.recodes.at(i).name << "\t";
				dir_exist = true;
			}
		}
		if (dir_exist) {
			cout << endl;
		}
		//打印txt
		SetConsoleTextAttribute(outHandle, 0x06);//黄色
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == ftxt) {
				cout << cata.recodes.at(i).name << "\t";
				ftxt_exist = true;
			}
		}
		if (ftxt_exist) {
			cout << endl;
		}
		//打印link
		SetConsoleTextAttribute(outHandle, 0x0b);//红色
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == link) {
				cout << cata.recodes.at(i).name << "\t";
				link_exist = true;
			}
		}
		if (link_exist) {
			cout << endl;
		}
		//打印undifined
		SetConsoleTextAttribute(outHandle, 0x08);//浅绿色
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == undefined) {
				cout << cata.recodes.at(i).name << "\t";
				undefined_exist = true;
			}
		}
		if (undefined_exist) {
			cout << endl;
		}


		SetConsoleTextAttribute(outHandle, 0x07);//白色
		if ((!dir_exist) && (!ftxt_exist) && (!undefined_exist) && (!link_exist)) {
			cout << "目录为空" << endl;
		}
	}
	else if (instruction.size() == 2) {
		bool dir_exist = false;
		bool ftxt_exist = false;
		bool undefined_exist = false;
		bool link_exist = false;
		HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		Catalog cata = CatalogController::ls(instruction[1]);
		if (cata.parent == -2) {//目录不存在
			cout << "要查看的路径不存在" << endl;
		}
		else {
			//打印目录
			SetConsoleTextAttribute(outHandle, 0x0d);//淡紫色
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == dir) {
					cout << cata.recodes.at(i).name << "\t";
					dir_exist = true;
				}
			}
			if (dir_exist) {
				cout << endl;
			}
			//打印txt
			SetConsoleTextAttribute(outHandle, 0x06);//黄色
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == ftxt) {
					cout << cata.recodes.at(i).name << "\t";
					ftxt_exist = true;
				}
			}
			if (ftxt_exist) {
				cout << endl;
			}
			//打印link
			SetConsoleTextAttribute(outHandle, 0x0b);//lv色
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == link) {
					cout << cata.recodes.at(i).name << "\t";
					link_exist = true;
				}
			}
			if (link_exist) {
				cout << endl;
			}
			//打印undifined
			SetConsoleTextAttribute(outHandle, 0x08);//hui se
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == undefined) {
					cout << cata.recodes.at(i).name << "\t";
					undefined_exist = true;
				}
			}
			if (undefined_exist) {
				cout << endl;
			}
			SetConsoleTextAttribute(outHandle, 0x07);//白色
			if ((!dir_exist) && (!ftxt_exist) && (!undefined_exist) && (!link_exist)) {
				cout << "目录为空" << endl;
			}
		}


	}
	else {
		cout << "ls指令有误" << endl;
	}
}

void Menu::debug(vector<string> instruction)
{
	if (instruction.size() < 2) {
		cout << "debug指令参数错误" << endl;
	}
	else {
		if (instruction[1] == "-t") {
			Menu::debug(true);
		}
		else if (instruction[1] == "-f") {
			Menu::debug(false);
		}
		else {
			cout << "debug指令参数错误" << endl;
		}

	}
}



void Menu::rename(vector<string> instruction)
{
	// size<=3
	if (instruction.size() < 3) {
		cout << "rename指令有误,请重新输入。" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 4) {
				int ret = CatalogController::rename(instruction[2], instruction[3], true);
				if (ret == 2) {
					cout << "重命名失败，原目录不存在" << endl;
				}
				else if (ret == 1) {
					cout << "重命名失败，新目录名不是一个合法的目录名" << endl;
				}
				else if (ret == 3) {
					cout << "重命名失败，新目录名已在当前目录下存在" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "rename指令重命名目录的参数应为[原目录名] [新目录名]" << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "rename指令重命名目录参数有误,应为-r" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::rename(instruction[1], instruction[2]);
					if (ret == 2) {
						cout << "重命名失败，原文件不存在" << endl;
					}
					else if (ret == 1) {
						cout << "重命名失败，新文件名不是一个合法的文件名" << endl;
					}
					else if (ret == 3) {
						cout << "重命名失败，新文件名已在当前文件下存在" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "rename指令重命名文件的参数应为[原文件名] [新文件名]，请重新输入" << endl;
				}
			}
		}
	}

}

void Menu::rm(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "rm指令有误,请重新输入。" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 3) {
				int ret = CatalogController::rm(instruction[2], true);
				if (ret == 2) {
					cout << "删除失败，目录 " << instruction[2] << " 不存在" << endl;
				}
				else if (ret == 8) {
					cout << "删除成功，保留打开文件" << endl;
				}
				else {
					//success
				}
			}
			// size==2||>3
			else {
				cout << "rm指令删除目录的参数应为 rm -r [dir] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "rm指令删除目录参数有误,应为-r" << endl;
			}
			else {
				if (instruction.size() == 2) {
					int ret = CatalogController::rm(instruction[1]);
					if (ret == 2) {
						cout << "删除失败，文件 " << instruction[1] << " 不存在" << endl;
					}
					else if (ret == 9) {
						cout << "删除失败，文件 " << instruction[1] << " 未关闭" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "rm指令删除文件的参数应为 rm [file] ，请重新输入" << endl;
				}
			}
		}
	}
}

void Menu::cp(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "cp指令有误,请重新输入。" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 3//单参数目录cp
			if (instruction.size() == 3) {
				Menu::cpDir(instruction);
			}
			//size == 4////双参数目录cp,先cp，再paste
			else if (instruction.size() == 4) {
				int ret = Menu::cpDir(instruction);//先cp
				if (ret != 1) {//若第一步复制完成，则执行paste，否则不用执行
					string useless = instruction[2];
					string dest = instruction[3];
					vector<string> copy;
					copy.push_back(useless);
					copy.push_back(dest);
					Menu::paste(copy);//再paste
				}
				else {
					//success
				}

			}
			// size==2||>4
			else {
				cout << "cp指令复制目录的参数应为 cp -r [dir] / cp -r [dir] [dest] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "cp指令复制目录参数有误,应为-r" << endl;
			}
			else {
				if (instruction.size() == 2) {//单参数文件cp
					Menu::cpFile(instruction);//先cp

				}
				//双参数文件cp
				else if (instruction.size() == 3) {
					int ret = Menu::cpFile(instruction);//先cp
					if (ret != 1) {//若第一步复制完成，则执行paste，否则不用执行
						string useless = instruction[1];
						string dest = instruction[2];
						vector<string> copy;
						copy.push_back(useless);
						copy.push_back(dest);
						Menu::paste(copy);//再paste
					}
					else {
						//success
					}
				}
				else {
					cout << "cp指令复制文件的参数应为 cp [file] / cp [file] [dest]" << endl;
				}
			}
		}
	}
}

int Menu::cpDir(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[2], true, false);
	if (ret == 1) {
		cout << "复制失败，目录 " << instruction[2] << " 不存在" << endl;
		return 1;
	}
	else {
		//success
	}
}

int Menu::cpFile(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[1], false, false);
	if (ret == 1) {
		cout << "复制失败，文件 " << instruction[1] << " 不存在" << endl;
		return 1;
	}
	else {
		//success
	}
}

void Menu::paste(vector<string> instruction)
{
	if ((instruction.size() == 1) || (instruction.size() == 2)) {
		int ret;
		if (instruction.size() == 1) {
			ret = CatalogController::paste("", false);
		}
		else if (instruction.size() == 2) {
			ret = CatalogController::paste(instruction[1], false);
		}
		if (ret == 1) {
			cout << "粘贴失败，缓存中没有数据" << endl;
		}
		else if (ret == 2) {
			cout << "粘贴失败，所复制的文件名或目录名已在当前目录存在" << endl;
		}
		else if (ret == 3) {
			cout << "粘贴失败，所粘贴的目标不是一个目录" << endl;
		}
		else {
			//success
		}
	}
	else {
		cout << "paste指令有误" << endl;
	}
}

void Menu::mv(vector<string> instruction)
{
	// size<=3
	if (instruction.size() < 3) {
		cout << "mv指令有误" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 4) {
				int ret = CatalogController::mv(instruction[2], instruction[3], true);
				if (ret == 1) {
					cout << "移动失败，原目录不存在" << endl;
				}
				else if (ret == 2) {
					cout << "移动失败，目标路径中已存在同名目录" << endl;
				}
				else if (ret == 3) {
					cout << "移动失败，移动的目标地址不是一个合法路径" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "mv指令移动目录的参数应为[移动目录名] [目标路径]" << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "mv指令移动目录参数有误,应为-r" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::mv(instruction[1], instruction[2], false);
					if (ret == 1) {
						cout << "移动失败，原文件不存在" << endl;
					}
					else if (ret == 2) {
						cout << "移动失败，目标路径中已存在同名文件" << endl;
					}
					else if (ret == 3) {
						cout << "移动失败，移动的目标地址不是一个合法路径" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "mv指令移动文件的参数应为[移动文件名] [目标路径]" << endl;
				}
			}
		}
	}

}

void Menu::cut(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "cut指令有误" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 3
			if (instruction.size() == 3) {
				Menu::cutDir(instruction);
			}
			//size == 4////双参数目录cut,先cp，再paste
			else if (instruction.size() == 4) {

				int ret = Menu::cutDir(instruction);//先cut

				if (ret != 1) {//若第一步复制完成，则执行paste，否则不用执行
					string useless = instruction[2];
					string dest = instruction[3];
					vector<string> copy;
					copy.push_back(useless);
					copy.push_back(dest);
					Menu::paste(copy);//再paste
				}
				else {
					//success
				}

			}
			// size==2||>3
			else {
				cout << "cut指令剪切目录的参数应为 cut -r [dir] / cut -r [dir] [dest] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "cut指令剪切目录参数有误,应为-r" << endl;
			}
			else {
				if (instruction.size() == 2) {
					Menu::cutFile(instruction);

				}
				//双参数文件cut
				else if (instruction.size() == 3) {
					int ret = Menu::cutFile(instruction);//先cut
					if (ret != 1) {//若第一步复制完成，则执行paste，否则不用执行
						string useless = instruction[1];
						string dest = instruction[2];
						vector<string> copy;
						copy.push_back(useless);
						copy.push_back(dest);
						Menu::paste(copy);//再paste
					}
					else {
						//success
					}
				}
				else {
					cout << "cut指令剪切文件的参数应为 cut [file] / cut [file] [dest]" << endl;
				}
			}
		}
	}
}

int Menu::cutDir(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[2], true, true);
	if (ret == 1) {
		cout << "剪切失败，目录 " << instruction[2] << " 不存在" << endl;
		return 1;
	}
	else {
		//success
	}
}

int Menu::cutFile(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[1], false, true);
	if (ret == 1) {
		cout << "剪切失败，文件 " << instruction[1] << " 不存在" << endl;
		return 1;
	}
	else {
		//success
	}
}

void Menu::ln(vector<string> instruction)
{
	// size<=3
	if (instruction.size() < 3) {
		cout << "ln指令有误,请重新输入。" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-s") {
			//size == 4 软连接文件 ln -s [dirA] [dirB]
			if (instruction.size() == 4) {
				int ret = CatalogController::link(instruction[2], instruction[3], 1);
				if (ret == 1) {
					cout << "软链接文件失败，原文件不存在" << endl;
				}
				else if (ret == 2) {
					cout << "软链接文件失败，目标路径下已存在同名文件" << endl;
				}
				else if (ret == 3) {
					cout << "软链接文件失败，目标路径不是一个合法的目录名" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "ln指令软链接文件的参数应为 ln -s [dirA] [dirB] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "ln指令软链接文件参数有误,应为 -s" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::link(instruction[1], instruction[2], 0);
					if (ret == 1) {
						cout << "硬链接文件失败，原文件不存在" << endl;
					}
					else if (ret == 2) {
						cout << "硬链接文件失败，目标路径下已存在同名文件" << endl;
					}
					else if (ret == 3) {
						cout << "硬链接文件失败，目标路径不是一个合法的目录名" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "ln指令硬链接文件的参数应为 ln [diaA] [dirB]" << endl;
				}
			}
		}
	}
}

void Menu::open(vector<string> instruction)
{
	// size<3
	if (instruction.size() < 3) {
		cout << "open指令有误" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 3 读方式打开
			if (instruction.size() == 3) {
				int ret = CatalogController::open(instruction[2], FileOpenStatus::read);
				if (ret) {
					cout << "文件打开失败，文件路径不存在" << endl;
				}
				else {
					//success
				}
			}
			// size>3
			else {
				cout << "open指令以读方式打开文件的参数应为 open -r [file] " << endl;
			}
		}
		else if (instruction[1] == "-w") {
			//size == 3 读方式打开
			if (instruction.size() == 3) {
				int ret = CatalogController::open(instruction[2], FileOpenStatus::write);
				if (ret) {
					cout << "文件打开失败，文件路径不存在" << endl;
				}
				else {
					//success
				}
			}
			// size>3
			else {
				cout << "open指令以写方式打开文件的参数应为 open -w [file] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "open指令打开文件参数有误,应为 -r/-w " << endl;
			}
		}
	}
}

void Menu::close(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::close(instruction[1]);
		if (flag) {
			cout << "文件关闭失败，文件不存在" << endl;
		}
	}
	else {
		cout << "close指令有误" << endl;
	}
}

void Menu::read(vector<string> instruction)
{
	if (instruction.size() == 2) {
		int flag = CatalogController::read(instruction[1]);

		if (flag == 1) {
			cout << "文件读取失败，文件不存在" << endl;
		}
		else if (flag == 2) {
			cout << "文件读取失败，文件未打开" << endl;
		}
		else {
			cin.clear();
			char c;
			cin.sync();
			while (c = getchar()) {
				if (c == 19 || c == 26 || c == -1) {
					break;
				}
			}
			getchar();
			system("cls");
			//success
		}
	}
	else {
		cout << "read指令有误" << endl;
	}
}

void Menu::write(vector<string> instruction)
{
	if (instruction.size() == 2) {
		int flag = CatalogController::write(instruction[1]);

		if (flag == 1) {
			cout << "文件写入失败，文件不存在" << endl;
		}
		else if (flag == 2) {
			cout << "文件写入失败，文件未打开" << endl;
		}
		else if (flag == 3) {
			cout << "文件写入失败，文件没有写权限" << endl;
		}
		else {
			cin.clear();
			char c;
			cin.sync();
			while (c = getchar()) {
				if (c == 19 || c == 26 || c == -1) {
					break;
				}
			}
			getchar();
			system("cls");
			//success
		}
	}
	else {
		cout << "write指令有误" << endl;
	}
}

void Menu::help()
{
	cout << endl;
	cout << "欢迎使用FOS文件管理系统，系统指令帮助文档如下：" << endl;
	cout << endl;
	cout << "指令集分为三大类：系统指令、目录指令和文件指令" << endl;
	cout << "一.系统指令" << endl;
	cout << "   1.注册：               signin" << endl;
	cout << "   2.登录：               signup" << endl;
	cout << "   3.登出：               signout" << endl;
	cout << "   4.调试：               debug" << endl;
	cout << "   5.格式化：             format" << endl;
	cout << "   6.关机：               shutdown" << endl;
	cout << "   7.重命名目录：         rename -r [dirA] [dirB]" << endl;
	cout << "   8.帮助：               help" << endl;
	cout << "   9.清屏：               clear" << endl;
	cout << "   10.修改当前用户密码：  pass" << endl;
	cout << "   11.修改指定用户密码：  pass [user]" << endl;
	cout << "   12.展示用户列表：      userlist" << endl;
	cout << "   13.更改管理员：        su" << endl;
	cout << "   14.输出空闲块列表：    disklist -e [page]" << endl;
	cout << "   15.输出占用块列表：    disklist -f [page]" << endl;
	cout << "   16.输出所有块列表：    disklist -a [page] " << endl;
	cout << "   17.删除指定用户：      deluser [user_name] " << endl;
	cout << endl;

	cout << "二.目录操作" << endl;
	cout << "   1.创建目录：           mkdir [dir]" << endl;
	cout << "   2.查看当前目录文件：   ls" << endl;
	cout << "   3.查看指定目录下文件： ls [dir]" << endl;
	cout << "   4.删除目录：           rm -r [dir]" << endl;
	cout << "   5.移动目录：           mv -r [dirA] [dirB]" << endl;
	cout << "   6.重命名目录：         rename -r [dirA] [dirB]" << endl;
	cout << "   7.进入目录：           cd [dir]" << endl;
	cout << "   8.双参数复制目录：     cp -r [dirA] [dirB]" << endl;
	cout << "   9.单参数复制目录：     cp -r [dir]" << endl;
	cout << "   10.单参数剪切：        cut -r [dir]" << endl;
	cout << "   11.双参数剪切：        cut -r [dirA] [dirB]" << endl;
	cout << "   12.粘贴目录：          paste [dir]" << endl;
	cout << endl;
	cout << "三.文件操作" << endl;
	cout << "   1.创建文件：           touch [file]" << endl;
	cout << "   2.删除文件：           rm [file]" << endl;
	cout << "   3.移动：               mv [fileA] [fileB]" << endl;
	cout << "   4.重命名文件：         rename [dirA] [dirB]" << endl;
	cout << "   5.双参数复制文件：     cp [file] [dir]" << endl;
	cout << "   6.单参数复制文件：     cp [file]" << endl;
	cout << "   7.单参数剪切文件：     cut [file]" << endl;
	cout << "   8.双参数剪切文件：     cut [file] [dir]" << endl;
	cout << "   9.粘贴文件：           paste [dir]" << endl;
	cout << "   10.硬链接文件：        ln [dirA] [dirB]" << endl;
	cout << "   11.软链接文件：        ln -s [dirA] [dirB]" << endl;
	cout << "   12.读方式打开：        open -r [file]" << endl;
	cout << "   13.写方式打开：        open -w [file]" << endl;
	cout << "   14.读文件：            read [file]" << endl;
	cout << "   15.写文件：            write [file]" << endl;
	cout << "   16.查找：              find [file] [dir]" << endl;
	cout << "   17.级联查找：          find -r [file] [dir]" << endl;
	cout << "   18.模糊查找：          find -a [file] [dir]" << endl;
	cout << "   19.级联模糊查找：      find -ar [file] [dir]" << endl;
	cout << endl;


}

void Menu::pass(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "pass指令有误" << endl;
	}
	else {
		if (instruction.size() == 1) {
			//用户更改
			string passwd, re_passwd;
			cout << "请输入新密码：";
			cin >> passwd;
			cout << "请再次输入新密码：";
			cin >> re_passwd;
			getchar();
			if (passwd != re_passwd) {
				cout << "两次密码不一致" << endl;
			}
			else {
				UserController::newPass(passwd);
				cout << "修改成功" << endl;
			}
		}
		else {
			//管理员更改
			if (USER_NAME == "root") {
				string passwd, re_passwd;
				cout << "请输入新密码：";
				cin >> passwd;
				cout << "请再次输入新密码：";
				cin >> re_passwd;
				getchar();
				if (passwd != re_passwd) {
					cout << "两次密码不一致" << endl;
				}
				else {
					int ret = UserController::newPass(instruction[1], passwd);
					if (!ret) {
						cout << "修改成功" << endl;
					}
					else if (ret == 1) {
						cout << "该用户无管理员权限，无法修改用户密码" << endl;
					}
					else if (ret == 2) {
						cout << "该用户不存在" << endl;
					}

				}
			}
			else {
				cout << "该用户无管理员权限，无法修改用户密码" << endl;
			}

		}
	}

}

void Menu::disklist(vector<string> instruction)
{
	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	int page = 1;
	// size>3
	if (instruction.size() > 3 || instruction.size() < 2) {
		cout << "disklist指令查看空间块列表的参数应为 disklist -e / -f / -a [page] " << endl;
		return;
	}
	if (instruction.size() == 3) {
		stringstream ss;
		ss << instruction[2];
		ss >> page;
		if (page <= 0) {
			cout << "页数应大于0" << endl;
			return;
		}
	}
	vector<pair<short, bool>> ret;
	if (instruction[1] == "-e") {
		ret = Disk::list(0);
	}
	else if (instruction[1] == "-f") {
		ret = Disk::list(1);
	}
	else if (instruction[1] == "-a") {
		ret = Disk::list(2);
	}
	else if (instruction[1].at(0) == '-') {
		cout << "disklist指令的参数有误,应为 -e/-f/-a " << endl;
		return;
	}
	int len = ret.size() / 20 + 1;
	if (page > len && page != 1) {
		cout << "页数应小于总页数（" << len << "）" << endl;
		return;
	}
	cout << "共有" << ret.size() << "条记录，共" << len << "页，当前为第" << page << "页" << endl;
	for (int i = 0 + page * 20 - 20; i < ret.size() && i < page * 20; i++) {
		cout << ret[i].first << "号块：";
		if (ret[i].second) {
			SetConsoleTextAttribute(outHandle, 0x04);
			cout << "已占用" << endl;
			SetConsoleTextAttribute(outHandle, 0x07);
		}
		else {
			SetConsoleTextAttribute(outHandle, 0x02);
			cout << "未占用" << endl;
			SetConsoleTextAttribute(outHandle, 0x07);
		}
	}

}

void Menu::delUser(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "deluser指令有误" << endl;
	}
	else {
		//管理员更改
		if (USER_NAME == "root") {
			Status ret = UserController::delUser(instruction[1]);
			if (ret == Status::userSignUpFailedUserNotFound) {
				cout << "该用户不存在" << endl;
			}
			else {
				cout << "删除成功" << endl;
			}
		}
		else {
			cout << "该用户无管理员权限" << endl;
		}
	}
}

void Menu::test(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "test指令有误" << endl;
	}
	string str;
	for (int i = 0; i < 4096; i++) {
		str += i % 10 + '0';
	}
	CatalogController::touch(instruction[1], str);

}

void Menu::find(vector<string> instruction)
{
	// size<3
	if (instruction.size() < 3) {
		cout << "find指令有误,请重新输入。" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 3 级联搜索
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::fd(instruction[2], instruction[3], true);
				if (ret.size() == 0) {
					cout << "查找结果为空" << endl;
				}
				else {
					cout << "共找到" << ret.size() << "条记录：" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "find指令级联查找的参数应为 find -r [file] [dir] " << endl;
			}
		}
		//模糊
		else if (instruction[1] == "-a") {
			//size == 3 模糊搜索
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::similarFd(instruction[2], instruction[3], false);
				if (ret.size() == 0) {
					cout << "查找结果为空" << endl;
				}
				else {
					cout << "共找到" << ret.size() << "条记录：" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "find指令模糊查找的参数应为 find -a [file] [dir] " << endl;
			}
		}
		else if (instruction[1] == "-ar") {
			//size == 3 级联and模糊搜索
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::similarFd(instruction[2], instruction[3], true);
				if (ret.size() == 0) {
					cout << "查找结果为空" << endl;
				}
				else {
					cout << "共找到" << ret.size() << "条记录：" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "find指令级联模糊查找的参数应为 find -ar [file] [dir] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "find指令查找的参数有误,应为 -a/-r/-ar" << endl;
			}
			else {
				if (instruction.size() == 3) {
					vector <pair<Ftxt, string>> ret;
					ret = CatalogController::fd(instruction[1], instruction[2], false);
					if (ret.size() == 0) {
						cout << "查找结果为空" << endl;
					}
					else {
						cout << "共找到" << ret.size() << "条记录：" << endl;
						for (auto p : ret) {
							cout << p.second << endl;
						}
					}
				}
				else {
					cout << "find指令查找的参数应为 find [file] [dir] " << endl;
				}
			}
		}
	}
}

void Menu::su()
{
	string passwd;
	cout << "请输入root密码：";
	cin >> passwd;
	getchar();

	int status = UserController::su(passwd);
	if (status == Status::userSignUpFailedUserNotFound) {
		cout << "root未创建" << endl;
	}
	else if (status == Status::userSignUpFailedPassError) {
		cout << "root密码错误" << endl;
	}
	else {
		//success
	}
}

void Menu::userlist()
{
	if (USER_NAME == "root") {
		vector<User> userlist = UserController::getList();
		for (User user : userlist) {
			cout << (short)user._id << '\t' << user.name << '\t' << user.pass << endl;
		}
	}
	else {
		cout << "该用户无管理员权限，无法查看用户列表" << endl;
	}
}





void Menu::start() {

	// 开机进入 /
	// cd /home/[USER_NAME]
	// mkdir /home/[USER_NAME]
	// path = /home/[USER_NAME]

	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	cout << "请登录(signup)后使用本系统。" << endl;
	while (1) {
		SetConsoleTextAttribute(outHandle, 0x0A);
		cout << USER_NAME;
		SetConsoleTextAttribute(outHandle, 0x07);
		cout << ":";
		SetConsoleTextAttribute(outHandle, 0x03);
		cout << CatalogController::path;
		SetConsoleTextAttribute(outHandle, 0x07);
		cout << "$ ";
		vector<string>instruction = Menu::getLine();

		//先分析非重复的指令，再分析目录和文件操作同名的指令
		//先分析非重复的指令,共 9 个.SignIn  SignUp ,  shutdown ,mkdir cd touch vi ls debug
		if (instruction[0].size() == 0);
		else if (instruction[0] == "signup") { //登录
			Menu::signup();
		}
		else if (instruction[0] == "signin") {//注册
			Menu::signin();
		}
		else if (instruction[0] == "shutdown") {
			return;
		}
		else if (instruction[0] == "format") {
			isFormat = true;
			return;
		}
		else if (instruction[0] == "debug") {
			Menu::debug(instruction);
		}
		else if (USER_NAME == "") {
			cout << "请先登录或注册后使用此系统" << endl;
			cout << "使用signup进行登录" << endl;
			cout << "使用signin进行注册" << endl;
		}
		else if (instruction[0] == "mkdir") {
			Menu::mkdir(instruction);
		}
		else if (instruction[0] == "debug") {
			Menu::debug(instruction);
		}
		else if (instruction[0] == "touch") {
			Menu::touch(instruction);
		}
		else if (instruction[0] == "cd") {
			Menu::cd(instruction);
		}
		else if (instruction[0] == "ls") {
			Menu::ls(instruction);
		}
		//再分析目录操作和文件操作同名的指令
		else if (instruction[0] == "rename") {
			Menu::rename(instruction);
		}
		else if (instruction[0] == "rm") {
			Menu::rm(instruction);
		}
		else if (instruction[0] == "cp") {
			Menu::cp(instruction);
		}
		else if (instruction[0] == "paste") {
			Menu::paste(instruction);
		}
		else if (instruction[0] == "mv") {
			Menu::mv(instruction);
		}
		else if (instruction[0] == "cut") {
			Menu::cut(instruction);
		}
		else if (instruction[0] == "ln") {
			Menu::ln(instruction);
		}
		else if (instruction[0] == "open") {
			Menu::open(instruction);
		}
		else if (instruction[0] == "close") {
			Menu::close(instruction);
		}
		else if (instruction[0] == "read") {
			Menu::read(instruction);
		}
		else if (instruction[0] == "write") {
			Menu::write(instruction);
		}
		else if (instruction[0] == "help") {
			Menu::help();
		}
		else if (instruction[0] == "clear") {
			system("cls");
		}
		else if (instruction[0] == "pass") {
			Menu::pass(instruction);
		}
		else if (instruction[0] == "su") {
			Menu::su();
		}
		else if (instruction[0] == "userlist") {
			Menu::userlist();
		}
		else if (instruction[0] == "disklist") {
			Menu::disklist(instruction);
		}
		else if (instruction[0] == "find") {
			Menu::find(instruction);
		}
		else if (instruction[0] == "signout") {
			USER = 255;
			USER_NAME = "";
			CatalogController::cd("/");
		}
		else if (instruction[0] == "deluser") {
			Menu::delUser(instruction);
		}
		else if (instruction[0] == "test") {
			Menu::test(instruction);
		}
		else {
			cout << "该指令不存在" << endl;
		}
		//
		/*instruction.clear();*/
	}

}