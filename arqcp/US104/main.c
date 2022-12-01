#include <stdint.h>
#include <stdio.h>

#include "generate_base_values.h"
#include "limits.h"
#include "print_result.h"
#include "random.h"
#include "sensores.h"

#define SEC_IN_DAY 86400

#define NUM_TEMPERATURE_REGISTERS SEC_IN_DAY / TEMPERATURES_SENSOR_INTERVAL
#define NUM_VEL_WIND_REGISTERS SEC_IN_DAY / VELOCITY_SENSOR_INTERVAL
#define NUM_DIR_WIND_REGISTERS SEC_IN_DAY / DIRECTION_SENSOR_INTERVAL
#define NUM_PLUVIO_REGISTERS SEC_IN_DAY / PLUVIO_SENSOR_INTERVAL
#define NUM_SOIL_HUMIDITY_REGISTERS SEC_IN_DAY / SOIL_HUMIDITY_SENSOR_INTERVAL
#define NUM_AIR_HUMIDITY_REGISTERS SEC_IN_DAY / AIR_HUMIDITY_SENSOR_INTERVAL

#define NUM_OF_SENSORS 6

uint64_t state = 0;
uint64_t inc = 0;

int main(void) {
    reset_seed();

    int **data[NUM_OF_SENSORS];

    int *temp_sensors[N_OF_TEMP_SENSORS];
    int *vel_wind_sensors[N_OF_VELOCITY_SENSORS];
    int *dir_wind_sensors[N_OF_DIRECTION_SENSORS];
    int *pluvio_sensors[N_OF_PLUVIO_SENSORS];
    int *soil_humidity_sensors[N_OF_SOIL_HUMIDITY_SENSORS];
    int *air_humidity_sensors[N_OF_AIR_HUMIDITY_SENSORS];

    data[TEMPERATURE_SENSORS_INDEX] = temp_sensors;
    data[VELOCITY_SENSORS_INDEX] = vel_wind_sensors;
    data[DIR_WIND_SENSORS_INDEX] = dir_wind_sensors;
    data[PLUVIO_SENSORS_INDEX] = pluvio_sensors;
    data[SOIL_HUMIDITY_SENSORS_INDEX] = soil_humidity_sensors;
    data[AIR_HUMIDITY_SENSORS_INDEX] = air_humidity_sensors;

    char base_temperatures[NUM_TEMPERATURE_REGISTERS];
    int temperatures[NUM_TEMPERATURE_REGISTERS];
    for (int j = 0; j < N_OF_TEMP_SENSORS; j++) {
        generate_base_temp_values(base_temperatures, NUM_TEMPERATURE_REGISTERS);

        char last_temp_read = TEMP_BASE_VALUE;
        for (int i = 0; i < NUM_TEMPERATURE_REGISTERS; i++) {
            last_temp_read = sens_temp(last_temp_read, pcg32_random_r());
            char base_temp_read =
                (i == 0 ? TEMP_BASE_VALUE : base_temperatures[i - 1]);
            temperatures[i] = (int)((last_temp_read + base_temp_read) / 2);
        }
        temp_sensors[j] = temperatures;
    }

    int vel_wind[NUM_VEL_WIND_REGISTERS];
    unsigned char last_read = pcg32_random_r() % 30;
    for (int j = 0; j < N_OF_VELOCITY_SENSORS; j++) {
        last_read = pcg32_random_r() % 30;

        for (int i = 0; i < NUM_VEL_WIND_REGISTERS; i++) {
            last_read = sens_velc_vento(last_read, pcg32_random_r());
            vel_wind[i] = (int)last_read;
        }
        vel_wind_sensors[j] = vel_wind;
    }

    int dir_wind[NUM_DIR_WIND_REGISTERS];
    unsigned short last_read_wind = pcg32_random_r() % 360;
    for (int j = 0; j < N_OF_DIRECTION_SENSORS; j++) {
        last_read_wind = pcg32_random_r() % 360;
        for (int i = 0; i < NUM_DIR_WIND_REGISTERS; i++) {
            last_read_wind = sens_dir_vento(last_read_wind, pcg32_random_r());
            dir_wind[i] = (int)last_read_wind;
        }
        dir_wind_sensors[j] = dir_wind;
    }

    int pluvio[NUM_PLUVIO_REGISTERS];
    unsigned char last_temp_read =
        temperatures[(TEMPERATURES_SENSOR_INTERVAL / PLUVIO_SENSOR_INTERVAL)];
    for (int j = 0; j < N_OF_PLUVIO_SENSORS; j++) {
        last_read = pcg32_random_r() % 5;
        for (int i = 0; i < NUM_PLUVIO_REGISTERS; i++) {
            last_temp_read = temperatures[i * (TEMPERATURES_SENSOR_INTERVAL /
                                               PLUVIO_SENSOR_INTERVAL)];
            last_read =
                sens_pluvio(last_read, last_temp_read, pcg32_random_r());
            pluvio[i] = (int)last_read;
        }
        pluvio_sensors[j] = pluvio;
    }

    int soil_humidity[NUM_SOIL_HUMIDITY_REGISTERS];
    unsigned char last_pluvio_read =
        pluvio[(NUM_PLUVIO_REGISTERS / NUM_SOIL_HUMIDITY_REGISTERS)];
    for (int j = 0; j < N_OF_SOIL_HUMIDITY_SENSORS; j++) {
        last_read = 10;
        for (int i = 0; i < NUM_SOIL_HUMIDITY_REGISTERS; i++) {
            last_pluvio_read = pluvio[i * (NUM_PLUVIO_REGISTERS /
                                           NUM_SOIL_HUMIDITY_REGISTERS)];
            last_read =
                sens_humd_solo(last_read, last_pluvio_read, pcg32_random_r());
            soil_humidity[i] = (int)last_read;
        }
        soil_humidity_sensors[j] = soil_humidity;
    }

    int air_humidity[NUM_AIR_HUMIDITY_REGISTERS];
    last_pluvio_read =
        pluvio[(NUM_PLUVIO_REGISTERS / NUM_SOIL_HUMIDITY_REGISTERS)];
    for (int j = 0; j < N_OF_AIR_HUMIDITY_SENSORS; j++) {
        last_read = 10;
        for (int i = 0; i < NUM_AIR_HUMIDITY_REGISTERS; i++) {
            last_pluvio_read = pluvio[i * (NUM_PLUVIO_REGISTERS /
                                           NUM_SOIL_HUMIDITY_REGISTERS)];
            last_read =
                sens_humd_atm(last_read, last_pluvio_read, pcg32_random_r());
            air_humidity[i] = (int)last_read;
        }
        air_humidity_sensors[j] = air_humidity;
    }

    printf("-- Leituras dos sensores --\n\n");
    print_result(data[TEMPERATURE_SENSORS_INDEX], NUM_TEMPERATURE_REGISTERS,
                 "Temperatura", "ºC", N_OF_TEMP_SENSORS);
    printf("\n");
    print_result(data[VELOCITY_SENSORS_INDEX], NUM_VEL_WIND_REGISTERS,
                 "Velocidade do Vento", "km/h", N_OF_VELOCITY_SENSORS);
    printf("\n");
    print_result(data[DIR_WIND_SENSORS_INDEX], NUM_DIR_WIND_REGISTERS,
                 "Direção do Vento", "º", N_OF_DIRECTION_SENSORS);
    printf("\n");
    print_result(data[PLUVIO_SENSORS_INDEX], NUM_PLUVIO_REGISTERS,
                 "Pluviosidade", "mm", N_OF_PLUVIO_SENSORS);
    printf("\n");
    print_result(data[SOIL_HUMIDITY_SENSORS_INDEX], NUM_SOIL_HUMIDITY_REGISTERS,
                 "Humidade do Solo", "%", N_OF_SOIL_HUMIDITY_SENSORS);
    printf("\n");
    print_result(data[AIR_HUMIDITY_SENSORS_INDEX], NUM_AIR_HUMIDITY_REGISTERS,
                 "Humidade do Ar", "%", N_OF_AIR_HUMIDITY_SENSORS);

    return 0;
}
