/**
 * @file bitmap.h
 *
 * @author Kevin Buffardi
 * @author Joshua Petrin
 */

#pragma once

#include <vector>

#include "../mpc_fs.hpp"

namespace mpc::lcdgui
{

    /**
     * @brief Represents a single Pixel in the image.
     *
     * A Pixel has a single bit value that is a zero (white) or a one (black).
     * So, this class essentially encapsulates a 2D array of bools.
     **/
    struct Pixel
    {
        /// Stores on/off status.
        bool on;

        /// Initializes a Pixel with a default off.
        Pixel() : on(false) {}

        /// Initializes a color Pixel with the specified on/off value.
        Pixel(bool set) : on(set) {}
    };

    /// To abbreviate a pixel matrix built as a vector of vectors
    typedef std::vector<std::vector<Pixel>> PixelMatrix;

    /**
     * @brief Represents a bitmap where a grid of pixels (in row-major order)
     * describes the color of each pixel within the image.
     * Functionality limited to Windows BMP formatted images with no compression
     * and 24 bit color depth.
     **/
    class Bitmap
    {
    private:
        PixelMatrix pixels;

    public:
        void open(fs::path p);

        /**
         * @brief Validates whether or not the current matrix of pixels
         * represents a proper image.
         *
         * Tests for non-zero-size rows and consistent non-zero-size columns for
         * each row.
         *
         * @return boolean value of whether or not the matrix is a valid image
         **/
        bool isImage() const;

        /**
         * @brief Provides a vector of vector of pixels representing the bitmap
         *
         * @return the bitmap image, represented by a matrix of RGB pixels
         **/
        PixelMatrix toPixelMatrix() const;

        /**
         * @brief Overwrites the current bitmap with that represented by a
         * matrix of pixels.
         *
         * Does _not_ validate that the new matrix of pixels is a proper image.
         *
         * @param m  a matrix of pixels to represent a bitmap
         **/
        void fromPixelMatrix(const PixelMatrix &m);

        void openFromData(char *data, const int size);
    };
} // namespace mpc::lcdgui
