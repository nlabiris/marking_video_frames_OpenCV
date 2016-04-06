#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"
#include "video_algorithms.h"

void absdiff(IplImage *previous_frame, IplImage *current_frame, int frame, FILE *fp, int width_img, int height_img, int *index_call, int threshold) {
	int i,j;
	unsigned char *pinput_previous;	// Previous frame imageData (input)
	unsigned char *pinput_current;		// Current frame imageData (input)
	int sumB=0;									// Sum of pixels (B)
	int sumG=0;									// Sum of pixels (G)
	int sumR=0;									// Sum of pixels (R)
	double similarityB=0.0;					// Similarity (B)
	double similarityG=0.0;					// Similarity (G)
	double similarityR=0.0;					// Similarity (R)


	pinput_current = (unsigned char*)current_frame->imageData;		// Assign imageData to current array (input)
	pinput_previous = (unsigned char*)previous_frame->imageData;	// Assign imageData to previous array (input)

	for(i=0;i<height_img;i++) {
		for(j=0;j<width_img;j++) {
			sumB=sumB+abs(pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+0]-pinput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+0]); // B
			sumG=sumG+abs(pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+1]-pinput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+1]); // G
			sumR=sumR+abs(pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+2]-pinput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+2]); // R
		}
	}
	
	similarityB=100-(sumB/((double)(width_img*height_img))/2.55); // Similarity calculation (B)
	similarityG=100-(sumG/((double)(width_img*height_img))/2.55); // Similarity calculation (G)
	similarityR=100-(sumR/((double)(width_img*height_img))/2.55); // Similarity calculation (R)

	// Threshold to mark frames we want
	// If similarity in any of the 3 channels (RGB) is < 95% then the frame is marked
	if(similarityB < threshold || similarityG < threshold || similarityR < threshold) {
		*index_call=1;
		if(frame<10) {
			fprintf(fp,"marked\tframe 0%d\t\tsum_B = %d\tsum_G = %d\tsum_R = %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\n",frame,sumB,sumG,sumR,similarityB,similarityG,similarityR);
		} else {
			fprintf(fp,"marked\tframe %d\t\tsum_B = %d\tsum_G = %d\tsum_R = %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\n",frame,sumB,sumG,sumR,similarityB,similarityG,similarityR);
		}
	} else {
		*index_call=0;
		if(frame<10) {
			fprintf(fp,"\tframe 0%d\t\tsum_B = %d\tsum_G = %d\tsum_R = %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\n",frame,sumB,sumG,sumR,similarityB,similarityG,similarityR);
		} else {
			fprintf(fp,"\tframe %d\t\tsum_B = %d\tsum_G = %d\tsum_R = %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\n",frame,sumB,sumG,sumR,similarityB,similarityG,similarityR);
		}
	}
}

void comprdiff(IplImage *decoded_previous, IplImage *decoded_current, int frame, FILE *fp, int width_img, int height_img, int *index_call, double parameter) {
	unsigned int encoded_diff=0;	// Compression difference between frames
	double threshold=0.0;			// Threshold
	//double parameter=0.005;			// Percentage of the whole frame
	CvMat *encoded_previous;		// Previous frame (encoded)
	CvMat *encoded_current;			// Current frame (encoded)


	encoded_previous = cvEncodeImage("previous.JPEG",decoded_previous,0);	// Encode the previous frame
	encoded_current = cvEncodeImage("current.JPEG",decoded_current,0);	// Encode the current frame
	
	encoded_diff=abs((encoded_previous->step)-(encoded_current->step));	// Difference of previous and current frame in bytes

	threshold=parameter*((double)(width_img*height_img*3));			// Multiply the parameter with the number of pixels of the whole frame

	// Threshold to mark frames we want
	// If the difference in bytes between the previous and current frame is > 0.5% of the whole image (in size) then the frame is marked
	if(encoded_diff > threshold) {
		*index_call=1;
		if(frame<10) {
			fprintf(fp,"marked\tframe 0%d\tencoded diff : %d bytes\n",frame,encoded_diff);
		} else {
			fprintf(fp,"marked\tframe %d\tencoded diff : %d bytes\n",frame,encoded_diff);
		}
	} else {
		*index_call=0;
		if(frame<10) {
			fprintf(fp,"\tframe 0%d\tencoded diff : %d bytes\n",frame,encoded_diff);
		} else {
			fprintf(fp,"\tframe %d\tencoded diff : %d bytes\n",frame,encoded_diff);
		}
	}
	
	cvReleaseMat(&encoded_previous);
	cvReleaseMat(&encoded_current);
}

