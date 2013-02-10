#include "http-parser.h"
#include "prefix.h"

#define READ_CH()		({read(fd,&ch,1);ch=toupper(ch);})

bool HTTPParse(int fd,HTTPRequest *http){
	char ch;
	READ_CH();
	switch(ch){
	case 'E':
		//GET or HEAD or DELETE
		READ_CH();
		switch(ch){
			case 'T':
				http->method=HTTPGet;
				break;
			case 'A':
				http->method=HTTPHead;
				break;
			case 'L':
				http->method=HTTPDelete;
				break;
			default:
				return false;
		}
		break;
	case 'O':
		READ_CH();
		switch(ch){
			case 'S':
				http->method=HTTPPost;
				break;
			case 'N':
				http->method=HTTPConnect;
				break;
			default:
				return false;
		}
		break;
	case 'U':
		http->method=HTTPPut;
		break;
	case 'R':
		http->method=HTTPTrace;
		break;
	case 'P':
		http->method=HTTPOptions;
		break;
	case 'A':
		http->method=HTTPPatch;
		break;
	default:
		return false;
	}
	do{
		read(fd,&ch,1);
	}while(ch!=' ');
	unsigned int i=0;
	read(fd,&ch,1);
	while(i<MAX_HTTP_PATH_LENGTH && ch!=' '){
		http->path[i]=ch;
		i++;
		read(fd,&ch,1);
	}
	http->path[i]=0;
	return true;
}