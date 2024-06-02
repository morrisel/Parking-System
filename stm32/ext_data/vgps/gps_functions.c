/** ---------------------------------------------------------- gps_functions.c --- *
 *                                                                                 *
 *  Implements functions for GPS simulation, including generating random           *
 *  coordinates, checking for movement, and simultating GPS data logging.          *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#include "gps_creator.h"
#include "gps_functions.h"

/* Function for generating random coordinates */
Coordinates generateRandomCoordinates()
{
	Coordinates coords;

	coords.x = (rand() % 100) + ((double)(rand() % 100) / 100);
	coords.y = (rand() % 100) + ((double)(rand() % 100) / 100);
	coords.z = (rand() % 100) + ((double)(rand() % 100) / 100);

	return coords;
}

/* Function to determine movement */
bool isMoving(Coordinates prev, Coordinates curr)
{
	return (prev.x != curr.x) || (prev.y != curr.y) || (prev.z != curr.z);
}

/* Function simulating GPS */
void simulateGPS(
		Coordinates *currpos,
		bool        *istimeron,
		int         *stimer,
		FILE        *alldf,
		FILE        *tmpdf)
{
	*currpos = generateRandomCoordinates();

	/* Output to screen */
	printf("Data from GPS: x %.2lf y %.2lf z %.2lf\n",
			currpos->x,
			currpos->y,
			currpos->z);

	/* Write to all data file */
	fprintf(alldf, "Data from GPS: x %.2lf y %.2lf z %.2lf\n",
			currpos->x,
			currpos->y,
			currpos->z);

	/* Write to temporary data file */
	fprintf(tmpdf, "Data from GPS: x %.2lf y %.2lf z %.2lf\n",
			currpos->x,
			currpos->y,
			currpos->z);

	/* Compare with previous coordinates */
	if (isMoving(*currpos, *currpos))
	{
		*stimer = 0; /* Reset static timer */
	}
	else
	{
		(*stimer) += 2;  /* Increase static timer */
	}

	/* 5 seconds without movement */
	if (*stimer >= 5)
	{
		*istimeron = false; /* Turn off dynamic timer */
	}
}
