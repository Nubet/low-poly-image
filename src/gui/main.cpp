#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>

#include <slint.h>

#include "lowpoly_service.hpp"
#include "main.h"

namespace {

bool parse_integer(const slint::SharedString &text, int *value)
{
    try {
        size_t consumed = 0;
        const std::string input(text.data());
        const int parsed = std::stoi(input, &consumed);
        if (consumed != input.size())
            return false;
        *value = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

slint::Image make_image(const lowpoly::ProcessResponse &response)
{
    slint::SharedPixelBuffer<slint::Rgb8Pixel> buffer(response.width, response.height);
    auto *destination = reinterpret_cast<uint8_t *>(buffer.begin());
    std::copy(response.rgb_pixels.begin(), response.rgb_pixels.end(), destination);
    return slint::Image(std::move(buffer));
}

} // namespace

int main()
{
    auto window = MainWindow::create();
    lowpoly::LowpolyService service;

    window->set_status_text("Ready");
    window->on_generate([window, &service] {
        int point_count = 0;
        int seed = 0;
        if (!parse_integer(window->get_point_count(), &point_count) ||
            !parse_integer(window->get_seed(), &seed)) {
            window->set_status_text("Points and seed must be integers");
            return;
        }

        window->set_status_text("Processing...");
        lowpoly::ProcessRequest request = {
            .input_path = std::string(window->get_input_path().data()),
            .point_count = point_count,
            .seed = static_cast<uint32_t>(seed),
        };
        lowpoly::ProcessResponse response = service.process(request);

        if (!response.succeeded()) {
            window->set_status_text("Could not process the input image");
            return;
        }

        window->set_output_image(make_image(response));
        const std::string status =
            "Generated " + std::to_string(response.triangle_count) + " triangles";
        window->set_status_text(slint::SharedString(status));
    });

    window->run();
    return 0;
}
