/** @file common.h (общий заголовочный файл)
    @brief Документированный файл.
    
    Здесь объявляется существование глобальных переменных.
    Определите их в main.cpp --> например, volatile unsigned tick_1ms
    Для схемы обратитесь к:
    Для дополнительной функциональности выводов обратитесь к:

*/

#ifndef COMMON_H
#define COMMON_H


/* Внешние объявления глобальных переменных для отсчета времени (External declarations of global timer variables) */
extern volatile unsigned reset_cnt;     /* Счетчик сбросов (Reset counter) */
extern volatile unsigned tick_count;    /* Общий счетчик тиков (General tick counter) */
extern volatile unsigned tick_1ms;      /* Флаг/счетчик тиков на 1 мс (1ms tick flag/counter) */
extern volatile unsigned tick_10ms;     /* Флаг/счетчик тиков на 10 мс (10ms tick flag/counter) */
extern volatile unsigned tick_100ms;    /* Флаг/счетчик тиков на 100 мс (100ms tick flag/counter) */


#endif /* COMMON_H */