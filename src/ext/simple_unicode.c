#include <bi/ext/simple_unicode.h>
#include <stdbool.h>
#include <string.h>

/*
|       UNICODE (UCS2/UCS4)           | UTF8 1st | 2nd byte | 3rd byte | 4th byte |    Valid Range    |
|-------------------------------------|----------|----------|----------|----------|-------------------|
|                            0xxxxxxx | 0xxxxxxx |     -    |     -    |     -    |  U+0000 .. U+007F |
|                   00000xxx-xxyyyyyy | 110xxxxx | 10yyyyyy |     -    |     -    |  U+0080 .. U+07FF |
|                   xxxxyyyy-yyzzzzzz | 1110xxxx | 10yyyyyy | 10zzzzzz |     -    |  U+0800 .. U+FFFF |
| 00000000-000wwwxx xxxxyyyy-yyzzzzzz | 11110www | 10xxxxxx | 10yyyyyy | 10zzzzzz | U+10000..U+10FFFF |

 - code point U+D800..U+DFFF are invalid. these are for surrogate pair.
 - code point upper U+110000 are invalid.
 - overlong UTF-8 is invalid. ASCII must 1byte, U+0080..U+07FF must 2bytes.

*/

uint16_t utf8_getch_as_ucs2(const char **txt, size_t *len)
{
  static const uint32_t REPLACEMENT_CHARACTER = 0xFFFD;
  const uint8_t *p = *(const uint8_t**)txt;
  uint16_t ch = REPLACEMENT_CHARACTER;

  // ASCII
  if( p[0] < 0x80 ) {
    *len -= 1;
    *txt += 1;
    return p[0];
  }

  // invalid 5bytes and 6bytes UTF8
  if( p[0] > 0b11111000 ) {
    *len -= 1;
    *txt += 1;
    return REPLACEMENT_CHARACTER;
  }

  // 4bytes UTF8 currently not supported
  if(p[0] >= 0b11110000) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
  }

  // 3bytes UTF8
  if(p[0] >= 0b11100000) {
    if(*len < 3) { // shortage
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    if( (p[1]&0b11000000) != 0b10000000 || // invalid sequence
        (p[2]&0b11000000) != 0b10000000
    ) {
      *len -= 1;
      return REPLACEMENT_CHARACTER;
    }
    ch = (p[0]&0b00001111) << 12 |
         (p[1]&0b00111111) << 6 |
         (p[2]&0b00111111);
    // invalid range. maybe overlong UTF-8.
    if( ch < 0x0800 /*|| 0xFFFF < ch*/ ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    *len -= 3;
    *txt += 3;
    return ch;
  }

  // 2bytes UTF8
  if(p[0] >= 0b11000000) {
    if(*len < 2) { // shortage
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    if( (p[1]&0b11000000) != 0b10000000 ) { // invalid sequence
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    ch  = (p[0]&0b00011111) << 6 |
          (p[1]&0b00111111);
    // invalid range.
    if( ch < 0x0080 || 0x07FF < ch ) {
      *len -= 1;
      *txt += 1;
      return REPLACEMENT_CHARACTER;
    }
    *len -= 2;
    *txt += 2;
    return ch;
  }

  *len -= 1;
  *txt += 1;
  return REPLACEMENT_CHARACTER;
}

uint16_t utf8_to_ucs2(uint32_t utf8)
{
  uint8_t a = utf8 >> 24 & 0xFF;
  uint8_t b = utf8 >> 16 & 0xFF;
  uint8_t c = utf8 >> 8 & 0xFF;
  uint8_t d = utf8 & 0xFF;

  if(a==0) {
    a = b;
    b = c;
    c = d;
    d = 0;
  }
  if(a==0) {
    a = b;
    b = c;
    c = 0;
    d = 0;
  }
  if(a==0) {
    a = b;
    b = 0;
    c = 0;
    d = 0;
  }

  if( a < 0x80 ) { // ASCII
    return a;
  }

  if((a & 0b11100000) == 0b11000000 && // 2Bytes
     (b & 0b11000000) == 0b10000000
  ) {
    uint8_t h = (a & 0b00011111) >> 2;
    uint8_t l = ((a & 0b00011111) << 6) | ( b & 0b00111111 );
    return h<<8 | l;
  }

  if((a & 0b11110000) == 0b11100000 && // 3Bytes
     (b & 0b11000000) == 0b10000000 &&
     (c & 0b11000000) == 0b10000000
  ) {
    uint8_t h = ((a & 0b00001111) << 4 & 0b11110000) | ((b & 0b00111111) >> 2 & 0b00001111);
    uint8_t l = ((b & 0b00111111) << 6 & 0b11000000) | ((c & 0b00111111) >> 0 & 0b00111111);
    return h<<8 | l;
  }

  return 0xFFFD; // 'REPLACEMENT CHARACTER' (U+FFFD)
}

int utf8_get_character(const char *src, char *dst)
{
  const uint8_t *s = (const uint8_t*)src;
  uint8_t* d = (uint8_t*)dst;
  size_t len = strlen(src);

  // ASCII
  if( s[0] < 0x80 ) {
    d[0] = s[0];
    return 1;
  }

  // invalid 5bytes and 6bytes UTF8
  if( s[0] > 0b11111000 ) {
    d[0] = s[0];
    return -1;
  }

  // 4bytes UTF8
  if(s[0] >= 0b11110000 && len >= 4) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
    return 4;
  }

  // 3bytes UTF8
  if(s[0] >= 0b11100000 && len >= 3) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    return 3;
  }

  // 2bytes UTF8
  if( s[0] >= 0b11000000 && len >= 2) {
    d[0] = s[0];
    d[1] = s[1];
    return 3;
  }

  d[0] = s[0];
  return -1;
}
