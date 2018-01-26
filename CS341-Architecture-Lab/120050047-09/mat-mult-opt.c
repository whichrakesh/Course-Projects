#include <iostream>
#define size 100
using namespace std;

int main()
{
    double A[size][size],B[size][size],C[size][size],D[size][size];
    for (int i=0; i<size; i++)
    {
        for(int j=0; j<size; j++)
        {
            A[i][j] = 0;
            cout << 1 <<'\t' << &A[i][j]<<endl;
            C[i][j] = 0;
            cout << 1 <<'\t' << &C[i][j]<<endl;
            if(i == j){
                B[i][j] = 2;
                cout << 1 <<'\t' << &B[i][j]<<endl;
            } 
            else 
            {
                B[i][j] = 0;
                cout << 1 <<'\t' << &B[i][j]<<endl;
            }
        }        
    }
    for (int j=0; j<size; j++)
    {
        C[0][j] = j;
        cout << 1 <<'\t' << &C[0][j]<<endl;
    }
    for (int i=0; i<size; i++)
        for(int j=0; j<size; j++)
        {
            D[j][i] = C[i][j];
            cout << 0 <<'\t' << &C[i][j]<<endl;
            cout << 1 <<'\t' << &D[j][i]<<endl;
        }
    //----------------------------------------
    
    for (int i=0; i<size; i++)
        for (int j=0; j<size; j++)
            for (int k=0; k<size; k++)
            {
                A[i][j] = A[i][j] + B[i][k]*D[j][k];        
                cout << 0 <<'\t' << &A[i][j] <<endl;
                cout << 0 <<'\t' << &B[i][k] <<endl;
                cout << 0 <<'\t' << &D[j][k] <<endl;
                cout << 1 <<'\t' << &A[i][j] <<endl;        
            }
        
}

