#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/random/random.h>

/* Define FIFO */
K_FIFO_DEFINE(sensor_fifo);

/* Data structure passed via FIFO */
struct sensor_data_t {
    void *fifo_reserved;  // Required by Zephyr FIFO API
    int16_t temperature;
};

/* Simulate ADC sample callback */
void adc_callback(struct k_timer *timer)
{
    struct sensor_data_t *data = k_malloc(sizeof(struct sensor_data_t));
    if (data) {
        data->temperature = sys_rand32_get() % 100;
        k_fifo_put(&sensor_fifo, data);
    }
}

void sensor_thread(void)
{
    while (1) {
        struct sensor_data_t *data = k_fifo_get(&sensor_fifo, K_FOREVER);
        printk("Received temperature: %d°C\n", data->temperature);
        k_free(data);
    }
}

/* Timer for periodic sampling */
K_TIMER_DEFINE(adc_timer, adc_callback, NULL);

/* Stack and thread */
#define STACK_SIZE 1024
#define PRIORITY 5
K_THREAD_STACK_DEFINE(sensor_stack, STACK_SIZE);
static struct k_thread sensor_tid;

void main(void)
{
    printk("Starting FIFO example...\n");

    k_thread_create(&sensor_tid, sensor_stack, STACK_SIZE,
                    (k_thread_entry_t)sensor_thread,
                    NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);

    k_timer_start(&adc_timer, K_SECONDS(1), K_SECONDS(2));
}
