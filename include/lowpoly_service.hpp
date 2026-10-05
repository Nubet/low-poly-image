#ifndef LOWPOLY_SERVICE_HPP
#define LOWPOLY_SERVICE_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace lowpoly {

struct ProcessRequest {
    std::string input_path;
    int point_count = 500;
    uint32_t seed = 0;
};

enum class ProcessStatus {
    success,
    invalid_request,
    input_error,
    triangulation_error,
};

enum class ImageFormat {
    png,
    jpeg,
};

struct ProcessResponse {
    ProcessStatus status = ProcessStatus::invalid_request;
    int width = 0;
    int height = 0;
    int triangle_count = 0;
    std::vector<uint8_t> rgb_pixels;

    bool succeeded() const { return status == ProcessStatus::success; }
};

class LowpolyService {
public:
    ProcessResponse process(const ProcessRequest &request) const;
    bool save_image(const ProcessResponse &response, const std::string &path,
                    ImageFormat format) const;
};

} // namespace lowpoly

#endif
