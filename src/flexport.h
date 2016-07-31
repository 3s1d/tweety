#ifndef __FLEXPORT_H
#define __FLEXPORT_H

/* Define macros for use in the functions:
 * These just expand to a macro with a similar name
 * _AND_ expand their argument which shall contain a comma.
 * This way, in the first place a single-argument macro is reference (by c code)
 * and in the second place this macro is redirected to a two-argument macro while
 * splitting the argument.
 */
#define PORT(p)		_PORT(p)
#define DDR(p)		_DDR(p)
#define PIN(p)		_PIN(p)
#define BIT(p)		_BIT(p)

// Macro for creating a mask beginning at the given port and extending w bits wide up:
#define MASK(p,w)	( _WIDTH(w) << _BIT(p) )

/* The following macros are the back-end to the ones above.
 * They get the split arguments and expand to the actual content for the macro.
 */
#define	_PORT(x,b)	PORT ## x
#define _DDR(x,b)	DDR ## x
#define _PIN(x,b)	PIN ## x
#define	_BIT(x,b)	P ## x ## b

// This macro results in a value with the least significant w bits set:
#define	_WIDTH(w)	(0xff >> (8-w))

/* Some additional short cut macros for configuring the DDR,
 * setting, clearing and reading single bits:
 */
#define INPUT(p)	_DDR(p) &= ~(1<<_BIT(p))
#define OUTPUT(p)	_DDR(p) |= (1<<_BIT(p))
#define SET(p)		_PORT(p) |= (1<<_BIT(p))
#define CLR(p)		_PORT(p) &= ~(1<<_BIT(p))
#define	GET(p)		(!! ( _PIN(p) & ( 1<<_BIT(p) ) ) )

#endif /* __FLEXPORT_H */
