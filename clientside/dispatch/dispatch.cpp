#include "dispatch.h"

#include "../tdreamsock/dreamSockLog.h"

// Common headers
#include <stdio.h>
#include <string.h>

Dispatch::Dispatch()
{
	mMaxSize = 1400;
	mCharArray = new char[mMaxSize];
	mSize		= 0;
	mReadCount	= 0;
}

Dispatch::~Dispatch()
{
}

/**********************************************
				ADMIN
************************************************/
void Dispatch::BeginReading()
{
	mReadCount = 0;
}

void Dispatch::Clear()
{
	mSize		= 0;
	mReadCount	= 0;
}

char *Dispatch::GetNewPoint(int length)
{
	char *tempData;

	// Check for overflow
	if(mSize + length > mMaxSize)
	{
		Clear(); 
	}
	tempData = mCharArray + mSize;
	mSize += length;

	return tempData;
}


/**********************************************
				WRITE
************************************************/
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

/**********************************************
				READ
************************************************/
char *Dispatch::Read(int s)
{
	static char c[2048];

	if(mReadCount+s > mSize)
		return NULL;
	else
		memcpy(&c, &mCharArray[mReadCount], s);

	mReadCount += s;

	return c;
}

char Dispatch::ReadByte()
{
	char c;

	if(mReadCount+1 > mSize)
		c = -1;
	else
		memcpy(&c, &mCharArray[mReadCount], 1);

	mReadCount++;

	return c;
}

short Dispatch::ReadShort()
{
	short c;

	if(mReadCount+2 > mSize)
		c = -1;
	else		
		memcpy(&c, &mCharArray[mReadCount], 2);

	mReadCount += 2;

	return c;
}

long Dispatch::ReadLong()
{
	long c;

	if(mReadCount+4 > mSize)
		c = -1;
	else
		memcpy(&c, &mCharArray[mReadCount], 4);

	mReadCount += 4;

	return c;
}

float Dispatch::ReadFloat()
{
	float c;

	if(mReadCount+4 > mSize)
		c = -1;
	else
		memcpy(&c, &mCharArray[mReadCount], 4);

	mReadCount += 4;

	return c;
}

char *Dispatch::ReadString()
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
