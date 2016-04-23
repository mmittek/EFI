/**
 * Since elipse expresses weird behavior of NOT complaining about undefined symbols
 * but the build works perfectly fine, I figured out a way of havign the symbols
 * and not influencing the building process...
 */
#ifndef TCCR0A
	#include <avr/iom328p.h>
#endif
