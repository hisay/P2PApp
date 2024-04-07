#include "CBuffer.h"
#include "inc.h"

bool	CBuffer::MoveBuffer()
{
	if (m_used)
	{
		m_total = m_total - m_used;
		memcpy(m_pBuffer, m_pBuffer + m_used, m_total);
		m_used = 0;
		return true;
	}
	return false;
}
bool	CBuffer::EnBuffer(unsigned int newSize)
{
	if (m_total + newSize > m_bufLen)
	{
		if (MoveBuffer() && m_total + newSize <= m_bufLen)
			return true;
		try {
			unsigned int nt = GET_ALIGN_SIZE(m_bufLen + newSize);
			unsigned char* pd = new unsigned char[nt];
			memcpy(pd, m_pBuffer, m_bufLen);
			delete[]m_pBuffer;

			m_pBuffer = pd;
			m_bufLen = nt;
		}
		catch (...) {
			return false;
		}
	}
	return true;
}

bool	CBuffer::AddBuffer(unsigned char* pbuf, unsigned int size)
{
	if (EnBuffer(size))
	{
		memcpy(m_pBuffer + m_total, pbuf, size);
		m_total += size;
		return true;
	}
	return false;
}

void	CBuffer::UseBuffer(unsigned int used)
{
	m_used += used;
	if (m_used && m_used == m_total)
	{
		m_used = m_total = 0;
		return;
	}
	if (m_used >= m_bufLen / 3)
	{
		MoveBuffer();
	}
	if (m_used && m_used == m_total)
	{
		m_used = m_total = 0;
	}
}