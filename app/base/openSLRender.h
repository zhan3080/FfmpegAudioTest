#ifndef LEARNFFMPEG_OPENSLRENDER_H
#define LEARNFFMPEG_OPENSLRENDER_H

//#include <cstdint>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
//#include <queue>
//#include <string>
//#include <thread.h>
#include <pthread.h>

#define MAX_QUEUE_BUFFER_SIZE 3

void InitSL();
void ClearAudioCache();
void RenderAudioFrame(uint8_t *pData, int dataSize);
void UnInitSL();
int CreateEngine();
int CreateOutputMixer();
int CreateAudioPlayer();
int GetAudioFrameQueueSize();
void StartRender();
void HandleAudioFrameQueue();
static void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

SLObjectItf m_EngineObj = NULL;
SLEngineItf m_EngineEngine = NULL;
SLObjectItf m_OutputMixObj = NULL;
SLObjectItf m_AudioPlayerObj = NULL;
SLPlayItf m_AudioPlayerPlay = NULL;
SLVolumeItf m_AudioPlayerVolume = NULL;
SLAndroidSimpleBufferQueueItf m_BufferQueue;

//queue<AudioFrame *> m_AudioFrameQueue;
//pthread *m_thread = NULL;
//mutex   m_Mutex;
//static pthread_mutex_t m_Mutex = PTHREAD_MUTEX_INITIALIZER;
//condition_variable m_Cond;
//volatile bool m_Exit = false;


#endif //LEARNFFMPEG_OPENSLRENDER_H
