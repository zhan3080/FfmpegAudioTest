package com.example.ffmepgtest.pcm.application;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

public class audioTrackPlayer {

    private native long _create(Object[] args);

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

        AudioTrack audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                sampleRateInHz, channelConfig,
                audioFormat, bufferSizeInBytes,
                AudioTrack.MODE_STREAM);

        return audioTrack;
    }

}
