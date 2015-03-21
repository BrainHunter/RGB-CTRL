#include "DebugStream.h"
#include "stdio.h"

size_t DebugStream::write(uint8_t c)
{
    return 0;
}
int DebugStream::read(void)
{
    return 0;
}
int DebugStream::available(void)
{
    return 0;
}
void DebugStream::flush(void)
{

}


size_t DebugStream::print(const char str[])
{
    printf(str);
    return 0;
}

size_t DebugStream::print(int i, int base)
{
    if(base == DEC) printf("%d", i);
    else if(base == HEX) printf("%X", i);
    else printf("not implemented %d %d", i, base);
    return 0;
}
