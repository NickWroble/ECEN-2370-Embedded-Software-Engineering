/**
 * @file ble.c
 * @author Nicholas Wroblewski
 * @date November 1st 2020
 * @brief Contains all the functions to interface the application with the HM-18
 *   BLE module and the LEUART driver
 *
 */


//***********************************************************************************
// Include files
//***********************************************************************************
#include "ble.h"
#include <string.h>

//***********************************************************************************
// defined files
//***********************************************************************************

#define CSIZE 64
typedef struct{
	char cbuf[CSIZE];
	uint8_t size_mask;
	uint32_t size;
	uint32_t read_ptr;
	uint32_t write_ptr;
} BLE_CIRCULAR_BUF;
#define CIRC_TEST_SIZE 3
typedef struct{
	char test_str[CIRC_TEST_SIZE][CSIZE];
	char result_str[CSIZE];
}CIRC_TEST_STRUCT;

//***********************************************************************************
// private variables
//***********************************************************************************
CIRC_TEST_STRUCT test_struct;
static BLE_CIRCULAR_BUF ble_cbuf;
/***************************************************************************//**
 * @brief BLE module
 * @details
 *  This module contains all the functions to interface the application layer
 *  with the HM-18 Bluetooth module.  The application does not have the
 *  responsibility of knowing the physical resources required, how to
 *  configure, or interface to the Bluetooth resource including the LEUART
 *  driver that communicates with the HM-18 BLE module.
 *
 ******************************************************************************/

//***********************************************************************************
// Private functions
//***********************************************************************************
static void ble_circ_init(void);
static void ble_circ_push(char* string);
static uint8_t ble_circ_space(void);
static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);
static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by);

//***********************************************************************************
// Global functions
//***********************************************************************************


/***************************************************************************//**
 * @brief
 * Initializes LEUART OPEN STRUCT
 * @details
 * takes all defintions and sets them equal
 *
 * @param[in] tx_event
 * event to be scheduled in STATE machine
 * @param[in] rx_event
 * event to be scheduled in STATE machine
 *
 *
 ******************************************************************************/

void ble_open(uint32_t tx_event, uint32_t rx_event){
	LEUART_OPEN_STRUCT LE_OS;
	LE_OS.baudrate 		= HM10_BAUDRATE;
	LE_OS.databits 		= HM10_DATABITS;
	LE_OS.enable 		=	HM10_ENABLE;
	LE_OS.parity 		=   HM10_PARITY;
	LE_OS.stopbits 		= HM10_STOPBITS;
	LE_OS.rx_done_evt 	= rx_event;
	LE_OS.tx_done_evt 	= tx_event;
	LE_OS.rx_loc 		= LEUART0_RX_ROUTE;
	LE_OS.tx_loc 		= LEUART0_TX_ROUTE;
	LE_OS.rx_pin_en 	= true;
	LE_OS.tx_pin_en 	= true;

	leuart_open(HM10_LEUART0,&LE_OS);
	ble_circ_init();
}


/***************************************************************************//**
 * @brief
 * BLE write takes a string and passes it into leaurt_start
 *
 * @details
 * This global function takes the string finds, the length, and then puts into leaurt start
 *
 * @param[in]*string
 * string
 *
 ******************************************************************************/

void ble_write(char* string){
		ble_circ_push(string);
		ble_circ_pop(CIRC_OPER);
}

/***************************************************************************//**
 * @brief
 *   BLE Test performs two functions.  First, it is a Test Driven Development
 *   routine to verify that the LEUART is correctly configured to communicate
 *   with the BLE HM-18 module.  Second, the input argument passed to this
 *   function will be written into the BLE module and become the new name
 *   advertised by the module while it is looking to pair.
 *
 * @details
 * 	 This global function will use polling functions provided by the LEUART
 * 	 driver for both transmit and receive to validate communications with
 * 	 the HM-18 BLE module.  For the assignment, the communication with the
 * 	 BLE module must use low energy design principles of being an interrupt
 * 	 driven state machine.
 *
 * @note
 *   For this test to run to completion, the phone most not be paired with
 *   the BLE module.  In addition for the name to be stored into the module
 *   a breakpoint must be placed at the end of the test routine and stopped
 *   at this breakpoint while in the debugger for a minimum of 5 seconds.
 *
 * @param[in] *mod_name
 *   The name that will be written to the HM-18 BLE module to identify it
 *   while it is advertising over Bluetooth Low Energy.
 *
 * @return
 *   Returns bool true if successfully passed through the tests in this
 *   function.
 ******************************************************************************/

