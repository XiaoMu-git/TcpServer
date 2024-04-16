#include "DataPackage.h"

Header::Header() {}

void Header::set(short length, short cmd) {
	this->length = length;
	this->cmd = cmd;
}

UserInfo::UserInfo() {}

void UserInfo::set(const char* username, const char* password) {
	strcpy(this->username, username);
	strcpy(this->password, password);
}

Result::Result() {}

void Result::set(bool result, const char* data) {
	this->result = result;
	strcpy(this->data, data);
}