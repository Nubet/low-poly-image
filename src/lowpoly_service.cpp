#include "lowpoly_service.hpp"

extern "C" {
#include "arena.h"
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

} // namespace lowpoly
