#define TRUE 1
#define FALSE 0

#define S5J_ADC_MAX_CHANNELS	6
#define MAX_PIN_NUM 3

#define HIGH 1
#define LOW 0

int adc_fd;
struct adc_msg_s *sample;

int analogInit(void);
int analogRead(int port);
void analogFinish(void);

void digitalWrite(int port, int value);
int digitalRead(int port);
