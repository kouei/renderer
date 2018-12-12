#include<cstdint>
#include"cuda_post_process.h"

using namespace CUDA_POST_PROCESS;

__global__
void bgra_to_greyscale(const uchar4 * bgraImage, uchar4 * greyImage, int numRows, int numCols)
{
	//TODO
	//Fill in the kernel to convert from color to greyscale
	//the mapping from components of a uchar4 to RGBA is:
	// .x -> R ; .y -> G ; .z -> B ; .w -> A
	//
	//The output (greyImage) at each pixel should be the result of
	//applying the formula: output = .299f * R + .587f * G + .114f * B;
	//Note: We will be ignoring the alpha channel for this conversion

	//First create a mapping from the 2D block and grid locations
	//to an absolute 2D location in the image, then use that to
	//calculate a 1D offset
	auto r = blockIdx.y * blockDim.y + threadIdx.y;
	auto c = blockIdx.x * blockDim.x + threadIdx.x;
	if (r >= numRows || c >= numCols) return;

	auto pos_1D = r * numCols + c;
	auto color = bgraImage[pos_1D];
	auto intensity = static_cast<uint8_t>(0.299f * color.z + 0.587f * color.y + 0.114f * color.x);
	greyImage[pos_1D] = uchar4{ intensity, intensity, intensity, 0xff };
}

void CUDA_POST_PROCESS::your_bgra_to_greyscale(const void * h_in_bgraImage, void * h_out_bgraImage, int numRows, int numCols)
{
	auto mem_size = sizeof(uchar4) * numRows * numCols;
	void * d_bgraImage = nullptr;
	void * d_greyImage = nullptr;

	cudaMalloc(&d_bgraImage, mem_size);
	cudaMalloc(&d_greyImage, mem_size);

	cudaMemcpy(d_bgraImage, h_in_bgraImage, mem_size, cudaMemcpyHostToDevice);
	bgra_to_greyscale <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uchar4 *>(d_bgraImage), reinterpret_cast<uchar4 *>(d_greyImage), numRows, numCols);
	cudaMemcpy(h_out_bgraImage, d_greyImage, mem_size, cudaMemcpyDeviceToHost);

	cudaFree(d_bgraImage);
	cudaFree(d_greyImage);

	cudaDeviceSynchronize(); 
	//checkCudaErrors(cudaGetLastError());
}


__global__
void gaussian_blur(const uint8_t * inputChannel, uint8_t * outputChannel, int numRows, int numCols, const float * filter, int filterWidth)
{
	auto r = static_cast<int>(blockIdx.y * blockDim.y + threadIdx.y);
	auto c = static_cast<int>(blockIdx.x * blockDim.x + threadIdx.x);
	if (r >= numRows || c >= numCols) return;

	auto halfWidth = filterWidth / 2;

	auto sum = 0.0f;
	for (auto j = -halfWidth; j <= halfWidth; ++j)
	{
		for (auto i = -halfWidth; i <= halfWidth; ++i)
		{
			auto rr = r + j;
			auto cc = c + i;
			rr = rr > (numRows - 1) ? (numRows - 1) : (rr < 0 ? 0 : rr);
			cc = cc >(numCols - 1) ? (numCols - 1) : (cc < 0 ? 0 : cc);

			auto jj = j + halfWidth;
			auto ii = i + halfWidth;

			sum += static_cast<float>(inputChannel[rr * numCols + cc]) * filter[jj * filterWidth + ii];
		}
	}
	sum = sum > 255.0f ? 255.0f : (sum < 0.0f ? 0.0f : sum);
	outputChannel[r * numCols + c] = static_cast<uint8_t>(sum);
}
__global__
void separateChannels(const uchar4 * inputImageBGRA, int numRows, int numCols, uint8_t * redChannel, uint8_t * greenChannel, uint8_t * blueChannel)
{
	auto r = blockIdx.y * blockDim.y + threadIdx.y;
	auto c = blockIdx.x * blockDim.x + threadIdx.x;
	if (r >= numRows || c >= numCols) return;

	auto pos_1D = r * numCols + c;
	auto BGRA = inputImageBGRA[pos_1D];
	redChannel[pos_1D] = BGRA.z;
	greenChannel[pos_1D] = BGRA.y;
	blueChannel[pos_1D] = BGRA.x;
}


__global__
void recombineChannels(const uint8_t * redChannel, const uint8_t * greenChannel, const uint8_t * blueChannel, uchar4 * outputImageBGRA, int numRows, int numCols)
{
	const int2 thread_2D_pos = make_int2(blockIdx.x * blockDim.x + threadIdx.x, blockIdx.y * blockDim.y + threadIdx.y);

	const int thread_1D_pos = thread_2D_pos.y * numCols + thread_2D_pos.x;

	//make sure we don't try and access memory outside the image
	//by having any threads mapped there return early
	if (thread_2D_pos.x >= numCols || thread_2D_pos.y >= numRows) return;

	auto red = redChannel[thread_1D_pos];
	auto green = greenChannel[thread_1D_pos];
	auto blue = blueChannel[thread_1D_pos];

	outputImageBGRA[thread_1D_pos] = make_uchar4(blue, green, red, 0xff);
}