void ecrdiff_v1(IplImage *current, IplImage *previous, CvSize size, int frame, double *ecr_call, double *ecr_max_call) {
	int i,j,k,l;
	double t_current=0.0,tx_current=0.0,ty_current=0.0;						// Sum variables (Sobel)
	double t_previous=0.0,tx_previous=0.0,ty_previous=0.0;						// Sum variables (Sobel)
	double wx[3][3], wy[3][3];						// X-axis, Y-axis masks (Sobel)
	int w_xor[3][3];							// Mask (Dilation)
	int t_xor_current=0, t_xor_previous=0;								// Sum variables (Dilation)
	double edge_pixels_previous=0;						// Edge pixels (previous)
	double edge_pixels_current=0;						// Edge pixels (previous)
	double exiting_pixels_previous=0;					// Exiting pixels (previous)
	double exiting_pixels_current=0;					// Exiting pixels (current)
	double ecr_previous=0.0;						// ECR (previous)
	double ecr_current=0.0;							// ECR (current)
	unsigned char *pinput_previous;						// Previous frame imageData (input)
	unsigned char *pinput_previous_gray;					// Previous grayscale frame imageData (input)
	unsigned char *poutput_previous_gray;					// Previous grayscale frame imageData (output)
	unsigned char *pinput_current;						// Current frame imageData (input)
	unsigned char *pinput_current_gray;					// Current grayscale frame imageData (input)
	unsigned char *poutput_current_gray;					// Current grayscale frame imageData (output)
	IplImage *previous_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage *current_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);


	pinput_previous = (unsigned char*)previous->imageData;			// Assign imageData to previous array (input)
	pinput_previous_gray = (unsigned char*)previous_gray->imageData;	// Assign imageData to previous greyscale array (input)
	poutput_previous_gray = (unsigned char*)previous_gray->imageData;	// Assign imageData to previous greyscale array (output)
	pinput_current = (unsigned char*)current->imageData;			// Assign imageData to current array (input)
	pinput_current_gray = (unsigned char*)current_gray->imageData;		// Assign imageData to current greyscale array (input)
	poutput_current_gray = (unsigned char*)current_gray->imageData;		// Assign imageData to current greyscale array (output)


// ---------------------------->  MASK-X for Convolution
	wx[0][0]= -1.0;	wx[1][0]= -2.0;	wx[2][0]= -1.0;
	wx[0][1]= 0.0;  wx[1][1]= 0.0;	wx[2][1]= 0.0;
	wx[0][2]= 1.0;	wx[1][2]= 2.0;	wx[2][2]= 1.0;
// ---------------------------->  MASK-Y for Convolution
	wy[0][0]= 1.0;	wy[1][0]= 0.0;	wy[2][0]= -1.0;
	wy[0][1]= 2.0;	wy[1][1]= 0.0;	wy[2][1]= -2.0;
	wy[0][2]= 1.0;	wy[1][2]= 0.0;  wy[2][2]= -1.0;
