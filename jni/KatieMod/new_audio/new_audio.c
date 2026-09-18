/**
 * AAudio implementation for Smash Hit
 */

#include <aaudio/AAudio.h>
#include <dlfcn.h>
#include <yiploader/yiploader.h>

#include "../builtin/smashhit.h"
#include "../log.h"

typedef struct AudioDeviceInternal {
	AAudioStream *stream;
} AudioDeviceInternal;

typedef struct QiAudioDeviceAndroid {
	void *vtable;
	QiAudio *audio;
	AudioDeviceInternal *impl;
} QiAudioDeviceAndroid;

// Stuff for loading AAudio dynamically
extern void *gLibAndroid;

aaudio_result_t (*pAAudio_createStreamBuilder)(AAudioStreamBuilder** builder);
void (*pAAudioStreamBuilder_setSampleRate)(AAudioStreamBuilder* builder, int32_t sampleRate);
void (*pAAudioStreamBuilder_setChannelCount)(AAudioStreamBuilder* builder, int32_t channelCount);
void (*pAAudioStreamBuilder_setFormat)(AAudioStreamBuilder* builder, aaudio_format_t format);
void (*pAAudioStreamBuilder_setPerformanceMode)(AAudioStreamBuilder* builder, aaudio_performance_mode_t mode);
void (*pAAudioStreamBuilder_setDataCallback)(AAudioStreamBuilder* builder, AAudioStream_dataCallback callback, void *userData);
aaudio_result_t (*pAAudioStreamBuilder_openStream)(AAudioStreamBuilder* builder, AAudioStream** stream);
aaudio_result_t (*pAAudioStreamBuilder_delete)(AAudioStreamBuilder* builder);
aaudio_result_t (*pAAudioStream_close)(AAudioStream* stream);
aaudio_result_t (*pAAudioStream_requestStart)(AAudioStream* stream);
int32_t (*pAAudioStream_getSamplesPerFrame)(AAudioStream* stream);

static void load_aaudio(void) {
#define LOAD(SYM) p ## SYM = dlsym(gLibAndroid, #SYM);
	LOAD(AAudio_createStreamBuilder)
	LOAD(AAudioStreamBuilder_setSampleRate)
	LOAD(AAudioStreamBuilder_setChannelCount)
	LOAD(AAudioStreamBuilder_setFormat)
	LOAD(AAudioStreamBuilder_setPerformanceMode)
	LOAD(AAudioStreamBuilder_setDataCallback)
	LOAD(AAudioStreamBuilder_openStream)
	LOAD(AAudioStreamBuilder_delete)
	LOAD(AAudioStream_close)
	LOAD(AAudioStream_requestStart)
	LOAD(AAudioStream_getSamplesPerFrame)
#undef LOAD
}

// We need QiAudio::fillBuffer() to get audio data from callback
static void (*QiAudio_fillBuffer)(QiAudio *self, void *buffer, int length);

static aaudio_data_callback_result_t process(AAudioStream *stream, QiAudioDeviceAndroid *self, int16_t *data, int32_t numFrames) {
	/**
	 * Obtain audio data and give it to the stream
	 */
	
	const int32_t bufferSize = pAAudioStream_getSamplesPerFrame(stream) * numFrames * sizeof *data;
	
	if (self->audio) {
		QiAudio_fillBuffer(self->audio, data, bufferSize);
	}
	else {
		memset(data, 0, bufferSize);
	}
	
	return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

static void attach(QiAudioDeviceAndroid *self, QiAudio *audio) {
	/**
	 * Attach a QiAudio provider to an output stream
	 */
	
	AAudioStreamBuilder *builder;
	
	if (pAAudio_createStreamBuilder(&builder) != AAUDIO_OK) {
		LogE("AAudio_createStreamBuilder failed");
		return;
	}
	
	pAAudioStreamBuilder_setSampleRate(builder, 44100);
	pAAudioStreamBuilder_setChannelCount(builder, 2);
	pAAudioStreamBuilder_setFormat(builder, AAUDIO_FORMAT_PCM_I16);
	pAAudioStreamBuilder_setPerformanceMode(builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
	pAAudioStreamBuilder_setDataCallback(builder, (void *) process, self);
	
	self->audio = NULL;
	
	self->impl = malloc(sizeof *self->impl);
	
	if (!self->impl) {
		LogE("Could not allocate memory for AudioDeviceInternal");
		return;
	}
	
	if (pAAudioStreamBuilder_openStream(builder, &self->impl->stream) != AAUDIO_OK) {
		pAAudioStreamBuilder_delete(builder);
		LogE("AAudioStreamBuilder_openStream failed");
		free(self->impl);
		self->impl = NULL;
		return;
	}
	else {
		pAAudioStreamBuilder_delete(builder);
	}
	
	self->audio = audio;
	
	if (pAAudioStream_requestStart(self->impl->stream) != AAUDIO_OK) {
		LogE("AAudioStream_requestStart failed");
		return;
	}
}

static void detach(QiAudioDeviceAndroid *self) {
	/**
	 * Deatch the audio provider, closing the output stream. this->audio is kept
	 * so we can reattach if needed (e.g. when using setEnabled)
	 */
	
	pAAudioStream_close(self->impl->stream);
	free(self->impl);
	self->impl = NULL;
}

static void setEnabled(QiAudioDeviceAndroid *self, bool enabled) {
	if (enabled) {
		if (self->impl == NULL && self->audio) {
			attach(self, self->audio);
		}
	}
	else {
		if (self->impl) {
			detach(self);
		}
	}
}

const char *KNInitNewAudio(void) {
	load_aaudio();
	
	QiAudio_fillBuffer = YipLookupSymbol("_ZN7QiAudio10fillBufferEPsi");
	
	YipHookFunction("_ZN19QiAudioDeviceOpenSl6attachEP7QiAudio", attach, true);
	YipHookFunction("_ZN19QiAudioDeviceOpenSl6detachEv", detach, true);
	YipHookFunction("_ZN19QiAudioDeviceOpenSl10setEnabledEb", setEnabled, true);
	
	return NULL;
}
