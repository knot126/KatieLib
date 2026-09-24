#ifndef _SMASHHIT_H_
#define _SMASHHIT_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

struct lua_State;
typedef struct lua_State lua_State;

typedef struct QiVec2 {
	float x, y;
} QiVec2;

typedef struct QiVec3 {
	float x, y, z;
} QiVec3;

typedef struct QiVec4 {
	float x, y, z, w;
} QiVec4;

typedef struct QiColor {
	float r, g, b, a;
} QiColor;

typedef struct QiQuat {
	float x, y, z, w;
} QiQuat;

typedef struct QiMatrix4 {
	float elements[16];
} QiMatrix4;

typedef struct QiArray {
	int length;
	int capacity;
	void *data;
} QiArray;

typedef struct QiAudio {
	// todo
} QiAudio;

#ifdef GRANNY
#define QI_STRING_LOCAL_SIZE 16
#else
#define QI_STRING_LOCAL_SIZE 32
#endif

typedef struct QiString {
	char *data;
	int allocated_size;
	int length;
	char cached[QI_STRING_LOCAL_SIZE];
} QiString;

typedef struct QiScript {
	void *_unknown0;
	void *fixed_chunk_allocator;
	lua_State* *state; // there is more this points to but idrc atm
} QiScript;

typedef struct Script {
	QiScript *script;
	// incomplete
} Script;

typedef struct _ResManHashTable {
	int size;
	int capacity;
	void *entries;
} _ResManHashTable;

typedef struct ResMan {
	union {
		// no idea what this does... void* used for explicit padding, would be
		// fine without probably.
		void *u0;
		char u1;
	};
	_ResManHashTable resource_map;
	QiString u3;
	QiString u4;
	QiString additionalPath;
	QiString u6;
} ResMan;

typedef struct Scene {
	QiString path;
	ResMan resman;
	QiScript script;
	// etc...
} Scene;

typedef struct Resource {
	ResMan *resMan;
	QiString path;
	void *resource;
	int type;
	/* 64-bit: 4 bytes padding */
} Resource;

typedef struct Gfx {
	// unknown
} Gfx;

typedef uint32_t QiByteOrder;

typedef struct QiOutputStreamVtable {
	void *destruct;
	void *destructWithFree;
	void *flush;
	void *writeInternal;
} QiOutputStreamVtable;

typedef struct QiOutputStream {
	QiOutputStreamVtable *vtable;
	QiByteOrder byteOrder;
	int position;
} QiOutputStream;

typedef struct QiInput {
	// unknown contents
} QiInput;

typedef struct QiFileInputStream {
	void *vtable;
	QiByteOrder byteOrder;
	int _unk;
	FILE *file;
	QiString path;
	int size;
	int position;
	void *androidAsset;
} QiFileInputStream;

typedef struct QiInput_Event {
	int type;
	int data;
	int x;
	int y;
} QiInput_Event;

typedef struct QiAudioChannel {
	/* Contents */
} QiAudioChannel;

typedef enum QiViewportMode {
	QI_VIEWPORT_MODE_PIXEL = 0,
	QI_VIEWPORT_MODE_PIXEL_FLIPPED = 1,
	QI_VIEWPORT_MODE_ORTHO = 2,
	QI_VIEWPORT_MODE_2D = 3,
	QI_VIEWPORT_MODE_3D = 4,
} QiViewportMode;

typedef struct QiViewport {
	QiViewportMode mode;
	int left;
	int top;
	int right;
	int bottom;
	float rotationScaler;
	float rotation;
	float aspectRatio;
	float fieldOfView;
	float nearPlane;
	float farPlane;
	QiVec3 cameraPos;
	QiQuat cameraRot;
	QiMatrix4 projectionMatrix;
	QiMatrix4 modelViewMatrix;
	QiArray unkArray1;
	float lastPickX;
	float lastPickY;
	float pickRelatedThing;
	QiArray unkArray2;
	int unkArray2_data[8];
	QiArray unkArray3;
	int unkArray3_data[8];
	QiArray unkArray4;
	int unkArray4_data[8];
	bool scissor;
	int sci1;
	int sci4;
	int sci2;
	int sci3;
} QiViewport;

// From Aladdin Enterprise's MD5 implemenation which Dennis uses.
typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

typedef struct QiMd5 {
	md5_state_t md5_state;
	md5_byte_t final_hash[16];
} QiMd5;

/// !!! PLATFORM SPECIFIC (mostly unfinished structs) ///

#if defined(__arm__) || defined(__i386__)

typedef struct Player {
	char _unknown0[0x7f4];
	int balls;
	int streak;
	char _unknown1[0xb0];
	int mode;
} Player;

typedef struct Level {
	char _unknown0[0xf4];
	float offsetZ;
} Level;

typedef struct QiFileOutputStream {
	char _unknown0[0xc];
	FILE *file;
	QiString path;
} QiFileOutputStream;

#elif defined(__aarch64__)

typedef struct Player {
	char _unknown0[0x8bc];
	int balls;
	int streak;
	char _unknown1[0xb8];
	int mode;
} Player;

typedef struct Level {
	char _unknown0[0x124];
	float offsetZ;
} Level;

typedef struct QiFileOutputStream {
	char _unknown0[0x10];
	FILE *file;
	QiString path;
} QiFileOutputStream;

#else
#warning smashhit.h not defined for this platform
#endif

typedef struct Game {
	void *device;
	QiInput *input;
	void *display;
	void *renderer;
	ResMan *resman;
	void *audio;
	void *debug;
	Gfx *gfx;
	Scene *menuScene;
	Scene *movieScene;
	Scene *hudScene;
	Level *level;
	Player *player;
	void *http_thread;
	// incomplete
} Game;

typedef struct Room {
	Level *level;
	QiString name;
	// todo!
} Room;

#endif

