#include <stdio.h> 
#include <stdlib.h>
#include <math.h>   
#include <assert.h>

#include "../include/fft.h"

#define  M_PI 3.1415926


/*********************************************************************************************
对N点序列进行fft变换：
1. A_re为该序列的实部，A_im为该序列的虚部；
2. 运算结果仍然存放在A_re和A_im 
3. 输入输出都是自然顺序

Note：
N必须为2的整数次幂
**********************************************************************************************/
void fft_1d(int N, double *A_re, double *A_im) 
{ 
	double *W_re, *W_im;

	W_re = (double*)malloc(sizeof(double)*N/2); 
	W_im = (double*)malloc(sizeof(double)*N/2); 
	assert(W_re != NULL && W_im != NULL);

	compute_W(N, W_re, W_im);
	fft_butterfly(N, A_re, A_im, W_re, W_im);
	permute_bitrev(N, A_re, A_im);

	free(W_re);
	free(W_im);

}
void _fft_2d(int W,int H,double *A_re,double *A_im,int shift)
{
	double* tmp_re = NULL;
	tmp_re = A_re;
	double* tmp_im = NULL;
	tmp_im = A_im;
	int n,i,j;
	for(n = 0 ; n < H ;n++)
	{
		fft_1d(W,tmp_re,tmp_im);
		tmp_re += W;
		tmp_im += W;
	}
	tmp_re = A_re;
	tmp_im = A_im;
	double *vLineRe;
	vLineRe = (double *)malloc(sizeof(double)*H);
	double *vLineIm;
	vLineIm = (double *)malloc(sizeof(double)*H);
	for(n =0 ; n < W ; n++)
	{
		for(j = 0 ; j < H ; j++)
		{
			vLineRe[j] = tmp_re[j*W];
			vLineIm[j] = tmp_im[j*W];
		}
		fft_1d(H,vLineRe,vLineIm);
		for(j = 0 ; j < H ; j++)
		{
			tmp_re[j*W] = vLineRe[j];
			tmp_im[j*W] = vLineIm[j];
		}
		tmp_re += 1;
		tmp_im += 1;
	}
	free(vLineRe);
	free(vLineIm);
}

void _ifft_2d(int W,int H,double *A_re,double *A_im,int shift)
{
	int i;
	for(i = 0; i<W*H ; i++)
	{
		A_im[i] *= -1;
	}
	_fft_2d(W,H,A_re,A_im,shift);
	for(i = 0; i<W*H ; i++)
	{
		A_im[i] *= -1;
	}
	for(i = 0; i<W*H ; i++)
	{
		A_im[i] /= (W*H);
		A_re[i] /= (W*H);
	}
}

/* W will contain roots of unity so that W[bitrev(i,log2n-1)] = e^(2*pi*i/n)
* n should be a power of 2
* Note: W is bit-reversal permuted because fft(..) goes faster if this is done.
*       see that function for more details on why we treat 'i' as a (log2n-1) bit number.
*/
void compute_W(int n, double *W_re, double *W_im)
{
	int i, br;
	int log2n = log_2(n);

	for (i=0; i<(n/2); i++)
	{
		br = bitrev(i,log2n-1); 
		W_re[br] = cos(((double)i*2.0*M_PI)/((double)n));  
		W_im[br] = -sin(((double)i*2.0*M_PI)/((double)n));  
	}

}


/* permutes the array using a bit-reversal permutation */ 
void permute_bitrev(int n, double *A_re, double *A_im) 
{ 
	int i, bri, log2n;
	double t_re, t_im;

	log2n = log_2(n); 

	for (i=0; i<n; i++)
	{
		bri = bitrev(i, log2n);

		/* skip already swapped elements */
		if (bri <= i) continue;

		t_re = A_re[i];
		t_im = A_im[i];
		A_re[i]= A_re[bri];
		A_im[i]= A_im[bri];
		A_re[bri]= t_re;
		A_im[bri]= t_im;
	}  
} 


/* treats inp as a numbits number and bitreverses it. 
* inp < 2^(numbits) for meaningful bit-reversal
*/ 
int bitrev(int inp, int numbits)
{
	int i, rev=0;
	for (i=0; i < numbits; i++)
	{
		rev = (rev << 1) | (inp & 1);
		inp >>= 1;
	}
	return rev;
}


/* returns log n (to the base 2), if n is positive and power of 2 */ 
int log_2(int n) 
{
	int res; 
	for (res=0; n >= 2; res++) 
		n = n >> 1; 
	return res; 
}

/* fft on a set of n points given by A_re and A_im. Bit-reversal permuted roots-of-unity lookup table
* is given by W_re and W_im. More specifically,  W is the array of first n/2 nth roots of unity stored
* in a permuted bitreversal order.
*
* FFT - Decimation In Time FFT with input array in correct order and output array in bit-reversed order.
*
* REQ: n should be a power of 2 to work. 
*
* Note: - See www.cs.berkeley.edu/~randit for her thesis on VIRAM FFTs and other details about VHALF section of the algo
*         (thesis link - http://www.cs.berkeley.edu/~randit/papers/csd-00-1106.pdf)
*       - See the foll. CS267 website for details of the Decimation In Time FFT implemented here.
*         (www.cs.berkeley.edu/~demmel/cs267/lecture24/lecture24.html)
*       - Also, look "Cormen Leicester Rivest [CLR] - Introduction to Algorithms" book for another variant of Iterative-FFT
*/
void fft_butterfly(int n, double *A_re, double *A_im, double *W_re, double *W_im) 
{
	double w_re, w_im, u_re, u_im, t_re, t_im;
	int m, g, b;
	int mt, k;

	/* for each stage */  
	for (m=n; m>=2; m=m>>1) 
	{
		/* m = n/2^s; mt = m/2; */
		mt = m >> 1;

		/* for each group of butterfly */ 
		for (g=0,k=0; g<n; g+=m,k++) 
		{
			/* each butterfly group uses only one root of unity. actually, it is the bitrev of this group's number k.
			* BUT 'bitrev' it as a log2n-1 bit number because we are using a lookup array of nth root of unity and
			* using cancellation lemma to scale nth root to n/2, n/4,... th root.
			*
			* It turns out like the foll.
			*   w.re = W[bitrev(k, log2n-1)].re;
			*   w.im = W[bitrev(k, log2n-1)].im;
			* Still, we just use k, because the lookup array itself is bit-reversal permuted. 
			*/
			w_re = W_re[k];
			w_im = W_im[k];

			/* for each butterfly */ 
			for (b=g; b<(g+mt); b++) 
			{
				/* t = w * A[b+mt] */
				t_re = w_re * A_re[b+mt] - w_im * A_im[b+mt];
				t_im = w_re * A_im[b+mt] + w_im * A_re[b+mt];

				/* u = A[b]; in[b] = u + t; in[b+mt] = u - t; */
				u_re = A_re[b];
				u_im = A_im[b];
				A_re[b] = u_re + t_re;
				A_im[b] = u_im + t_im;
				A_re[b+mt] = u_re - t_re;
				A_im[b+mt] = u_im - t_im;

			}
		}
	}
}
