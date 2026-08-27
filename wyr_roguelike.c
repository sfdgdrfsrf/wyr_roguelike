#include "libretro.h"
#include "stb_truetype.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define W 640u
#define H 360u
#define MAX_MODS 32
#define SAVE_VERSION 1u
#define CURSE_THRESHOLD 12
#define HP_MAX 100

typedef enum { MOD_COMBAT, MOD_RESOURCE, MOD_UTILITY, MOD_NARRATIVE, MOD_WILDCARD } mod_type_t;
typedef struct {
  uint32_t id; const char *text_left; const char *text_right;
  uint32_t buff_type, buff_value, curse_type, curse_value;
} wyr_scenario_t;

#include "scenarios.h"

typedef struct {
  uint32_t type,value;
} modifier_t;

typedef struct {
  uint32_t version;
  uint32_t current_level;
  uint32_t scenario_index;
  uint32_t rng_state;
  uint64_t frame;
  int32_t hp;
  uint32_t curse_count,buff_count;
  modifier_t curses[MAX_MODS];
  modifier_t buffs[MAX_MODS];
} save_state_t;

static struct {
  retro_environment_t env; retro_video_refresh_t video;
  retro_input_state_t input;
  uint32_t *framebuffer;
  save_state_t s;
  bool game_loaded;
} G;

static uint32_t rng_next(void) {
  uint32_t x=G.s.rng_state ? G.s.rng_state : 0xA341316Cu;
  x ^= x<<13; x ^= x>>17; x ^= x<<5; G.s.rng_state=x; return x;
}
static void putpix(unsigned x,unsigned y,uint32_t c){ if(x<W&&y<H) G.framebuffer[y*W+x]=c; }
static void rect(unsigned x,unsigned y,unsigned w,unsigned h,uint32_t c){
  for(unsigned yy=y;yy<y+h&&yy<H;yy++) for(unsigned xx=x;xx<x+w&&xx<W;xx++) putpix(xx,yy,c);
}
/* Compact 5x7 text rasterizer. Printable ASCII gets a deterministic glyph pattern;
 * UTF-8 bytes outside ASCII are represented safely as boxes. */
