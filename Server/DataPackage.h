#ifndef _DATAPACKAGE_
#define _DATAPACKAGE_

#include <iostream>

enum CMD {
	CMD_ERROR,
	CMD_LOGIN,
	CMD_QUIT,
	CMD_RESULT
};

class Header {
public:
	short length;
	short cmd;

	Header();
	void set(short length, short cmd);
};

class UserInfo {
public:
	char username[32];
	char password[32];

	UserInfo();
	void set(const char* username, const char* password);
};

class Result {
public:
	bool result;
	char data[31];

	Result();
	void set(bool result, const char* data);
};

#endif // !_DATAPACKAGE_
