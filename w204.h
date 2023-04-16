#ifndef W204_H_
#define W204_H_

/* HD44780 */
#define LINE_COUNT  0x04
#define LINE_LENGTH 0x14
#define LINE1	0x00
#define LINE2	LINE3 + LINE_LENGTH
#define LINE3	LINE1 + LINE_LENGTH
#define	LINE4 	LINE2 + LINE_LENGTH
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

/* Moves cursor and shifts display without changing DDRAM contents */
#define SHIFT_INSTRUCTION   0x10
#define SHIFT_CURSOR_LEFT   0x00
#define SHIFT_CURSOR_RIGHT  0x04
#define SHIFT_DISPLAY_LEFT  0x08
#define SHIFT_DISPLAY_RIGHT 0x0C

/* Typdef for Display Output */
#define MAX_CHAR_LENGTH 20
#define MAX_OUTPUT_STREAMS 6

typedef struct stream_out_t {
    char data0[MAX_CHAR_LENGTH];
    char data1[MAX_CHAR_LENGTH];
    char data2[MAX_CHAR_LENGTH];
    char data3[MAX_CHAR_LENGTH];
    char data4[MAX_CHAR_LENGTH];
    char data5[MAX_CHAR_LENGTH];  
} stream_out_t;

/* Function Prototypes */
void w204_init( uint8_t );
void w204_set_stream_out( stream_out_t* stream );
void w204_puts( char* string );
void w204_update( stream_out_t* stream );
void w204_clear( void );
void w204_send_8_bit_instruction( uint8_t, uint8_t );
void w204_send_8_bit_instructions( uint8_t, uint8_t, uint8_t, uint8_t );
void w204_send_8_bit_data( uint8_t, uint8_t );
void w204_send_8_bit_data_n( uint8_t, uint8_t, uint8_t );
void w204_move_cursor( uint8_t, uint8_t );
void w204_shift_cursor_left( void );
void w204_shift_cursor_right( void );
void w204_shift_display_left( void );
void w204_shift_display_right( void );
void w204_shift_display_up( void );
void w204_shift_display_down( void );

#endif /* W204_H_ */