bool ble_test(char *mod_name){
	uint32_t	str_len;

	CORE_DECLARE_IRQ_STATE;
	CORE_ENTER_CRITICAL();

	// This test will limit the test to the proper setup of the LEUART
	// peripheral, routing of the signals to the proper pins, pin
	// configuration, and transmit/reception verification.  The test
	// will communicate with the BLE module using polling routines
	// instead of interrupts.
	// How is polling different than using interrupts?
	// ANSWER: Polling works by continually looking for it and then will stop when it's found what its supposed to ,
	// while an interrupt will stop the program to perform an operation
	// How does interrupts benefit the system for low energy operation?
	// ANSWER: It only stops the program when something comes up
	// How does interrupts benefit the system that has multiple tasks?
	// ANSWER:It lets the function to work on other things such as print and data.

	// First, you will need to review the DSD HM10 datasheet to determine
	// what the default strings to write data to the BLE module and the
	// expected return statement from the BLE module to test / verify the
	// correct response

	// The test_str is used to tell the BLE module to end a Bluetooth connection
	// such as with your phone.  The ok_str is the result sent from the BLE module
	// to the micro-controller if there was not active BLE connection at the time
	// the break command was sent to the BLE module.
	// Replace the test_str "" with the command to break or end a BLE connection
	// Replace the ok_str "" with the result that will be returned from the BLE
	//   module if there was no BLE connection
	char		test_str[80] = "AT";
	char		ok_str[80] = "OK";


	// output_str will be the string that will program a name to the BLE module.
	// From the DSD HM10 datasheet, what is the command to program a name into
	// the BLE module?
	// Answer:AT+NAME?

	// The  output_str will be a string concatenation of the DSD HM10 command
	// and the input argument sent to the ble_test() function
	// Replace the output_str "" with the command to change the program name
	// Replace the result_str "" with the first part of the expected result
	//  the backend of the expected response will be concatenated with the
	//  input argument
	char		output_str[80] = "AT+NAME";
	char		result_str[80] = "OK+Set:";


	// To program the name into your module, you must reset the module after you
	// have sent the command to update the modules name.  What is the DSD HM10
	// name to reset the module?
	// Replace the reset_str "" with the command to reset the module
	// Replace the reset_result_str "" with the expected BLE module response to
	//  to the reset command
	char		reset_str[80] = "AT+RESET";
	char		reset_result_str[80] = "OK+RESET";
	char		return_str[80];

	bool		success;
	bool		rx_disabled, rx_en, tx_en;
	uint32_t	status;

	// These are the routines that will build up the entire command and response
	// of programming the name into the BLE module.  Concatenating the command or
	// response with the input argument name
	strcat(output_str, mod_name);
	strcat(result_str, mod_name);

	// The test routine must not alter the function of the configuration of the
	// LEUART driver, but requires certain functionality to insure the logical test
	// of writing and reading to the DSD HM10 module.  The following c-lines of code
	// save the current state of the LEUART driver that will be used later to
	// re-instate the LEUART configuration

	status = leuart_status(HM10_LEUART0);
	if (status & LEUART_STATUS_RXBLOCK) {
		rx_disabled = true;
		// Enabling, unblocking, the receiving of data from the LEUART RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKDIS);
	}
	else rx_disabled = false;
	if (status & LEUART_STATUS_RXENS) {
		rx_en = true;
	} else {
		rx_en = false;
		// Enabling the receiving of data from the RX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_RXENS));
	}

	if (status & LEUART_STATUS_TXENS){
		tx_en = true;
	} else {
		// Enabling the transmission of data to the TX port
		leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXEN);
		while (!(leuart_status(HM10_LEUART0) & LEUART_STATUS_TXENS));
		tx_en = false;
	}
//	leuart_cmd_write(HM10_LEUART0, (LEUART_CMD_CLEARRX | LEUART_CMD_CLEARTX));

	// This sequence of instructions is sending the break ble connection
	// to the DSD HM10 module.
	// Why is this command required if you want to change the name of the
	// DSD HM10 module?
	// ANSWER: Because the name can't be changed during an active connection
	str_len = strlen(test_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, test_str[i]);
	}

	// What will the ble module response back to this command if there is
	// a current ble connection?
	// ANSWER: OK+whatever transmits
	str_len = strlen(ok_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (ok_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// This sequence of code will be writing or programming the name of
	// the module to the DSD HM10
	str_len = strlen(output_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, output_str[i]);
	}

	// Here will be the check on the response back from the DSD HM10 on the
	// programming of its name
	str_len = strlen(result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// It is now time to send the command to RESET the DSD HM10 module
	str_len = strlen(reset_str);
	for (int i = 0; i < str_len; i++){
		leuart_app_transmit_byte(HM10_LEUART0, reset_str[i]);
	}

	// After sending the command to RESET, the DSD HM10 will send a response
	// back to the micro-controller
	str_len = strlen(reset_result_str);
	for (int i = 0; i < str_len; i++){
		return_str[i] = leuart_app_receive_byte(HM10_LEUART0);
		if (reset_result_str[i] != return_str[i]) {
				EFM_ASSERT(false);;
		}
	}

	// After the test and programming have been completed, the original
	// state of the LEUART must be restored
	if (!rx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXDIS);
	if (rx_disabled) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_RXBLOCKEN);
	if (!tx_en) leuart_cmd_write(HM10_LEUART0, LEUART_CMD_TXDIS);
	leuart_if_reset(HM10_LEUART0);

	success = true;


	CORE_EXIT_CRITICAL();
	return success;
}


