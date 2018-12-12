#include"wic_utility.h"
#include "my_assert.h"

using namespace WIC_UTILITY;
using namespace ERROR_MESSAGE;
using std::move;
using std::swap;

WicImagingFactoryObject::WicImagingFactoryObject() noexcept
{
	my_assert(CoInitialize(nullptr) == S_OK);
	my_assert
	(
		CoCreateInstance
		(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wic_image_factory)
		) == S_OK
	);
}
WicImagingFactoryObject::WicImagingFactoryObject(WicImagingFactoryObject && rhs) noexcept
	:wic_image_factory(move(rhs.wic_image_factory))
{
	rhs.wic_image_factory = nullptr;
}

WicImagingFactoryObject & WicImagingFactoryObject::operator= (WicImagingFactoryObject && rhs) noexcept
{
	if (&rhs != this)
	{
		swap(wic_image_factory, rhs.wic_image_factory);
	}
	return *this;
}

WicImagingFactoryObject::~WicImagingFactoryObject() noexcept
{
	wic_image_factory->Release();
}

WicImagingFactoryObject::operator IWICImagingFactory *() noexcept { return wic_image_factory; }
WicImagingFactoryObject::operator const IWICImagingFactory *() const noexcept { return wic_image_factory; }
IWICImagingFactory & WicImagingFactoryObject::operator *() noexcept { return *wic_image_factory; }
const IWICImagingFactory & WicImagingFactoryObject::operator *() const noexcept { return *wic_image_factory; }
IWICImagingFactory * WicImagingFactoryObject::operator ->() noexcept { return wic_image_factory; }
const IWICImagingFactory * WicImagingFactoryObject::operator ->() const noexcept { return wic_image_factory; }

WicBitmapDecoderObject::WicBitmapDecoderObject(const basic_string<TCHAR> & file_name, IWICImagingFactory * wic_image_factory) noexcept
{
	if (
		wic_image_factory->CreateDecoderFromFilename
		(
			file_name.c_str(),						// Image to be decoded
			NULL,                           // Do not prefer a particular vendor
			GENERIC_READ,                   // Desired read access to the file
			WICDecodeMetadataCacheOnDemand, // Cache metadata when needed
			&wic_bitmap_decoder             // Pointer to the decoder
		) != S_OK
		)
	{
		auto error_message = SystemErrorMessageObject().message_string + TEXT("File Name: ") + file_name;
		MessageBox(NULL, error_message.c_str(), TEXT("WIC Error"), MB_OK);
		exit(-1);
	}
}
WicBitmapDecoderObject::WicBitmapDecoderObject() noexcept
	: wic_bitmap_decoder(nullptr)
{}

WicBitmapDecoderObject::WicBitmapDecoderObject(WicBitmapDecoderObject && rhs) noexcept
	: wic_bitmap_decoder(rhs.wic_bitmap_decoder)
{
	rhs.wic_bitmap_decoder = nullptr;
}

WicBitmapDecoderObject & WicBitmapDecoderObject::operator= (WicBitmapDecoderObject && rhs) noexcept
{
	if (&rhs != this)
	{
		swap(wic_bitmap_decoder, rhs.wic_bitmap_decoder);
	}
	return *this;
}

WicBitmapDecoderObject::~WicBitmapDecoderObject() noexcept
{
	if (wic_bitmap_decoder)
	{
		wic_bitmap_decoder->Release();
	}
}

WicBitmapDecoderObject::operator IWICBitmapDecoder *() noexcept { return wic_bitmap_decoder; }
WicBitmapDecoderObject::operator const IWICBitmapDecoder *() const noexcept { return wic_bitmap_decoder; }
IWICBitmapDecoder & WicBitmapDecoderObject::operator *() noexcept { return *wic_bitmap_decoder; }
const IWICBitmapDecoder & WicBitmapDecoderObject::operator *() const noexcept { return *wic_bitmap_decoder; }
IWICBitmapDecoder * WicBitmapDecoderObject::operator ->() noexcept { return wic_bitmap_decoder; }
const IWICBitmapDecoder * WicBitmapDecoderObject::operator ->() const noexcept { return wic_bitmap_decoder; }



WicBitmapFrameDecodeObject::WicBitmapFrameDecodeObject(IWICBitmapDecoder * wic_bitmap_decoder) noexcept
{
	my_assert(wic_bitmap_decoder->GetFrame(0, &wic_bitmap_frame_decode) == S_OK);
}
WicBitmapFrameDecodeObject::WicBitmapFrameDecodeObject() noexcept
	: wic_bitmap_frame_decode(nullptr)
{}
WicBitmapFrameDecodeObject::WicBitmapFrameDecodeObject(WicBitmapFrameDecodeObject && rhs) noexcept
	: wic_bitmap_frame_decode(rhs.wic_bitmap_frame_decode)
{
	rhs.wic_bitmap_frame_decode = nullptr;
}
WicBitmapFrameDecodeObject & WicBitmapFrameDecodeObject::operator= (WicBitmapFrameDecodeObject && rhs) noexcept
{
	if (&rhs != this)
	{
		swap(wic_bitmap_frame_decode, rhs.wic_bitmap_frame_decode);
	}
	return *this;
}

WicBitmapFrameDecodeObject::~WicBitmapFrameDecodeObject() noexcept
{
	if (wic_bitmap_frame_decode)
	{
		wic_bitmap_frame_decode->Release();
	}
}

WicBitmapFrameDecodeObject::operator IWICBitmapFrameDecode *() noexcept { return wic_bitmap_frame_decode; }
WicBitmapFrameDecodeObject::operator const IWICBitmapFrameDecode *() const noexcept { return wic_bitmap_frame_decode; }
IWICBitmapFrameDecode & WicBitmapFrameDecodeObject::operator *() noexcept { return *wic_bitmap_frame_decode; }
const IWICBitmapFrameDecode & WicBitmapFrameDecodeObject::operator *() const noexcept { return *wic_bitmap_frame_decode; }
IWICBitmapFrameDecode * WicBitmapFrameDecodeObject::operator ->() noexcept { return wic_bitmap_frame_decode; }
const IWICBitmapFrameDecode * WicBitmapFrameDecodeObject::operator ->() const noexcept { return wic_bitmap_frame_decode; }

