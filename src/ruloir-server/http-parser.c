#include "http-parser.h"
#include "char-buffer.h"
#include "prefix.h"

#define READ_CH()		({ch=toupper(CharBufferRead(fd,char_buf));})
#define CLEANUP()		({CharBufferFree(char_buf);})

bool HTTPParse(int fd,HTTPRequest *http){
	char ch;
	CharBuffer *char_buf=CharBufferNew();
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
				CLEANUP();
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
				CLEANUP();
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
		CLEANUP();
		return false;
	}
	do{
		ch=CharBufferRead(fd,char_buf);
	}while(ch!=' ');
	unsigned int i=0;
	ch=CharBufferRead(fd,char_buf);
	while(i<MAX_HTTP_PATH_LENGTH && ch!=' '){
		http->path[i]=ch;
		i++;
		ch=CharBufferRead(fd,char_buf);
	}
	http->path[i]=0;
	CLEANUP();
	return true;
}