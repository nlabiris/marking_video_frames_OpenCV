// argv[1]: input video file
// argv[2]: output TXT file


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cv.h"
#include "highgui.h"
#include "video_algorithms.h"

int main(int argc, char **argv) {
	int threshold=90;			// Threshold to mark frames we want
	int p[3];
	char buffer[20];
	int ci=1;					// Counter
	int i=0,index=0;
	int width_img=0;			// Frame width
	int height_img=0;			// Frame height
	int frame=0;				// Frame number (index)
	int msec;
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
	
	// Text variables
	CvScalar black = CV_RGB(255,0,0);
	CvFont font1;
	int thickness = 2.0;
	char text1[20] = "0"; // frame number
	char text2[20] = "0"; // frame msec positiion
	double hscale = 1.0;
	double vscale = 1.0;
	double shear = 0.0;

	// Check if the user gave arguments
	if(argc != 4) {
		fprintf(stderr, "\nUSAGE: %s <input_video_file> <output_TXT_file> <threshold>\n", argv[0]);
		return EXIT_FAILURE;
	}



	/**** STAGE 1: PROCESS FRAMES ****/
	
	capture = cvCreateFileCapture(argv[1]);	// Open video file to start capture
	if(!capture) {
		printf("Error opening video file! (cvCreateFileCapture)\n");
		return EXIT_FAILURE;
	}

	fp=fopen(argv[2],"w");	// Open file to write stats
	if(fp == NULL) {
		printf("Error opening file! (fopen)\n");
		return EXIT_FAILURE;
	}
	
	threshold = atoi(argv[3]);	// Threshold to mark frames we want
	
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
	
	cvInitFont(&font1,CV_FONT_HERSHEY_SIMPLEX,hscale,vscale,shear,thickness,CV_AA);
	
	CvPoint pt1 = cvPoint(5,30);
	CvPoint pt2 = cvPoint(5,70);
	
	fprintf(fp,"Filename\t:\t%s\n\nFrame width\t:\t%d\nFrame height\t:\t%d\nFPS\t\t:\t%f\nTotal frames\t:\t%d\n\n\n\n",argv[1],width_img,height_img,fps,total_frames);
	printf("Filename\t:\t%s\n\nFrame width\t:\t%d\nFrame height\t:\t%d\nFPS\t\t:\t%f\nTotal frames\t:\t%d\n\n\n\n",argv[1],width_img,height_img,fps,total_frames);

	printf("Start processing frames...\n\n");
	
	start = clock(); // Start timer

	bgr_frame=cvQueryFrame(capture);												// Grab first frame
	current_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the current frame
	previous_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the previous frame
	cvCopy(bgr_frame,previous_frame,NULL);											// Save the copy
	
	// Grab frames from the video until NULL
	while((bgr_frame=cvQueryFrame(capture)) != NULL) {
		/* When entering this loop, we have already grabbed a frame
		 * so the frame counter starts from 2
		 */
		frame = cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);					// Get the current frame number
		
		cvCopy(bgr_frame,current_frame,NULL);											// Save the copy
		
		/**** START PROCESSING ****/
		absdiff(previous_frame, current_frame, frame, fp, width_img, height_img, &index, threshold);
		/**** END PROCESSING ****/

		cvCopy(bgr_frame,previous_frame,NULL);	// Save the copy

		if(index==1) {
			check_frames[frame]=1;	// It means that the specific frame is marked
		}
		
		printf("Processing frame %d...\r",frame);
		fflush(stdout);
	}
	
	cvReleaseImage(&bgr_frame);			// Release bgr_frame
	cvReleaseImage(&previous_frame);	// Release previous_frame
	cvReleaseImage(&current_frame);			// Release current_frame
	cvReleaseCapture(&capture);			// Release capture
	
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
	
	//If there is no markeed frames, exit
	if(marked_frames == 0) {
		return EXIT_SUCCESS;
	}
	
	
	
	/**** STAGE 2: EXPORT IMAGES ****/
	
	capture = cvCreateFileCapture(argv[1]);	// Re-Open video file to start capture
	if(!capture) {
		printf("Error opening video file! (cvCreateFileCapture)\n");
		return EXIT_FAILURE;
	}

	p[0] = CV_IMWRITE_PNG_COMPRESSION;
	p[1] = 0;
	p[2] = 0;
	
	printf("Start writing frames...\n\n");
	
	start = clock(); // Start timer

	bgr_frame = cvQueryFrame(capture);	// Retrieve frame
	new_frame = cvCreateImage(size, bgr_frame->depth, bgr_frame->nChannels);	// Create the new frame
	
	do {
		frame = cvGetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES);	// Get the current frame number
		msec = cvGetCaptureProperty(capture,CV_CAP_PROP_POS_MSEC);
		msec=msec/1000;
		
		// If the index number of the current frame is equal to the frame we want, then write it to the stream.
		if(frame == list_of_frames[frame]) {
			cvCopy(bgr_frame,new_frame,NULL);	// Save the copy
			sprintf(buffer,"%06d",ci);			// number of digits in the image name
			strcat(buffer,".png");				// Add at the end of the image the file extension
			
			sprintf(text1,"%d frame",frame); // int to char via sprintf()
			cvPutText(new_frame,text1,pt1,&font1,black); // frame number

			sprintf(text2,"%d sec",msec); // int to char via sprintf()
			cvPutText(new_frame,text2,pt2,&font1,black); // frame msec position
			
			cvSaveImage(buffer,new_frame,p);	// Save current frame
			ci++;
		} else {
			sprintf(buffer,"%06d",ci);			// number of digits in the image name
			strcat(buffer,".png");				// Add at the end of the image the file extension
			cvSaveImage(buffer,new_frame,p);	// Save current frame
			ci++;
		}
		
		printf("Writing frame %d...\r",frame);
		fflush(stdout); // For '/r' to work we have to flush the output stream
	} while((bgr_frame=cvQueryFrame(capture)) != NULL);
		
	stop = clock(); 		// Stop timer
	diff = stop - start;	// Get difference between start time and current time;
	printf("\n\nTotal time writing frames : %f minutes\t%f seconds\n", (((float)diff)/CLOCKS_PER_SEC)/60, ((float)diff)/CLOCKS_PER_SEC);
	printf("Writing completed!\n\n");
	
	fclose(fp);					// Close file pointer
	free(list_of_frames);		// Free list_of_frames
	free(check_frames);			// Free check_frames
	cvReleaseImage(&bgr_frame);	// Release bgr_frame
	cvReleaseImage(&new_frame);	// Release new_frame
	cvReleaseCapture(&capture);	// Release capture

	return EXIT_SUCCESS;
}
