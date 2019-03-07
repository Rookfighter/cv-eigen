/* pgm.h
 *
 * Author: Fabian Meyer
 * Created On: 03 Mar 2019
 */
#ifndef CVE_IMAGEIO_PGM_H_
#define CVE_IMAGEIO_PGM_H_

#include <string>
#include <fstream>
#include "cve/image.h"

namespace cve
{
namespace pgm
{
    void load(const std::string &filename, ImageGray &img)
    {
        std::ifstream is(filename);

        if(is.bad() || is.fail())
            throw std::runtime_error("cannot open " + filename);

        // find start signature "P5"
        while(!is.eof() && is.get() != 'P');

        if(is.eof() || is.get() != '5')
            throw std::runtime_error(filename + ": missing PGM start sequence");

        int value = is.get();
        while(!is.eof() && value != ' ' && value != '\n')
            value = is.get();

        // check if line is a comment
        value = is.get();
        while(!is.eof() && value == '#')
        {
            // search for end of line
            while(is.get() != '\n');
            // update value to first char of line
            value = is.get();
        }


        // trim all empty lines
        while(!is.eof() && value == '\n')
            value = is.get();

        ImageGray::Index width = 0;
        while(!is.eof() && value >= 48 && value < 58)
        {
            width = 10 * width + value - 48;
            value = is.get();
        }

        // find next valid sequence
        while(!is.eof() && (value < 48 || value >= 58))
            value = is.get();

        ImageGray::Index height = 0;
        while(!is.eof() && value >= 48 && value < 58)
        {
            height = 10 * height + value - 48;
            value = is.get();
        }

        while(!is.eof() && value != ' ' && value != '\n')
            value = is.get();

        value = is.get();
        while(!is.eof() && (value < 48 || value >= 58))
            value = is.get();

        // find next line
        while(is.eof() && value != '\n')
            value = is.get();

        if(is.eof())
            throw std::runtime_error(filename + ": incomplete PGM file");

        if(width <= 0 || height <= 0)
            throw std::runtime_error(filename + ": invalid PGM image size");
        img.resize(height, width);

        // read image data, data is stored in row major format
        for(ImageGray::Index row = 0; row < height; ++row)
        {
            for(ImageGray::Index col = 0; col < width; ++col)
            {
                img(row, col)(0) = is.get();
            }
        }
    }

    void save(const std::string &filename, const ImageGray &img)
    {
        std::ofstream os(filename);

        os << "P5\n"
            << img.cols() << ' '
            << img.rows() << '\n'
            << "255" << '\n';

        for(ImageGray::Index row = 0; row < img.rows(); ++row)
        {
            for(ImageGray::Index col = 0; col < img.cols(); ++col)
            {
                os << img(row, col)(0);
            }
        }
    }
}
}

#endif