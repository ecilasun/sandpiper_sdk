#include <stdint.h>

// Taken from DXUT locklesspipe (c) Microsoft

template <uint8_t cbBufferSizeLog2>
class CLocklessPipe
{
public:
	CLocklessPipe() : m_readOffset(0), m_writeOffset(0)
	{
#if defined(PLATFORM_WINDOWS)
		m_pbBuffer = (uint8_t *)_aligned_malloc(c_cbBufferSize, E_CACHELINE_SIZE);
#else
		m_pbBuffer = (uint8_t *)aligned_alloc(c_cbBufferSize, E_CACHELINE_SIZE);
#endif
	}

	~CLocklessPipe()
	{
		if (m_pbBuffer)
#if defined(PLATFORM_WINDOWS)
			_aligned_free(m_pbBuffer);
#else
			free(m_pbBuffer);
#endif
		m_pbBuffer = 0;
	}

	size_t GetBufferSize() const { return c_cbBufferSize; }
	EInline size_t BytesAvailable() const { return m_writeOffset - m_readOffset; }
	EInline size_t FreeSpace() const { return c_cbBufferSize - ( m_writeOffset - m_readOffset ); }

	uint32_t EInline Read(void* pvDest, const size_t cbDest)
	{
		size_t readOffset = m_readOffset;
		const size_t writeOffset = m_writeOffset;

		const size_t cbAvailable = writeOffset - readOffset;
		if( cbDest > cbAvailable )
			return 0;

		EReadWriteBarrier(0);

		uint8_t* pbDest = (uint8_t *)pvDest;
		const size_t actualReadOffset = readOffset & c_sizeMask;
		size_t bytesLeft = cbDest;

		const size_t cbTailBytes = bytesLeft < c_cbBufferSize - actualReadOffset ? bytesLeft : c_cbBufferSize - actualReadOffset;
		memcpy( pbDest, m_pbBuffer + actualReadOffset, cbTailBytes );
		bytesLeft -= cbTailBytes;

		//EAssert(bytesLeft == 0, "Item not an exact multiple of ring buffer, this will cause multiple memcpy() calls during Read()");

//		if( bytesLeft ) // The assert above should ensure we don't hit this.
//			memcpy( pbDest + cbTailBytes, m_pbBuffer, bytesLeft );

		readOffset += cbDest;
		m_readOffset = readOffset;

		return 1;
	}

	uint32_t EInline Write(const void* pvSrc, const size_t cbSrc)
	{
		const size_t readOffset = m_readOffset;
		size_t writeOffset = m_writeOffset;

		const size_t cbAvailable = c_cbBufferSize - ( writeOffset - readOffset );
		if( cbSrc > cbAvailable )
			return 0;

		const uint8_t* pbSrc = ( const uint8_t* )pvSrc;
		const size_t actualWriteOffset = writeOffset & c_sizeMask;
		size_t bytesLeft = cbSrc;

		const size_t cbTailBytes = bytesLeft < c_cbBufferSize - actualWriteOffset ? bytesLeft : c_cbBufferSize - actualWriteOffset;
		memcpy( m_pbBuffer + actualWriteOffset, pbSrc, cbTailBytes );
		bytesLeft -= cbTailBytes;

		//EAssert(bytesLeft == 0, "Item not an exact multiple of ring buffer, this will cause multiple memcpy() calls during Write()");

		//if( bytesLeft )	// The assert above should ensure we don't hit this.
		//	memcpy( m_pbBuffer, pbSrc + cbTailBytes, bytesLeft );

		EReadWriteBarrier(0);

		writeOffset += cbSrc;
		m_writeOffset = writeOffset;

		return 1;
	}

private:
	const static uint8_t c_cbBufferSizeLog2 = cbBufferSizeLog2 < 31 ? cbBufferSizeLog2 : 31;
	const static size_t c_cbBufferSize = ( 1 << c_cbBufferSizeLog2 );
	const static size_t c_sizeMask = c_cbBufferSize - 1;
	CLocklessPipe( const CLocklessPipe& ) = delete;
	CLocklessPipe& operator =(const CLocklessPipe&) = delete;
private:
	volatile uint32_t m_readOffset;
	volatile uint32_t m_writeOffset;
	uint8_t *m_pbBuffer;
};