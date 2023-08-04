/**
 * @file bitmap.cpp
 *
 * @author Kevin Buffardi, Ph.D
 * @author Joshua Petrin
 */

#include <memory>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <cmath>

#include "bitmap.hpp"
#include <file/FileUtil.hpp>

using namespace mpc::lcdgui;

typedef unsigned char uchar_t;  ///< Ensure only positive parsing

const int BMP_MAGIC_ID = 2;  ///< Length in bytes of the file identifier.


const uint8_t MONO_R_VAL_ON = 0;  ///< ON red val
const uint8_t MONO_G_VAL_ON = 0;  ///< ON green val
const uint8_t MONO_B_VAL_ON = 0;  ///< ON blue val

const uint8_t MONO_R_VAL_OFF = 255;  ///< OFF red val
const uint8_t MONO_G_VAL_OFF = 255;  ///< OFF green val
const uint8_t MONO_B_VAL_OFF = 255;  ///< OFF blue val

/// Windows BMP-specific format data
struct bmpfile_magic
{
    uchar_t magic[BMP_MAGIC_ID];  ///< 'B' and 'M'
};

/**
 * Generic 14-byte bitmap header
 */
struct bmpfile_header
{
    uint32_t file_size;  ///< The number of bytes in the bitmap file.
    uint16_t creator1;   ///< Two bytes reserved.
    uint16_t creator2;   ///< Two bytes reserved.
    uint32_t bmp_offset; ///< Offset from beginning to bitmap bits.
};

/**
 * @brief Mircosoft's defined header structure for Bitmap version 3.x.
 *
 * https://msdn.microsoft.com/en-us/library/dd183376%28v=vs.85%29.aspx
 */
struct bmpfile_dib_info
{
  uint32_t header_size;           ///< The size of this header.
  int32_t  width;                 ///< Width of the image, in pixels.
  int32_t  height;                ///< Height of the image, in pixels.
  uint16_t num_planes;            ///< Number of planes. Almost always 1.
  uint16_t bits_per_pixel;        ///< Bits per pixel. Can be 0, 1, 4, 8, 16, 24, or 32.
  uint32_t compression;           ///< https://msdn.microsoft.com/en-us/library/cc250415.aspx
  uint32_t bmp_byte_size;         ///< The size of the image in bytes.
  int32_t  hres;                  ///< Horizontal resolution, pixels/meter
  int32_t  vres;                  ///< Vertical resolution, pixels/meter
  uint32_t num_colors;            ///< The number of color indices used in the color table.
  uint32_t num_important_colors;  ///< The number of colors used by the bitmap.
};

/**
 * @brief The color table for the monochrome image palette.
 *
 * Whatever 24-bit color is specified in the palette in the BMP will show up in
 * the actual image.
 */
struct bmpfile_color_table
{
    // I discovered on my system that the entire RGB num in the palette is
    // parsed as little-endian. Not sure if this is the same on all systems,
    // but here, the colors are in reverse order.
    uint8_t blue;     ///< Blue component
    uint8_t green;    ///< Green component
    uint8_t red;      ///< Red component
    uint8_t reserved; ///< Should be 0.
};

