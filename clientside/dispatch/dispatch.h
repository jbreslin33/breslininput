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
	char*			data;
	char			outgoingData[1400];

	//admin
	int             getReadCount() { return readCount; }
	void			BeginReading(void);
	void			BeginReading(int s);
	char			*GetNewPoint(int length);
	void			Clear(void);
	bool			GetOverFlow(void)	{ return overFlow; }
	int				GetSize(void)		{ return size; }
	void			SetSize(int s)		{ size = s; }

	//write
	void			Write(const void *d, int length);
	void			WriteByte(char c);
	void			WriteShort(short c);
	void			WriteLong(long c);
	void			WriteFloat(float c);
	void			WriteString(const char *s);

	//read
	char			*Read(int s);
	char			ReadByte(void);
	short			ReadShort(void);
	long			ReadLong(void);
	float			ReadFloat(void);
	char			*ReadString(void);

};



#endif
