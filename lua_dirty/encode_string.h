#ifndef ENCODE_STRING_H_
#define ENCODE_STRING_H_



/* the path encoding on Windows is ansi(mbcs)
  convert it to utf8
  if success, then return 1 same as other str* functions, like str_upper().
  if fail, return 0
*/
struct lua_State;
int 
str_mbcs2utf8(struct lua_State *L);

#endif //ENCODE_STRING_H_
