package com.example.ffmepgtest.pcm.application;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class audioTrackPlayer {

    private static final String TAG = "audioTrackPlayer";

    private native void _create();

    AudioTrack audioTrack;

    //native生成audioTrackPlayer全局对象，通过audioTrackPlayer提供AudioTrack方法调用的封装
    public AudioTrack createAudioTrack(int sampleRateInHz, int channelsNB) {
        Log.i(TAG, "createAudioTrack sampleRateInHz:" + sampleRateInHz + ",sampleRateInHz:" + sampleRateInHz);
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

    public int write(byte[] audioData, int offsetInBytes, int sizeInBytes) {
        if (audioTrack == null) {
            Log.e(TAG, "write audioTrack is null");
            return -1;
        }
        return audioTrack.write(audioData, offsetInBytes, sizeInBytes);
    }

    public void play() {
        Log.i(TAG, "play audioTrack:" + audioTrack);
        if (audioTrack == null) {
            return;
        }
        audioTrack.play();

    }

    public void stop() {
        if (audioTrack == null) {
            return;
        }
        audioTrack.stop();
        audioTrack.release();
    }

    public void create() {
        _create();
    }

}
