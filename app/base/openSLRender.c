

#include "logger.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"

#define TAG "ffmpeg_audioPlayer"
#define DEBUG true

#include <unistd.h>
#include "OpenSLRender.h"

#define OUTPUT_FILE "/sdcard/output.pcm"

FILE *pcmFile;
void *buffer;
uint8_t *out_buffer;

void InitSL() {
    LogI(TAG,DEBUG, "Init");
    int result = -1;
    result = CreateEngine();
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::InitSL CreateEngine fail. result=%d", result);
        return;
    }

    result = CreateOutputMixer();
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::InitSL CreateOutputMixer fail. result=%d", result);
        return;
    }

    result = CreateAudioPlayer();
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::InitSL CreateAudioPlayer fail. result=%d", result);
        return;
    }

    //pthread_create(m_thread, NULL, CreateSLWaitingThread, NULL);

    if(result != SL_RESULT_SUCCESS) {
        LogE(TAG,DEBUG, "OpenSLRender::InitSL fail. result=%d", result);
        UnInitSL();
    }

}

void renderAudioFrame(uint8_t *pData, int dataSize) {
    LogE(TAG,DEBUG, "renderAudioFrame pData=%p, dataSize=%d",pData, dataSize);
    /*if(m_AudioPlayerPlay) {
        if (pData != nullptr && dataSize > 0) {

            //temp resolution, when queue size is too big.
            while(GetAudioFrameQueueSize() >= MAX_QUEUE_BUFFER_SIZE && !m_Exit)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }

            std::unique_lock<std::mutex> lock(m_Mutex);
            AudioFrame *audioFrame = new AudioFrame(pData, dataSize);
            m_AudioFrameQueue.push(audioFrame);
            m_Cond.notify_all();
            lock.unlock();
        }
    }*/
}

void unInitSL() {
    LogE(TAG,DEBUG, "unInitSL");

    if (m_AudioPlayerPlay) {
        (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_STOPPED);
        m_AudioPlayerPlay = NULL;
    }

    if (m_AudioPlayerObj) {
        (*m_AudioPlayerObj)->Destroy(m_AudioPlayerObj);
        m_AudioPlayerObj = NULL;
        m_BufferQueue = NULL;
    }

    if (m_OutputMixObj) {
        (*m_OutputMixObj)->Destroy(m_OutputMixObj);
        m_OutputMixObj = NULL;
    }

    if (m_EngineObj) {
        (*m_EngineObj)->Destroy(m_EngineObj);
        m_EngineObj = NULL;
        m_EngineEngine = NULL;
    }

}

int CreateEngine() {
    LogI(TAG,DEBUG, "CreateEngine");
    SLresult result = SL_RESULT_SUCCESS;
    result = slCreateEngine(&m_EngineObj, 0, NULL, 0, NULL, NULL);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateEngine slCreateEngine fail. result=%d", result);
        return -1;
    }

    result = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateEngine Realize fail. result=%d", result);
        return -1;
    }

    result = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_EngineEngine);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateEngine GetInterface fail. result=%d", result);
        return -1;
    }

    return result;
}

int CreateOutputMixer() {
    LogI(TAG,DEBUG, "createOutputMixer");
    SLresult result = SL_RESULT_SUCCESS;
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

    result = (*m_EngineEngine)->CreateOutputMix(m_EngineEngine, &m_OutputMixObj, 1, mids, mreq);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
        return -1;
    }

    result = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateOutputMixer CreateOutputMix fail. result=%d", result);
        return -1;
    }

    return result;
}

int CreateAudioPlayer() {
    LogI(TAG,DEBUG, "createAudioPlayer");
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//format type
            (SLuint32)2,//channel count
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// bits per sample
            SL_PCMSAMPLEFORMAT_FIXED_16,// container size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// channel mask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObj};
    SLDataSink slDataSink = {&outputMix, NULL};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result;


    result = (*m_EngineEngine)->CreateAudioPlayer(m_EngineEngine, &m_AudioPlayerObj, &slDataSource, &slDataSink, 3, ids, req);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer CreateAudioPlayer fail. result=%d", result);
        return -1;
    }

    result = (*m_AudioPlayerObj)->Realize(m_AudioPlayerObj, SL_BOOLEAN_FALSE);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer Realize fail. result=%d", result);
        return -1;
    }

    result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_PLAY, &m_AudioPlayerPlay);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
        return -1;
    }

    result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_BUFFERQUEUE, &m_BufferQueue);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
        return -1;
    }

    result = (*m_BufferQueue)->RegisterCallback(m_BufferQueue, AudioPlayerCallback, NULL);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer RegisterCallback fail. result=%d", result);
        return -1;
    }

    result = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_VOLUME, &m_AudioPlayerVolume);
    if(result != SL_RESULT_SUCCESS)
    {
        LogE(TAG,DEBUG, "OpenSLRender::CreateAudioPlayer GetInterface fail. result=%d", result);
        return -1;
    }


    return result;
}

void startRender() {
    LogI(TAG,DEBUG, "startRender");
    (*m_AudioPlayerPlay)->SetPlayState(m_AudioPlayerPlay, SL_PLAYSTATE_PLAYING);
    AudioPlayerCallback(m_BufferQueue, NULL);
}


void getPcmData(void **pcm)
{
    while(!feof(pcmFile))
    {
        fread(out_buffer, 44100 * 2 * 2, 1, pcmFile);
        if(out_buffer == NULL)
        {
            LogE(TAG,DEBUG,"read end");
            break;
        } else{
            LogI(TAG,DEBUG,"reading");
        }
        *pcm = out_buffer;
        break;
    }
}

void AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {

    getPcmData(&buffer);
    // for streaming playback, replace this test by logic to find and fill the next buffer
    if (NULL != buffer) {
        SLresult result;
        // enqueue another buffer
        result = (*bufferQueue)->Enqueue(bufferQueue, buffer, 44100 * 2 * 2);
    }
}


void* play_audio(void *argv)
{
    LogI(TAG, DEBUG, "play_audio begin");
    pcmFile = fopen(OUTPUT_FILE, "r");
    if(pcmFile == NULL)
    {
        LogE(TAG,DEBUG,"fopen file error");
        return NULL;
    }
    out_buffer = malloc(44100 * 2 * 2);
    unInitSL();
    InitSL();
    startRender();
    LogI(TAG,DEBUG,"play_audio end");
    return NULL;
}
