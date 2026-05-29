/*
 * kalman_filter.c
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#include "kalman_filter.h"
#include <stdio.h>
#include <math.h>



/************************************************************************
 * @fn				- Kalman_Filter_Init
 *
 * @brief			- Initializes the Kalman Filter structure with default values.
 *
 * @param[in,out]	- kalman_Filter: Pointer to the KalmanFilter structure.
 *
 * @return			- none
 *
 * @Note			- Sets initial process noise, measurement noise, angle,
 * 					  and error covariance matrix elements to defaults.
 ************************************************************************/
void Kalman_Filter_Init(KalmanFilter* kalman_Filter)
{

    kalman_Filter->q_Angle = 0.001f;
    kalman_Filter->r_Measure = 0.03f;
    kalman_Filter->angle = 0.0f;
    kalman_Filter->p[0][0] = 0.0f;
    kalman_Filter->p[0][1] = 0.0f;
    kalman_Filter->p[1][0] = 0.0f;
    kalman_Filter->p[1][1] = 0.0f;
}


/************************************************************************
 * @fn				- Kalman_Filter_Get_Angle
 *
 * @brief			- Calculates the filtered angle using the Kalman algorithm.
 *
 * @param[in,out]	- kalman_Filter: Pointer to the KalmanFilter structure.
 * @param[in]		- new_Angle: The new angle measured by the accelerometer.
 * @param[in]		- dt: Delta time (time elapsed since last calculation).
 *
 * @return			- float: The updated, filtered angle.
 *
 * @Note			- Currently, the predict step assumes 0.0f gyro rate.
 * 					  For a full 1D Kalman filter, a 'rate' parameter from
 * 					  the gyroscope should be added.
 ************************************************************************/
float Kalman_Filter_Get_Angle(KalmanFilter* kalman_Filter, float new_Angle, float dt)
{
    // Step 1: Predict
    kalman_Filter->angle += 0.0f; // No gyroscope rate, angle remains the same

    kalman_Filter->p[0][0] += dt * (dt * kalman_Filter->p[1][1] - kalman_Filter->p[0][1] - kalman_Filter->p[1][0] + kalman_Filter->q_Angle);
    kalman_Filter->p[0][1] -= dt * kalman_Filter->p[1][1];
    kalman_Filter->p[1][0] -= dt * kalman_Filter->p[1][1];
    kalman_Filter->p[1][1] += 0.0f; // No process noise for bias

    // Step 2: Update
    float s = kalman_Filter->p[0][0] + kalman_Filter->r_Measure;
    float k[2];
    k[0] = kalman_Filter->p[0][0] / s;
    k[1] = kalman_Filter->p[1][0] / s;

    float y = new_Angle - kalman_Filter->angle;
    kalman_Filter->angle += k[0] * y;

    float p00_temp = kalman_Filter->p[0][0];
    float p01_temp = kalman_Filter->p[0][1];

    kalman_Filter->p[0][0] -= k[0] * p00_temp;
    kalman_Filter->p[0][1] -= k[0] * p01_temp;
    kalman_Filter->p[1][0] -= k[1] * p00_temp;
    kalman_Filter->p[1][1] -= k[1] * p01_temp;

    return kalman_Filter->angle;
}
