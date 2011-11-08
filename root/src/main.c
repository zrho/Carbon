/**
 * Carbon Operating System
 * Copyright (C) 2011 Lukas Heidemann
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include <carbon/thread.h>
#include <carbon/futex.h>

#define A_0 1.0
#define B_0 1.0 / sqrt(2.0)
#define T_0 1.0 / 4.0
#define P_0 1.0

typedef struct request_t {
    int iterations;
    double result;
} request_t;

void *worker(void *_req) {
    // Calculate rounds
    request_t *req = (request_t *) _req;
    int i = req->iterations;

    printf("Worker started (%d iterations).\n", i);

    double a = A_0;
    double b = B_0;
    double t = T_0;
    double p = P_0;

    for (; i > 0; --i) {
        double a_new = (a + b) / 2;
        double b_new = sqrt(a * b);
        double t_new = t - p * pow(a - a_new, 2);
        double p_new = 2 * p;

        a = a_new;
        b = b_new;
        t = t_new;
        p = p_new;
    }

    req->result = pow(a + b, 2) / (4 * t);
	return &req->result;
}

int main(void) {
	pthread_t worker_tid;
	request_t request;
	double *result_ptr;

	request.iterations = 2;

	printf("Creating worker...\n");
	pthread_create(&worker_tid, 0, &worker, (void *) &request);
	pthread_join(worker_tid, (void *) &result_ptr);
	printf("Result received.\n");
	printf("Result: %f\n", *result_ptr);

    return 0;
}
