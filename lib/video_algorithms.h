#ifndef VIDEO_ALGORITHMS_H
#define VIDEO_ALGORITHMS_H

void absdiff(IplImage *previous_frame, IplImage *current_frame, IplImage *new_frame, int frame, FILE *fp, int width_img, int height_img, int *index_call);
void comprdiff(IplImage *decoded_previous, IplImage *decoded_current, int frame, FILE *fp, int width_img, int height_img, int *index_call);
void ecr_calculation(IplImage *current, IplImage *previous, CvSize size, int frame, double *ecr_call, double *ecr_max_call);
void mark_frames(int frame, FILE *fp, int *index_call, double *ecr_call, double *ecr_max_call);
void ecrdiff(IplImage *current, IplImage *previous, CvSize size, int frame, FILE *fp, int *index_call);
void histdiff(IplImage *previous_frame, IplImage *current_frame, IplImage *new_frame, int frame, FILE *fp, int width_img, int height_img, int *index_call);

#endif