// ---------------------------->  MASK for Dilation
	w_xor[0][0]= 1;	w_xor[1][0]= 1;	w_xor[2][0]= 1;
	w_xor[0][1]= 1;	w_xor[1][1]= 1;	w_xor[2][1]= 1;
	w_xor[0][2]= 1;	w_xor[1][2]= 1;	w_xor[2][2]= 1;

	/********** Convert to grayscale **********/
	for(i=0;i<current->height;i++) {
		for(j=0;j<current->width;j++) {
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=0.299*pinput_previous[i*previous->widthStep+j*previous->nChannels+2]+0.587*pinput_previous[i*previous->widthStep+j*previous->nChannels+1]+0.114*pinput_previous[i*previous->widthStep+j*previous->nChannels+0];
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=0.299*pinput_current[i*current->widthStep+j*current->nChannels+2]+0.587*pinput_current[i*current->widthStep+j*current->nChannels+1]+0.114*pinput_current[i*current->widthStep+j*current->nChannels+0];
		}
	}

	/********** Sobel edge detector **********/
	for (i=1;i<current->height-1;i++) {
		for (j=1;j<current->width-1;j++) {
			tx_previous = 0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					tx_previous = tx_previous + poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] * wx[k][l];
				}
			}

			ty_previous=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					ty_previous = ty_previous + poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] * wy[k][l];
				}
			}
				
			t_previous =  sqrt(tx_previous*tx_previous + ty_previous*ty_previous);
			if (t_previous>255) t_previous=255;
			if (t_previous<0) t_previous=0;
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=(unsigned char) t_previous;

			tx_current = 0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					tx_current = tx_current + poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] * wx[k][l];
				}
			}

			ty_current=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					ty_current = ty_current + poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] * wy[k][l];
				}
			}
				
			t_current =  sqrt(tx_current*tx_current + ty_current*ty_current);
			if (t_current>255) t_current=255;
			if (t_current<0) t_current=0;
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=(unsigned char) t_current;
		}
	}

	for(i=0;i<current->height;i++) {
		for(j=0;j<current->width;j++) {
			/********** Convert image to binary (Thresholding) **********/
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] > 100) {
				poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=255;
			} else {
				poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=0;
			}
			
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] > 100) {
				poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=255;
			} else {
				poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=0;
			}
			
			/********** Count all edge pixels **********/
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]==255) {
				edge_pixels_previous++;
			}
			
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]==255) {
				edge_pixels_current++;
			}
		}
	}

	/********** Dilation (XOR) **********/
	for (i=1;i<current->height-1;i++) {
		for (j=1;j<current->width-1;j++) {
			t_xor_previous=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					t_xor_previous = t_xor_previous + (poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] ^ w_xor[k][l]);
				}
			}
			if (t_xor_previous>255) t_xor_previous=255;
			if (t_xor_previous<0) t_xor_previous=0;
			pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=(unsigned char) t_xor_previous;

			t_xor_current=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					t_xor_current = t_xor_current + (poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] ^ w_xor[k][l]);
				}
			}
			if (t_xor_current>255) t_xor_current=255;
			if (t_xor_current<0) t_xor_current=0;
			pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=(unsigned char) t_xor_current;
		}
	}

	for (i=0;i<current->height;i++) {
		for (j=0;j<current->width;j++) {
			/********** Invert the frames **********/
			pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=255-pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0];
			pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=255-pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0];
			
			/********** AND **********/
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] = poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] & pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0];
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] = poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] & pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0];
			
			/********** Count all exiting pixels **********/
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]==0) {
				exiting_pixels_previous++;
			}
			
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]==0) {
				exiting_pixels_current++;
			}
		}
	}

	ecr_previous=exiting_pixels_previous/edge_pixels_previous;	// Dividing the exiting pixels with the edge pixels the previous frame
	ecr_current=exiting_pixels_current/edge_pixels_current;		// Dividing the exiting pixels with the edge pixels the current frame

	// We keep the ECR of the previous or current frame
	if(ecr_previous>ecr_current) {
		ecr_call[frame]=ecr_previous;
	} else {
		ecr_call[frame]=ecr_current;
	}

	// We keep the max ECR of the whole video
	if(ecr_call[frame] >= 10 && ecr_call[frame] <= 20) {
		*ecr_max_call = ecr_call[frame];
	}

	cvReleaseImage(&previous_gray);
	cvReleaseImage(&current_gray);
}


void mark_frames(int frame, FILE *fp, int *index_call, double *ecr_call, double *ecr_max_call) {
	// Threshold to mark frames we want
	// if the ECR of the frame is > of the 30% of the max ECR of the whole video then the frame is marked
	if(ecr_call[frame] > (*ecr_max_call)*0.3) {
		*index_call=1;
		if(frame<10) {
			fprintf(fp,"marked\tframe 0%d\t\tecr = %.4f\n",frame,ecr_call[frame]);
		} else {
			fprintf(fp,"marked\tframe %d\t\tecr = %.4f\n",frame,ecr_call[frame]);
		}
	} else {
		*index_call=0;
		if(frame<10) {
			fprintf(fp,"\tframe 0%d\t\tecr = %.4f\n",frame,ecr_call[frame]);
		} else {
			fprintf(fp,"\tframe %d\t\tecr = %.4f\n",frame,ecr_call[frame]);
		}
	}
}

