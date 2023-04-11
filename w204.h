#ifndef W204_H_
#define W204_H_

/* HD44780 */
#define LINE1	0
#define LINE2	LINE1 + 0x40
#define LINE3	LINE1 + 0x14
#define	LINE4 	LINE2 + 0x14

/* DISPLAY CONTROL */
#define DISPLAY_OFF     0x08    
#define DISPLAY_ON      0x0C 
#define CURSOR_ON       0x0A    // 0b00001010 -> sets display off
#define BLINK_ON        0x09    // 0b00001001 -> sets display off

#define CLEAR_DISPLAY   0x01
#define RETURN_HOME     0x02

/* FUNCTION SET DEFINITION (EUROPEAN ONLY) */
#define FUNCTION_SET_EUROPEAN   0x39 // 2 a. 4 line display
#define FUNCTION_SET_EUROPEAN_2 0x3B // 2 a. 4 line display

/* ENTRY MODE SET */
#define CURSOR_DIR_LEFT_SHIFT     0x07
#define CURSOR_DIR_LEFT_NO_SHIFT  0x06 
#define CURSOR_DIR_RIGHT_SHIFT    0x05 
#define CURSOR_DIR_RIGHT_NO_SHIFT 0x04 

/* CHARACTER GRAPHICS MODE W/ INTERNAL POWER */
#define CHARACTER_MODE_INTERNAL_PWR    0x17
#define CHARACTER_MODE_NO_INTERNAL_PWR 0x13
#define GRAPHIC_MODE_NO_INTERNAL_PWR   0x1B 
#define GRAPHIC_MODE_INTERNAL_PWR      0x1F 

/* READS BUSY FLAG ; READS ADDRESS COUNTER CONTENTS 
   DON'T USE WITH NORMAL WRITE FUNCTION */
#define READ_BUSY_FLAG 0x01 // RS RW := 0 1

/* Function prototypes */

void w204_send_8_bit( uint8_t );

uint8_t* w204_read_8_bit( uint8_t, uint8_t* );

#endif /* W204_H_ */

