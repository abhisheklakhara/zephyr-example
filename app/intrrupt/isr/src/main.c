#include <zephyr/kernel.h>              // Needed for k_is_in_isr()
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

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
    
    if (k_is_in_isr()) {
        printk("button_pressed: Running in ISR context\n");
    } else {
        printk("button_pressed: Running in thread context\n");
    }
}

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

    printk("Button setup complete.\n");

    while (1) {
        if (k_is_in_isr()) {
            printk("main: In ISR context (unexpected)\n");
        } else {
            printk("main: In thread context (expected)\n");
        }
        k_msleep(1000);
    }

    return 0;
}

