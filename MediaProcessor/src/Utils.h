#ifndef UTILS_H
#define UTILS_H

#include <filesystem>
#include <string>
#include <unordered_map>
#include <utility>

namespace fs = std::filesystem;

namespace MediaProcessor::Utils {

/**
 * @brief Executes a command in the system shell.
 *
 * @return true if the command executes successfully, false otherwise.
 */
bool runCommand(const std::string &command);

/**
 * @brief Ensures that a directory exists, making it if necessary.
 *
 * @return true if the directory exists or is made successfully, false otherwise.
 */
bool ensureDirectoryExists(const fs::path &path);

/**
 * @brief Removes a file if it exists.
 *
 * @return true if the file is removed successfully or does not exist, false otherwise.
 */
bool removeFileIfExists(const fs::path &filePath);

/**
 * @brief Checks if a string contains whitespace characters.
 *
 * @return true if the string contains whitespace, false otherwise.
 */
bool containsWhitespace(const std::string &str);

/**
 * @brief Prepares the output paths for audio and video processing.
 *
 * @return A pair containing the output audio path and the output video path.
 */
std::pair<fs::path, fs::path> prepareOutputPaths(const fs::path &videoPath);

/**
 * @brief Trims trailing whitespace from a string.
 *
 * @return A new string with trailing whitespace removed.
 */
std::string trimTrailingSpace(const std::string &str);

/**
 * @brief Gets the duration of a media file (audio or video) using ffprobe.
 *
 * @return The duration of the media in seconds, or -1 if an error occurred.
 */
double getMediaDuration(const fs::path &mediaPath);

/**
 * @brief Checks if a value is within a specified range (inclusive).
 *
 * @return true if the value is within the range, false otherwise.
 */
template <typename T>
bool isWithinRange(T value, T lowerBound, T upperBound) {
    return value >= lowerBound && value <= upperBound;
}

/**
 * @brief Converts an enum value to its corresponding string representation.
 *
 * @tparam T Enum type.
 * @param value The enum value.
 * @param valueMap Map of enum values to their string representations.
 * @return The string representation of the enum value.
 */
template <typename T>
std::string enumToString(const T &value, const std::unordered_map<T, std::string> &valueMap);

}  // namespace MediaProcessor::Utils

#endif  // UTILS_H
