#ifndef FFMPEGCONFIGMANAGER_H
#define FFMPEGCONFIGMANAGER_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "ConfigManager.h"
#include "Utils.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

enum class AudioCodec { AAC, MP3, FLAC, OPUS, UNKNOWN };
enum class VideoCodec { H264, H265, VP8, VP9, COPY, UNKNOWN };
enum class CodecStrictness { VERY, STRICT, NORMAL, UNOFFICIAL, EXPERIMENTAL };

/**
 * @brief Manages FFmpeg-specific configurations and user settings
 *
 * Provides an interface for setting and retrieving configuration options,
 * including file paths, codecs, and other processing parameters.
 */
class FFmpegConfigManager {
   public:
    FFmpegConfigManager();

    // Global Setters
    void setOverwrite(bool overwrite);
    void setCodecStrictness(CodecStrictness strictness);
    void setInputFilePath(const fs::path inputFilePath);
    void setOutputFilePath(const fs::path outputFilePath);

    // Global Getters
    bool getOverwrite() const;
    CodecStrictness getCodecStrictness() const;
    fs::path getInputFilePath() const;
    fs::path getOutputFilePath() const;

    // Audio Setters
    void setAudioCodec(AudioCodec codec);
    void setAudioSampleRate(int sampleRate);
    void setAudioChannels(int channels);

    // Audio Getters
    AudioCodec getAudioCodec() const;
    int getAudioSampleRate() const;
    int getAudioChannels() const;

    // Video Setters
    void setVideoCodec(VideoCodec codec);

    // Video Getters
    VideoCodec getVideoCodec() const;

    // Value Map Getters
    const std::unordered_map<AudioCodec, std::string>& getAudioCodecAsString() const;
    const std::unordered_map<VideoCodec, std::string>& getVideoCodecAsString() const;
    const std::unordered_map<CodecStrictness, std::string>& getCodecStrictnessAsString() const;

    friend class FFmpegController;

   protected:
    struct FFmpegGlobalSettings {
        bool overwrite;
        CodecStrictness strictness;
        fs::path inputFilePath;
        fs::path outputFilePath;

        FFmpegGlobalSettings() : overwrite(false), strictness(CodecStrictness::EXPERIMENTAL) {}
    } m_globalSettings;

    struct FFmpegAudioSettings {
        AudioCodec codec;
        int sampleRate;
        int numChannels;

        FFmpegAudioSettings() : codec(AudioCodec::AAC), sampleRate(48000), numChannels(2) {}
    } m_audioSettings;

    struct FFmpegVideoSettings {
        VideoCodec codec;

        FFmpegVideoSettings() : codec(VideoCodec::COPY) {}
    } m_videoSettings;

    // Update Global, Audio, or Video Settings
    void updateSettings(const struct FFmpegGlobalSettings& globalSettings = FFmpegGlobalSettings(),
                        const struct FFmpegAudioSettings& audioSettings = FFmpegAudioSettings(),
                        const struct FFmpegVideoSettings& videoSettings = FFmpegVideoSettings());

   private:
    std::unordered_map<AudioCodec, std::string> m_audioCodecAsString;
    std::unordered_map<VideoCodec, std::string> m_videoCodecAsString;
    std::unordered_map<CodecStrictness, std::string> m_codecStrictnessAsString;

    // Channel # allowed options
    static constexpr const int AAC_MAX_CHANNELS = 8;
    static constexpr const int MP3_MAX_CHANNELS = 2;
    static constexpr const int FLAC_MAX_CHANNELS = 8;
    static constexpr const int OPUS_MAX_CHANNELS = 8;

    // Sample rate allowed options
    static constexpr const std::array<int, 13> AAC_SAMPLE_RATES = {
        7350, 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000};
    static constexpr const std::array<int, 9> MP3_SAMPLE_RATES = {8000,  12000, 11025, 16000, 24000,
                                                                  22050, 32000, 44100, 48000};
    static constexpr const std::array<int, 5> OPUS_SAMPLE_RATES = {8000, 12000, 16000, 24000,
                                                                   48000};
    static constexpr const int FLAC_MAX_SAMPLE_RATE = 1048575;

    template <typename T, std::size_t N>
    void validateOption(const T optionValue, const std::array<T, N>& validOptions);

    template <typename T>
    void validateOption(const T optionValue, const T optionMax);

    void validateAudio();

    void validateVideo();
};

}  // namespace MediaProcessor

#endif  // FFMPEGCONFIGMANAGER_H
