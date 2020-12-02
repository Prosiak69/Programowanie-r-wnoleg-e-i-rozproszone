#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "RLE_Parsing.h"

void CalculateUpdate(char* readFrom, char* writeTo, int sizeX, int sizeY);
void Output(int my_rank, int total_ranks, FILE* outputfile, char* mat_start, int sizeX, int sizeY, int step);
void ExchangeGhostlines(int my_rank,int total_ranks,int matrix_dim_x, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high);
void ExchangeGhostlinesNonblocking(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high);

int main(int argc, char *argv[])
{
	int MAX_STEPS = 100;
	int OUTPUT_EVERY_X_STEPS = 20;
	if(argc >= 3)
    {
		MAX_STEPS = atoi(argv[2]);
		OUTPUT_EVERY_X_STEPS = atoi(argv[3]);
	}

	FILE *outputfile;

	MPI_Init(&argc,&argv);
	int my_rank,total_ranks;
	MPI_Comm_size(MPI_COMM_WORLD,&total_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);


	if(my_rank == 0)
    {
	 	outputfile = fopen("conway_output.txt", "w");
		if (outputfile == NULL)
		{
		    printf("Error opening file!\n");
		}
	}
	if(my_rank ==0 && argc < 2)
    {
		printf("Please provide a file name");
		MPI_Abort(MPI_COMM_WORLD,1);

	}

	if(total_ranks < 3)
    {
 		if(my_rank==0)
        {
            printf("Please try with a least 3 ranks!");
        }
		return 0;
	}

	char* my_matrix_part_writeeven;
	char* my_matrix_part_writeodd;
	int matrix_dim_x = -1;
	int total_matsize_y = -1;
	loadFile(argv[1],&my_matrix_part_writeodd,&my_matrix_part_writeeven, &matrix_dim_x, &total_matsize_y);
	MPI_Barrier(MPI_COMM_WORLD);

	if(my_rank == 0)
    {
        printf("Finished parsing data\n");
    }
	if(my_rank != 0 || outputfile != NULL)
    {
		Output(my_rank,total_ranks,outputfile,my_matrix_part_writeodd,matrix_dim_x,total_matsize_y,-1);
	}

	for(int step_count = 0; step_count <= MAX_STEPS; step_count++)
    {
		char* pos_of_my_low = my_matrix_part_writeeven + matrix_dim_x*(total_matsize_y-2);
		if(step_count % 2 == 0)
        {
			pos_of_my_low = my_matrix_part_writeodd + matrix_dim_x*(total_matsize_y-2);
		}

		char* pos_of_my_high = my_matrix_part_writeeven + matrix_dim_x;
		if(step_count % 2 == 0)
        {
			pos_of_my_high = my_matrix_part_writeodd + matrix_dim_x;
		}

		char* my_received_ghostline_top = my_matrix_part_writeodd;
		if(step_count % 2 == 1){
 			my_received_ghostline_top = my_matrix_part_writeeven;
		}
		char* my_received_ghostline_bot = my_matrix_part_writeodd + matrix_dim_x*(total_matsize_y-1);
		if(step_count % 2 == 1){
 			my_received_ghostline_bot = my_matrix_part_writeeven + matrix_dim_x*(total_matsize_y-1);
		}

		ExchangeGhostlines(my_rank,total_ranks,matrix_dim_x,my_received_ghostline_top,my_received_ghostline_bot,pos_of_my_low,pos_of_my_high);

		if(step_count % 2 ==0)
        {
			CalculateUpdate(my_matrix_part_writeodd,my_matrix_part_writeeven,matrix_dim_x,total_matsize_y);
		}
        else
        {
			CalculateUpdate(my_matrix_part_writeeven,my_matrix_part_writeodd,matrix_dim_x,total_matsize_y);
		}

		MPI_Barrier(MPI_COMM_WORLD);



		if(my_rank != 0 || outputfile != NULL)
        {
			if(step_count % OUTPUT_EVERY_X_STEPS == 0)
            {
				if(step_count % 2 == 0)
                {
					Output(my_rank,total_ranks,outputfile,my_matrix_part_writeeven,matrix_dim_x,total_matsize_y,step_count);
				} 
                else 
                {
					Output(my_rank,total_ranks,outputfile,my_matrix_part_writeodd ,matrix_dim_x,total_matsize_y,step_count);
				}
			}
		}

		if(my_rank == 0 && step_count % 10 == 0)
        {
			printf("Finished step %d\n",step_count);
		}
		MPI_Barrier( MPI_COMM_WORLD );
	}


	if(my_rank == 0)
    {
		fclose(outputfile);
	}
	free(my_matrix_part_writeeven);
	free(my_matrix_part_writeodd);
	MPI_Finalize();

	return 0;
}