void Bitmap::openFromData(char* data, const int size)
{
    size_t pos = 0;
    // Check to make sure that the first two bytes of the file are the "BM"
    // identifier that identifies a bitmap image.
    if (data[0] != 'B' || data[1] != 'M')
    {
        std::cout << "BMP data is not in proper BMP format; it does "
                              << "not begin with the magic bytes!\n";
    }
    else
    {
        pos += 2;
        
        bmpfile_header header;
        for (auto i = 0; i < sizeof(header); i++)
            ((char*)(&header))[i] = data[pos + i];
        
        pos += sizeof(header);
        
        bmpfile_dib_info dib_info;
        for (auto i = 0; i < sizeof(dib_info); i++)
            ((char*)(&dib_info))[i] = data[pos + i];
        
        pos += sizeof(dib_info);
        
        bmpfile_color_table color1;
        for (auto i = 0; i < sizeof(color1); i++)
            ((char*)(&color1))[i] = data[pos + i];
        
        pos += sizeof(color1);

        bmpfile_color_table color2;
        for (auto i = 0; i < sizeof(color2); i++)
            ((char*)(&color2))[i] = data[pos + i];
        
        pos += sizeof(color2);

        if (dib_info.bits_per_pixel != 1)
        {
            std::cout << "BMP data uses " << dib_info.bits_per_pixel
                      << " bits per pixel (bit depth). This implementation"
                      << " of Bitmap only supports 1-bit (monochrome)."
                      << std::endl;
        }
        // No support for compressed images
        else if (dib_info.compression != 0)
        {
            std::cout << "BMP data is compressed. "
                      << "Bitmap only supports uncompressed images."
                      << std::endl;
        }
        // Check for the reserved bits in the color palette
        else if (color1.reserved != 0)
        {
            std::cout << "BMP data does not have a good color palette"
                      << " for monochrome display;"
                      << " its first reserved bits are not 0."
                      << std::endl;
        }
        else if (color2.reserved != 0)
        {
            std::cout << "BMP data does not have a good color palette"
                      << " for monochrome display;"
                      << " its second reserved bits are not 0."
                      << std::endl;
        }
        else  // All clear! Bitmap is (probably) in proper format.
        {
            // clear the Pixel vector if already holds information
            for(int i = 0; i < pixels.size(); ++i)
            {
                pixels[i].clear();
            }
            pixels.clear();

            // Check for this here and so that we know later whether we
            // need to insert each row at the bottom or top of the image.
            bool flip = true;
            
            if (dib_info.height < 0)
            {
                flip = false;
                dib_info.height = -dib_info.height;
            }

            pos = header.bmp_offset;


            // The number of bytes in a row of pixels
            int row_bytes = 0;
            // All but the last byte
            row_bytes += dib_info.width / 8;
            // Is there a last byte?
            row_bytes += (dib_info.width % 8 != 0)? 1 : 0;
            // Rows are padded so that they're always a multiple of 4 bytes
            row_bytes += (row_bytes % 4 == 0)? 0 : (4 - row_bytes%4);
            

            std::unique_ptr<char[]> row_data(new char[row_bytes]);

            // Transcribe Pixels from the image.
            for (int row = 0; row < dib_info.height; ++row)
            {
                std::vector<Pixel> row_pixels;
                bool high;
                
                for (auto i = 0; i < row_bytes; i++)
                    row_data.get()[i] = data[pos + i];
                
                pos += row_bytes;
                
                // In a monochrome image, each bit is a pixel.
                // First we cover all bits except the ones in the last byte.
                for (int col = 0; col < dib_info.width / 8; ++col)
                {
                    for (int bit = 7; bit >= 0; --bit)
                    {
                        high = ((row_data.get()[col] & (1 << bit)) != 0);
                        row_pixels.push_back(Pixel(high));
                    }
                }

                // Then we cover the bits we missed at the end.
                for (int rev_bit = 0;
                    rev_bit < dib_info.width % 8;
                    ++rev_bit)
                {
                    high = (row_data.get()[dib_info.width/8]
                        & (1 << (7 - rev_bit))) != 0;
                    row_pixels.push_back(Pixel(high));
                }

                if (flip)
                    pixels.insert(pixels.begin(), row_pixels);
                else
                    pixels.push_back(row_pixels);
            }
        }
    }//end else (is an image)
}

