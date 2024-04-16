#ifndef _DATADATA_
#define _DATADATA_

enum TYPE {
	DATA_ERROR,
	DATA_LOGIN,
	DATA_LOGIN_RESPONSE,
	DATA_LOGOUT,
	DATA_LOGOUT_RESPONSE,
	DATA_COMMAND,
	DATA_COMMAND_RESPONSE
};

class Header {
public:
	short type;
	short size;

	Header();
};

class UserInfo : public Header {
public:
	char username[32];
	char password[32];

	UserInfo();
};

class Response : public Header {
public:
	short state;
	char data[2];

	Response();
};

class Command : public Header {
public:
	char command[32];

	Command();
};

#endif // !_DATADATA_
