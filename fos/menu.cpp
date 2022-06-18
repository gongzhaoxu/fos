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
	cout << "�������û�����";
	cin >> username;
	cout << "�������û����룺";
	cin >> passwd;
	cout << "���ٴ������û����룺";
	cin >> re_passwd;
	getchar();
	if (passwd != re_passwd) {
		cout << "������������벻һ��" << endl;
	}
	else {
		int status = UserController::signIn(username, passwd);
		if (status == Status::userSignInFailedNameNull) {
			cout << "�û���Ϊ��" << endl;
		}
		else if (status == Status::userSignInFailedPassNull) {
			cout << "����Ϊ��" << endl;
		}
		else if (status == Status::userSignInFailedNameOverflow) {
			cout << "�û�������" << endl;
		}
		else if (status == Status::userSignInFailedPassOverflow) {
			cout << "�������" << endl;
		}
		else if (status == Status::userSignInFailedUserExisted) {
			cout << "�û��Ѵ���" << endl;
		}
		else if (status == Status::userSignInFailedUserOverflow) {
			cout << "��ǰ�û��б�ע����������ɾ�������û�������ע�ᡣ" << endl;
		}
		else {
			cout << "ע��ɹ�" << endl;
		}
	}
}

void Menu::signup()
{
	string username;
	string passwd, re_passwd;
	cout << "�������û�����";
	cin >> username;
	cout << "�������û����룺";
	cin >> passwd;
	getchar();

	int status = UserController::signUp(username, passwd);
	if (status == Status::userSignUpFailedUserNotFound) {
		cout << "δ�ҵ����û�" << endl;
	}
	else if (status == Status::userSignUpFailedPassError) {
		cout << "������û��������" << endl;
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
			cout << "��Ŀ¼���Ѵ����� " << instruction[1] << " ͬ��Ŀ¼" << endl;
		}
	}
	else {
		cout << "mkdirָ������" << endl;
	}
}

void Menu::touch(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::touch(instruction[1]);
		if (flag == true) {
			cout << "��Ŀ¼���Ѵ����� " << instruction[1] << " ͬ���ļ�" << endl;
		}
	}
	else {
		cout << "touchָ������" << endl;
	}
}


void Menu::cd(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::cd(instruction[1]);
		if (flag) {
			cout << "Ҫ�����Ŀ¼ " << instruction[1] << " �����ڣ�����������" << endl;
		}
	}
	else {
		cout << "cdָ������" << endl;
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
		//��ӡĿ¼
		SetConsoleTextAttribute(outHandle, 0x0d);//����ɫ
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == dir) {
				cout << cata.recodes.at(i).name << "\t";
				dir_exist = true;
			}
		}
		if (dir_exist) {
			cout << endl;
		}
		//��ӡtxt
		SetConsoleTextAttribute(outHandle, 0x06);//��ɫ
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == ftxt) {
				cout << cata.recodes.at(i).name << "\t";
				ftxt_exist = true;
			}
		}
		if (ftxt_exist) {
			cout << endl;
		}
		//��ӡlink
		SetConsoleTextAttribute(outHandle, 0x0b);//��ɫ
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == link) {
				cout << cata.recodes.at(i).name << "\t";
				link_exist = true;
			}
		}
		if (link_exist) {
			cout << endl;
		}
		//��ӡundifined
		SetConsoleTextAttribute(outHandle, 0x08);//ǳ��ɫ
		for (int i = 0; i < cata.recodes.size(); i++) {
			if (cata.recodes.at(i).type == undefined) {
				cout << cata.recodes.at(i).name << "\t";
				undefined_exist = true;
			}
		}
		if (undefined_exist) {
			cout << endl;
		}


		SetConsoleTextAttribute(outHandle, 0x07);//��ɫ
		if ((!dir_exist) && (!ftxt_exist) && (!undefined_exist) && (!link_exist)) {
			cout << "Ŀ¼Ϊ��" << endl;
		}
	}
	else if (instruction.size() == 2) {
		bool dir_exist = false;
		bool ftxt_exist = false;
		bool undefined_exist = false;
		bool link_exist = false;
		HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		Catalog cata = CatalogController::ls(instruction[1]);
		if (cata.parent == -2) {//Ŀ¼������
			cout << "Ҫ�鿴��·��������" << endl;
		}
		else {
			//��ӡĿ¼
			SetConsoleTextAttribute(outHandle, 0x0d);//����ɫ
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == dir) {
					cout << cata.recodes.at(i).name << "\t";
					dir_exist = true;
				}
			}
			if (dir_exist) {
				cout << endl;
			}
			//��ӡtxt
			SetConsoleTextAttribute(outHandle, 0x06);//��ɫ
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == ftxt) {
					cout << cata.recodes.at(i).name << "\t";
					ftxt_exist = true;
				}
			}
			if (ftxt_exist) {
				cout << endl;
			}
			//��ӡlink
			SetConsoleTextAttribute(outHandle, 0x0b);//lvɫ
			for (int i = 0; i < cata.recodes.size(); i++) {
				if (cata.recodes.at(i).type == link) {
					cout << cata.recodes.at(i).name << "\t";
					link_exist = true;
				}
			}
			if (link_exist) {
				cout << endl;
			}
			//��ӡundifined
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
			SetConsoleTextAttribute(outHandle, 0x07);//��ɫ
			if ((!dir_exist) && (!ftxt_exist) && (!undefined_exist) && (!link_exist)) {
				cout << "Ŀ¼Ϊ��" << endl;
			}
		}


	}
	else {
		cout << "lsָ������" << endl;
	}
}