void ecrdiff_v2(IplImage *current, IplImage *previous, CvSize size, int frame, FILE *fp, int *index_call) {
	int i,j,k,l;
	double t_prev=0.0,tx_prev=0.0,ty_prev=0.0;						// Sum variables (Sobel)
	double t_curr=0.0,tx_curr=0.0,ty_curr=0.0;						// Sum variables (Sobel)
	double wx[3][3], wy[3][3];						// X-axis, Y-axis masks (Sobel)
	int w_xor[3][3];							// Mask (Dilation)
	int t_xor_prev=0;								// Sum variables (Dilation)
	int t_xor_curr=0;								// Sum variables (Dilation)
	double edge_pixels_previous=0;						// Edge pixels (previous)
	double edge_pixels_current=0;						// Edge pixels (previous)
	double exiting_pixels_previous=0;					// Exiting pixels (previous)
	double exiting_pixels_current=0;					// Exiting pixels (current)
	double ecr_previous=0.0;						// ECR (previous)
	double ecr_current=0.0;							// ECR (current)
	double ecrdiff=0.0;							// ECR Difference
	unsigned char *pinput_previous;						// Previous frame imageData (input)
	unsigned char *pinput_previous_gray;					// Previous grayscale frame imageData (input)
	unsigned char *poutput_previous_gray;					// Previous grayscale frame imageData (output)
	unsigned char *pinput_current;						// Current frame imageData (input)
	unsigned char *pinput_current_gray;					// Current grayscale frame imageData (input)
	unsigned char *poutput_current_gray;					// Current grayscale frame imageData (output)
	IplImage *previous_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);
	IplImage *current_gray = cvCreateImage(size, IPL_DEPTH_8U, 1);


	pinput_previous = (unsigned char*)previous->imageData;			// Assign imageData to previous array (input)
	pinput_previous_gray = (unsigned char*)previous_gray->imageData;	// Assign imageData to previous greyscale array (input)
	poutput_previous_gray = (unsigned char*)previous_gray->imageData;	// Assign imageData to previous greyscale array (output)
	pinput_current = (unsigned char*)current->imageData;			// Assign imageData to current array (input)
	pinput_current_gray = (unsigned char*)current_gray->imageData;		// Assign imageData to current greyscale array (input)
	poutput_current_gray = (unsigned char*)current_gray->imageData;		// Assign imageData to current greyscale array (output)


// ---------------------------->  MASK-X for Convolution
	wx[0][0]= -1.0;	wx[1][0]= -2.0;	wx[2][0]= -1.0;
	wx[0][1]= 0.0;  wx[1][1]= 0.0;	wx[2][1]= 0.0;
	wx[0][2]= 1.0;	wx[1][2]= 2.0;	wx[2][2]= 1.0;
// ---------------------------->  MASK-Y for Convolution
	wy[0][0]= 1.0;	wy[1][0]= 0.0;	wy[2][0]= -1.0;
	wy[0][1]= 2.0;	wy[1][1]= 0.0;	wy[2][1]= -2.0;
	wy[0][2]= 1.0;	wy[1][2]= 0.0;  wy[2][2]= -1.0;
