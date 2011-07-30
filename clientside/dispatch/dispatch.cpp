#include "dispatch.h"
#include "../client/client.h"

// Common headers
#include <stdio.h>
#include <string.h>

Dispatch::Dispatch()
{
	//your char array is going out of scope i think.... that is why when your 

	data		= outgoingData;
	//data = tempy;
	maxSize		= sizeof(outgoingData);
	//maxSize = sizeof(tempy);
	size		= 0;
	readCount	= 0;
	overFlow	= false;
}
Dispatch::Dispatch(char *d, int length)
{
	//your char array is going out of scope i think.... that is why when your 
	data		= d;
	//data = tempy;
	maxSize		= length;
	//maxSize = sizeof(tempy);
	size		= 0;
	readCount	= 0;
	overFlow	= false;
}

Dispatch::~Dispatch()
{
}
/*
void Dispatch::Init(char *d, int length)
{
	data		= d;
	maxSize		= length;
	size		= 0;
	readCount	= 0;
	overFlow	= false;
}
*/
void Dispatch::Clear(void)
{
	size		= 0;
	readCount	= 0;
	overFlow	= false;
}

char *Dispatch::GetNewPoint(int length)
{
	char *tempData;

	// Check for overflow
	if(size + length > maxSize)
	{
		Clear(); 
		overFlow = true;
	}

	tempData = data + size;
	size += length;

	return tempData;
}



void Dispatch::Write(const void *d, int length)
{
	memcpy(GetNewPoint(length), d, length);		
}

void Dispatch::WriteByte(char c)
{
	char *buf;

	buf = GetNewPoint(1);

	memcpy(buf, &c, 1);
}

void Dispatch::WriteShort(short c)
{
	char *buf;

	buf = GetNewPoint(2);

	memcpy(buf, &c, 2);
}

void Dispatch::WriteLong(long c)
{
	char *buf;
	
	buf = GetNewPoint(4);

	memcpy(buf, &c, 4);
}

void Dispatch::WriteFloat(float c)
{
	char *buf;
	
	buf = GetNewPoint(4);

	memcpy(buf, &c, 4);
}

void Dispatch::WriteString(const char *s)
{
	if(!s)
	{
		return;
	}
	else
		Write(s, strlen(s) + 1);
}

void Dispatch::BeginReading(void)
{
	readCount = 0;
}

void Dispatch::BeginReading(int s)
{
	size = s;
	readCount = 0;
}

char *Dispatch::Read(int s)
{
	static char c[2048];

	if(readCount+s > size)
		return NULL;
	else
		memcpy(&c, &data[readCount], s);

	readCount += s;

	return c;
}

char Dispatch::ReadByte(void)
{
	char c;

	if(readCount+1 > size)
		c = -1;
	else
		memcpy(&c, &data[readCount], 1);

	readCount++;

	return c;
}

short Dispatch::ReadShort(void)
{
	short c;

	if(readCount+2 > size)
		c = -1;
	else		
		memcpy(&c, &data[readCount], 2);

	readCount += 2;

	return c;
}

long Dispatch::ReadLong(void)
{
	long c;

	if(readCount+4 > size)
		c = -1;
	else
		memcpy(&c, &data[readCount], 4);

	readCount += 4;

	return c;
}

float Dispatch::ReadFloat(void)
{
	float c;

	if(readCount+4 > size)
		c = -1;
	else
		memcpy(&c, &data[readCount], 4);

	readCount += 4;

	return c;
}

char *Dispatch::ReadString(void)
{
	static char string[2048];
	int	l, c;

	l = 0;

	do
	{
		c = ReadByte();

		if (c == -1 || c == 0)
			break;

		string[l] = c;
		l++;
	} while(l < sizeof(string)-1);

	string[l] = 0;

	return string;
}
