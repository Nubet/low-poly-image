#include "lowpoly_service.hpp"

extern "C" {
#include "arena.h"
#include "image.h"
#include "processing.h"
}

namespace lowpoly {
namespace {

class ArenaOwner {
public:
    ArenaOwner() = default;
    ArenaOwner(const ArenaOwner &) = delete;
    ArenaOwner &operator=(const ArenaOwner &) = delete;

    ~ArenaOwner() { arena_free(&arena_); }

    Arena *get() { return &arena_; }

private:
    Arena arena_ = {};
};

ProcessStatus map_status(LowpolyProcessStatus status)
{
    switch (status) {
    case LOWPOLY_PROCESS_SUCCESS:
        return ProcessStatus::success;
    case LOWPOLY_PROCESS_INPUT_ERROR:
        return ProcessStatus::input_error;
    case LOWPOLY_PROCESS_TRIANGULATION_ERROR:
        return ProcessStatus::triangulation_error;
    case LOWPOLY_PROCESS_INVALID_REQUEST:
    default:
        return ProcessStatus::invalid_request;
    }
}

} // namespace

ProcessResponse LowpolyService::process(const ProcessRequest &request) const
{
    ArenaOwner arena;
    LowpolyProcessRequest c_request = {
        .input_path = request.input_path.c_str(),
        .point_count = request.point_count,
        .seed = request.seed,
    };
    LowpolyProcessResult c_result = {};

    LowpolyProcessStatus c_status = lowpoly_process(arena.get(), &c_request, &c_result);
    ProcessResponse response = {.status = map_status(c_status)};
    if (c_status != LOWPOLY_PROCESS_SUCCESS)
        return response;

    response.width = c_result.output.width;
    response.height = c_result.output.height;
    response.triangle_count = c_result.triangle_count;

    const auto *pixels = reinterpret_cast<const uint8_t *>(c_result.output.pixels);
    const size_t pixel_bytes = static_cast<size_t>(response.width) *
                               static_cast<size_t>(response.height) * sizeof(Pixel);
    response.rgb_pixels.assign(pixels, pixels + pixel_bytes);
    return response;
}

bool LowpolyService::save_image(const ProcessResponse &response, const std::string &path,
                                ImageFormat format) const
{
    if (!response.succeeded() || response.width <= 0 || response.height <= 0 || path.empty())
        return false;

    const size_t pixel_count = static_cast<size_t>(response.width) *
                               static_cast<size_t>(response.height);
    if (response.rgb_pixels.size() != pixel_count * sizeof(Pixel))
        return false;

    Image image = {
        .width = response.width,
        .height = response.height,
        .pixels = reinterpret_cast<Pixel *>(
            const_cast<uint8_t *>(response.rgb_pixels.data())),
    };
    if (format == ImageFormat::jpeg)
        return image_save_as_jpeg(&image, path.c_str(), 95) != 0;

    return image_save_as_png(&image, path.c_str()) != 0;
}

} // namespace lowpoly