// ---------------------------->  MASK for Dilation
	w_xor[0][0]= 1;	w_xor[1][0]= 1;	w_xor[2][0]= 1;
	w_xor[0][1]= 1;	w_xor[1][1]= 1;	w_xor[2][1]= 1;
	w_xor[0][2]= 1;	w_xor[1][2]= 1;	w_xor[2][2]= 1;

	/********** Convert to grayscale **********/
	for(i=0;i<current->height;i++) {
		for(j=0;j<current->width;j++) {
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=0.299*pinput_previous[i*previous->widthStep+j*previous->nChannels+2]+0.587*pinput_previous[i*previous->widthStep+j*previous->nChannels+1]+0.114*pinput_previous[i*previous->widthStep+j*previous->nChannels+0];
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=0.299*pinput_current[i*current->widthStep+j*current->nChannels+2]+0.587*pinput_current[i*current->widthStep+j*current->nChannels+1]+0.114*pinput_current[i*current->widthStep+j*current->nChannels+0];
		}
	}

	/********** Sobel edge detector **********/
	for (i=1;i<current->height-1;i++) {
		for (j=1;j<current->width-1;j++) {
			tx_prev = 0;
			tx_curr = 0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					tx_prev = tx_prev + poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] * wx[k][l];
					tx_curr = tx_curr + poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] * wx[k][l];
				}
			}

			ty_prev=0;
			ty_curr=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					ty_prev = ty_prev + poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] * wy[k][l];
					ty_curr = ty_curr + poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] * wy[k][l];
				}
			}
				
			t_prev =  sqrt(tx_prev*tx_prev + ty_prev*ty_prev);
			t_curr=  sqrt(tx_curr*tx_curr + ty_curr*ty_curr);
			if (t_prev>255) t_prev=255;
			if (t_prev<0) t_prev=0;
			if (t_curr>255) t_curr=255;
			if (t_curr<0) t_curr=0;
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=(unsigned char) t_prev;
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=(unsigned char) t_curr;
		}
	}

	for(i=0;i<current->height;i++) {
		for(j=0;j<current->width;j++) {
			/********** Convert image to binary (Thresholding) **********/
			// Previous
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] > 100) {
				poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=255;
			} else {
				poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=0;
			}
			
			// Current
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] > 100) {
				poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=255;
			} else {
				poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=0;
			}
			
			/********** Count all edge pixels **********/
			// Previous
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]==255) {
				edge_pixels_previous++;
			}
	
			// Current			
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]==255) {
				edge_pixels_current++;
			}
		}
	}

	/********** Dilation (XOR) **********/
	for (i=1;i<current->height-1;i++) {
		for (j=1;j<current->width-1;j++) {
			t_xor_prev=0;
			t_xor_curr=0;
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					t_xor_prev = t_xor_prev + (poutput_previous_gray[(i+k)*previous_gray->widthStep+(j+l)*previous_gray->nChannels+0] ^ w_xor[k][l]);
					t_xor_curr = t_xor_curr + (poutput_current_gray[(i+k)*current_gray->widthStep+(j+l)*current_gray->nChannels+0] ^ w_xor[k][l]);
				}
			}
			if (t_xor_prev>255) t_xor_prev=255;
			if (t_xor_prev<0) t_xor_prev=0;
			if (t_xor_curr>255) t_xor_curr=255;
			if (t_xor_curr<0) t_xor_curr=0;
			pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=(unsigned char) t_xor_prev;
			pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=(unsigned char) t_xor_curr;
		}
	}

	/********** Invert the frames **********/
	for (i=0;i<current->height;i++) {
		for (j=0;j<current->width;j++) {
			pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]=255-pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0];
			pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]=255-pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0];
		}
	}

	/********** AND **********/
	for (i=0;i<current->height;i++) {
		for (j=0;j<current->width;j++) {
			poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] = poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0] & pinput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0];
			poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] = poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0] & pinput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0];
		}
	}

	/********** Count all exiting pixels **********/
	for (i=0;i<current->height;i++) {
		for (j=0;j<current->width;j++) {
			// Previous
			if(poutput_previous_gray[i*previous_gray->widthStep+j*previous_gray->nChannels+0]==0) {
				exiting_pixels_previous++;
			}
	
			// Current
			if(poutput_current_gray[i*current_gray->widthStep+j*current_gray->nChannels+0]==0) {
				exiting_pixels_current++;
			}
		}
	}

	ecr_previous=exiting_pixels_previous/edge_pixels_previous;	// Dividing the exiting pixels with the edge pixels the previous frame
	ecr_current=exiting_pixels_current/edge_pixels_current;		// Dividing the exiting pixels with the edge pixels the current frame

	ecrdiff=ecr_previous-ecr_current;
	if(ecrdiff<0) ecrdiff=ecrdiff*(-1.0);
		
	// Threshold to mark frames we want
	// if the ECR of the frame is > of the 30% of the max ECR of the whole video then the frame is marked
	if(ecrdiff > 0.5) {
		*index_call=1;
		if(frame<10) {
			fprintf(fp,"marked\tframe 0%d\t\tecrdiff = %.4f\n",frame,ecrdiff);
		} else {
			fprintf(fp,"marked\tframe %d\t\tecrdiff = %.4f\n",frame,ecrdiff);
		}
	} else {
		*index_call=0;
		if(frame<10) {
			fprintf(fp,"\tframe 0%d\t\tecrdiff = %.4f\n",frame,ecrdiff);
		} else {
			fprintf(fp,"\tframe %d\t\tecrdiff = %.4f\n",frame,ecrdiff);
		}
	}

	cvReleaseImage(&previous_gray);
	cvReleaseImage(&current_gray);
}

