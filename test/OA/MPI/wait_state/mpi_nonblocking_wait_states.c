/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2011,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2011,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2011,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2011,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 */

/* *INDENT-OFF* */
#include <config.h>

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <scorep/SCOREP_User.h>

int
main( int    argc,
      char** argv )
{
    int	retVal = 0; /* return value */
    int k, myrank=0, np=1;

    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if(np%2!=0)
    {
        printf("Wrong number of processes, has to be even\n");
        return 1;
    }

    for(k=0;k<1;k++)
    {
        SCOREP_USER_REGION_DEFINE( mainRegion );
        SCOREP_USER_OA_PHASE_BEGIN( mainRegion, "mainRegion", SCOREP_USER_REGION_TYPE_COMMON);

        int comm_partner;
        int tag=0;
        int buffer1=-1;
        int buffer2=-1;
        int buffer3=-1;
        int buffer4=-1;
        void* mpi_buffer=malloc(MPI_BSEND_OVERHEAD+sizeof(int));
        MPI_Buffer_attach(mpi_buffer,MPI_BSEND_OVERHEAD+sizeof(int));

        MPI_Status status;
        MPI_Status statuses[3];
        MPI_Request requests[3];
        MPI_Request request;
        int indices[3];
        int index;
        if(myrank%2==0)
        {
                comm_partner=myrank+1;
                buffer1=0;
                buffer2=1;
                buffer3=2;
                buffer4=3;
                printf("Starting sends...\n");
//                MPI_Issend(&buffer1,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&request);
                MPI_Ibsend(&buffer2,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[0]));
		// MPI Error: ready send requires that the receive is already posted.
		// This is not guaranteed by this code.
                // Because of the sleep before the receive it is rather unlikely that the
                // receive is already posted.
                MPI_Isend(&buffer3,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[1]));
                MPI_Isend(&buffer4,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[2]));
//                printf("Cancelling send 1\n");
//                MPI_Cancel(&request);
//                printf("Freeing request send 1\n");
//                PMPI_Request_free(&request);

                printf("Waiting for sends...\n");

                MPI_Waitall(3,requests,statuses);
                MPI_Waitany(3,requests,&index,statuses);
                MPI_Waitsome(3,requests,&index,indices,statuses);


        }
        else
        {
                comm_partner=myrank-1;
                sleep(5);
//				  MPI_Recv(&buffer1,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&status);
//				  printf("0 Received %d\n",buffer1);
//				  MPI_Recv(&buffer1,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&status);
//				  printf("1 Received %d\n",buffer1);
//				  MPI_Recv(&buffer1,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&status);
//				  printf("2 Received %d\n",buffer1);
//				  printf("Receives completed\n");
//                MPI_Irecv(&buffer1,1,MPI_INT,comm_partner,100,MPI_COMM_WORLD,&(requests[0]));
                printf("Starting receives...\n");
                MPI_Irecv(&buffer2,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[0]));
                MPI_Irecv(&buffer3,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[1]));
                MPI_Irecv(&buffer4,1,MPI_INT,comm_partner,tag,MPI_COMM_WORLD,&(requests[2]));
                sleep(3);
                printf("Starting tests...\n");
                int flag=0;
                while(!flag)
                {
				MPI_Test(&(requests[0]),&flag,&status);
				MPI_Testany(3,requests,&index,&flag,&status);
				MPI_Testsome(3,requests,&index,indices,statuses);
				MPI_Testall(3,requests,&flag,statuses);

                }
                printf("Tests completed\n");
                //MPI_Waitall(4,requests,statuses);

        }


        tag++;

        SCOREP_USER_OA_PHASE_END( mainRegion );
        int buf_size;
        MPI_Buffer_detach(mpi_buffer,&buf_size);
        free(mpi_buffer);
    }

    MPI_Finalize();
    return retVal;
}
