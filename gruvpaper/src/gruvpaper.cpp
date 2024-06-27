#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <getopt.h>
#include "CImg.h"

using namespace cimg_library;

const char* DEFAULT_COLOR_BACKGROUND = "#32302F";
const char* DEFAULT_COLOR_EMPTY_SQUARE = "#504945";
const char* DEFAULT_COLOR_SQUARE_BORDER = "#665C54";
const char* DEFAULT_COLOR_COLORMAP[] = {"#458588", "#689D6A", "#98971A", "#D79921", "#D65D0E"};

struct Config {
    int image_width = 1920;
    int image_height = 1080;
    int square_size = 14;
    int inner_margin = 10;
    int outer_margin = 10;
    std::string output_dir = ".";
    std::string output_filename = "image_with_colored_squares.png";
    std::string color_background = DEFAULT_COLOR_BACKGROUND;
    std::string color_empty_square = DEFAULT_COLOR_EMPTY_SQUARE;
    std::string color_square_border = DEFAULT_COLOR_SQUARE_BORDER;
    std::vector<std::string> color_colormap = {DEFAULT_COLOR_COLORMAP, DEFAULT_COLOR_COLORMAP + 5};
    std::vector<int> data_values;
};

void fill_empty_squares(CImg<unsigned char>& image, const Config& config, int& max_squares_x, int& max_squares_y, int& start_x, int& start_y) {
    int image_width = image.width();
    int image_height = image.height();
    int total_width = image_width - 2 * config.outer_margin;
    int total_height = image_height - 2 * config.outer_margin;

    max_squares_x = (total_width + config.inner_margin) / (config.square_size + config.inner_margin);
    max_squares_y = (total_height + config.inner_margin) / (config.square_size + config.inner_margin);

    int total_width_squares = max_squares_x * (config.square_size + config.inner_margin) - config.inner_margin;
    int total_height_squares = max_squares_y * (config.square_size + config.inner_margin) - config.inner_margin;

    start_x = config.outer_margin + (total_width - total_width_squares) / 2;
    start_y = config.outer_margin + (total_height - total_height_squares) / 2;

    unsigned char empty_square_color[3];
    sscanf(config.color_empty_square.c_str(), "#%02hhx%02hhx%02hhx", &empty_square_color[0], &empty_square_color[1], &empty_square_color[2]);
    unsigned char square_border_color[3];
    sscanf(config.color_square_border.c_str(), "#%02hhx%02hhx%02hhx", &square_border_color[0], &square_border_color[1], &square_border_color[2]);

    for (int row = 0; row < max_squares_y; ++row) {
        for (int col = 0; col < max_squares_x; ++col) {
            int x0 = start_x + col * (config.square_size + config.inner_margin);
            int y0 = start_y + row * (config.square_size + config.inner_margin);
            int x1 = x0 + config.square_size;
            int y1 = y0 + config.square_size;
            image.draw_rectangle(x0, y0, x1, y1, empty_square_color);
            image.draw_rectangle(x0, y0, x1, y1, square_border_color, 1.0f, ~0U); // Border
        }
    }
}

void color_middle_lines(CImg<unsigned char>& image, const Config& config, int start_x, int start_y, int max_squares_x, int max_squares_y) {
    int num_lines_to_color = config.data_values.size();

    if (max_squares_y < num_lines_to_color) {
        throw std::runtime_error("The image is too small to fit the required lines.");
    }

    int start_line = (max_squares_y - num_lines_to_color) / 2;

    for (size_t i = 0; i < config.data_values.size(); ++i) {
        int percentage = config.data_values[i];
        int capped_percentage = std::min(percentage, 100);
        int num_squares_to_color = (capped_percentage * max_squares_x) / 100;
        int row = start_line + i;

        unsigned char color[3];
        sscanf(config.color_colormap[i % config.color_colormap.size()].c_str(), "#%02hhx%02hhx%02hhx", &color[0], &color[1], &color[2]);

        for (int col = 0; col < num_squares_to_color; ++col) {
            int x0 = start_x + col * (config.square_size + config.inner_margin);
            int y0 = start_y + row * (config.square_size + config.inner_margin);
            int x1 = x0 + config.square_size;
            int y1 = y0 + config.square_size;
            image.draw_rectangle(x0, y0, x1, y1, color);
        }
    }
}

