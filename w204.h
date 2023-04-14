#ifndef W204_H_
#define W204_H_

/* HD44780 */
#define LINE1	0x00
#define LINE2	LINE3 + 0x20
#define LINE3	LINE1 + 0x20
#define	LINE4 	LINE2 + 0x20
#define DDRAM_ADDR 0x80

/* OPCODE */
#define RSRW00 0x00
#define RSRW01 0x01
#define RSRW10 0x02
#define RSRW11 0x03

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

/* Function Prototypes */
void w204_init( uint8_t );
void w204_send_8_bit_instruction( uint8_t, uint8_t );
void w204_send_8_bit_instructions( uint8_t, uint8_t, uint8_t, uint8_t );
void w204_send_8_bit_data( uint8_t, uint8_t );
void w204_send_8_bit_data_n( uint8_t, uint8_t, uint8_t );
void w204_puts( char* string );
void w204_move_cursor( uint8_t, uint8_t );

#endif /* W204_H_ */