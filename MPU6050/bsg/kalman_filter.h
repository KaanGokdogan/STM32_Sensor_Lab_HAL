/*
 * kalman_filter.h
 *
 *  Created on: 25 May 2026
 *      Author: kaan_
 */

#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_


typedef struct
{
    float q_Angle;
    float r_Measure;
    float angle;
    float p[2][2];
} KalmanFilter;

void Kalman_Filter_Init(KalmanFilter* kf);
float Kalman_Filter_Get_Angle(KalmanFilter* kalman_Filter, float new_Angle, float dt);

#endif /* KALMAN_FILTER_H_ */
