#include"i2c.h"
#include"cmu.h"
#include"brd_config.h"
#include"sleep_routines.h"

#define READ 1
#define WRITE 0

//**
//structs
//**
static I2C_STATE_MACHINE i2c_state;
static void i2c_ACK(void);
static void i2c_NACK(void);
static void i2c_RXDATAV(void);
static void i2c_bus_reset(I2C_TypeDef *i2c);
static void send_data(void);
static void end_i2c(void);

void i2c_open(I2C_TypeDef *i2c, I2C_OPEN_STRUCT *i2c_setup){
	I2C_Init_TypeDef I2C_Init_Vals;

	if(i2c == I2C0)
		CMU_ClockEnable(cmuClock_I2C0, true);
	if(i2c == I2C1)
		CMU_ClockEnable(cmuClock_I2C1, true);

	if((i2c->IF & 0x01)== 0){ 						//if i2c interrupt flag is not set
		i2c->IFS = 0x01;
		EFM_ASSERT(i2c->IF & 0x01); 			//check if IF is set
		i2c->IFC = 0x01;
	}
	else{
		i2c->IFC = 0x01;
		EFM_ASSERT(!(i2c->IF & 0X01));
	}

	I2C_Init_Vals.enable 		= true;
	I2C_Init_Vals.master 		= true;
	I2C_Init_Vals.refFreq 		= i2c_setup->refFreq;
	I2C_Init_Vals.freq 			= i2c_setup->freq;
	I2C_Init_Vals.clhr			= i2c_setup->clhr;
	I2C_Init(i2c, &I2C_Init_Vals);

	i2c->ROUTEPEN 				= 	(I2C_ROUTEPEN_SCLPEN* i2c_setup->OUT_ROUTE_SCL_EN)| (I2C_ROUTEPEN_SDAPEN*i2c_setup -> OUT_ROUTE_SDA_EN);
	i2c->ROUTELOC0 				= 	SCLLOC;
	i2c->ROUTELOC0 				|= 	SDALOC;
	if(i2c == I2C0)
		NVIC_EnableIRQ(I2C0_IRQn);
	if(i2c == I2C1)
		NVIC_EnableIRQ(I2C1_IRQn);
}

void I2C0_IRQHandler(void){
	int int_flag = I2C0->IF;
	I2C0->IFC = int_flag;
	if(int_flag & I2C_IF_ACK)
		i2c_ACK();
	if(int_flag & I2C_IF_NACK)
		i2c_NACK();
	if(int_flag & I2C_IF_RXDATAV)
		i2c_RXDATAV();
}

void I2C1_IRQHandler(void){
	int int_flag = I2C1->IF;
	I2C1->IFC = int_flag;
	if(int_flag & I2C_IF_ACK)
		i2c_ACK();
	if(int_flag & I2C_IF_NACK)
		i2c_NACK();
	if(int_flag & I2C_IF_RXDATAV)
		i2c_RXDATAV();
}

void i2c_start(I2C_TypeDef *i2c, uint32_t SA, uint32_t SR, uint32_t callback){
	EFM_ASSERT((i2c->STATE & _I2C_STATE_STATE_MASK) == I2C_STATE_STATE_IDLE); //check if the state machine is busy
	sleep_block_mode(I2C_EM_BLOCK);

	i2c_state.I2Cn 			= 	i2c;
	i2c_state.isBusy 		= 	true;
	i2c_state.slave_addr	= 	SA;
	i2c_state.slave_reg 	= 	SR;
	i2c_state.state 		= 	start;
	i2c_state.callback 		=	callback;
	i2c_bus_reset(i2c);
	send_data();
}

void read_data(uint32_t *data_loc){
	if(!i2c_state.isBusy)
		*data_loc = i2c_state.data;
	else
		EFM_ASSERT(false);
	return;
}

//**
// private function prototypes
static void end_i2c(void){
	i2c_state.isBusy = false;
	i2c_state.state = idle;
	sleep_unblock_mode(I2C_EM_BLOCK);
	add_scheduled_event(i2c_state.callback);
}

static void i2c_bus_reset(I2C_TypeDef *i2c){ //reset master and slave i2c
	if(i2c->STATE & I2C_STATE_BUSY){
		i2c->CMD = I2C_CMD_ABORT;
		while(i2c->STATE & I2C_STATE_BUSY);
	}

	uint32_t state = i2c->IEN;
	i2c->IEN = 0;
	i2c->IFC = i2c->IF;
	i2c->CMD = I2C_CMD_CLEARTX;
	i2c->CMD = I2C_CMD_START | I2C_CMD_STOP;
	while(!(i2c->IF & _I2C_IF_MSTOP_MASK));
	i2c->IFC = i2c->IF;
	i2c->IEN = state;
	i2c->CMD = I2C_CMD_ABORT;
}

static void send_data(){
    switch (i2c_state.state){
            case idle:
                EFM_ASSERT(false);
                break;
            case start:
                i2c_state.I2Cn->CMD     = I2C_CMD_START;
                i2c_state.I2Cn->TXDATA    = (i2c_state.slave_addr<<1)|WRITE;
                break;
            case measure:
                i2c_state.I2Cn->TXDATA  = i2c_state.slave_reg;
                break;
            case process:
                i2c_state.I2Cn->CMD     = I2C_CMD_START;
                i2c_state.I2Cn->TXDATA     = (i2c_state.slave_addr<<1)|READ;
                break;
            case MSB:
                EFM_ASSERT(false);
                break;
            case stop:
                i2c_state.I2Cn->CMD     = I2C_CMD_ACK;
                break;
            case done:
                i2c_state.I2Cn->CMD     =I2C_CMD_NACK;
                i2c_state.I2Cn->CMD     =I2C_CMD_STOP;
                break;
        }
}

static void i2c_ACK(void){
	 switch (i2c_state.state){
	 	 case start:
	 		 i2c_state.state = measure;
	 		 send_data();
	 		break;
	 	 case measure:
	 		i2c_state.state = process;
	 		send_data();
	 		break;
	 	 case process:
	 		i2c_state.state = MSB;
	 		break;
	 	 case MSB:
	 		EFM_ASSERT(false);
	 		break;
	 	 case stop:
	 		EFM_ASSERT(false);
	 		break;
	 	case done:
			EFM_ASSERT(false);
			break;
	 	default:
	 		EFM_ASSERT(false);
	 }
  }
static void i2c_NACK(void){
	 switch (i2c_state.state){
	 	 case start:
	 		EFM_ASSERT(false);
	 		break;
	 	 case measure:
	 		EFM_ASSERT(false);
	 		break;
	 	 case process:
	 		send_data();
	 		break;
	 	 case MSB:
	 		EFM_ASSERT(false);
	 		break;
	 	 case stop:
	 		EFM_ASSERT(false);
	 		break;
	 	case done:
			EFM_ASSERT(false);
			break;
	 	default:
	 		EFM_ASSERT(false);
	 }
  }

static void i2c_RXDATAV(void){
    switch (i2c_state.state){
            case start:
                EFM_ASSERT(false);
                break;
            case measure:
                EFM_ASSERT(false);
                break;
            case process:
                EFM_ASSERT(false);
                break;
            case MSB:
                i2c_state.data = i2c_state.I2Cn->RXDATA<<8;
                i2c_state.state = stop;
                send_data();
                break;
            case stop:
                i2c_state.data |= i2c_state.I2Cn->RXDATA;
                i2c_state.state= done;
                send_data();
                end_i2c();
                break;
            case done:
                EFM_ASSERT(false);
                break;
            default:
                EFM_ASSERT(false);
    }
}

//**
