//mpiexec -n 3 projectname.exe
#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <Windows.h>
#define MAX_MESSAGE_SIZE 100
#define BUFFER_SIZE 100

using namespace std;


void first_task(int *buffer_size, int *message_size) {
	int TAG = 0;
	int number_from_console;
	MPI_Status status;
	cout << "Enter number for process 1: ";
	cin >> number_from_console;

	MPI_Send(&number_from_console, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD);
	number_from_console *= 2;
	MPI_Send(&number_from_console, 1, MPI_INT, 2, TAG, MPI_COMM_WORLD);


	int buf_size = *buffer_size;
	char * buf = (char*)malloc(buf_size);
	
	MPI_Recv(&buf, buf_size, MPI_CHAR, 1, TAG, MPI_COMM_WORLD, &status);
	cout << "PROCESS #1 RECEIVE BUFFER FROM PROCESS #2: " << buf << endl;
	
	char * buffer = (char *)malloc(buf_size);
	MPI_Buffer_attach(buffer, buf_size + MPI_BSEND_OVERHEAD);
	buffer = buf;
	MPI_Bsend(&buffer, buf_size, MPI_CHAR, 2, TAG, MPI_COMM_WORLD);
	MPI_Buffer_detach(&buffer, &buf_size);

	free(buffer);

	char * message = (char*)malloc(*message_size + 1);
	MPI_Recv(message, *message_size + 1, MPI_CHAR, 2, TAG, MPI_COMM_WORLD, &status);
	free(message);
}
void second_task(int *buffer_size, int *message_size) {
	int TAG = 0;
	int number_from_first_process;
	MPI_Status status;
	MPI_Recv(&number_from_first_process, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	cout << "PROCESS #2 RECEIVE NUMBER FROM PROCESS #1: " << number_from_first_process << endl;

	const char * my_message = "and ba a sleep to sab we end : ";
	*buffer_size = sizeof(my_message);
	int buf_size = *buffer_size;
	char *buf = (char*)malloc(buf_size + MPI_BSEND_OVERHEAD);
	
	MPI_Buffer_attach(buf, buf_size + MPI_BSEND_OVERHEAD);
	buf = (char*)my_message;
	
	MPI_Bsend(&buf, buf_size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
	cout << "PROCESS #2 SEND MESSAGE " << buf << endl;
	MPI_Buffer_detach(&buf, &buf_size);
	free(buf);

	char * message = (char*)malloc(*message_size + 1);
	MPI_Recv(message, *message_size + 1, MPI_CHAR, 2, TAG, MPI_COMM_WORLD, &status);
	cout << "PROCESS #2 RECEIVE MESSAGE FROM PROCESS #3: " << message << endl;
	free(message);
	
}
void thirth_task(int *buffer_size, int *message_size, char * word) {
	
	int TAG = 0;
	int number_from_first_process;
	MPI_Status status;
	MPI_Recv(&number_from_first_process, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &status);
	cout << "PROCESS #3 RECEIVE MESSAGE FROM PROCESS #1: " << number_from_first_process << endl;
	
	char * buf = (char*)malloc(*buffer_size);
	MPI_Recv(&buf, *buffer_size, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &status);
	cout << "PROCESS #3 RECEIVE MESSAGE FROM PROCESS #1: " << buf << endl;
	

	char * message = (char *)malloc(*message_size+1);
	

	strncpy(message, word, *message_size + 1);

	for (int i = 0; i < 2; ++i)
		MPI_Ssend(message, *message_size + 1, MPI_CHAR, i, TAG, MPI_COMM_WORLD);

	free(message);

}

int main(int argc, char * argv[]) {
	int message_size = 4, buffer_size = sizeof("and by a sleep to say we end : ");
	MPI_Init(&argc, &argv); int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	if (rank == 0) {
		first_task(&buffer_size, &message_size);
	}
	else if (rank == 1) {
		second_task(&buffer_size, &message_size);
	}
	else if (rank == 2) {
		thirth_task(&buffer_size, &message_size, argv[1]);
	}
	MPI_Finalize();

	return 0;
}
