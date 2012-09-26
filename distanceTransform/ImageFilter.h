/*
 * Interface to ImageFilter class
 * An image filter receives image rows one at a time, processes it, and forwards
 * the result to the following filter (or image consumer).
 */

// $Id: ImageFilter.h 112 2012-07-11 07:58:28Z Nicolas.Normand $

#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include <sys/types.h>

#include <assert.h>
#include <algorithm>
#include <iostream>

#include "LUTBasedNSDistanceTransformConfig.h"

void freerow(void *);

/**
 * An ImageConsumer accepts pixels one row at a time in the standard scan order.
 * A call to beginOfImage() initialises the ImageConsumer,
 * followed by as many calls to processRow() as the number of rows in the image
 * and a final call to endOfImage().
 */
template <typename inputPixelType>
class ImageConsumer {
public:
    virtual void processRow(const inputPixelType* inputRow) = 0;
    virtual void beginOfImage(int cols, int rows) = 0;
    virtual void endOfImage() = 0;
};

/**
 * A RowImageProducer generates image data row by row. The data is pushed to the
 * ImageConsumer attached to it.
 */
template <typename outputPixelType>
class RowImageProducer {
public:
    RowImageProducer(ImageConsumer<outputPixelType>* consumer) : _consumer(consumer) {
	if (_consumer == NULL) {
	    exit(1);
	}
    }
    bool hasMoreRows();
    void produceRow();
    void produceAllRows();
    virtual ~RowImageProducer() { delete _consumer; }
protected:
    ImageConsumer<outputPixelType>* _consumer;
};

/**
 * An ImageFilter is an image consumer and producer.
 * It redirects calls to beginOfImage(), processRow() and endOfImage() to the
 * next ImageConsumer (optionally modifying the size and content of the image).
 */
template <typename inputPixelType, typename outputPixelType>
class ImageFilter: public ImageConsumer<inputPixelType> {
public:
    ImageFilter(ImageConsumer<outputPixelType>* consumer) : _consumer(consumer) {
	if (_consumer == NULL) {
	    exit(1);
	}
    }
    virtual ~ImageFilter() { delete _consumer; }
    void beginOfImage(int cols, int rows) {_consumer->beginOfImage(cols, rows);}
    void endOfImage() {_consumer->endOfImage();}
protected:
    ImageConsumer<outputPixelType>* _consumer;
};

/**
 * A TeeImageFilter forwards the image data to two ImageConsumer intances
 * instead of one.
 */
template <typename inputPixelType, typename outputPixelType>
class TeeImageFilter: public ImageFilter<inputPixelType, outputPixelType> {
public:
    typedef ImageFilter<inputPixelType, outputPixelType> super;

    TeeImageFilter(ImageConsumer<inputPixelType>* consumer, ImageConsumer<inputPixelType>* consumer2) :
	ImageFilter<inputPixelType, outputPixelType>(consumer),
	_consumer2(consumer2) {}
    ~TeeImageFilter() { delete _consumer2; }

    void beginOfImage(int cols, int rows) {
	super::beginOfImage(cols, rows);

	if (_consumer2) {
	    _consumer2->beginOfImage(cols, rows);
	}
    }

    void processRow(const outputPixelType* inputRow);
    /*
    void processRow(const outputPixelType* inputRow) {
	_consumer->processRow(inputRow);

	if (_consumer2) {
	    _consumer2->processRow(inputRow);
	}
    }*/

    void endOfImage() {
	super::endOfImage();

	if (_consumer2) {
	    _consumer2->endOfImage();
	}
    }
protected:
    ImageConsumer<outputPixelType>* _consumer2;
};

#endif