
class matrix
{
    public:
	CvMat *mat, *matapprox, *mat2, *Result, *r, *g, *b, 
	      *r_t, *g_t, *b_t, *rcov, *gcov, *bcov, *Ur, *Ub, *Ug,
	      *V, *D, *U2r, *U2b, *U2g,  *U2Tr, *U2Tb, *U2Tg,
	      *rResult, *gResult, *bResult, *rmatapprox, *gmatapprox, *bmatapprox;

	matrix(int height, int width)
	{
	 	mat = cvCreateMat(height, width, CV_32FC3), 
		matapprox = cvCreateMat(height, width, CV_32FC3), 
		mat2 = cvCreateMat(height, width, CV_32FC3), 		
		Result = cvCreateMat(floor(0.5*height), width, CV_32FC3), 
		r = cvCreateMat(height, width, CV_32FC1), 
		g = cvCreateMat(height, width, CV_32FC1), 
		b = cvCreateMat(height, width, CV_32FC1), 
		rResult = cvCreateMat(floor(0.5*height), width, CV_32FC1), 
		gResult = cvCreateMat(floor(0.5*height), width, CV_32FC1), 
		bResult = cvCreateMat(floor(0.5*height), width, CV_32FC1), 
		rmatapprox = cvCreateMat(height, width, CV_32FC1), 
		gmatapprox = cvCreateMat(height, width, CV_32FC1), 
		bmatapprox = cvCreateMat(height, width, CV_32FC1); 
	}
	
};

