#ifndef FFMPEGCONTROLLER_H
#define FFMPEGCONTROLLER_H

#include <filesystem>
#include <string>

#include "FFmpegControllerBuilder.h"

namespace fs = std::filesystem;

namespace MediaProcessor {

class FilterController {
   public:
    static std::string buildFC();  // forward decl until FilterController ready
};

class FFmpegController {
   public:
    FFmpegController(FFmpegControllerBuilder &ffmpegBuilder);

    // Extract audio from file and return true if successful, otherwise false
    bool extractAudio(const fs::path &inputVideoPath, const fs::path &outputAudioPath);

    // Chunk the audio and return the chunk paths if successful, otherwise an empty string
    std::string chunkAudio(const std::vector<double> &startTimes,
                           const std::vector<double> durations, const int numChunks,
                           const fs::path &outputAudioPath, const fs::path &chunksDir);

    // Merge chunked audio with video and return true if successful, otherwise false
    bool mergeChunks(const fs::path &outputPath);

    // Get the audio duration and return it if successful, otherwise returns -1
    double getAudioDuration(const fs::path &audioPath);

    // Merge audio with video and return true if successful, otherwise false
    bool mergeMedia(const fs::path &inputVideoPath, const fs::path &inputAudioPath,
                    const fs::path &outputPath);

    FFmpegControllerBuilder &m_ffmpegBuilder;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLER_H */