/***************************************************************************//**
 * @brief
 *   Circular Buff Test is a Test Driven Development function to validate
 *   that the circular buffer implementation
 *
 * @details
 * 	 This Test Driven Development test has tests integrated into the function
 * 	 to validate that the routines can successfully identify whether there
 * 	 is space available in the circular buffer, the write and index pointers
 * 	 wrap around, and that one or more packets can be pushed and popped from
 * 	 the circular buffer.
 *
 * @note
 *   If anyone of these test will fail, an EFM_ASSERT will occur.  If the
 *   DEBUG_EFM=1 symbol is defined for this project, exiting this function
 *   confirms that the push, pop, and the associated utility functions are
 *   working.
 *
 * @par
 *   There is a test escape that is not possible to test through this
 *   function that will need to be verified by writing several ble_write()s
 *   back to back and verified by checking that these ble_write()s were
 *   successfully transmitted to the phone app.
 *
 ******************************************************************************/

 void circular_buff_test(void){
	 bool buff_empty;
	 int test1_len = 50;
	 int test2_len = 25;
	 int test3_len = 5;

	 // Why this 0 initialize of read and write pointer?
	 // Student Response: 0 is the base address of the circular buffer array

	 ble_cbuf.read_ptr = 0;
	 ble_cbuf.write_ptr = 0;

	 // Why do none of these test strings contain a 0?
	 // Student Response: I'm not sure, lines 362 and 372 contain zeros
	 //
	 for (int i = 0;i < test1_len; i++){
		 test_struct.test_str[0][i] = i+1;
	 }
	 test_struct.test_str[0][test1_len] = 0;

	 for (int i = 0;i < test2_len; i++){
		 test_struct.test_str[1][i] = i + 20;
	 }
	 test_struct.test_str[1][test2_len] = 0;

	 for (int i = 0;i < test3_len; i++){
		 test_struct.test_str[2][i] = i +  35;
	 }
	 test_struct.test_str[2][test3_len] = 0;

	 // What is this test validating?
	 // Student response: that the free space is equal to the max size of the buffer
	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Why is there only one push to the circular buffer at this stage of the test
	 // Student Response:
	 //
	 ble_circ_push(&test_struct.test_str[0][0]);

	 // What is this test validating?
	 // Student response: that the open space is equal to the previously pushed data
	 EFM_ASSERT(ble_circ_space() == (CSIZE - test1_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: The buffer still contains elements after popping
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test1_len; i++){
		 EFM_ASSERT(test_struct.test_str[0][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: That the resulting string length is the same of the inputed string
	 EFM_ASSERT(strlen(test_struct.result_str) == test1_len);

	 // What is this test validating?
	 // Student response: that the buffer is empty
	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // What does this next push on the circular buffer test?
	 // Student Response: Pushes 1 on to the buffer, see line 358

	 ble_circ_push(&test_struct.test_str[1][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response: Pushes 35 onto the buffer, see line 368
	 ble_circ_push(&test_struct.test_str[2][0]);


	 EFM_ASSERT(ble_circ_space() == (CSIZE - test2_len - 1 - test3_len - 1));

	 // What does this next push on the circular buffer test?
	 // Student Response:  That the circular buffer can handle ptr overflows
	 EFM_ASSERT(abs(ble_cbuf.write_ptr - ble_cbuf.read_ptr) < CSIZE);

	 // Why is the expected buff_empty test = false?
	 // Student Response: All of the elements on the buffer have been popped
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test2_len; i++){
		 EFM_ASSERT(test_struct.test_str[1][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: That the resulting string matched the inputted string.
	 EFM_ASSERT(strlen(test_struct.result_str) == test2_len);

	 EFM_ASSERT(ble_circ_space() == (CSIZE - test3_len - 1));

	 // Why is the expected buff_empty test = false?
	 // Student Response: That the LEUART bus is not busy when pop is called
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == false);
	 for (int i = 0; i < test3_len; i++){
		 EFM_ASSERT(test_struct.test_str[2][i] == test_struct.result_str[i]);
	 }

	 // What is this test validating?
	 // Student response: That the resulting string matched the inputted string.
	 EFM_ASSERT(strlen(test_struct.result_str) == test3_len);

	 EFM_ASSERT(ble_circ_space() == CSIZE);

	 // Using these three writes and pops to the circular buffer, what other test
	 // could we develop to better test out the circular buffer?
	 // Student Response: Not really sure, checks for pointer read/ write overflow
	 // handling were tested, tests to see if the free space on the buffer was greater
	 // than the new packet were used. Maybe we could test if the new data rate on the buffer
	 // matches the average data consumption rate of the LEUART bus?


	 // Why is the expected buff_empty test = true?
	 // Student Response: All of the elements have been popped off
	 //
	 buff_empty = ble_circ_pop(CIRC_TEST);
	 EFM_ASSERT(buff_empty == true);
	 ble_write("\nPassed Circular Buffer Test\n");

 }

 /***************************************************************************//**
  * @brief
  *  Sets up a new buffer to all of its necessary starting values
  *
  *
  ******************************************************************************/
 static void ble_circ_init(void){
	 ble_cbuf.read_ptr 	= 0;
	 ble_cbuf.write_ptr = 0;
	 ble_cbuf.size 		= CSIZE;
	 ble_cbuf.size_mask = CSIZE - 1;
 }

 /***************************************************************************//**
  * @brief
  *  Pushes a packet to the circular buffer
  *
  * @details Enters infinite loop if the string to be pushed is larger than the
  * free space on the buffer. Else, copies the string onto the buffer and updates
  * the write pointer.
  *
  ******************************************************************************/
 static void ble_circ_push(char* string){
	 if(ble_circ_space() == ble_cbuf.size_mask)
		 return;
	 if(ble_circ_space() < strlen(string) + 1)
		 EFM_ASSERT(false);
	 ble_cbuf.cbuf[ble_cbuf.write_ptr] = (char)strlen(string) + 1;
	 for(int i = 0; i < strlen(string);i++)
		 ble_cbuf.cbuf[(ble_cbuf.write_ptr + i +1)& ble_cbuf.size_mask ] = string[i];
	 update_circ_wrtindex(&ble_cbuf, strlen(string) + 1);
 }

 /***************************************************************************//**
  * @brief
  *  Pops a packet off of the circular buffer
  *
  * @details Returns false on successful pop and true on failure. Checks if the LEUART is busy
  * and if the buffer is empty. Reads data from the buffer, sends it on the LEUART bus and
  * update the read pointer
  *
  *
  ******************************************************************************/
 bool ble_circ_pop(bool test){
	 int packetLength = ble_cbuf.cbuf[ble_cbuf.read_ptr];
	 if(leuart_tx_busy(HM10_LEUART0))
		 return true;
	 if(ble_cbuf.write_ptr == ble_cbuf.read_ptr)
		 return true;
	 char pop_str[packetLength -1];
	 for(int i = 0; i < packetLength -1; i++)
		 pop_str[i] = ble_cbuf.cbuf[(ble_cbuf.read_ptr + i +1) & ble_cbuf.size_mask ];
	 if(test){
		 for(int j = 0; j < ble_cbuf.size;j++)
			 test_struct.result_str[j] = 0;
		 for(int i = 0; i < packetLength -1;i++)
			 test_struct.result_str[i] = pop_str[i];
	 }
	 else
		 leuart_start(HM10_LEUART0, pop_str, packetLength - 1);
	 update_circ_readindex(&ble_cbuf, packetLength);
	 return false;
 }


 /***************************************************************************//**
  * @brief
  *  Returns the number of free char spaces on the buffer
  *
  *
  ******************************************************************************/
 static uint8_t ble_circ_space(void){
	 if(ble_cbuf.write_ptr >= ble_cbuf.read_ptr)
		 return (ble_cbuf.size - abs(ble_cbuf.write_ptr - ble_cbuf.read_ptr));
	 else
		 return ((ble_cbuf.read_ptr - ble_cbuf.write_ptr));
 }

 /***************************************************************************//**
  * @brief
  *  Updates the write ptr of the circular buffer
  *
  * @details Uses a size mask of the buffer to avoid integer division
  *
  * @param[in] BLE_CIRCULAR_BUF *index_struct
  * Struct of the circular buffer array
  * @param[in] uint32_t update_by
  * how much should the write pointer be updated by
  *
  ******************************************************************************/
 static void update_circ_wrtindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	 index_struct->write_ptr = (index_struct->write_ptr + update_by) &index_struct->size_mask;
 }

 /***************************************************************************//**
  * @brief
  *  Updates the read ptr of the circular buffer
  *
  * @details Uses a size mask of the buffer to avoid integer division
  *
  * @param[in] BLE_CIRCULAR_BUF *index_struct
  * Struct of the circular buffer array
  * @param[in] uint32_t update_by
  * how much should the read pointer be updated by
  *
  ******************************************************************************/
 static void update_circ_readindex(BLE_CIRCULAR_BUF *index_struct, uint32_t update_by){
	 index_struct->read_ptr = (index_struct->read_ptr + update_by) &index_struct->size_mask;
 }



