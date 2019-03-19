/*---单尺度Retinex算法---*/
/*
Rxy = pow(10,(log10(Sxy)-log10(Fxy*Sxy))//是卷积不是乘
利用Rxy值的离散程度来检测图片的光泽程度
完成通道分离，积分，计算lambda，计算Fxy
对三通道分别做SSR_Cal，求和
*/
#include <opencv2/opencv.hpp>
#include "stdio.h"
#include <iostream>

using namespace std;
using namespace cv;

/*---单尺度Retinex后续计算，以及统计学方法：相对平均偏差---*/
/*
后续计算：卷积，做log10，求pow(10,(log10(Sxy[i][j]) - VecB[i][j]))，得到Rxy
(∑fabs(Rxy-average))/average
*/
double SSR_Cal(Mat& src,double** Fxy)
{
    int N1 = src.rows ,N2 = src.rows;
    int M1 = src.cols ,M2 = src.cols;
    /*---Sxy初始化---*/
    double ** Sxy = NULL;
    double ** answer = NULL;
    Sxy = (double **)malloc(src.rows * sizeof(double*));
    answer = (double **)malloc(src.rows * sizeof(double*));
    for(int i = 0;i <src.rows;i++)
    {
        Sxy[i] = (double *)malloc(src.cols * sizeof(double));
        answer[i] = (double *)malloc(src.cols * sizeof(double));
    }
    for(int i = 0;i < src.rows;i++)
    {
        uchar* data = src.ptr<uchar>(i);
        for(int j = 0;j < src.cols;j++)
        {
            Sxy[i][j] = data[j];
            //printf("%lf ",Sxy[i][j]);
        }
        //printf("\n");
    }
    double **VecB = NULL;
    VecB = (double **)malloc((src.rows * 2 -1) * sizeof(double *));
    for(int i = 0;i < src.rows * 2 -1;i++)
    {
        VecB[i] = (double *)malloc((src.cols * 2 - 1) * sizeof(double));
    }
    /*---做卷积然后做log10---*/
    for(int i = 0;i < N1+N2-1;i++)
    {
        for(int j = 0;j < M1+M2-1;j++)
        {
            double temp = 0.0;
            for(int m = 0;m < N1;m++)
            {
                for(int n = 0;n < M1;n++)
                {
                    if((i-m)>=0&&(i-m)<N2&&(j-n)>=0&&(j-n)<M2)
                    {
                        temp += Fxy[m][n] * Sxy[i-m][j-n];
                    }
                }
            }
            VecB[i][j] = log10(temp);
            //printf("%lf ",temp);
        }
        //printf("\n");
    }
    /*---做pow（10，n）得出Rxy---*/
    double sum = 0.0;
    for(int i = 0;i < N1;i++)
    {
        for(int j = 0;j < M1;j++)
        {
            answer[i][j] = pow(10,(log10(Sxy[i][j]) - VecB[i][j]));
            //printf("%lf ",answer[i][j]);
            sum += answer[i][j];
        }
        //printf("\n");
    }
    /*---相对平均偏差---*/
    //sum /= N1 * M1;
    //printf("%lf",sum);
    double aver = sum;
    aver /=(1.0 * N1 * M1);
    printf("%lf", aver);
    sum = 0.0;
    for(int i = 0;i < N1;i++)
    {
        for(int j = 0;j < M1;j++)
        {
            //sum += fabs(answer[i][j] - aver);
            sum += (answer[i][j] - aver)*(answer[i][j] - aver);
        }
        //printf("\n");
    }
    sum /= (1.0 *N1 * M1);
    sum = sqrt(sum);
    //sum /= aver;
    //sum *= 1000;
    //int guangze = (int)sum;

    free(Sxy);
    free(answer);
    free(VecB);
    return sum;
}

int SSR(Mat& img)
{
    /*---初始参数：高斯环绕尺度C，经验值70~90；三通道初始化---*/
    double c = 80;
    double sum = 0.0;
    double lambda;
    int i ,j;
    Mat rImg,gImg,bImg;
    vector<Mat> channels;
    split(img,channels);
    bImg = channels.at(0);
    gImg = channels.at(1);
    rImg = channels.at(2);
    for(i = 0;i < rImg.rows;i++)
    {
        for(j = 0;j < rImg.cols;j++)
        {
            sum += exp((-1)*((i*i + j *j)/(c*c)));
        }
    }
    lambda = 1.0/sum;
    //printf("%lf,%lf\n",sum,lambda);
    /*---计算Fxy---*/
    double **fx = NULL;
    fx = (double **)malloc(rImg.rows * sizeof(double *));
    for(i = 0;i < rImg.rows;i++)
    {
        fx[i] = (double *)malloc(rImg.cols * sizeof(double));
    }
    for(i = 0;i < rImg.rows;i++)
    {
        for(j = 0;j < rImg.cols;j++)
        {
            fx[i][j] =  lambda * exp((-1)*((i * i + j * j)/(c * c)));           
        }
    }
    printf("\n%f,%f,%f",SSR_Cal(rImg,fx),SSR_Cal(gImg,fx),SSR_Cal(bImg,fx));
    //printf("\n%d",SSR_Cal(rImg,fx)+SSR_Cal(gImg,fx)+SSR_Cal(bImg,fx));
    //free(fx);
    //return SSR_Cal(rImg,fx)+SSR_Cal(gImg,fx)+SSR_Cal(bImg,fx);
    return 0;
}

int main(int argc, char** argv)
{
    char* filename = "test.jpg";
    if(argc > 1){
        filename = argv[1];
    }
    Mat img = imread(filename); 
    int rst = SSR(img);
    cout << rst << endl;
    return 0;
}