void Menu::debug(vector<string> instruction)
{
	if (instruction.size() < 2) {
		cout << "debugָ���������" << endl;
	}
	else {
		if (instruction[1] == "-t") {
			Menu::debug(true);
		}
		else if (instruction[1] == "-f") {
			Menu::debug(false);
		}
		else {
			cout << "debugָ���������" << endl;
		}

	}
}



void Menu::rename(vector<string> instruction)
{
	// size<=3
	if (instruction.size() < 3) {
		cout << "renameָ������,���������롣" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 4) {
				int ret = CatalogController::rename(instruction[2], instruction[3], true);
				if (ret == 2) {
					cout << "������ʧ�ܣ�ԭĿ¼������" << endl;
				}
				else if (ret == 1) {
					cout << "������ʧ�ܣ���Ŀ¼������һ���Ϸ���Ŀ¼��" << endl;
				}
				else if (ret == 3) {
					cout << "������ʧ�ܣ���Ŀ¼�����ڵ�ǰĿ¼�´���" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "renameָ��������Ŀ¼�Ĳ���ӦΪ[ԭĿ¼��] [��Ŀ¼��]" << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "renameָ��������Ŀ¼��������,ӦΪ-r" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::rename(instruction[1], instruction[2]);
					if (ret == 2) {
						cout << "������ʧ�ܣ�ԭ�ļ�������" << endl;
					}
					else if (ret == 1) {
						cout << "������ʧ�ܣ����ļ�������һ���Ϸ����ļ���" << endl;
					}
					else if (ret == 3) {
						cout << "������ʧ�ܣ����ļ������ڵ�ǰ�ļ��´���" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "renameָ���������ļ��Ĳ���ӦΪ[ԭ�ļ���] [���ļ���]������������" << endl;
				}
			}
		}
	}

}

void Menu::rm(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "rmָ������,���������롣" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 3) {
				int ret = CatalogController::rm(instruction[2], true);
				if (ret == 2) {
					cout << "ɾ��ʧ�ܣ�Ŀ¼ " << instruction[2] << " ������" << endl;
				}
				else if (ret == 8) {
					cout << "ɾ���ɹ����������ļ�" << endl;
				}
				else {
					//success
				}
			}
			// size==2||>3
			else {
				cout << "rmָ��ɾ��Ŀ¼�Ĳ���ӦΪ rm -r [dir] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "rmָ��ɾ��Ŀ¼��������,ӦΪ-r" << endl;
			}
			else {
				if (instruction.size() == 2) {
					int ret = CatalogController::rm(instruction[1]);
					if (ret == 2) {
						cout << "ɾ��ʧ�ܣ��ļ� " << instruction[1] << " ������" << endl;
					}
					else if (ret == 9) {
						cout << "ɾ��ʧ�ܣ��ļ� " << instruction[1] << " δ�ر�" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "rmָ��ɾ���ļ��Ĳ���ӦΪ rm [file] ������������" << endl;
				}
			}
		}
	}
}

