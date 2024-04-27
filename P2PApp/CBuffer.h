#pragma once

#define		DEF_BUFF_LEN	512
#define	GET_ALIGN_SIZE(k) ( ((k)+(DEF_BUFF_LEN-1)) & ~(DEF_BUFF_LEN-1))

class CBuffer
{
public:
	CBuffer(unsigned int defSize)
	{
		m_bufLen = GET_ALIGN_SIZE(defSize);
		m_pBuffer = new unsigned char[m_bufLen];
		m_used = m_total = 0;
	}

	CBuffer() :CBuffer(DEF_BUFF_LEN) {}
	virtual ~CBuffer() { delete[]m_pBuffer; m_pBuffer = nullptr; } 
	unsigned char* GetCurBuff() { return m_pBuffer + m_used; }
	unsigned int   GetCufBuffLen() { return m_total - m_used; }
	unsigned int   GetBuffLen() { return m_bufLen; }
	unsigned char* GetBuff() { return m_pBuffer ; }
	void	UseBuffer(unsigned int used);
	bool	AddBuffer(unsigned char* pbuf, unsigned int size);
protected:
	bool	EnBuffer(unsigned int newSize);
	bool	MoveBuffer();
	unsigned	int		m_bufLen;
	unsigned	char*	m_pBuffer;
	unsigned	int		m_used, m_total;
};


