#ifndef LIBRETRO_H
#define LIBRETRO_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
#define RETRO_API
#define RETRO_DEVICE_JOYPAD 1
#define RETRO_DEVICE_ID_JOYPAD_B 0
#define RETRO_DEVICE_ID_JOYPAD_Y 8
#define RETRO_DEVICE_ID_JOYPAD_LEFT 13
#define RETRO_DEVICE_ID_JOYPAD_RIGHT 14
#define RETRO_DEVICE_ID_JOYPAD_START 3
#define RETRO_DEVICE_ID_JOYPAD_SELECT 2
#define RETRO_ENVIRONMENT_SET_PIXEL_FORMAT 10
#define RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS 11
#define RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY 9
#define RETRO_ENVIRONMENT_SET_MESSAGE 6
#define RETRO_ENVIRONMENT_GET_LOG_INTERFACE 27
#define RETRO_PIXEL_FORMAT_RGB565 2
#define RETRO_MEMORY_SAVE_RAM 0
#define RETRO_DEVICE_MASK 0xFF
#define RETRO_DEVICE_SUBCLASS_MASK 0xFF00
#define RETRO_DEVICE_SUBCLASS_SHIFT 8
typedef void (*retro_video_refresh_t)(const void*,unsigned,unsigned,size_t);
typedef void (*retro_audio_sample_t)(int16_t,int16_t);
typedef size_t (*retro_audio_sample_batch_t)(const int16_t*,size_t);
typedef int16_t (*retro_input_state_t)(unsigned,unsigned,unsigned,unsigned);
typedef bool (*retro_environment_t)(unsigned,void*);
typedef void (*retro_log_printf_t)(int,const char*,...);
struct retro_game_geometry { unsigned base_width,base_height,max_width,max_height; float aspect_ratio; };
struct retro_system_timing { double fps,sample_rate; };
struct retro_system_av_info { struct retro_game_geometry geometry; struct retro_system_timing timing; };
struct retro_system_info { const char *library_name,*library_version,*valid_extensions,*need_fullpath,*block_extract,*api_version; };
struct retro_game_info { const char *path,*data; size_t size; const char *meta; };
struct retro_input_descriptor { unsigned port,device,index,id; const char *description; };
void retro_init(void); void retro_deinit(void); unsigned retro_api_version(void);
void retro_get_system_info(struct retro_system_info*); void retro_get_system_av_info(struct retro_system_av_info*);
void retro_set_environment(retro_environment_t); void retro_set_video_refresh(retro_video_refresh_t);
void retro_set_audio_sample(retro_audio_sample_t); void retro_set_audio_sample_batch(retro_audio_sample_batch_t);
void retro_set_input_poll(retro_input_state_t); void retro_set_input_state(retro_input_state_t);
void retro_run(void); void retro_reset(void); bool retro_load_game(const struct retro_game_info*);
void retro_unload_game(void); size_t retro_serialize_size(void); bool retro_serialize(void*,size_t);
bool retro_unserialize(const void*,size_t); void *retro_get_memory_data(unsigned); size_t retro_get_memory_size(unsigned);
#ifdef __cplusplus
}
#endif
#endif
