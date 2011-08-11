#ifndef DISPATCH_H
#define DISPATCH_H

class Client;

class Dispatch
{
public:
	Dispatch();
	~Dispatch();
/**************************************************
*			VARIABLES
**************************************************/

	int				mMaxSize;
	int				mSize;
	int				mReadCount;
	char*			mCharArray;

/**************************************************
*			METHODS
**************************************************/

	//admin
	int             getReadCount() { return mReadCount; }
	void			BeginReading();
	char			*GetNewPoint(int length);
	void			Clear();
	int				GetSize()		{ return mSize; }
	void			SetSize(int s)		{ mSize = s; }

	//write
	void			Write      (const void *d, int length);
	void			WriteByte  (char c);
	void			WriteShort (short c);
	void			WriteLong  (long c);
	void			WriteFloat (float c);
	void			WriteString(const char *s);

	//read
	char			*Read      (int s);
	char			ReadByte   ();
	short			ReadShort  ();
	long			ReadLong   ();
	float			ReadFloat  ();
	char			*ReadString();

};



#endif
