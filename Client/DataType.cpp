#include "DataType.h"

Header::Header() {
	this->type = DATA_ERROR;
	this->size = sizeof Header;
}

UserInfo::UserInfo() {
	this->type = DATA_ERROR;
	this->size = sizeof UserInfo;
}

Response::Response() {
	this->type = DATA_ERROR;
	this->size = sizeof Response;
	this->state = 0;
}

Command::Command() {
	this->type = DATA_COMMAND;
	this->size = sizeof Command;
}