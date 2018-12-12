#ifndef _WIC_UTILITY_
#define _WIC_UTILITY_

#include <wincodec.h>
#include "error_message.h"

namespace WIC_UTILITY
{
	using std::basic_string;

	struct WicImagingFactoryObject
	{
		WicImagingFactoryObject() noexcept;
		WicImagingFactoryObject(const WicImagingFactoryObject & rhs) = delete;
		WicImagingFactoryObject(WicImagingFactoryObject && rhs) noexcept;
		WicImagingFactoryObject & operator= (const WicImagingFactoryObject & rhs) = delete;
		WicImagingFactoryObject & operator= (WicImagingFactoryObject && rhs) noexcept;
		~WicImagingFactoryObject() noexcept;

		operator IWICImagingFactory *() noexcept;
		operator const IWICImagingFactory *() const noexcept;
		IWICImagingFactory & operator *() noexcept;
		const IWICImagingFactory & operator *() const noexcept;
		IWICImagingFactory * operator ->() noexcept;
		const IWICImagingFactory * operator ->() const noexcept;

		IWICImagingFactory * wic_image_factory;
	};

	struct WicBitmapDecoderObject
	{
		WicBitmapDecoderObject(const basic_string<TCHAR> & file_name, IWICImagingFactory * wic_image_factory) noexcept;
		WicBitmapDecoderObject() noexcept;
		WicBitmapDecoderObject(const WicBitmapDecoderObject & rhs) = delete;
		WicBitmapDecoderObject(WicBitmapDecoderObject && rhs) noexcept;
		WicBitmapDecoderObject & operator= (const WicBitmapDecoderObject & rhs) = delete;
		WicBitmapDecoderObject & operator= (WicBitmapDecoderObject && rhs) noexcept;
		~WicBitmapDecoderObject() noexcept;

		operator IWICBitmapDecoder *() noexcept;
		operator const IWICBitmapDecoder *() const noexcept;
		IWICBitmapDecoder & operator *() noexcept;
		const IWICBitmapDecoder & operator *() const noexcept;
		IWICBitmapDecoder * operator ->() noexcept;
		const IWICBitmapDecoder * operator ->() const noexcept;

		IWICBitmapDecoder * wic_bitmap_decoder;
	};

	struct WicBitmapFrameDecodeObject
	{
		WicBitmapFrameDecodeObject(IWICBitmapDecoder * wic_bitmap_decoder) noexcept;
		WicBitmapFrameDecodeObject() noexcept;
		WicBitmapFrameDecodeObject(const WicBitmapFrameDecodeObject & rhs) = delete;
		WicBitmapFrameDecodeObject(WicBitmapFrameDecodeObject && rhs) noexcept;
		WicBitmapFrameDecodeObject & operator= (const WicBitmapFrameDecodeObject & rhs) = delete;
		WicBitmapFrameDecodeObject & operator= (WicBitmapFrameDecodeObject && rhs) noexcept;
		~WicBitmapFrameDecodeObject() noexcept;

		operator IWICBitmapFrameDecode *() noexcept;
		operator const IWICBitmapFrameDecode *() const noexcept;
		IWICBitmapFrameDecode & operator *() noexcept;
		const IWICBitmapFrameDecode & operator *() const noexcept;
		IWICBitmapFrameDecode * operator ->() noexcept;
		const IWICBitmapFrameDecode * operator ->() const noexcept;

		IWICBitmapFrameDecode * wic_bitmap_frame_decode;
	};

};

#endif