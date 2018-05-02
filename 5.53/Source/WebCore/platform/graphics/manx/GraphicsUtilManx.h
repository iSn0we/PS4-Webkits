#ifndef GraphicsUtilManx_h
#define GraphicsUtilManx_h

#include <wtf/text/AtomicString.h>
#include <wtf/text/WTFString.h>

class UCharFromString {
public:
    UCharFromString(const AtomicString& s)
        : m_string(s)
    {
        if (s.is8Bit())
            m_vector = s.string().charactersWithNullTermination();
    }

    const UChar* characters16() const
    {
        if (m_string.is8Bit())
            return m_vector.data();
        return m_string.characters16();
    }

    operator const AtomicString&() const
    {
        return m_string;
    }

    operator const String&() const
    {
        return m_string.string();
    }
    
    unsigned length() const
    {
        return m_string.length();
    }
    
private:
    Vector<UChar> m_vector;
    const AtomicString& m_string;
};

#endif