void CalculateUpdate(char* readFrom, char* writeTo, int sizeX, int totalSizeY)
{
	for(int x=0;x<sizeX;x++)
    {
		for(int y=1;y<totalSizeY-1;y++){
			writeTo[x + sizeX*y] = readFrom[ x + sizeX * (y - 1) ];
		}
	}
}





void ExchangeGhostlines(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high)
{
	for(int i=0;i<size;i++)
    {
		my_received_ghostline_top[i] = 0;
		my_received_ghostline_bot[i] = 0;
	}
};



void ExchangeGhostlinesNonblocking(int my_rank,int total_ranks,int size, char* my_received_ghostline_top,char* my_received_ghostline_bot,char* pos_of_my_low,char* pos_of_my_high)
{
};


void Output(int my_rank, int total_ranks, FILE* outputfile, char* mat_start, int sizeX, int sizeY, int step)
{
 	static unsigned char white[3];
	white[0]=255; white[1]=255; white[2]=255;

 	static unsigned char black[3];
	black[0]=0; black[1]=0; black[2]=0;

	if(my_rank == 0) {
		char filename[100];
		sprintf(filename,"ConwayOutput_Step%d.ppm",step);
		FILE *fp = fopen(filename, "wb"); 
		(void) fprintf(fp, "P6\n%d %d\n255\n", sizeX, (sizeY-2)*total_ranks - 1);

		fprintf(outputfile, "Step %d: \n", step);
		char* subbuff = malloc((sizeX + 2)*sizeof(char));

		for(int iy=1;iy<sizeY - 2;iy++){
			memcpy( subbuff,  &mat_start[iy * sizeX], sizeX );

			for(int ib = 0; ib < sizeX; ib++){
				if(subbuff[ib]==0){
					subbuff[ib]='-';
					(void) fwrite(black, 1, 3, fp);
				}
				else {
					subbuff[ib]='O';
					(void) fwrite(white, 1, 3, fp);
				};
			}

			subbuff[sizeX] = '\n';
			subbuff[sizeX + 1] = '\0';
			fprintf(outputfile, "%s", subbuff );

		}

		char* recvbuff = malloc( (sizeX*(sizeY))*sizeof(char) );
		for(int proc = 1; proc < total_ranks; proc++){
			MPI_Recv(recvbuff,sizeX * (sizeY-2),MPI_CHAR,proc,4,MPI_COMM_WORLD,MPI_STATUS_IGNORE);

			for(int line=0;line<sizeY-2;line++){
				memcpy( subbuff,  &recvbuff[line * sizeX], sizeX );
				for(int ib = 0; ib < sizeX; ib++){

					if(subbuff[ib]==0){
						subbuff[ib]='-';
						fwrite(black, 1, 3, fp);
					}
					else {
						subbuff[ib]='O';
						fwrite(white, 1, 3, fp);
					};
				}

				subbuff[sizeX] = '\n';
				subbuff[sizeX + 1] = '\0';

				fprintf(outputfile, "%s", subbuff );
			}
		}

		fclose(fp);

		free(subbuff);
	} else {
		MPI_Send(mat_start + 1*sizeX,sizeX * (sizeY-2),MPI_CHAR,0,4,MPI_COMM_WORLD);
	}
}
