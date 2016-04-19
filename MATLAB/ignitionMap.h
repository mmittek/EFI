typedef struct {
 uint16_t rpm_min;
 uint16_t rpm_max;
 uint16_t rpm_step;
 uint16_t load_min;
 uint16_t load_max;
 uint16_t load_step;
 uint16_t values[][];
} ignitionMap_t;


ignitionMap_t myMap ={ 0,
 16000,
 1000,
 0,
 100,
 10,
 { {2}, {1} }};