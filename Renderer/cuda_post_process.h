#ifndef _CUDA_POST_PROCESS_
#define _CUDA_POST_PROCESS_

namespace CUDA_POST_PROCESS
{
	void your_bgra_to_greyscale(const void * h_in_bgraImage, void * h_out_bgraImage, int numRows, int numCols);
	void your_gaussian_blur(const void * h_inputImageBGRA, void * h_outputImageBGRA, int numRows, int numCols, float * h_filter, int filterWidth);
	void your_average_pooling(const void * h_input_buffer, void * h_output_buffer, int numRows, int numCols, int pooling_width);
};

#endif
