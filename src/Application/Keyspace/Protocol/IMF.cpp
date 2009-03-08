#include <stdlib.h>
#include <string.h>
#include "IMF.h"

#define CR					13
#define LF					10
#define CS_CR				"\015"
#define CS_LF				"\012"
#define CS_CRLF				CS_CR CS_LF

static char* SkipWhitespace(char* p)
{
	while (p && *p && *p <= ' ') p++;
	
	return p;	
}

static char* SeekWhitespace(char* p)
{
	if (!p)
		return NULL;
	
	while (p && *p && *p != ' ') p++;
	
	if (!*p)
		return NULL;
	
	return p;	
}

static char* SkipCrlf(char* p)
{
	if (!p)
		return NULL;
	
	while (*p && (*p == CS_CR[0] || *p == CS_LF[0])) p++;
	
	if (!*p)
		return NULL;
	
	return p;	
}

static char* SeekCrlf(char* p)
{
	if (!p)
		return NULL;
	
	while (*p && (p[0] != CS_CR[0] && p[1] != CS_LF[0])) p++;
	
	if (!*p)
		return NULL;
	
	return p;	
}

static int LineParse(char* buf, int len, int offs, const char** values[3])
{
	char* p;
	
	p = SkipWhitespace(buf + offs);
	
	*values[0] = p;
	p = SeekWhitespace(p);
	if (!p) return -1;
	
	*p++ = '\0';
	p = SkipWhitespace(p);
	
	*values[1] = p;
	p = SeekWhitespace(p);
	if (!p) return -1;
	
	*p++ = '\0';
	p = SkipWhitespace(p);
	
	*values[2] = p;
	p = SeekCrlf(p);
	if (!p) return -1;
	
	*p = '\0';
	p += 2;
	
	return (int) (p - buf);	
}

int IMFHeader::RequestLine::Parse(char* buf, int len, int offs)
{
	const char** values[3];
	
	values[0] = &method;
	values[1] = &uri;
	values[2] = &version;
	
	return LineParse(buf, len, offs, values);
}

int IMFHeader::StatusLine::Parse(char* buf, int len, int offs)
{
	const char** values[3];
	
	values[0] = &version;
	values[1] = &code;
	values[2] = &reason;
	
	return LineParse(buf, len, offs, values);
}

IMFHeader::IMFHeader()
{
	numKeyval = 0;
	capKeyval = KEYVAL_BUFFER_SIZE;
	keyvalues = keyvalBuffer;
	data = NULL;
}

IMFHeader::~IMFHeader()
{
	if (keyvalues != keyvalBuffer)
		delete[] keyvalues;
}

int IMFHeader::Parse(char* buf, int len, int offs)
{
	char* p;
	char* key;
	char* value;
	int nkv = numKeyval;
	KeyValue* keyvalue;

	data = buf;
	
	p = buf + offs;
	p = SkipCrlf(p);
	if (!p)
		return -1;
	
	while (p < buf + len) {
		key = p;
		p = strchr(p, ':');
		
		if (p)
		{
			*p++ = '\0';
			p = SkipWhitespace(p);
			
			value = p;
			p = SeekCrlf(p);
			if (p)
			{
				*p = '\0';
				p += 2;
				//strtolower(key);
				
				keyvalue = GetKeyValues(nkv);
				
				keyvalue[nkv].keyStart = (int) (key - buf);
				keyvalue[nkv].valueStart = (int) (value - buf);
				nkv++;
			}
		}
		else
		{
			p =  key;
			break;
		}
	}
	
	numKeyval = nkv;
	
	return (int) (p - buf);
	
}

const char* IMFHeader::GetField(const char* key)
{
	KeyValue* keyval;
	int i;
	
	if (!data)
		return NULL;
	
	i = 0;
	while (i < numKeyval)
	{
		keyval = &keyvalues[i];
		if (strcasecmp(key, data + keyval->keyStart) == 0)
			return data + keyval->valueStart;

		i++;
	}
	
	return NULL;
}


IMFHeader::KeyValue* IMFHeader::GetKeyValues(int newSize)
{
	KeyValue* newkv;
	
	if (newSize < capKeyval)
		return keyvalues;
	
	capKeyval *= 2;
	newkv = new KeyValue[capKeyval];
	memcpy(newkv, keyvalues, numKeyval * sizeof(KeyValue));
	
	if (keyvalues != keyvalBuffer)
		delete[] keyvalues;
	
	keyvalues = newkv;
	
	return keyvalues;
}