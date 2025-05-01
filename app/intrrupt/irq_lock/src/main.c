#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <inttypes.h>

#define SW0_NODE DT_ALIAS(sw0)

#if !DT_NODE_HAS_STATUS_OKAY(SW0_NODE)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

/* Shared resource */
static volatile int button_press_count = 0;

/* Button interrupt handler */
void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk(">> Button interrupt triggered\n");

    printk(">> Locking IRQs...\n");
    unsigned int key = irq_lock();  // Lock interrupts

    /* Critical section begins */
    button_press_count++;
    printk(">> [CRITICAL] Button press count incremented: %d\n", button_press_count);
    /* Critical section ends */

    irq_unlock(key);  // Unlock interrupts
    printk(">> IRQs unlocked\n");
}

/* Main thread */
int main(void)
{
    int ret;

    if (!gpio_is_ready_dt(&button)) {
        printk("Button device not ready\n");
        return 0;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    printk("=== IRQ Lock/Unlock Button Demo ===\n");

    while (1) {
        printk("Main thread: Running...\n");
        k_msleep(1000);  // Idle work
    }

    return 0;
}

