#include "base.h"
#include "disk.h"
#include "ftxt.h"
#include "freeblock.h"
#include "inode.h"
#include "catalog.h"
#include "user.h"
#include "menu.h"
#define FILE "main.cpp"
void format(bool isInit) {
	if (isInit) {
		Disk::init();
		FreeBlock::format();
		UserController::format();
	}
}
void init() {
	FreeBlock::init();
	CatalogController::init();
	UserController::init();
}
void over() {
	FreeBlock::over();
	InodeController::over();
	UserController::over();
}
int main() {
	Menu::debug(false);
	format(false);
	init();
	Menu::start();
	over();
	if (isFormat) {
		format(true);
		init();
		over();
	}
}