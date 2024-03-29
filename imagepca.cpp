#include<stdio.h>
#include<cv.h>
#include<highgui.h> 

#include"matrix.h"

#define THREAD_LIMIT 3

struct pca_data
{
	int thread_id, height, width;
	CvMat *m, *m_t, *mcov, *D, *U, *V, *U2, *U2T, *mResult, *matapprox;
};

void *PrincipalComponents(void *threadarg)
{
	int i, j, num;
	struct pca_data *thread;
	thread = (struct pca_data *)threadarg;
	//Finding Transpose of the matrix
	cvTranspose(thread->m, thread->m_t);
	//Calculating covariance matrix
	cvmMul(thread->m, thread->m_t, thread->mcov); 
	for(i = 0 ; i < thread->height ; i++)
  		for(j = 0 ; j < thread->height ; j++)
  		{
			num = cvmGet(thread->mcov, i, j); 
   			num /= ((thread->width)-1); 
   			cvmSet(thread->mcov, i, j, num);
		}
	//Finding Eigen vectors
	cvSVD(thread->mcov, thread->D, thread->U, thread->V);
	//Creating reduced data set Ureduce
	for(i = 0 ; i < thread->height ; i++)
  		for(j = 0 ; j < floor((thread->height)*0.5) ; j++)
    			cvmSet(thread->U2, i, j, cvmGet(thread->U, i, j));

 	cvmTranspose(thread->U2, thread->U2T); 
	cvmMul(thread->U2T, thread->m, thread->mResult);
	cvmMul(thread->U2, thread->mResult, thread->matapprox);

	pthread_exit(NULL);
} 	

struct pca_data data[THREAD_LIMIT];	

int main(int argc, char *argv[])
{
	pthread_t threads[THREAD_LIMIT]; 
 	int i, j, k, t, rc; 
 	float s = 0, num, num2, s2 = 0, n1, n2, n3;
	CvScalar sum;
	double avg; 
 	IplImage *img = 0;  
 	IplImage stub, *dst, *dst1, *dst2, *dst3; 
	CvMat *mt, *mt2;
	void* status;
 	img = cvLoadImage(argv[1], 1); 
	printf("Processing a %dx%d image with %d channels\n", img->height, img->width, img->nChannels); 
 	cvSaveImage("start.jpg", img); 

	matrix m(img->height, img->width);
 	cvConvert(img, m.mat); 
 	m.mat2 = cvCloneMat(m.mat); 
 
 	for(int row = 0; row<m.mat->rows; row++)
 	{
    		float s1 = 0, s2 = 0, s3 = 0;  
    		float* ptr = (float*)(m.mat->data.ptr + row*m.mat->step); 
    		for(int col = 0 ; col < m.mat->cols ; col++)
    		{
        		s1 += *ptr++; 
        		s2 += *ptr++; 
        		s3 += *ptr++; 
    		}
    		s1 /= (m.mat2->cols); 
    		s2 /= (m.mat2->cols); 
    		s3 /= (m.mat2->cols); 
    		float* ptr2 = (float*)(m.mat2->data.ptr + row*m.mat->step); 
    		for(int col = 0 ; col < m.mat2->cols ; col++)
    		{
        		*ptr2 -= s1;  *ptr2++; 
        		*ptr2 -= s2;  *ptr2++; 
        		*ptr2 -= s3;  *ptr2++; 
    		}
 	}
 	for(int row = 0; row < m.mat->rows; row++)
 	{
    		float* ptr = (float*)(m.mat->data.ptr + row*m.mat->step); 
    		for(int col = 0 ; col < m.mat->cols ; col++)
    		{
        		cvmSet(m.r, row, col, *ptr++); 
        		cvmSet(m.g, row, col, *ptr++); 
        		cvmSet(m.b, row, col, *ptr++); 
    		}
 	}
	//Initializing thread structure variable mat objects
	for(i = 0 ; i < THREAD_LIMIT ; i++)
	{
		data[i].height = img->height;
		data[i].width = img->width;
		data[i].m = cvCreateMat(img->height, img->width, CV_32FC1);
		data[i].m_t = cvCreateMat(img->width, img->height, CV_32FC1);
		data[i].mcov = cvCreateMat(img->height, img->height, CV_32FC1);
		data[i].U = cvCreateMat(img->height, img->height, CV_32FC1);
		data[i].D = cvCreateMat(img->height, img->height, CV_32FC1);
		data[i].V = cvCreateMat(img->height, img->height, CV_32FC1);
		data[i].U2 = cvCreateMat(img->height, floor(img->height*0.5), CV_32FC1);
		data[i].U2T = cvCreateMat(floor(img->height*0.5), img->height, CV_32FC1);
		data[i].mResult = cvCreateMat(floor(0.5*img->height), img->width, CV_32FC1);
		data[i].matapprox = cvCreateMat(img->height, img->width, CV_32FC1);
	}
	//Passing data into structure variables
	data[0].m = cvCloneMat(m.r);
	data[1].m = cvCloneMat(m.g);
	data[2].m = cvCloneMat(m.b);
	//Creating 3 threads
	for(t = 0 ; t < THREAD_LIMIT ; t++)
	{
		data[t].thread_id = t;
  		rc = pthread_create(&threads[t], NULL, PrincipalComponents, (void*)&data[t]);
  		if(rc)
  		{
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
    		}
 	}
	//Joining 3 threads
	for(t = 0 ; t < THREAD_LIMIT ; t++)
	{
		rc = pthread_join(threads[t], &status);
		if (rc)
		{
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
    	}
	//Storing results generated by the thread structure variables
	m.rResult = cvCloneMat(data[0].mResult);
	m.gResult = cvCloneMat(data[1].mResult);
	m.bResult = cvCloneMat(data[2].mResult);

 	m.rmatapprox = cvCloneMat(data[0].matapprox); 
 	m.gmatapprox = cvCloneMat(data[1].matapprox); 
 	m.bmatapprox = cvCloneMat(data[2].matapprox); 

	cvMerge(m.rResult, m.gResult, m.bResult, NULL, m.Result); 
 	dst = cvGetImage(m.Result, &stub); 
 	cvSaveImage("compressed.jpg", dst); 
 	cvMerge(m.rmatapprox, m.gmatapprox, m.bmatapprox, NULL, m.matapprox); 
 	dst2 = cvGetImage(m.matapprox, &stub);  
 	cvSaveImage("decompressed.jpg", dst2); 

	mt = cvCreateMat(img->height, img->width, CV_32FC3);
	mt2 = cvCreateMat(img->height, img->width, CV_32FC3);

	//To find percentage similarity between uncompressed and decompressed images
	cvAbsDiff(m.mat, m.matapprox, mt);
	cvDiv(mt, m.mat, mt2, 1);
	sum = cvAvg(mt2);
	avg = (sum.val[0]+sum.val[1]+sum.val[2])/3.0;
	printf("Similarity = %.2f%\n", (1-avg)*100);

	return 0; 
}
