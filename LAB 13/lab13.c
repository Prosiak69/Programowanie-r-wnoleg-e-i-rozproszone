//Mnożenie macierzy przez wektor na kwadratowej siatce procesów


#include "mpi.h"  
#include <stdio.h>  
#include <math.h>  
int main (int argc, char *argv[]) 
{
int myid, myid2d, numprocs, i, j, nidms, newid1, newid2;
double tim;

MPI_Status stat;
MPI_Comm cartcom ,	splitcom1 ,	splitcom2	;

int dims [ 2 ] ; 
int pers [ 2 ] ;
int coord [ 2 ] ;
int rem [ 2 ] ;

int p,q ,n;
double *a ; 
double *x ;
double *t ; 
double *y ;
double *w;

ndims=2;

MPI_Init(&argc ,&argv ) ;
MPI_Comm_size(MPI_COMM_WORLD,&numprocs) ;
MPI_Comm_rank(MPI_COMM_WORLD,&myid) ;

if (myid==0){
scanf ("%d",&n);
}

MPI_Bcast(&n,1 ,MPI_INT,0 ,MPI_COMM_WORLD) ;


dims [0]=0; dims [1]=0;

pers [0]=1;	pers [1]=1;

//	krok	1:	tworzenie	s i a t k i	kartezjań skiej

MPI_Dims_create(numprocs , ndims , dims) ;
MPI_Cart_create(MPI_COMM_WORLD,2 , dims , pers ,1,&cartcom) ;
MPI_Comm_rank(cartcom ,&myid2d) ;
MPI_Cart_coords(cartcom , myid2d ,2 , coord ) ;

p=dims [ 0 ] ;	// dims[0]==dims[1]== sqrt (numprocs)
int myrow=coord [ 0 ] ; 56	int mycol=coord [ 1 ] ;

//	komunikatory	dla	kolumn	i	wierszy

rem[0]=1; rem[1]=0;
MPI_Cart_sub(cartcom ,rem,&splitcom1 ) ;
MPI_Comm_rank( splitcom1 ,&newid1) ;

rem[0]=0; rem[1]=1;
MPI_Cart_sub(cartcom ,rem,&splitcom2 ) ;
MPI_Comm_rank( splitcom2 ,&newid2) ;

//	alokacja	t a b l i c	i	lokalne	generowanie	danych

q=n/p;

a=malloc (q*q*sizeof *a) ;
x=malloc (q*sizeof *x) ; 74 t=malloc (q*sizeof*t ) ; 75 y=malloc (q*sizeof*y) ;
w=malloc (q*sizeof *w) ;

if (mycol==0){
for ( i =0;i<q ; i++){
x [ i ]=(myrow+1)*10+mycol+1;
y [ i ]=(myrow+1)*10+mycol+1;
}
}

for ( i =0;i<q ; i++)
for ( j =0;j<q ; j++)
a [ i *q+j ]=(myrow+1)*10+mycol+1;

if (myid2d==0)
tim=MPI_Wtime() ;

//	krok	2:	x( i ) −> P( i , i )

if (( mycol==0)&&(myrow!=0) ){
MPI_Send(x ,q ,MPI_DOUBLE,myrow,200 , splitcom2 ) ;
}

if ((myrow==mycol)&&(myrow!=0) ){
MPI_Recv(x ,q ,MPI_DOUBLE,0 ,200 , splitcom2 ,&stat ) ;
}

//	krok	3: P( i , i ) −> x( i ) do P(∗ , i )

MPI_Bcast(x ,q ,MPI_DOUBLE, mycol , splitcom1 ) ;

//	krok	4:	obliczenia	lokalne	t<−A( i , j )x( j )

for ( i =0;i<q ; i++){
t [ i ]=0;
for ( j =0;j<q ; j++)
t [ i]+=a [ i *q+j ]*x [ j ] ;
}

//	krok	5:	sumowanie	t	przez P( i ,∗) − wynik w P( i ,0)

MPI_Reduce(t ,w,q ,MPI_DOUBLE,MPI_SUM,0 , splitcom2 ) ;

if (mycol==0){

for ( i =0;i<q ; i++) 121 y [ i]+=w[ i ] ;
}

if (myid2d==0){
tim=MPI_Wtime()−tim ;
printf ("Czas␣␣=␣%l f \n" ,tim) ;
printf ("Mflops=␣%l f \n" ,(2*(double)n/1000000.0)*n/tim) ;

}

//	krok	6:	zwolnienie	utworzonych	komunikatorów

MPI_Comm_free(&splitcom1 ) ;
MPI_Comm_free(&splitcom2 ) ;
MPI_Comm_free(&cartcom) ;
MPI_Finalize () ;
return 0;
}

