#include <stdio.h>
#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ultrasonic.h>
#include <esp_err.h>

extern "C"{
    void app_main();
}
/*    Ultrasonic defines  */
#define MAX_DISTANCE_CM 500 // 5m max
#define TRIGGER_GPIO GPIO_NUM_17
#define ECHO_GPIO GPIO_NUM_16
#define MEASURE_FREQ_MS 1000
/*    Ultrasonic defines end*/

void ultrasonic_measure(void *pvParameters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    float *dist = (float*)pvParameters;
    while (true)
    {
        esp_err_t res = ultrasonic_measure(&sensor, MAX_DISTANCE_CM, dist);
        switch (res)
        {
            case ESP_OK:
                *dist = *dist * 100;                
                printf("Distance: %0.1f cm\n", *dist);
                break;
            case ESP_ERR_ULTRASONIC_PING:
                printf("Cannot ping (device is in invalid state)\n");
                break;
            case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                printf("Ping timeout (no device found)\n");
                break;
            case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                printf("Echo timeout (i.e. distance too big)\n");
                break;
            default:
                printf("%s\n", esp_err_to_name(res));
        }
        vTaskDelay(MEASURE_FREQ_MS / portTICK_PERIOD_MS);
    }
}

void app_main()
{
    float distance = -1;
    xTaskCreate(ultrasonic_measure, "ultrasonic_test", configMINIMAL_STACK_SIZE * 3, &distance, 5, NULL);
    while(1)
    {
        vTaskDelay(pdTICKS_TO_MS(1000));
    }
}