void Bitmap::open(std::string filename)
{
    auto file = moduru::file::FileUtil::fopenw(filename, "r");

    if (!file)
    {
        std::cout << filename << " could not be opened. Does it exist? "
                  << "Is it already open by another program?\n";
    }
    else
    {
        bmpfile_magic magic;
        fread(magic.magic, sizeof(char), BMP_MAGIC_ID, file);
        
        // Check to make sure that the first two bytes of the file are the "BM"
        // identifier that identifies a bitmap image.
        if (magic.magic[0] != 'B' || magic.magic[1] != 'M')
        {
            std::cout << filename << " is not in proper BMP format; it does "
                                  << "not begin with the magic bytes!\n";
        }
        else
        {
            // Read the file headers
            bmpfile_header header;
            fread(&header.file_size, sizeof(header.file_size), 1, file);
            fread(&header.creator1, sizeof(header.creator1), 1, file);
            fread(&header.creator2, sizeof(header.creator2), 1, file);
            fread(&header.bmp_offset, sizeof(header.bmp_offset), 1, file);

            bmpfile_dib_info dib_info;

            fread(&dib_info.header_size, sizeof(dib_info.header_size), 1, file);
            fread(&dib_info.width, sizeof(dib_info.width), 1, file);
            fread(&dib_info.height, sizeof(dib_info.height), 1, file);
            fread(&dib_info.num_planes, sizeof(dib_info.num_planes), 1, file);
            fread(&dib_info.bits_per_pixel, sizeof(dib_info.bits_per_pixel), 1, file);
            fread(&dib_info.compression, sizeof(dib_info.compression), 1, file);
            fread(&dib_info.bmp_byte_size, sizeof(dib_info.bmp_byte_size), 1, file);
            fread(&dib_info.hres, sizeof(dib_info.hres), 1, file);
            fread(&dib_info.vres, sizeof(dib_info.vres), 1, file);
            fread(&dib_info.num_colors, sizeof(dib_info.num_colors), 1, file);
            fread(&dib_info.num_important_colors, sizeof(dib_info.num_important_colors), 1, file);

            // Read the 2-color palette for monochrome
            bmpfile_color_table color1;
            fread(&color1.blue, sizeof(color1.blue), 1, file);
            fread(&color1.green, sizeof(color1.green), 1, file);
            fread(&color1.red, sizeof(color1.red), 1, file);
            fread(&color1.reserved, sizeof(color1.reserved), 1, file);

            bmpfile_color_table color2;
            fread(&color2.blue, sizeof(color2.blue), 1, file);
            fread(&color2.green, sizeof(color2.green), 1, file);
            fread(&color2.red, sizeof(color2.red), 1, file);
            fread(&color2.reserved, sizeof(color2.reserved), 1, file);

            // Only support for 1-bit images
            if (dib_info.bits_per_pixel != 1)
            {
                std::cout << filename << " uses " << dib_info.bits_per_pixel
                          << " bits per pixel (bit depth). This implementation"
                          << " of Bitmap only supports 1-bit (monochrome)."
                          << std::endl;
            }
            // No support for compressed images
            else if (dib_info.compression != 0)
            {
                std::cout << filename << " is compressed. "
                          << "Bitmap only supports uncompressed images."
                          << std::endl;
            }
            // Check for the reserved bits in the color palette
            else if (color1.reserved != 0)
            {
                std::cout << filename << " does not have a good color palette"
                          << " for monochrome display;"
                          << " its first reserved bits are not 0."
                          << std::endl;
            }
            else if (color2.reserved != 0)
            {
                std::cout << filename << " does not have a good color palette"
                          << " for monochrome display;"
                          << " its second reserved bits are not 0."
                          << std::endl;
            }
            else  // All clear! Bitmap is (probably) in proper format.
            {
                // clear the Pixel vector if already holds information
                for(int i = 0; i < pixels.size(); ++i)
                {
                    pixels[i].clear();
                }
                pixels.clear();

                // Check for this here and so that we know later whether we
                // need to insert each row at the bottom or top of the image.
                bool flip = true;
                if (dib_info.height < 0)
                {
                    flip = false;
                    dib_info.height = -dib_info.height;
                }

                // Move to the data
                fseek(file, header.bmp_offset, SEEK_SET);

                // The number of bytes in a row of pixels
                int row_bytes = 0;
                // All but the last byte
                row_bytes += dib_info.width / 8;
                // Is there a last byte?
                row_bytes += (dib_info.width % 8 != 0)? 1 : 0;
                // Rows are padded so that they're always a multiple of 4 bytes
                row_bytes += (row_bytes % 4 == 0)? 0 : (4 - row_bytes%4);

                std::unique_ptr<char[]> row_data(new char[row_bytes]);

                // Transcribe Pixels from the image.
                for (int row = 0; row < dib_info.height; ++row)
                {
                    std::vector<Pixel> row_pixels;
                    bool high;

                    fread(row_data.get(), sizeof(char), row_bytes, file);

                    // In a monochrome image, each bit is a pixel.
                    // First we cover all bits except the ones in the last byte.
                    for (int col = 0; col < dib_info.width / 8; ++col)
                    {
                        for (int bit = 7; bit >= 0; --bit)
                        {
                            high = ((row_data.get()[col] & (1 << bit)) != 0);
                            row_pixels.emplace_back(high);
                        }
                    }

                    // Then we cover the bits we missed at the end.
                    for (int rev_bit = 0;
                        rev_bit < dib_info.width % 8;
                        ++rev_bit)
                    {
                        high = (row_data.get()[dib_info.width/8]
                            & (1 << (7 - rev_bit))) != 0;
                        row_pixels.emplace_back(high);
                    }

                    if (flip)
                        pixels.insert(pixels.begin(), row_pixels);
                    else
                        pixels.push_back(row_pixels);
                }
            }

            fclose(file);
        }//end else (is an image)
    }//end else (can open file)
}


