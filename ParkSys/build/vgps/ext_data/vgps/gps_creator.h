/** ------------------------------------------------------------ gps_creator.h --- *
 *                                                                                 *
 * This header file defines the Coordinates structure and declares                 *
 * functions for GPS simulation.                                                   *
 *                                                                                 *
 * ------------------------------------------------------------------------------ **/
#ifndef GPS_CREATOR_H
#define GPS_CREATOR_H

/* Structure for storing coordinates */
typedef struct
{
	double x;
	double y;
	double z;
} Coordinates;

/* Function declarations */
Coordinates generateRandomCoordinates();

bool isMoving(
		Coordinates        previousPosition,
		Coordinates        currentPosition);

void simulateGPS(
		Coordinates        *currentPosition, 
		bool               *isTimerOn,
		int                *staticTimer,
		FILE               *allDataFile,
		FILE               *tempDataFile);

#endif /* GPS_CREATOR_H */
