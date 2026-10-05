#include <algorithm>
#include <cstdint>
#include <random>
#include <string>
#include <thread>
#include <utility>

#include <nfd.h>
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

uint32_t generate_random_seed()
{
    std::random_device random_device;
    return random_device();
}

const char *process_error_message(lowpoly::ProcessStatus status)
{
    switch (status) {
    case lowpoly::ProcessStatus::invalid_request:
        return "Invalid processing parameters";
    case lowpoly::ProcessStatus::input_error:
        return "Could not open the selected image";
    case lowpoly::ProcessStatus::triangulation_error:
        return "Could not generate the low-poly mesh";
    case lowpoly::ProcessStatus::success:
        return "Ready";
    }

    return "Could not process the input image";
}

bool choose_input_file(std::string *path)
{
    nfdu8char_t *selected_path = nullptr;
    nfdu8filteritem_t filters[] = {{"Image files", "png,jpg,jpeg,bmp"}};
    nfdopendialogu8args_t arguments = {};
    arguments.filterList = filters;
    arguments.filterCount = 1;

    const nfdresult_t result = NFD_OpenDialogU8_With(&selected_path, &arguments);
    if (result != NFD_OKAY)
        return false;

    *path = selected_path;
    NFD_FreePathU8(selected_path);
    return true;
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
    const bool file_dialog_ready = NFD_Init() == NFD_OKAY;
    std::jthread worker;

    window->set_status_text("Ready");
    window->on_browse_input([window, file_dialog_ready] {
        if (!file_dialog_ready) {
            window->set_status_text("File picker is unavailable");
            return;
        }

        std::string path;
        if (choose_input_file(&path))
            window->set_input_path(slint::SharedString(path));
    });

    window->on_generate([window, &worker] {
        int point_count = 0;
        int seed = 0;
        if (!parse_integer(window->get_point_count(), &point_count) ||
            (window->get_use_fixed_seed() && !parse_integer(window->get_seed(), &seed))) {
            window->set_status_text("Points and seed must be valid integers");
            return;
        }

        if (window->get_input_path().empty()) {
            window->set_status_text("Choose an input image first");
            return;
        }

        const uint32_t used_seed = window->get_use_fixed_seed()
            ? static_cast<uint32_t>(seed)
            : generate_random_seed();
        window->set_seed(slint::SharedString(std::to_string(used_seed)));

        lowpoly::ProcessRequest request = {
            .input_path = std::string(window->get_input_path().data()),
            .point_count = point_count,
            .seed = used_seed,
        };

        const slint::ComponentWeakHandle<MainWindow> weak_window(window);
        window->set_status_text("Processing...");
        window->set_is_processing(true);

        worker = std::jthread([weak_window, request = std::move(request), used_seed] {
            lowpoly::LowpolyService service;
            lowpoly::ProcessResponse response = service.process(request);

            slint::invoke_from_event_loop(
                [weak_window, response = std::move(response), used_seed]() mutable {
                    const auto window = weak_window.lock();
                    if (!window)
                        return;

                    window.value()->set_is_processing(false);
                    if (!response.succeeded()) {
                        window.value()->set_status_text(
                            process_error_message(response.status));
                        return;
                    }

                    window.value()->set_output_image(make_image(response));
                    const std::string status =
                        "Generated " + std::to_string(response.triangle_count) +
                        " triangles (seed: " + std::to_string(used_seed) + ")";
                    window.value()->set_status_text(slint::SharedString(status));
                });
        });
    });

    window->run();
    if (worker.joinable())
        worker.join();
    if (file_dialog_ready)
        NFD_Quit();
    return 0;
}