void Bitmap::save(std::string filename) const
{
    std::ofstream file(filename.c_str(), std::ios::out | std::ios::binary);

    if (file.fail())
    {
        std::cout << filename << " could not be opened for editing. "
                  << "Is it already open by another program, "
                  << "or is it read-only?"
                  << std::endl;
        return;
    }
    else if(!isImage())
    {
        std::cout << "Bitmap cannot be saved. It is not a valid image."
                  << std::endl;
    }
    else
    {
        // Write all the header information that the BMP file format requires.
        bmpfile_magic magic;
        magic.magic[0] = 'B';
        magic.magic[1] = 'M';
        file.write((char*)(&magic), sizeof(magic));

        bmpfile_header header = { 0 };
        header.bmp_offset = sizeof(bmpfile_magic) + sizeof(bmpfile_header)
                + sizeof(bmpfile_dib_info) + 2*sizeof(bmpfile_color_table);
        
        // TODO: vv These lines are lazy and bad.
        int bytes_per_row = 0;
        for (int i = 0; i < pixels[0].size(); i += 32)
            bytes_per_row += 4;
        header.file_size = header.bmp_offset + bytes_per_row * pixels.size();

        file.write((char*)(&header), sizeof(header));
        bmpfile_dib_info dib_info = { 0 };
        dib_info.header_size = sizeof(bmpfile_dib_info);
        dib_info.width = pixels[0].size();
        dib_info.height = pixels.size();
        dib_info.num_planes = 1;
        dib_info.bits_per_pixel = 1;  // monochrome
        dib_info.compression = 0;
        dib_info.bmp_byte_size = 0;
        dib_info.hres = 200;
        dib_info.vres = 200;
        dib_info.num_colors = 2;
        dib_info.num_important_colors = 0;
        file.write((char*)(&dib_info), sizeof(dib_info));

        // Color palettes.
        // First is the '0' color...
        bmpfile_color_table off_color;
        off_color.red = MONO_R_VAL_OFF;
        off_color.green = MONO_G_VAL_OFF;
        off_color.blue = MONO_B_VAL_OFF;
        off_color.reserved = 0;
        file.write((char*)(&off_color), sizeof(off_color));
        // ...then the '1' color
        bmpfile_color_table on_color;
        on_color.red = MONO_R_VAL_ON;
        on_color.green = MONO_G_VAL_ON;
        on_color.blue = MONO_B_VAL_ON;
        on_color.reserved = 0;
        file.write((char*)(&on_color), sizeof(on_color));


        // Write each row and column of Pixels into the image file -- we write
        // the rows upside-down to satisfy the easiest BMP format.
        for (int row = pixels.size() - 1; row >= 0; --row)
        {
            const std::vector<Pixel>& row_data = pixels[row];

            int bytes_written = 0;
            int bit = 7;
            char next_byte = '\0';

            for (int col = 0; col < row_data.size(); col++)
            {
                next_byte += (row_data[col].on)? (1 << bit) : 0;

                // file.put((uchar_t)(pix.blue));
                // file.put((uchar_t)(pix.green));
                // file.put((uchar_t)(pix.red));

                if (bit > 0)
                {
                    --bit;
                }
                else
                {
                    file.put(next_byte);
                    ++bytes_written;
                    bit = 7;
                    next_byte = '\0';
                }
            }

            if (row_data.size() % 8 != 0)
            {
                file.put(next_byte);
                ++bytes_written;
            }

            // Rows are padded so that they're always a multiple of 4
            // bytes. This line skips the padding at the end of each row.
            for (int i = 0; i < 4 - bytes_written % 4; i++)
            {
                file.put(0);
            }
        }
    }

    file.close();
}
    

bool Bitmap::isImage() const
{
    const int height = pixels.size();

    if (height == 0 || pixels[0].size() == 0)
    {
        return false;
    }

    const int width = pixels[0].size();

    for (int row = 0; row < height; row++)
    {
        if (pixels[row].size() != width)
            return false;
    }
    return true;
}


PixelMatrix Bitmap::toPixelMatrix() const
{
    if( isImage() )
    {
        return pixels;
    }
    else
    {
        return PixelMatrix();
    }
}


void Bitmap::fromPixelMatrix(const PixelMatrix & values)
{
    pixels = values;
}
