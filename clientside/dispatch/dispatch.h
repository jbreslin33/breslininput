#ifndef DISPATCH_H
#define DISPATCH_H

class Client;

class Dispatch
{
public:
	Dispatch();
	~Dispatch();

	bool			overFlow;
	int				maxSize;
	int				size;
	int				readCount;
	char*			dataPointer;
	char			dataBuffer[1400];

	//admin
	int             getReadCount() { return readCount; }
	void			BeginReading();
	char			*GetNewPoint(int length);
	void			Clear();
	bool			GetOverFlow()	{ return overFlow; }
	int				GetSize()		{ return size; }
	void			SetSize(int s)		{ size = s; }

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