void Menu::cp(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "cpָ������,���������롣" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 3//������Ŀ¼cp
			if (instruction.size() == 3) {
				Menu::cpDir(instruction);
			}
			//size == 4////˫����Ŀ¼cp,��cp����paste
			else if (instruction.size() == 4) {
				int ret = Menu::cpDir(instruction);//��cp
				if (ret != 1) {//����һ��������ɣ���ִ��paste��������ִ��
					string useless = instruction[2];
					string dest = instruction[3];
					vector<string> copy;
					copy.push_back(useless);
					copy.push_back(dest);
					Menu::paste(copy);//��paste
				}
				else {
					//success
				}

			}
			// size==2||>4
			else {
				cout << "cpָ���Ŀ¼�Ĳ���ӦΪ cp -r [dir] / cp -r [dir] [dest] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "cpָ���Ŀ¼��������,ӦΪ-r" << endl;
			}
			else {
				if (instruction.size() == 2) {//�������ļ�cp
					Menu::cpFile(instruction);//��cp

				}
				//˫�����ļ�cp
				else if (instruction.size() == 3) {
					int ret = Menu::cpFile(instruction);//��cp
					if (ret != 1) {//����һ��������ɣ���ִ��paste��������ִ��
						string useless = instruction[1];
						string dest = instruction[2];
						vector<string> copy;
						copy.push_back(useless);
						copy.push_back(dest);
						Menu::paste(copy);//��paste
					}
					else {
						//success
					}
				}
				else {
					cout << "cpָ����ļ��Ĳ���ӦΪ cp [file] / cp [file] [dest]" << endl;
				}
			}
		}
	}
}

int Menu::cpDir(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[2], true, false);
	if (ret == 1) {
		cout << "����ʧ�ܣ�Ŀ¼ " << instruction[2] << " ������" << endl;
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
		cout << "����ʧ�ܣ��ļ� " << instruction[1] << " ������" << endl;
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
			cout << "ճ��ʧ�ܣ�������û������" << endl;
		}
		else if (ret == 2) {
			cout << "ճ��ʧ�ܣ������Ƶ��ļ�����Ŀ¼�����ڵ�ǰĿ¼����" << endl;
		}
		else if (ret == 3) {
			cout << "ճ��ʧ�ܣ���ճ����Ŀ�겻��һ��Ŀ¼" << endl;
		}
		else {
			//success
		}
	}
	else {
		cout << "pasteָ������" << endl;
	}
}

void Menu::mv(vector<string> instruction)
{
	// size<=3
	if (instruction.size() < 3) {
		cout << "mvָ������" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 4
			if (instruction.size() == 4) {
				int ret = CatalogController::mv(instruction[2], instruction[3], true);
				if (ret == 1) {
					cout << "�ƶ�ʧ�ܣ�ԭĿ¼������" << endl;
				}
				else if (ret == 2) {
					cout << "�ƶ�ʧ�ܣ�Ŀ��·�����Ѵ���ͬ��Ŀ¼" << endl;
				}
				else if (ret == 3) {
					cout << "�ƶ�ʧ�ܣ��ƶ���Ŀ���ַ����һ���Ϸ�·��" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "mvָ���ƶ�Ŀ¼�Ĳ���ӦΪ[�ƶ�Ŀ¼��] [Ŀ��·��]" << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "mvָ���ƶ�Ŀ¼��������,ӦΪ-r" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::mv(instruction[1], instruction[2], false);
					if (ret == 1) {
						cout << "�ƶ�ʧ�ܣ�ԭ�ļ�������" << endl;
					}
					else if (ret == 2) {
						cout << "�ƶ�ʧ�ܣ�Ŀ��·�����Ѵ���ͬ���ļ�" << endl;
					}
					else if (ret == 3) {
						cout << "�ƶ�ʧ�ܣ��ƶ���Ŀ���ַ����һ���Ϸ�·��" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "mvָ���ƶ��ļ��Ĳ���ӦΪ[�ƶ��ļ���] [Ŀ��·��]" << endl;
				}
			}
		}
	}

}

