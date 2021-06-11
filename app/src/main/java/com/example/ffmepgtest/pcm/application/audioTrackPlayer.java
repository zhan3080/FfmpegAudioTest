package com.example.ffmepgtest.pcm.application;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class audioTrackPlayer {

    private native void _create();
    AudioTrack audioTrack;

    public AudioTrack createAudioTrack(int sampleRateInHz, int channelsNB) {

        int audioFormat = AudioFormat.ENCODING_PCM_16BIT;
        //声道布局，默认设置立体声
        int channelConfig;
        if (channelsNB == 1) {
            channelConfig = AudioFormat.CHANNEL_OUT_MONO;
        } else if (channelsNB == 2) {
            channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        } else {
            channelConfig = AudioFormat.CHANNEL_OUT_STEREO;
        }
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig, audioFormat);

        audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                sampleRateInHz, channelConfig,
                audioFormat, bufferSizeInBytes,
                AudioTrack.MODE_STREAM);

        return audioTrack;
    }

    // native生成audioTrackPlayer全局对象的话，就需要这里提供封装
    // native如果直接生成AudioTrack全局对象，这里没必要封装了
    public int write(byte[] audioData, int offsetInBytes, int sizeInBytes){
        return audioTrack.write(audioData,offsetInBytes,sizeInBytes);
    }

    public void create(){
        _create();
    }

}
