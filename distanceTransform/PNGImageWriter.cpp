/*
 *  PNGImageWriter.cpp
 *  LUTBasedNSDistanceTransform
 *  $Id: PNGImageWriter.cpp 108 2012-07-11 06:09:48Z Nicolas.Normand $
 */

#include "PNGImageWriter.h"

PNGImageWriter::PNGImageWriter(FILE* output, bool lineBuffered) :
_output(output),
_lineBuffered(lineBuffered) {
}

void PNGImageWriter::beginOfImage(int cols, int rows) {
    _png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    _info_ptr = png_create_info_struct(_png_ptr);
    if (!_info_ptr) {
        png_destroy_write_struct(&_png_ptr, NULL);
        exit(1);
    }

    png_set_IHDR(_png_ptr,
		 _info_ptr,
		 cols,
		 rows,
		 16,
		 PNG_COLOR_TYPE_GRAY,
		 PNG_INTERLACE_NONE,
		 NULL/*PNG_COMPRESSION_TYPE_DEFAULT*/,
		 PNG_FILTER_TYPE_DEFAULT);
    //png_set_filter(_png_ptr, 0, PNG_FILTER_NONE);
    //png_set_packing(_png_ptr);
    png_set_compression_level(_png_ptr, 0); // No compression
    png_set_filter(_png_ptr, 0 /* method */, PNG_NO_FILTERS);

    png_color_8 sig_bit;
    sig_bit.gray  = 16;
    sig_bit.red   = 0;
    sig_bit.green = 0;
    sig_bit.blue  = 0;
    sig_bit.alpha = 0;
    png_set_sBIT(_png_ptr, _info_ptr, &sig_bit);
    //png_set_shift(_png_ptr, &sig_bit);

    //png_set_gamma(_png_ptr, 1., 1.);
    png_set_strip_16(_png_ptr);

    //if (lsb)
    //png_set_swap(_png_ptr);

    // flush output after every row
    if (_lineBuffered)
	png_set_flush(_png_ptr, 1);

    //png_set_filler(_png_ptr, 0, PNG_FILLER_BEFORE);

    // Represent black as 1 and white as 0 instead of the PNG default.
    //png_set_invert_mono(_png_ptr);

    png_init_io(_png_ptr, _output);

    /* write the file information */
    png_write_info(_png_ptr, _info_ptr);
}

void PNGImageWriter::processRow(const GrayscalePixelType* inputRow) {
    if (setjmp(png_jmpbuf(_png_ptr))) {
	png_destroy_write_struct(&_png_ptr, &_info_ptr);
	fprintf(stderr, "Error during PNGImageWriter::processRow\n");
	exit(1);
    }

    png_write_row(_png_ptr, (png_byte*) inputRow);
    //png_write_flush(_png_ptr);
}

void PNGImageWriter::endOfImage() {
    png_write_end(_png_ptr, _info_ptr);
    png_destroy_write_struct(&_png_ptr, &_info_ptr);
}