void Menu::cut(vector<string> instruction)
{
	// size<2
	if (instruction.size() < 2) {
		cout << "cutָ������" << endl;
	}
	// size>=2
	else {
		if (instruction[1] == "-r") {
			//size == 3
			if (instruction.size() == 3) {
				Menu::cutDir(instruction);
			}
			//size == 4////˫����Ŀ¼cut,��cp����paste
			else if (instruction.size() == 4) {

				int ret = Menu::cutDir(instruction);//��cut

				if (ret != 1) {//����һ��������ɣ���ִ��paste��������ִ��
					string useless = instruction[2];
					string dest = instruction[3];
					vector<string> copy;
					copy.push_back(useless);
					copy.push_back(dest);
					Menu::paste(copy);//��paste
				}
				else {
					//success
				}

			}
			// size==2||>3
			else {
				cout << "cutָ�����Ŀ¼�Ĳ���ӦΪ cut -r [dir] / cut -r [dir] [dest] " << endl;
			}
		}
		else {

			if (instruction[1].at(0) == '-') {
				cout << "cutָ�����Ŀ¼��������,ӦΪ-r" << endl;
			}
			else {
				if (instruction.size() == 2) {
					Menu::cutFile(instruction);

				}
				//˫�����ļ�cut
				else if (instruction.size() == 3) {
					int ret = Menu::cutFile(instruction);//��cut
					if (ret != 1) {//����һ��������ɣ���ִ��paste��������ִ��
						string useless = instruction[1];
						string dest = instruction[2];
						vector<string> copy;
						copy.push_back(useless);
						copy.push_back(dest);
						Menu::paste(copy);//��paste
					}
					else {
						//success
					}
				}
				else {
					cout << "cutָ������ļ��Ĳ���ӦΪ cut [file] / cut [file] [dest]" << endl;
				}
			}
		}
	}
}

int Menu::cutDir(vector<string> instruction)
{
	int ret = CatalogController::cp(instruction[2], true, true);
	if (ret == 1) {
		cout << "����ʧ�ܣ�Ŀ¼ " << instruction[2] << " ������" << endl;
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
		cout << "����ʧ�ܣ��ļ� " << instruction[1] << " ������" << endl;
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
		cout << "lnָ������,���������롣" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-s") {
			//size == 4 �������ļ� ln -s [dirA] [dirB]
			if (instruction.size() == 4) {
				int ret = CatalogController::link(instruction[2], instruction[3], 1);
				if (ret == 1) {
					cout << "�������ļ�ʧ�ܣ�ԭ�ļ�������" << endl;
				}
				else if (ret == 2) {
					cout << "�������ļ�ʧ�ܣ�Ŀ��·�����Ѵ���ͬ���ļ�" << endl;
				}
				else if (ret == 3) {
					cout << "�������ļ�ʧ�ܣ�Ŀ��·������һ���Ϸ���Ŀ¼��" << endl;
				}
				else {
					//success
				}
			}
			// size==3||>4
			else {
				cout << "lnָ���������ļ��Ĳ���ӦΪ ln -s [dirA] [dirB] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "lnָ���������ļ���������,ӦΪ -s" << endl;
			}
			else {
				if (instruction.size() == 3) {
					int ret = CatalogController::link(instruction[1], instruction[2], 0);
					if (ret == 1) {
						cout << "Ӳ�����ļ�ʧ�ܣ�ԭ�ļ�������" << endl;
					}
					else if (ret == 2) {
						cout << "Ӳ�����ļ�ʧ�ܣ�Ŀ��·�����Ѵ���ͬ���ļ�" << endl;
					}
					else if (ret == 3) {
						cout << "Ӳ�����ļ�ʧ�ܣ�Ŀ��·������һ���Ϸ���Ŀ¼��" << endl;
					}
					else {
						//success
					}
				}
				else {
					cout << "lnָ��Ӳ�����ļ��Ĳ���ӦΪ ln [diaA] [dirB]" << endl;
				}
			}
		}
	}
}

void Menu::open(vector<string> instruction)
{
	// size<3
	if (instruction.size() < 3) {
		cout << "openָ������" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 3 ����ʽ��
			if (instruction.size() == 3) {
				int ret = CatalogController::open(instruction[2], FileOpenStatus::read);
				if (ret) {
					cout << "�ļ���ʧ�ܣ��ļ�·��������" << endl;
				}
				else {
					//success
				}
			}
			// size>3
			else {
				cout << "openָ���Զ���ʽ���ļ��Ĳ���ӦΪ open -r [file] " << endl;
			}
		}
		else if (instruction[1] == "-w") {
			//size == 3 ����ʽ��
			if (instruction.size() == 3) {
				int ret = CatalogController::open(instruction[2], FileOpenStatus::write);
				if (ret) {
					cout << "�ļ���ʧ�ܣ��ļ�·��������" << endl;
				}
				else {
					//success
				}
			}
			// size>3
			else {
				cout << "openָ����д��ʽ���ļ��Ĳ���ӦΪ open -w [file] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "openָ����ļ���������,ӦΪ -r/-w " << endl;
			}
		}
	}
}

