// argv[1]: input video file
// argv[2]: output video file
// argv[3]: output TXT file


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"
#include "video_algorithms.h"

int main(int argc, char **argv) {
	int i=0,index=0;
	int width_img=0;			// Frame width
	int height_img=0;			// Frame height
	int frame=0;				// Frame number (index)
	int total_frames=0;			// Total frames
	int marked_frames=0;		// Marked frame
	int *check_frames;			// Contains indeces of marked frames
	int *list_of_frames;		// List of frames
	double fps=0.0;				// FPS (Frames Per Second)
	IplImage *bgr_frame;		// Frame
	IplImage *previous_frame;	// Previous frame
	IplImage *current_frame;	// Current frame
	IplImage *new_frame;		// New frame
	CvCapture* capture;			// Capture struct
	CvSize size;				// Size of frame (width x height)
	FILE *fp;					// TXT file pointer
	clock_t start, stop, diff;	// Timer
	
	// Check if the user gave arguments
	if(argc != 4) {
		fprintf(stderr, "\nUSAGE: %s <input_video_file> <output_video_file> <output_TXT_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	


	/**** STAGE 1: PROCESS FRAMES ****/
	
	capture = cvCreateFileCapture(argv[1]);	// Open video file to start capture
	if(!capture) {
		printf("Error opening video file! (cvCreateFileCapture)\n");
		return EXIT_FAILURE;
	}

	fp=fopen(argv[3],"w");	// Open file to write stats
	if(fp == NULL) {
		printf("Error opening file! (fopen)\n");
		return EXIT_FAILURE;
	}
	
	fps = cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);					// Get the FPS
	width_img = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH);		// Get the frame width
	height_img = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT);	// Get the frame height
	total_frames = cvGetCaptureProperty(capture,CV_CAP_PROP_FRAME_COUNT);	// Get total frames
	size = cvSize(width_img,height_img);									// Get size of frames

	check_frames = (int *)malloc(sizeof(*check_frames) * total_frames);
	list_of_frames = (int *)malloc(sizeof(*list_of_frames) * total_frames);
	if (check_frames == NULL || list_of_frames == NULL) {
		printf("Error allocating memory!\n");
		return EXIT_FAILURE;
	}

	// Initialize arrays
	for(i=0;i<total_frames;i++) {
		check_frames[i]=0;
		list_of_frames[i]=0;
	}
	
	fprintf(fp,"Filename\t:\t%s\n\nFrame width\t:\t%d\nFrame height\t:\t%d\nFPS\t\t:\t%f\nTotal frames\t:\t%d\n\n\n\n",argv[1],width_img,height_img,fps,total_frames);
	printf("Filename\t:\t%s\n\nFrame width\t:\t%d\nFrame height\t:\t%d\nFPS\t\t:\t%f\nTotal frames\t:\t%d\n\n\n\n",argv[1],width_img,height_img,fps,total_frames);

	printf("Start processing frames...\n\n");
	
	start = clock(); // Start timer

	bgr_frame=cvQueryFrame(capture);												// Grab first frame
	previous_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the previous frame
	cvCopy(bgr_frame,previous_frame,NULL);											// Save the copy
	
	// Grab frames from the video until NULL
	while((bgr_frame=cvQueryFrame(capture)) != NULL) {
		/* When entering this loop, we have already grabbed a frame
		 * so the frame counter starts from 2
		 */
		frame = cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);					// Get the current frame number
		
		current_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the current frame
		cvCopy(bgr_frame,current_frame,NULL);											// Save the copy
		new_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);		// Create the new frame	
		
		/**** START PROCESSING ****/
		absdiff(previous_frame, current_frame, new_frame, frame, fp, width_img, height_img, &index);
		/**** END PROCESSING ****/

		cvReleaseImage(&previous_frame);		// Release previous frame
		previous_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the previous frame
		cvCopy(bgr_frame,previous_frame,NULL);	// Save the copy

		cvReleaseImage(&current_frame);			// Release current_frame
		cvReleaseImage(&new_frame);				// Release new_frame
		
		if(index==1) {
			check_frames[frame]=1;	// It means that the specific frame is marked
		}
		
		printf("Processing frame %d...\r",frame);
		fflush(stdout);
	}
	
	stop = clock();			// Stop timer
	diff = stop - start;	// Get difference between start time and current time;
	printf("\n\nTotal time processing frames : %f minutes\t%f seconds\n", (((float)diff)/CLOCKS_PER_SEC)/60, ((float)diff)/CLOCKS_PER_SEC);
	printf("Processing completed!\n");

	fprintf(fp,"\n\n\n\nMarked frames\n\n");
	printf("\n\n\n\nMarked frames\n\n");
	
	for(i=0;i<total_frames;i++) {
		// If we have a marked frame
		if(check_frames[i]==1) {
			list_of_frames[i]=i;
			fprintf(fp,"frame %d\n",i);	// Write to file only marked frames
			printf("frame %d\n",i);		// Write to file only marked frames
			marked_frames++;			// Counter to keep track of the total marked frames
		}
	}
	
	fprintf(fp,"\n\nTotal marked frames\t:\t%d\n",marked_frames);
	printf("\n\nTotal marked frames\t:\t%d\n\n",marked_frames);
	
	fclose(fp);							// Close file pointer
	cvReleaseImage(&bgr_frame);			// Release bgr_frame
	cvReleaseImage(&previous_frame);	// Release previous_frame
	cvReleaseCapture(&capture);			// Release capture

	//If there is no markeed frames, exit
	if(marked_frames == 0) {
		return EXIT_SUCCESS;
	}
	
	
	
	/**** STAGE 2: WRITE VIDEO ****/
	
	capture = cvCreateFileCapture(argv[1]);	// Re-Open video file to start capture
	if(!capture) {
		printf("Error opening video file! (cvCreateFileCapture)\n");
		return EXIT_FAILURE;
	}

	CvVideoWriter *writer = cvCreateVideoWriter(argv[2],CV_FOURCC('F','M','P','4'),fps,size,1);

	printf("Start writing frames...\n\n");
	
	start = clock(); // Start timer

	bgr_frame = cvQueryFrame(capture);	// Retrieve frame
	new_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the new frame
	
	do {
		frame = cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);	// Get the current frame number

		// If the index number of the current frame is equal to the frame we want, then write it to the stream.
		if(frame == list_of_frames[frame]) {
			cvCopy(bgr_frame,new_frame,NULL);	// Save the copy
			cvWriteFrame(writer, new_frame);	// Write frame to video stream
		} else {
			cvWriteFrame(writer, new_frame);	// Write frame to video stream
		}
		
		printf("Writing frame %d...\r",frame);
		fflush(stdout); // For '/r' to work we have to flush the output stream
	} while((bgr_frame=cvQueryFrame(capture)) != NULL);
		
	stop = clock(); 		// Stop timer
	diff = stop - start;	// Get difference between start time and current time;
	printf("\n\nTotal time writing frames : %f minutes\t%f seconds\n", (((float)diff)/CLOCKS_PER_SEC)/60, ((float)diff)/CLOCKS_PER_SEC);
	printf("Writing completed!\n\n");

	free(list_of_frames);			// Free list_of_frames
	free(check_frames);				// Free check_frames
	cvReleaseImage(&bgr_frame);		// Release bgr_frame
	cvReleaseImage(&new_frame);		// Release new_frame
	cvReleaseVideoWriter(&writer);	// Release video writer
	cvReleaseCapture(&capture);		// Release capture

	return EXIT_SUCCESS;
}