void histdiff(IplImage *previous_frame, IplImage *current_frame, IplImage *new_frame, int frame, FILE *fp, int width_img, int height_img, int *index_call) {
	int i,j;
	int hist_r[256],hist_g[256],hist_b[256];				// Histograms for BGR
	double hist_sum_b,hist_sum_g,hist_sum_r;				// Sum Histograms for BGR
	double hist_sum_b_previous,hist_sum_g_previous,hist_sum_r_previous;	// Sum Histograms for BGR (previous frame)
	double hist_sum_b_current,hist_sum_g_current,hist_sum_r_current;	// Sum Histograms for BGR (current frame)
	double mean_b,mean_g,mean_r;						// Mean Values for BGR
	double var_sum_b,var_sum_g,var_sum_r,variance_b,variance_g,variance_r;	// Variance Values for BGR
	unsigned char *poutput_previous;					// Previous frame imageData (output)
	unsigned char *pinput_current;						// Current frame imageData (input)
	unsigned char *poutput_current;						// Current frame imageData (output)
	unsigned char *poutput_new;						// New frame imageData (output)
	double similarity0=0.0;							// Similarity (B)
	double similarity1=0.0;							// Similarity (G)
	double similarity2=0.0;							// Similarity (R)


	pinput_current = (unsigned char*)current_frame->imageData;		// Assign imageData to current array (input)
	poutput_current = (unsigned char*)current_frame->imageData;		// Assign imageData to current array (output)
	poutput_previous = (unsigned char*)previous_frame->imageData;		// Assign imageData to previous array (output)
	poutput_new = (unsigned char*)new_frame->imageData;			// Assign imageData to new array (output)

	for(i=0;i<height_img;i++) {
		for(j=0;j<width_img;j++) {
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+0]=0; // B
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+1]=0; // G
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+2]=0; // R
		}
	}

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+0]=pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+0]; // B
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+1]=pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+1]; // G
			poutput_new[i*new_frame->widthStep+j*new_frame->nChannels+2]=pinput_current[i*current_frame->widthStep+j*current_frame->nChannels+2]; // R
		}
	}

	/**********PREVIOUS**********/

	// histogram previous frame
	for(i=0;i<256;i++) {
		hist_b[i] = 0;
		hist_g[i] = 0;
		hist_r[i] = 0;
	}

	hist_sum_b = 0.0;
	hist_sum_g = 0.0;
	hist_sum_r = 0.0;

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_b[poutput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+0]]++; // Histogram (B)
		}
	}

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_g[poutput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+1]]++; // Histogram (G)
		}
	}

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_r[poutput_previous[i*previous_frame->widthStep+j*previous_frame->nChannels+2]]++; // Histogram (R)
		}
	}

	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_b=hist_sum_b+(i*hist_b[i]); // Sum of histogram (B)
	}


	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_g=hist_sum_g+(i*hist_g[i]); // Sum of histogram (G)
	}


	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_r=hist_sum_r+(i*hist_r[i]); // Sum of histogram (R)
	}

	hist_sum_b_previous=hist_sum_b;
	hist_sum_g_previous=hist_sum_g;
	hist_sum_r_previous=hist_sum_r;



	/**********CURRENT**********/

	// histogram current frame
	for(i=0;i<256;i++) {
		hist_b[i] = 0;
		hist_g[i] = 0;
		hist_r[i] = 0;
	}

	hist_sum_b = 0.0;
	hist_sum_g = 0.0;
	hist_sum_r = 0.0;
	var_sum_b = 0.0;
	var_sum_g = 0.0;
	var_sum_r = 0.0;

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_b[poutput_current[i*current_frame->widthStep+j*current_frame->nChannels+0]]++; // Histogram (B)
		}
	}

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_g[poutput_current[i*current_frame->widthStep+j*current_frame->nChannels+1]]++; // Histogram (G)
		}
	}

	for (i=0;i<height_img;i++) {
		for (j=0;j<width_img;j++) {
			hist_r[poutput_current[i*current_frame->widthStep+j*current_frame->nChannels+2]]++; // Histogram (R)
		}
	}

	for(i=0;i<256;i++) {
		// hist_b[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_b=hist_sum_b+(i*hist_b[i]);		// Sum of histogram (B)
	}
	mean_b=hist_sum_b/((double)(width_img*height_img));	// Mean value (B)

	for(i=0;i<256;i++) {
		// hist_g[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_g=hist_sum_g+(i*hist_g[i]);		// Sum of histogram (G)
	}
	mean_g=hist_sum_g/((double)(width_img*height_img));	// Mean value (G)

	for(i=0;i<256;i++) {
		// hist_r[i] : number of pixel that has a specific hue
		// i : hue
		hist_sum_r=hist_sum_r+(i*hist_r[i]);		// Sum of histogram (R)
	}
	mean_r=hist_sum_r/((double)(width_img*height_img));	// Mean value (R)

	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		var_sum_b=var_sum_b+((i-mean_b)*(i-mean_b)*hist_b[i]/((double)(width_img*height_img)));
	}
	variance_b=sqrt(var_sum_b); // Variance value (B)

	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		var_sum_g=var_sum_g+((i-mean_g)*(i-mean_g)*hist_g[i]/((double)(width_img*height_img)));
	}
	variance_g=sqrt(var_sum_g); // Variance value (G)

	for(i=0;i<256;i++) {
		// hist[i] : number of pixel that has a specific hue
		// i : hue
		var_sum_r=var_sum_r+((i-mean_r)*(i-mean_r)*hist_r[i]/((double)(width_img*height_img)));
	}
	variance_r=sqrt(var_sum_r); // Variance value (R)

	hist_sum_b_current=hist_sum_b;
	hist_sum_g_current=hist_sum_g;
	hist_sum_r_current=hist_sum_r;

	hist_sum_b=abs(hist_sum_b_previous-hist_sum_b_current); // Histogram difference (B)
	hist_sum_g=abs(hist_sum_g_previous-hist_sum_g_current); // Histogram difference (G)
	hist_sum_r=abs(hist_sum_r_previous-hist_sum_r_current); // Histogram difference (R)

	similarity0=100-(hist_sum_b/((double)(width_img*height_img))/2.55); // Similarity calculation (B)
	similarity1=100-(hist_sum_g/((double)(width_img*height_img))/2.55); // Similarity calculation (G)
	similarity2=100-(hist_sum_r/((double)(width_img*height_img))/2.55); // Similarity calculation (R)

	// Threshold to mark frames we want
	// if similarity in any of the 3 channels (RGB) is < 95% then the frame is marked
	if(similarity0 < 90 || similarity1 < 90 || similarity2 < 90) {
		*index_call=1;
		if(frame<10) {
			fprintf(fp,"marked\tframe 0%d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\t\tmean_B = %.1f\tmean_G = %.1f\tmean_R = %.1f\t\tvariance_B = %.1f\tvariance_G = %.1f\tvariance_R = %.1f\n",frame,similarity0,similarity1,similarity2,mean_b,mean_g,mean_r,variance_b,variance_g,variance_r);
		} else {
			fprintf(fp,"marked\tframe %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\t\tmean_B = %.1f\tmean_G = %.1f\tmean_R = %.1f\t\tvariance_B = %.1f\tvariance_G = %.1f\tvariance_R = %.1f\n",frame,similarity0,similarity1,similarity2,mean_b,mean_g,mean_r,variance_b,variance_g,variance_r);
		}
	} else {
		*index_call=0;
		if(frame<10) {
			fprintf(fp,"\tframe 0%d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\t\tmean_B = %.1f\tmean_G = %.1f\tmean_R = %.1f\t\tvariance_B = %.1f\tvariance_G = %.1f\tvariance_R = %.1f\n",frame,similarity0,similarity1,similarity2,mean_b,mean_g,mean_r,variance_b,variance_g,variance_r);
		} else {
			fprintf(fp,"\tframe %d\t\tsimilarity_B = %.4f\tsimilarity_G = %.4f\tsimilarity_R = %.4f\t\tmean_B = %.1f\tmean_G = %.1f\tmean_R = %.1f\t\tvariance_B = %.1f\tvariance_G = %.1f\tvariance_R = %.1f\n",frame,similarity0,similarity1,similarity2,mean_b,mean_g,mean_r,variance_b,variance_g,variance_r);
		}
	}
}
