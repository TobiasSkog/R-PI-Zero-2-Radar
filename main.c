#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>

#define TRIGGER_PIN 11  // GPIO pin for Trigger
#define ECHO_PIN 13  // GPIO pin for Echo
#define SERVO_PIN 12 // GPIO pin for Servo
volatile uint32_t startTick, endTick;
volatile int pulseCaptured = 0;

// Callback function to detect pulse duration
void echoCallback(int gpio, int level, uint32_t tick) {
    if (level == 0) {
        endTick = tick;
        pulseCaptured = 1;
        printf("Echo Pin LOW, pulse captured. Duration: %u\n", tick - startTick);
    } else if (level == 1) {
        startTick = tick;
        printf("Echo Pin HIGH\n");
    }
}

void setServoAngle(int angle) {
    printf("Servo angle: %d\n", angle);
    int pulseWidth = 1000 + (angle * 1000 / 180);
    printf("Servo width: %d\n", pulseWidth);
    gpioServo(SERVO_PIN, pulseWidth);
}

float measureDistance() {
    printf("measure distance\n");
    gpioWrite(TRIGGER_PIN, 1);
    //usleep(10);
    gpioDelay(10);
    gpioWrite(TRIGGER_PIN, 0);

    pulseCaptured = 0;
    startTick = endTick = 0;
    int timeout = 1000000; // Timeout after 1 second
    while (!pulseCaptured && timeout--) {
        gpioDelay(100); // Increase delay to allow more time for pulse capture
    }
    if (!pulseCaptured) {
        printf("Timeout reached, no pulse detected\n");
        return -1;
    }

    printf("WE HAVE A PULSE!\n");
    float duration = (float) endTick - (float) startTick;
    printf("Duration: %f\n", duration);
    if (duration >= 38000) {
        printf("Out of range\n");
        return -1;
    } else {
        return (float) ((duration * 0.0343) / 2);
    }
}

// Thread for handling the servo
void *servoThread(void *arg) {
    printf("Starting servo thread\n");
    while (1) {
        printf("Setting Servo Angle to 0\n");
        setServoAngle(0);
        printf("Servo angle set to 0... Sleeping...\n");
        gpioSleep(PI_TIME_RELATIVE, 10, 0);

        printf("Setting Servo Angle to 180\n");
        setServoAngle(180);
        gpioSleep(PI_TIME_RELATIVE, 10, 0);
        printf("Servo angle set to 180... Sleeping...\n");
    }
    return NULL;
}

// Thread for controlling the radar
void *radarThread(void *arg) {
    printf("Starting radar thread\n");
    gpioSetAlertFunc(ECHO_PIN, echoCallback);

    while (1) {
        printf("Getting distance...\n");
        float distance = measureDistance();
        if (distance > 0) {
            printf("Distance: %.2f cm\n", distance);
        } else if (distance == -1) {
            printf("Got -1 returned from measure\n");
            gpioTerminate();
            exit(0);
        } else {
            printf("Nothing found\n");
        }
        // Suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
        gpioDelay(60000);
        //usleep(60000);
    }
    return NULL;
}

void shutdown_handler(int signum) {
    // Terminate pigpio library
    printf("Program is exiting. Cleanup done.\n");
    gpioTerminate();

    // Exit the program
    exit(0);
}

int main(void) {
    // Initialize the pigpio library
    printf("Starting main()\n");

    printf("Init...\n");
    if (gpioInitialise() < 0) {
        printf("Initialisation Failed\n");
        return 1;
    }
    printf("Initialisation Successful\n");

    // Set up signal handlers
    gpioSetSignalFunc(SIGINT, shutdown_handler); // Ctrl+C
    gpioSetSignalFunc(SIGTERM, shutdown_handler); // Termination request
    gpioSetSignalFunc(SIGHUP, shutdown_handler); // Hangup signal
    gpioSetSignalFunc(SIGCONT, shutdown_handler); // Continue signal

    // Configure GPIO Pins
    gpioSetMode(TRIGGER_PIN, PI_OUTPUT);
    gpioSetMode(ECHO_PIN, PI_INPUT);
    gpioSetMode(SERVO_PIN, PI_OUTPUT);

    // Create threads for the servo and radar
    pthread_t servoThreadID, radarThreadID;
    pthread_create(&servoThreadID, NULL, servoThread, NULL);
    pthread_create(&radarThreadID, NULL, radarThread, NULL);

    pthread_join(servoThreadID, NULL);
    pthread_join(radarThreadID, NULL);

    gpioTerminate();
    printf("Exiting...\n");
    return 0;
}