void CUDA_POST_PROCESS::your_gaussian_blur(const void * h_inputImageBGRA, void * h_outputImageBGRA, int numRows, int numCols, float * h_filter, int filterWidth)
{
	void * d_inputImageBGRA = nullptr;
	void * d_outputImageBGRA = nullptr;
	void * d_red = nullptr;
	void * d_green = nullptr;
	void * d_blue = nullptr;
	void * d_redBlurred = nullptr;
	void * d_greenBlurred = nullptr;
	void * d_blueBlurred = nullptr;
	void * d_filter = nullptr;

	auto mem_size = sizeof(uchar4) * numRows * numCols;
	auto channel_mem_size = sizeof(uint8_t) * numRows * numCols;
	auto filter_mem_size = sizeof(float) * filterWidth * filterWidth;
	cudaMalloc(&d_inputImageBGRA, mem_size);
	cudaMalloc(&d_outputImageBGRA, mem_size);
	cudaMalloc(&d_red, channel_mem_size);
	cudaMalloc(&d_green, channel_mem_size);
	cudaMalloc(&d_blue, channel_mem_size);
	cudaMalloc(&d_redBlurred, channel_mem_size);
	cudaMalloc(&d_greenBlurred, channel_mem_size);
	cudaMalloc(&d_blueBlurred, channel_mem_size);
	cudaMalloc(&d_filter, filter_mem_size);

	cudaMemcpy(d_inputImageBGRA, h_inputImageBGRA, mem_size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_filter, h_filter, filter_mem_size, cudaMemcpyHostToDevice);

	separateChannels <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uchar4 *>(d_inputImageBGRA), numRows, numCols, reinterpret_cast<uint8_t *>(d_red), reinterpret_cast<uint8_t *>(d_green), reinterpret_cast<uint8_t *>(d_blue));

	cudaDeviceSynchronize(); //checkCudaErrors(cudaGetLastError());

	gaussian_blur <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uint8_t *>(d_red), reinterpret_cast<uint8_t *>(d_redBlurred), numRows, numCols, reinterpret_cast<float *>(d_filter), filterWidth);
	gaussian_blur <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uint8_t *>(d_green), reinterpret_cast<uint8_t *>(d_greenBlurred), numRows, numCols, reinterpret_cast<float *>(d_filter), filterWidth);
	gaussian_blur <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uint8_t *>(d_blue), reinterpret_cast<uint8_t *>(d_blueBlurred), numRows, numCols, reinterpret_cast<float *>(d_filter), filterWidth);



	cudaDeviceSynchronize(); //checkCudaErrors(cudaGetLastError());

	recombineChannels <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uint8_t *>(d_redBlurred), reinterpret_cast<uint8_t *>(d_greenBlurred), reinterpret_cast<uint8_t *>(d_blueBlurred), reinterpret_cast<uchar4 *>(d_outputImageBGRA), numRows, numCols);

	cudaMemcpy(h_outputImageBGRA, d_outputImageBGRA, mem_size, cudaMemcpyDeviceToHost);


	cudaFree(d_inputImageBGRA);
	cudaFree(d_outputImageBGRA);
	cudaFree(d_red);
	cudaFree(d_green);
	cudaFree(d_blue);
	cudaFree(d_redBlurred);
	cudaFree(d_greenBlurred);
	cudaFree(d_blueBlurred);
	cudaFree(d_filter);

	cudaDeviceSynchronize(); //checkCudaErrors(cudaGetLastError());
}



__global__
void average_pooling(const uchar4 * d_input_buffer, uchar4 * d_output_buffer, int numRows, int numCols, int pooling_width)
{
	auto r = blockIdx.y * blockDim.y + threadIdx.y;
	auto c = blockIdx.x * blockDim.x + threadIdx.x;
	if (r >= numRows || c >= numCols) return;

	auto rr = r * pooling_width;
	auto cc = c * pooling_width;

	float avg_red = 0.0f;
	float avg_green = 0.0f;
	float avg_blue = 0.0f;

	for (auto j = 0; j < pooling_width; ++j)
	{
		for (auto i = 0; i < pooling_width; ++i)
		{
			auto BGRA = d_input_buffer[(rr + j) * numCols * pooling_width + cc + i];
			avg_red += BGRA.z;
			avg_green += BGRA.y;
			avg_blue += BGRA.x;
		}
	}
	auto total = pooling_width * pooling_width;
	avg_red /= total;
	avg_green /= total;
	avg_blue /= total;

	auto red = static_cast<uint8_t>(avg_red);
	auto green = static_cast<uint8_t>(avg_green);
	auto blue = static_cast<uint8_t>(avg_blue);

	d_output_buffer[r * numCols + c] = make_uchar4(blue, green, red, 0xff);
}

void CUDA_POST_PROCESS::your_average_pooling(const void * h_input_buffer, void * h_output_buffer, int numRows, int numCols, int pooling_width)
{
	void * d_input_buffer = nullptr;
	void * d_output_buffer = nullptr;
	auto output_mem_size = sizeof(uchar4) * numRows * numCols;
	auto input_mem_size = output_mem_size * pooling_width * pooling_width;

	cudaMalloc(&d_input_buffer, input_mem_size);
	cudaMalloc(&d_output_buffer, output_mem_size);

	cudaMemcpy(d_input_buffer, h_input_buffer, input_mem_size, cudaMemcpyHostToDevice);
	average_pooling <<< dim3(128, 128), dim3(16, 16) >>> (reinterpret_cast<uchar4 *>(d_input_buffer), reinterpret_cast<uchar4 *>(d_output_buffer), numRows, numCols, pooling_width);
	cudaMemcpy(h_output_buffer, d_output_buffer, output_mem_size, cudaMemcpyDeviceToHost);

	cudaFree(d_input_buffer);
	cudaFree(d_output_buffer);
}
