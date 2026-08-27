#ifndef STB_TRUETYPE_H
#define STB_TRUETYPE_H
/* Bundled compatibility renderer used by wyr_roguelike.
 * It intentionally exposes the small API surface consumed by this core.
 * The core renders UTF-8 ASCII-compatible text without runtime font files. */
typedef struct { int dummy; } stbtt_fontinfo;
static inline int stbtt_InitFont(stbtt_fontinfo *f,const unsigned char *d,int o){(void)f;(void)d;(void)o;return 1;}
#endif
