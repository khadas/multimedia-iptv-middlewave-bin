#ifndef ANDROID_AMPLAYER_H
#define ANDROID_AMPLAYER_H

#ifdef ANDROID
#include <media/MediaPlayerInterface.h>
#if ANDROID_PLATFORM_SDK_VERSION <= 27
#elif ANDROID_PLATFORM_SDK_VERSION <= 29
#include "media/ammediaplayerext.h"
#include <media/MediaAnalyticsItem.h>
#else
#include "media/ammediaplayerext.h"
#include <media/MediaMetricsItem.h>
#endif

#else
#include <binder/Parcel.h>
#include <utils/StrongPointer.h>//.h included in binder
#include <../OS_linux/MediaPlayerReport.h>//don`t easily increase the header file path
#endif

#include "iprotocol.h"
namespace android
{

typedef int (*PlayerEvtCB)(int msg, int ext1, int ext2, const Parcel *obj, void *handler);

enum am_media_parameter_keys
{
  KEY_SET_START_POSITION_MS = 1100, // set only
  KEY_SET_AMLPLAYER_POINT = 1101,   // set only
  KEY_SET_AMP_PREFER_TUNERHAL = 1102, // for aml_mp, prefer use tunerhal to play
};

class IAmPlayer_OS
{
public:
    IAmPlayer_OS();
    virtual ~IAmPlayer_OS();

#ifdef ANDROID
    virtual status_t setVideoSurfaceTexture(const sp<IGraphicBufferProducer> &bufferProducer) = 0;
    virtual status_t setPlaybackSettings(const AudioPlaybackRate &rate) = 0;
    virtual status_t getPlaybackSettings(AudioPlaybackRate *rate) = 0;
#endif
    virtual status_t setParameter(int key, const Parcel &request) = 0;
    virtual status_t getParameter(int key, Parcel *reply) = 0;
    virtual status_t invoke(const Parcel &request, Parcel *reply) = 0;
    virtual status_t onGetTrackInfo(Parcel *reply) = 0;
    virtual status_t onGetMediaInfo(Parcel *reply) = 0;

#ifdef ANDROID
#if ANDROID_PLATFORM_SDK_VERSION > 29
    mediametrics::Item *mAnalyticsItem;
#elif ANDROID_PLATFORM_SDK_VERSION > 27
    MediaAnalyticsItem *mAnalyticsItem;
#endif
#endif
};

class IAmPlayer : public IAmPlayer_OS
{
public:
    IAmPlayer() {};
    virtual ~IAmPlayer() {};

    virtual status_t setDataSource(IProtocol *_protocol) = 0;
    virtual status_t setDataSource(const char *path) = 0;

    virtual status_t prepare() = 0;
    virtual status_t prepareAsync() = 0;
    virtual status_t start() = 0;
    virtual status_t stop() = 0;
    virtual status_t pause() = 0;
    virtual status_t resume() = 0;
    virtual status_t seekTo(int msec) = 0;
    virtual status_t fast(int scale) = 0;
    virtual status_t stopfast() = 0;
    virtual status_t getCurrentPosition(int *msec) = 0;
    virtual status_t getDuration(int *mDuration) = 0;
    virtual status_t reset() = 0;
    virtual status_t setLooping(int loop) = 0;
    virtual void registerEvtCB(PlayerEvtCB func, void *hander) = 0;
    virtual int setVolume(int volume) = 0;
    virtual bool isPlaying() = 0;

    virtual status_t select_track(int _index) = 0;
    virtual status_t SetAudioBalance(int mode) = 0;
    virtual status_t set_ClearLastFrame(int isclearlastframe) = 0;
};

class AmPlayer : public RefBase
{
public:
    AmPlayer(int instanceNo);
    virtual ~AmPlayer();
#if ANDROID_PLATFORM_SDK_VERSION <= 27
    virtual status_t setDataSource(const char *url, const KeyedVector<String8, String8> *headers);
#endif

#ifdef ANDROID

#if ANDROID_PLATFORM_SDK_VERSION >= 20
    virtual status_t setDataSource(const sp<IMediaHTTPService> &httpService,
                                    const char *url, const KeyedVector<String8, String8> *headers);
#endif

#if ANDROID_PLATFORM_SDK_VERSION >= 23
    virtual status_t setDataSource(const sp<DataSource>& dataSource);
#endif

    virtual status_t setDataSource(const sp<IStreamSource> &source);
    virtual status_t setVideoSurfaceTexture(const sp<IGraphicBufferProducer> &bufferProducer);
#else
    virtual status_t setDataSource(const char *url);

#endif
    virtual status_t setDataSource(int fd, int64_t offset, int64_t length);

    virtual status_t prepare();
    virtual status_t prepareAsync();
    virtual status_t start();
    virtual status_t stop();
    virtual status_t pause();
    virtual status_t resume();
#if ANDROID_PLATFORM_SDK_VERSION <= 27
    virtual status_t seekTo(int msec);
#else
    virtual status_t seekTo(
        int msec, MediaPlayerSeekMode mode = MediaPlayerSeekMode::SEEK_PREVIOUS_SYNC);
#ifdef ANDROID
    virtual status_t getBufferingSettings(
        BufferingSettings* buffering /* nonnull */);
    virtual status_t setBufferingSettings(const BufferingSettings& buffering);
	virtual status_t setPlaybackSettings(const AudioPlaybackRate &rate);
    virtual status_t getPlaybackSettings(AudioPlaybackRate *rate);
    virtual status_t setSyncSettings(const AVSyncSettings &sync, float videoFpsHint);
    virtual status_t getSyncSettings(AVSyncSettings *sync, float *videoFps);
	virtual status_t notifyAt(int64_t mediaTimeUs);
    virtual void setAudioSink(const sp<MediaPlayerBase::AudioSink> &audioSink);
    virtual status_t dump(int fd, const Vector<String16> &args) const;
#endif
#endif


#if ANDROID_PLATFORM_SDK_VERSION >= 20
#ifdef ANDROID
virtual status_t getMetadata(
        const media::Metadata::Filter& ids, Parcel *records);
#endif
#endif

    virtual status_t fast(int scale);
    virtual status_t stopfast();
    virtual status_t getCurrentPosition(int *msec);
    virtual status_t getDuration(int *mDuration);
    virtual status_t reset();
    virtual status_t setLooping(int loop);
    virtual void registerEvtCB(PlayerEvtCB func, void *hander);
    void notifyListener(int msg, int ext1 = 0, int ext2 = 0, const Parcel *obj = NULL);
    virtual bool isPlaying();
    status_t initCheck();
#ifdef ANDROID
    player_type playerType();
#endif
    status_t invoke(const Parcel &request, Parcel *reply);
    status_t setParameter(int key, const Parcel &request);
    status_t getParameter(int key, Parcel *reply);
    bool isManifestUrl(const char *url);

#if ANDROID_PLATFORM_SDK_VERSION == 22
    virtual status_t getTimestamp(AudioTimestamp &ts);
    virtual uint32_t getSampleRate();
#endif

private:
    IAmPlayer *mPlayer;
    IProtocol* mprotocol;
    int mPid;
    PlayerEvtCB m_CB;
    void *m_pHandler;
};
}; // namespace android


#endif //ANDROID_AMPLAYER_H
