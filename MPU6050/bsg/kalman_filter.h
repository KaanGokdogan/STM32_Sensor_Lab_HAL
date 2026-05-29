/*
 * kalman_filter.h
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_

/**
 * @brief Structure to hold the Kalman Filter state and tuning parameters.
 */
typedef struct
{
    float q_Angle;		// Process noise variance for the accelerometer angle
    float r_Measure;	// Measurement noise variance
    float angle;		// The angle calculated by the Kalman filter
    float p[2][2];		// Error covariance matrix
} KalmanFilter;

/*
 * Functions
 */
void Kalman_Filter_Init(KalmanFilter* kf);
float Kalman_Filter_Get_Angle(KalmanFilter* kalman_Filter, float new_Angle, float dt);

#endif /* KALMAN_FILTER_H_ */