void Menu::close(vector<string> instruction)
{
	if (instruction.size() == 2) {
		bool flag = CatalogController::close(instruction[1]);
		if (flag) {
			cout << "�ļ��ر�ʧ�ܣ��ļ�������" << endl;
		}
	}
	else {
		cout << "closeָ������" << endl;
	}
}

void Menu::read(vector<string> instruction)
{
	if (instruction.size() == 2) {
		int flag = CatalogController::read(instruction[1]);

		if (flag == 1) {
			cout << "�ļ���ȡʧ�ܣ��ļ�������" << endl;
		}
		else if (flag == 2) {
			cout << "�ļ���ȡʧ�ܣ��ļ�δ��" << endl;
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
		cout << "readָ������" << endl;
	}
}

void Menu::write(vector<string> instruction)
{
	if (instruction.size() == 2) {
		int flag = CatalogController::write(instruction[1]);

		if (flag == 1) {
			cout << "�ļ�д��ʧ�ܣ��ļ�������" << endl;
		}
		else if (flag == 2) {
			cout << "�ļ�д��ʧ�ܣ��ļ�δ��" << endl;
		}
		else if (flag == 3) {
			cout << "�ļ�д��ʧ�ܣ��ļ�û��дȨ��" << endl;
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
		cout << "writeָ������" << endl;
	}
}

void Menu::help()
{
	cout << endl;
	cout << "��ӭʹ��FOS�ļ�����ϵͳ��ϵͳָ������ĵ����£�" << endl;
	cout << endl;
	cout << "ָ���Ϊ�����ࣺϵͳָ�Ŀ¼ָ����ļ�ָ��" << endl;
	cout << "һ.ϵͳָ��" << endl;
	cout << "   1.ע�᣺               signin" << endl;
	cout << "   2.��¼��               signup" << endl;
	cout << "   3.�ǳ���               signout" << endl;
	cout << "   4.���ԣ�               debug" << endl;
	cout << "   5.��ʽ����             format" << endl;
	cout << "   6.�ػ���               shutdown" << endl;
	cout << "   7.������Ŀ¼��         rename -r [dirA] [dirB]" << endl;
	cout << "   8.������               help" << endl;
	cout << "   9.������               clear" << endl;
	cout << "   10.�޸ĵ�ǰ�û����룺  pass" << endl;
	cout << "   11.�޸�ָ���û����룺  pass [user]" << endl;
	cout << "   12.չʾ�û��б�      userlist" << endl;
	cout << "   13.���Ĺ���Ա��        su" << endl;
	cout << "   14.������п��б�    disklist -e [page]" << endl;
	cout << "   15.���ռ�ÿ��б�    disklist -f [page]" << endl;
	cout << "   16.������п��б�    disklist -a [page] " << endl;
	cout << "   17.ɾ��ָ���û���      deluser [user_name] " << endl;
	cout << endl;

	cout << "��.Ŀ¼����" << endl;
	cout << "   1.����Ŀ¼��           mkdir [dir]" << endl;
	cout << "   2.�鿴��ǰĿ¼�ļ���   ls" << endl;
	cout << "   3.�鿴ָ��Ŀ¼���ļ��� ls [dir]" << endl;
	cout << "   4.ɾ��Ŀ¼��           rm -r [dir]" << endl;
	cout << "   5.�ƶ�Ŀ¼��           mv -r [dirA] [dirB]" << endl;
	cout << "   6.������Ŀ¼��         rename -r [dirA] [dirB]" << endl;
	cout << "   7.����Ŀ¼��           cd [dir]" << endl;
	cout << "   8.˫��������Ŀ¼��     cp -r [dirA] [dirB]" << endl;
	cout << "   9.����������Ŀ¼��     cp -r [dir]" << endl;
	cout << "   10.���������У�        cut -r [dir]" << endl;
	cout << "   11.˫�������У�        cut -r [dirA] [dirB]" << endl;
	cout << "   12.ճ��Ŀ¼��          paste [dir]" << endl;
	cout << endl;
	cout << "��.�ļ�����" << endl;
	cout << "   1.�����ļ���           touch [file]" << endl;
	cout << "   2.ɾ���ļ���           rm [file]" << endl;
	cout << "   3.�ƶ���               mv [fileA] [fileB]" << endl;
	cout << "   4.�������ļ���         rename [dirA] [dirB]" << endl;
	cout << "   5.˫���������ļ���     cp [file] [dir]" << endl;
	cout << "   6.�����������ļ���     cp [file]" << endl;
	cout << "   7.�����������ļ���     cut [file]" << endl;
	cout << "   8.˫���������ļ���     cut [file] [dir]" << endl;
	cout << "   9.ճ���ļ���           paste [dir]" << endl;
	cout << "   10.Ӳ�����ļ���        ln [dirA] [dirB]" << endl;
	cout << "   11.�������ļ���        ln -s [dirA] [dirB]" << endl;
	cout << "   12.����ʽ�򿪣�        open -r [file]" << endl;
	cout << "   13.д��ʽ�򿪣�        open -w [file]" << endl;
	cout << "   14.���ļ���            read [file]" << endl;
	cout << "   15.д�ļ���            write [file]" << endl;
	cout << "   16.���ң�              find [file] [dir]" << endl;
	cout << "   17.�������ң�          find -r [file] [dir]" << endl;
	cout << "   18.ģ�����ң�          find -a [file] [dir]" << endl;
	cout << "   19.����ģ�����ң�      find -ar [file] [dir]" << endl;
	cout << endl;


}

void Menu::pass(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "passָ������" << endl;
	}
	else {
		if (instruction.size() == 1) {
			//�û�����
			string passwd, re_passwd;
			cout << "�����������룺";
			cin >> passwd;
			cout << "���ٴ����������룺";
			cin >> re_passwd;
			getchar();
			if (passwd != re_passwd) {
				cout << "�������벻һ��" << endl;
			}
			else {
				UserController::newPass(passwd);
				cout << "�޸ĳɹ�" << endl;
			}
		}
		else {
			//����Ա����
			if (USER_NAME == "root") {
				string passwd, re_passwd;
				cout << "�����������룺";
				cin >> passwd;
				cout << "���ٴ����������룺";
				cin >> re_passwd;
				getchar();
				if (passwd != re_passwd) {
					cout << "�������벻һ��" << endl;
				}
				else {
					int ret = UserController::newPass(instruction[1], passwd);
					if (!ret) {
						cout << "�޸ĳɹ�" << endl;
					}
					else if (ret == 1) {
						cout << "���û��޹���ԱȨ�ޣ��޷��޸��û�����" << endl;
					}
					else if (ret == 2) {
						cout << "���û�������" << endl;
					}

				}
			}
			else {
				cout << "���û��޹���ԱȨ�ޣ��޷��޸��û�����" << endl;
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
		cout << "disklistָ��鿴�ռ���б�Ĳ���ӦΪ disklist -e / -f / -a [page] " << endl;
		return;
	}
	if (instruction.size() == 3) {
		stringstream ss;
		ss << instruction[2];
		ss >> page;
		if (page <= 0) {
			cout << "ҳ��Ӧ����0" << endl;
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
		cout << "disklistָ��Ĳ�������,ӦΪ -e/-f/-a " << endl;
		return;
	}
	int len = ret.size() / 20 + 1;
	if (page > len && page != 1) {
		cout << "ҳ��ӦС����ҳ����" << len << "��" << endl;
		return;
	}
	cout << "����" << ret.size() << "����¼����" << len << "ҳ����ǰΪ��" << page << "ҳ" << endl;
	for (int i = 0 + page * 20 - 20; i < ret.size() && i < page * 20; i++) {
		cout << ret[i].first << "�ſ飺";
		if (ret[i].second) {
			SetConsoleTextAttribute(outHandle, 0x04);
			cout << "��ռ��" << endl;
			SetConsoleTextAttribute(outHandle, 0x07);
		}
		else {
			SetConsoleTextAttribute(outHandle, 0x02);
			cout << "δռ��" << endl;
			SetConsoleTextAttribute(outHandle, 0x07);
		}
	}

}

void Menu::delUser(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "deluserָ������" << endl;
	}
	else {
		//����Ա����
		if (USER_NAME == "root") {
			Status ret = UserController::delUser(instruction[1]);
			if (ret == Status::userSignUpFailedUserNotFound) {
				cout << "���û�������" << endl;
			}
			else {
				cout << "ɾ���ɹ�" << endl;
			}
		}
		else {
			cout << "���û��޹���ԱȨ��" << endl;
		}
	}
}

void Menu::test(vector<string> instruction)
{
	if (instruction.size() > 2) {
		cout << "testָ������" << endl;
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
		cout << "findָ������,���������롣" << endl;
	}
	// size>=3
	else {
		if (instruction[1] == "-r") {
			//size == 3 ��������
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::fd(instruction[2], instruction[3], true);
				if (ret.size() == 0) {
					cout << "���ҽ��Ϊ��" << endl;
				}
				else {
					cout << "���ҵ�" << ret.size() << "����¼��" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "findָ������ҵĲ���ӦΪ find -r [file] [dir] " << endl;
			}
		}
		//ģ��
		else if (instruction[1] == "-a") {
			//size == 3 ģ������
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::similarFd(instruction[2], instruction[3], false);
				if (ret.size() == 0) {
					cout << "���ҽ��Ϊ��" << endl;
				}
				else {
					cout << "���ҵ�" << ret.size() << "����¼��" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "findָ��ģ�����ҵĲ���ӦΪ find -a [file] [dir] " << endl;
			}
		}
		else if (instruction[1] == "-ar") {
			//size == 3 ����andģ������
			if (instruction.size() == 4) {
				vector <pair<Ftxt, string>> ret;
				ret = CatalogController::similarFd(instruction[2], instruction[3], true);
				if (ret.size() == 0) {
					cout << "���ҽ��Ϊ��" << endl;
				}
				else {
					cout << "���ҵ�" << ret.size() << "����¼��" << endl;
					for (auto p : ret) {
						cout << p.second << endl;
					}

				}
			}
			// size==3||>4
			else {
				cout << "findָ���ģ�����ҵĲ���ӦΪ find -ar [file] [dir] " << endl;
			}
		}
		else {
			if (instruction[1].at(0) == '-') {
				cout << "findָ����ҵĲ�������,ӦΪ -a/-r/-ar" << endl;
			}
			else {
				if (instruction.size() == 3) {
					vector <pair<Ftxt, string>> ret;
					ret = CatalogController::fd(instruction[1], instruction[2], false);
					if (ret.size() == 0) {
						cout << "���ҽ��Ϊ��" << endl;
					}
					else {
						cout << "���ҵ�" << ret.size() << "����¼��" << endl;
						for (auto p : ret) {
							cout << p.second << endl;
						}
					}
				}
				else {
					cout << "findָ����ҵĲ���ӦΪ find [file] [dir] " << endl;
				}
			}
		}
	}
}

void Menu::su()
{
	string passwd;
	cout << "������root���룺";
	cin >> passwd;
	getchar();

	int status = UserController::su(passwd);
	if (status == Status::userSignUpFailedUserNotFound) {
		cout << "rootδ����" << endl;
	}
	else if (status == Status::userSignUpFailedPassError) {
		cout << "root�������" << endl;
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
		cout << "���û��޹���ԱȨ�ޣ��޷��鿴�û��б�" << endl;
	}
}





void Menu::start() {

	// �������� /
	// cd /home/[USER_NAME]
	// mkdir /home/[USER_NAME]
	// path = /home/[USER_NAME]

	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	cout << "���¼(signup)��ʹ�ñ�ϵͳ��" << endl;
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

		//�ȷ������ظ���ָ��ٷ���Ŀ¼���ļ�����ͬ����ָ��
		//�ȷ������ظ���ָ��,�� 9 ��.SignIn  SignUp ,  shutdown ,mkdir cd touch vi ls debug
		if (instruction[0].size() == 0);
		else if (instruction[0] == "signup") { //��¼
			Menu::signup();
		}
		else if (instruction[0] == "signin") {//ע��
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
			cout << "���ȵ�¼��ע���ʹ�ô�ϵͳ" << endl;
			cout << "ʹ��signup���е�¼" << endl;
			cout << "ʹ��signin����ע��" << endl;
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
		//�ٷ���Ŀ¼�������ļ�����ͬ����ָ��
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
			cout << "��ָ�����" << endl;
		}
		//
		/*instruction.clear();*/
	}

}