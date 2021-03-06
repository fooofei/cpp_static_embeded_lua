
#include "encode_string.h"

#include "../lua/lua.h"
#include "../lua/lauxlib.h"


/* return 0 if success 
  if success, the argument `outs` and `outl` is the result,
  should remember to free
  `outs` is null-terminated
  return -1 if fail
*/

#ifdef WIN32
#include <Windows.h>
static
int
mbcs2utf16le_win(const char * s, size_t l, wchar_t ** outs, size_t * outl)
{
  int r;
  size_t tempsize = 0;
  wchar_t * temps = 0;

  // the return size is in characters, not contain null-terminated character
  r = MultiByteToWideChar(CP_OEMCP, 0, s, (int)l, NULL, 0);
  if (r <= 0) {
    return -1;
  }
  r += 1; // add the null-terminated character
  tempsize = (size_t)r;
  temps = (wchar_t*)malloc(sizeof(wchar_t)*tempsize);
  if (!temps) {
    return -1;
  }

  for (;;)
  {
    r = MultiByteToWideChar(CP_OEMCP, 0, s, (int)l, temps, (int)tempsize);
    if (r > 0) {
      temps[r] = 0;
      *outs = temps;
      *outl = (size_t)r;
      return 0;
    }
    break;
  }

  if (temps) {
    free(temps);
    temps = 0;
  }
  return -1;
}

static
int
utf16le2utf8_win(const wchar_t * s, size_t l, char ** outs, size_t * outl)
{
  int r;
  size_t tempsize = 0;
  char * temps = 0;

  // the return size is in characters, not contain null-terminated character
  r = WideCharToMultiByte(CP_UTF8, 0, s, (int)l, 0, 0, NULL, NULL);
  if (r <= 0) {
    return -1;
  }
  r += 1;

  tempsize = (size_t)r;
  temps = (char*)malloc(sizeof(char)*tempsize);
  if (!temps) {
    return -1;
  }
  
  for (;;)
  {
    r = WideCharToMultiByte(CP_UTF8, 0, s, (int)l, temps,(int)tempsize, NULL, NULL);
    if (r > 0) {
      temps[r] = 0;
      *outs = temps;
      *outl = (size_t)r;
      return 0;
    }
    break;
  }

  if (temps) {
    free(temps);
    temps = 0;
  }
  return -1;
}


#endif



int
str_mbcs2utf8(lua_State *L)
{
  size_t l_src_mbcs=0;
  size_t l_utf16le = 0;
  size_t l_utf8 = 0;
  wchar_t * s_utf16le = 0;
  char * s_utf8 = 0;
  luaL_Buffer b;
  int err;

  const char *s = luaL_checklstring(L, 1, &l_src_mbcs);
  if (!(s && l_src_mbcs)) {
    return 0;
  }

  /* the posix no need to convert
    posix use iconv, have to link iconv library, is too heavy
  */
#ifdef WIN32
  err = mbcs2utf16le_win(s, l_src_mbcs, &s_utf16le, &l_utf16le);
  if (err) {
    return 0;
  }

  for (;;)
  {
    err = utf16le2utf8_win(s_utf16le, l_utf16le, &s_utf8, &l_utf8);
    if (err) {
      break;
    }

    char *p = luaL_buffinitsize(L, &b, l_utf8+1);

    if (!p) {
      break;
    }
    memcpy(p, s_utf8, l_utf8);
    p[l_utf8] = 0;
    luaL_pushresultsize(&b, l_utf8);

    free(s_utf16le);
    free(s_utf8);
    return 1;
    break;
  }

  if (s_utf16le) {
    free(s_utf16le);
  }
  if (s_utf8) {
    free(s_utf8);
  }

  return 0;
 
#else
  return 0;
#endif
}
