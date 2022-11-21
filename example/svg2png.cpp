#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <sstream>

#include <unistd.h>
#include <stdlib.h>
#include <filesystem>
#include <fstream>

#include <lunasvg.h>

using namespace lunasvg;

int help(int exit_code)
{
    std::cout << "Usage: \n   svg2png [filename] [resolution] [bgColor]\n\nExamples: \n    $ svg2png input.svg\n    $ svg2png input.svg 512x512\n    $ svg2png input.svg 512x512 0xff00ffff\n\n";
    return exit_code;
}

bool setup(int argc, char** argv, std::string& filename, std::uint32_t& width, std::uint32_t& height, std::uint32_t& bgColor)
{
    if(argc > 1) filename.assign(argv[1]);
    if(argc > 2)
    {
        std::stringstream ss;

        ss << argv[2];
        ss >> width;

        if(ss.fail() || ss.get() != 'x')
            return false;

        ss >> height;
    }

    if(argc > 3)
    {
        std::stringstream ss;

        ss << std::hex << argv[3];
        ss >> std::hex >> bgColor;
    }

    return true;
}

int main(int argc, char** argv)
{
    std::string filename;
    std::uint32_t width = 0, height = 0;
    std::uint32_t bgColor = 0x00000000;
    if(!setup(argc, argv, filename, width, height, bgColor)) return help(1);

    std::uintmax_t size = std::filesystem::file_size(argv[1]);
    std::fstream f1;
    f1.open(argv[1], std::ios::in | std::ios::binary);
    char* buffer = (char*)calloc(size, sizeof(char));
    f1.read(buffer, size);
    f1.close();

    auto document = Document::loadFromData(buffer, size);
    if (!document){
        std::cout << "Document::loadFromFile failed" << std::endl;
        return help(2);
    }

    auto bitmap = document->renderToBitmap(width, height, bgColor);
    if(!bitmap.valid()){
        std::cout << "document->renderToBitmap failed" << std::endl;
        return help(3);
    } 

    auto basename = filename.substr(filename.find_last_of("/\\") + 1);
    basename.append(".png");

    bitmap.convertToRGBA();
    stbi_write_png(basename.c_str(), int(bitmap.width()), int(bitmap.height()), 4, bitmap.data(), 0);

    std::cout << "Generated PNG file : " << basename << std::endl;

    return 0;
}
