#include "FFmpegController.h"

#include <iostream>

#include "FFmpegControllerBuilder.h"
#include "Utils.h"

namespace MediaProcessor {

FFmpegController::FFmpegController(FFmpegControllerBuilder &ffmpegBuilder)
    : m_ffmpegBuilder(ffmpegBuilder) {}

bool FFmpegController::extractAudio(const fs::path &inputVideoPath,
                                    const fs::path &outputAudioPath) {
    CommandBuilder cmd;
    cmd.addArgument(m_ffmpegBuilder.getFFmpegPath().string());

    if (m_ffmpegBuilder.getOverwrite()) {
        cmd.addFlag("-y");  // overwrite
    } else {
        cmd.addFlag("-n");  // fail without prompt if file exists
    }

    cmd.addFlag("-i", inputVideoPath.string());                                 // input
    cmd.addFlag("-ar", std::to_string(m_ffmpegBuilder.getAudioSampleRate()));   // sample rate
    cmd.addFlag("-ac", std::to_string(m_ffmpegBuilder.getNumAudioChannels()));  // channels
    cmd.addFlag("-c:a", m_ffmpegBuilder.getAudioCodec());                       // codec
    cmd.addArgument(outputAudioPath.string());                                  // output

    return Utils::runCommand(cmd.build());
}

std::string FFmpegController::chunkAudio(const std::vector<double> &startTimes,
                                         const std::vector<double> durations, const int numChunks,
                                         const fs::path &outputAudioPath,
                                         const fs::path &chunksDir) {
    std::string chunks;
    // Chunk the audio
    for (int i = 0; i < numChunks; ++i) {
        fs::path chunkPath = chunksDir / ("chunk_" + std::to_string(i) + ".wav");

        // Set higher precision for chunk boundaries
        std::ostringstream ssStartTime, ssDuration;
        ssStartTime << std::fixed << std::setprecision(6) << startTimes[i];
        ssDuration << std::fixed << std::setprecision(6) << durations[i];

        CommandBuilder cmd;
        cmd.addArgument(m_ffmpegBuilder.getFFmpegPath().string());

        if (m_ffmpegBuilder.getOverwrite()) {
            cmd.addFlag("-y");  // overwrite
        } else {
            cmd.addFlag("-n");  // fail without prompt if file exists
        }

        cmd.addFlag("-ss", ssStartTime.str());                                     // input position
        cmd.addFlag("-t", ssDuration.str());                                       // duration
        cmd.addFlag("-i", outputAudioPath.string());                               // input
        cmd.addFlag("-ar", std::to_string(m_ffmpegBuilder.getAudioSampleRate()));  // sample rate
        cmd.addFlag("-ac", std::to_string(m_ffmpegBuilder.getNumAudioChannels()));  // channels
        cmd.addFlag("-c:a", m_ffmpegBuilder.getAudioCodec());                       // codec
        cmd.addArgument(chunkPath.string());                                        // output

        if (!Utils::runCommand(cmd.build())) {
            return std::string();
        }
        chunks.append(chunkPath);
    }

    return chunks;
}

bool FFmpegController::mergeChunks(const fs::path &outputPath) {
    // Merge processed chunks with `crossfade` using CommandBuilder
    CommandBuilder cmd;
    cmd.addArgument(m_ffmpegBuilder.getFFmpegPath().string());

    if (m_ffmpegBuilder.getOverwrite()) {
        cmd.addFlag("-y");  // overwrite
    } else {
        cmd.addFlag("-n");  // fail without prompt if file exists
    }

    const std::vector<fs::path> chunkPaths = m_ffmpegBuilder.getChunkPaths();
    for (const auto &chunkPath : chunkPaths) {
        cmd.addFlag("-i", chunkPath.string());  // input
    }

    // If there's more then one processed chunk
    if (static_cast<int>(chunkPaths.size()) >= 2) {
        cmd.addFlag("-filter_complex",
                    FilterController::buildFC());  // forward decl. in FFmpegController.h for now
        cmd.addFlag("-map", "[outa]");
    }

    cmd.addFlag("-c:a", m_ffmpegBuilder.getAudioCodec());                      // codec
    cmd.addFlag("-ar", std::to_string(m_ffmpegBuilder.getAudioSampleRate()));  // sample rate
    cmd.addArgument(outputPath.string());                                      // output

    return Utils::runCommand(cmd.build());
}

double FFmpegController::getAudioDuration(const fs::path &audioPath) {
    CommandBuilder cmd;
    cmd.addArgument("ffprobe");
    cmd.addFlag("-v", "error");
    cmd.addFlag("-show_entries", "format=duration");
    cmd.addFlag("-of", "default=noprint_wrappers=1:nokey=1");
    cmd.addArgument(audioPath.string());

    FILE *pipe = popen(cmd.build().c_str(), "r");
    if (!pipe) {
        std::cerr << "Error: Failed to run ffprobe to get audio duration." << std::endl;
        return -1;
    }

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    try {
        return std::stod(result);
    } catch (std::exception &e) {
        std::cerr << "Error: Could not parse audio duration." << std::endl;
        return -1;
    }
}

bool FFmpegController::mergeMedia(const fs::path &inputVideoPath, const fs::path &inputAudioPath,
                                  const fs::path &outputPath) {
    std::cout << "Merging video and audio..." << std::endl;

    CommandBuilder cmd;
    cmd.addArgument(m_ffmpegBuilder.getFFmpegPath().string());

    if (m_ffmpegBuilder.getOverwrite()) {
        cmd.addFlag("-y");  // overwrite
    } else {
        cmd.addFlag("-n");  // fail without prompt if file exists
    }

    cmd.addFlag("-i", inputVideoPath);
    cmd.addFlag("-i", inputAudioPath);
    cmd.addFlag("-c:v", m_ffmpegBuilder.getVideoCodec());
    cmd.addFlag("-c:a", m_ffmpegBuilder.getAudioCodec());
    cmd.addFlag("-strict", m_ffmpegBuilder.getStrictness());
    cmd.addFlag("-map", m_ffmpegBuilder.getVideoMapping());
    cmd.addFlag("-map", m_ffmpegBuilder.getAudioMapping());
    cmd.addFlag("-shortest");
    cmd.addArgument(outputPath);

    std::string ffmpegCommand = cmd.build();

    std::cout << "Running FFmpeg command: " << ffmpegCommand << std::endl;
    bool result = Utils::runCommand(ffmpegCommand);

    return result;
}

}  // namespace MediaProcessor