static void glyph(unsigned x,unsigned y,unsigned char ch,uint32_t c,unsigned scale){
  if(ch<32||ch>126) { rect(x,y,5*scale,7*scale,c); return; }
  uint32_t seed=(uint32_t)ch*2654435761u;
  for(unsigned gy=0;gy<7;gy++) for(unsigned gx=0;gx<5;gx++){
    if(gx==0||gx==4||gy==0||gy==6 || ((seed>>(gx+gy*5))&1u))
      rect(x+gx*scale,y+gy*scale,scale,scale,c);
  }
}
static unsigned text(unsigned x,unsigned y,const char *s,uint32_t c,unsigned scale,unsigned max){
  unsigned n=0;
  while(*s && n<max){ unsigned char ch=(unsigned char)*s++; if(ch<128){glyph(x+n*6*scale,y,ch,c,scale);n++;} }
  return n;
}
static void draw(void){
  memset(G.framebuffer,0,W*H*sizeof(uint32_t));
  const wyr_scenario_t *sc=&SCENARIOS[G.s.scenario_index%SCENARIO_COUNT];
  rect(0,0,W,H,0x10131Au);
  rect(0,0,W,42,0x202838u);
  text(18,12,"WOULD YOU RATHER",0xFFFFFFFFu,2,30);
  text(500,12,"LEVEL",0xAFC4FFu,1,8);
  rect(550,9,70,22,0x0B0E14u); text(560,15,(char[12]){0},0,1,0);
  rect(18,62,292,245,0x18212Fu); rect(330,62,292,245,0x18212Fu);
  text(34,80,"OPTION A",0x66FF88u,2,10);
  text(346,80,"OPTION B",0x66FF88u,2,10);
  text(34,112,sc->text_left,0xFFFFFFFFu,1,44);
  text(346,112,sc->text_right,0xFFFFFFFFu,1,44);
  text(34,270,"BUFF",0x66FF88u,1,6);
  text(346,270,"CURSE",0xFF6666u,1,6);
  rect(18,320,604,22,0x202838u);
  text(28,327,"LEFT/RIGHT SELECT   START PAUSE   SELECT RESTART",0xC8D2E8u,1,70);
  G.video(G.framebuffer,W,H,W*sizeof(uint32_t));
}
static void choose(bool right){
  const wyr_scenario_t *sc=&SCENARIOS[G.s.scenario_index%SCENARIO_COUNT];
  uint32_t bv=sc->buff_value+(right?1u:0u), cv=sc->curse_value+(right?0u:1u);
  if(G.s.buff_count<MAX_MODS) G.s.buffs[G.s.buff_count++]=(modifier_t){sc->buff_type,bv};
  if(G.s.curse_count<MAX_MODS) G.s.curses[G.s.curse_count++]=(modifier_t){sc->curse_type,cv};
  if(right) G.s.hp -= (int32_t)(cv*2u);
  else G.s.hp += (int32_t)bv;
  if(G.s.hp>HP_MAX)G.s.hp=HP_MAX;
  G.s.current_level++;
  G.s.scenario_index=(G.s.scenario_index+1u)%SCENARIO_COUNT;
  (void)rng_next();
  if(G.s.hp<=0 || G.s.curse_count>=CURSE_THRESHOLD) retro_reset();
}
void retro_set_environment(retro_environment_t e){G.env=e;}
void retro_set_video_refresh(retro_video_refresh_t v){G.video=v;}
void retro_set_audio_sample(retro_audio_sample_t a){(void)a;}
void retro_set_audio_sample_batch(retro_audio_sample_batch_t a){(void)a;}
void retro_set_input_poll(retro_input_state_t p){(void)p;}
void retro_set_input_state(retro_input_state_t p){G.input=p;}
unsigned retro_api_version(void){return 1;}
void retro_get_system_info(struct retro_system_info *i){memset(i,0,sizeof(*i));i->library_name="wyr_roguelike";i->library_version="1.0";i->valid_extensions="";i->api_version="1";}
void retro_get_system_av_info(struct retro_system_av_info *i){memset(i,0,sizeof(*i));i->geometry.base_width=W;i->geometry.base_height=H;i->geometry.max_width=W;i->geometry.max_height=H;i->geometry.aspect_ratio=(float)W/(float)H;i->timing.fps=60.0;i->timing.sample_rate=44100.0;}
void retro_init(void){
  G.framebuffer=(uint32_t*)0; /* allocated once during init */
  static uint32_t fb[W*H]; G.framebuffer=fb;
  memset(&G.s,0,sizeof(G.s)); G.s.version=SAVE_VERSION; G.s.rng_state=0x12345678u; G.s.hp=HP_MAX;
}
void retro_deinit(void){G.framebuffer=NULL;G.game_loaded=false;}
void retro_reset(void){uint32_t seed=G.s.rng_state?G.s.rng_state:0x12345678u;memset(&G.s,0,sizeof(G.s));G.s.version=SAVE_VERSION;G.s.rng_state=seed;G.s.hp=HP_MAX;}
bool retro_load_game(const struct retro_game_info *g){(void)g;G.game_loaded=true;return true;}
void retro_unload_game(void){G.game_loaded=false;}
void retro_run(void){
  if(!G.game_loaded||!G.video)return;
  if(G.input){
    G.input(0,RETRO_DEVICE_JOYPAD,0,RETRO_DEVICE_ID_JOYPAD_LEFT) ? choose(false): (void)0;
    if(G.input(0,RETRO_DEVICE_JOYPAD,0,RETRO_DEVICE_ID_JOYPAD_RIGHT)) choose(true);
    if(G.input(0,RETRO_DEVICE_JOYPAD,0,RETRO_DEVICE_ID_JOYPAD_START)) {}
    if(G.input(0,RETRO_DEVICE_JOYPAD,0,RETRO_DEVICE_ID_JOYPAD_SELECT)) retro_reset();
  }
  G.s.frame++;
  (void)G.s.frame;
  draw();
}
size_t retro_serialize_size(void){return sizeof(save_state_t);}
bool retro_serialize(void *data,size_t size){if(!data||size<sizeof(save_state_t))return false;memcpy(data,&G.s,sizeof(G.s));return true;}
bool retro_unserialize(const void *data,size_t size){
  if(!data||size<sizeof(save_state_t))return false;
  save_state_t tmp; memcpy(&tmp,data,sizeof(tmp));
  if(tmp.version!=SAVE_VERSION||tmp.scenario_index>=SCENARIO_COUNT||tmp.curse_count>MAX_MODS||tmp.buff_count>MAX_MODS)return false;
  G.s=tmp;return true;
}
void *retro_get_memory_data(unsigned id){(void)id;return NULL;}
size_t retro_get_memory_size(unsigned id){(void)id;return 0;}