void print_usage(const char* prog_name) {
    std::cout << "Usage: " << prog_name << " [options] <data_values...>\n"
              << "Options:\n"
              << "  -w, --width WIDTH          Set the image width (default: 1920)\n"
              << "  -h, --height HEIGHT        Set the image height (default: 1080)\n"
              << "  -s, --square-size SIZE     Set the square size (default: 14)\n"
              << "  -i, --inner-margin MARGIN  Set the inner margin (default: 10)\n"
              << "  -o, --outer-margin MARGIN  Set the outer margin (default: 10)\n"
              << "  -b, --background COLOR     Set the background color (default: #32302F)\n"
              << "  -e, --empty-square COLOR   Set the empty square color (default: #504945)\n"
              << "  -r, --border COLOR         Set the square border color (default: #665C54)\n"
              << "  -c, --colormap COLORS      Set the colormap (comma-separated, default: #458588,#689D6A,#98971A,#D79921,#D65D0E)\n"
              << "  -d, --output-dir DIR       Set the output directory (default: .)\n"
              << "  -f, --output-file FILE     Set the output file name (default: image_with_colored_squares.png)\n"
              << "  -?, --help                 Show this help message\n";
}

int main(int argc, char* argv[]) {
    Config config;

    static struct option long_options[] = {
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"square-size", required_argument, 0, 's'},
        {"inner-margin", required_argument, 0, 'i'},
        {"outer-margin", required_argument, 0, 'o'},
        {"background", required_argument, 0, 'b'},
        {"empty-square", required_argument, 0, 'e'},
        {"border", required_argument, 0, 'r'},
        {"colormap", required_argument, 0, 'c'},
        {"output-dir", required_argument, 0, 'd'},
        {"output-file", required_argument, 0, 'f'},
        {"help", no_argument, 0, '?'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "w:h:s:i:o:b:e:r:c:d:f:?", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'w': config.image_width = std::stoi(optarg); break;
            case 'h': config.image_height = std::stoi(optarg); break;
            case 's': config.square_size = std::stoi(optarg); break;
            case 'i': config.inner_margin = std::stoi(optarg); break;
            case 'o': config.outer_margin = std::stoi(optarg); break;
            case 'b': config.color_background = optarg; break;
            case 'e': config.color_empty_square = optarg; break;
            case 'r': config.color_square_border = optarg; break;
            case 'c': {
                config.color_colormap.clear();
                std::string colors = optarg;
                size_t pos = 0;
                while ((pos = colors.find(',')) != std::string::npos) {
                    config.color_colormap.push_back(colors.substr(0, pos));
                    colors.erase(0, pos + 1);
                }
                config.color_colormap.push_back(colors);
                break;
            }
            case 'd': config.output_dir = optarg; break;
            case 'f': config.output_filename = optarg; break;
            case '?': print_usage(argv[0]); return 0;
            default: print_usage(argv[0]); return 1;
        }
    }

    for (int index = optind; index < argc; ++index) {
        config.data_values.push_back(std::stoi(argv[index]));
    }

    if (config.output_dir.empty() || config.output_filename.empty() || config.data_values.empty()) {
        std::cerr << "Output directory, output file name, and data values are required.\n";
        print_usage(argv[0]);
        return 1;
    }

    CImg<unsigned char> image(config.image_width, config.image_height, 1, 3, 0);
    unsigned char background_color[3];
    sscanf(config.color_background.c_str(), "#%02hhx%02hhx%02hhx", &background_color[0], &background_color[1], &background_color[2]);
    image.fill(background_color[0], background_color[1], background_color[2]);

    int max_squares_x, max_squares_y, start_x, start_y;

    try {
        fill_empty_squares(image, config, max_squares_x, max_squares_y, start_x, start_y);
        color_middle_lines(image, config, start_x, start_y, max_squares_x, max_squares_y);

        std::string output_path = config.output_dir + "/" + config.output_filename;
        image.save_png(output_path.c_str());
        //std::cout << "Image saved as " << output_path << std::endl;
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
