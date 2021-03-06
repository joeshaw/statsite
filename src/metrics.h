#ifndef METRICS_H
#define METRICS_H
#include <stdint.h>
#include "counter.h"
#include "timer.h"
#include "hashmap.h"

typedef enum {
    UNKNOWN,
    KEY_VAL,
    COUNTER,
    TIMER
} metric_type;

typedef struct key_val {
    char *name;
    double val;
    struct key_val *next;
} key_val;

typedef struct {
    hashmap *counters;  // Hashmap of name -> counter structs
    hashmap *timers;    // Map of name -> timer structs
    key_val *kv_vals;   // Linked list of key_val structs
    double eps;         // The error for timers
    double *quantiles;  // Array of quantiles
    uint32_t num_quants;    // Size of quantiles array
} metrics;

typedef int(*metric_callback)(void *data, metric_type type, char *name, void *val);

/**
 * Initializes the metrics struct.
 * @arg eps The maximum error for the quantiles
 * @arg quantiles A sorted array of double quantile values, must be on (0, 1)
 * @arg num_quants The number of entries in the quantiles array
 * @return 0 on success.
 */
int init_metrics(double eps, double *quantiles, uint32_t num_quants, metrics *m);

/**
 * Initializes the metrics struct, with preset configurations.
 * This defaults to a epsilon of 0.01 (1% error), and quantiles at
 * 0.5, 0.95, and 0.99.
 * @return 0 on success.
 */
int init_metrics_defaults(metrics *m);

/**
 * Destroys the metrics
 * @return 0 on success.
 */
int destroy_metrics(metrics *m);

/**
 * Adds a new sampled value
 * arg type The type of the metrics
 * @arg name The name of the metric
 * @arg val The sample to add
 * @return 0 on success.
 */
int metrics_add_sample(metrics *m, metric_type type, char *name, double val);

/**
 * Iterates through all the metrics
 * @arg m The metrics to iterate through
 * @arg data Opaque handle passed to the callback
 * @arg cb A callback function to invoke. Called with a type, name
 * and value. If the type is KEY_VAL, it is a pointer to a double,
 * for a counter, it is a pointer to a counter, and for a timer it is
 * a pointer to a timer. Return non-zero to stop iteration.
 * @return 0 on success.
 */
int metrics_iter(metrics *m, void *data, metric_callback cb);

#endif
