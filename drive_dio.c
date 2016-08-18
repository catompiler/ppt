#include "drive_dio.h"
#include <string.h>
#include <stddef.h>


//! Структура периферии ввода-вывода цифровых входов/выходов.
typedef struct _Drive_Dio_Io {
    GPIO_TypeDef* GPIO;
    uint16_t pin;
    drive_dio_inversion_t inversion;
} drive_dio_io_t;

//! Структура цифрового входа.
typedef struct _Drive_Dio_In {
    drive_dio_io_t io;
    drive_dio_input_type_t type;
} drive_dio_in_t;

//! Структура цифрового выхода.
typedef struct _Drive_Dio_Out {
    drive_dio_io_t io;
    drive_dio_output_type_t type;
} drive_dio_out_t;

//! Структура цифровых входов-выходов привода.
typedef struct _Drive_Dio {
    drive_dio_in_t inputs[DRIVE_DIO_INPUTS_COUNT];
    drive_dio_out_t outputs[DRIVE_DIO_OUTPUTS_COUNT];
    drive_dio_on_input_changed_callback_t on_input_changed_callback;
} drive_dio_t;

static drive_dio_t dio;



err_t drive_dio_init(void)
{
    memset(&dio, 0x0, sizeof(drive_dio_t));
    
    return E_NO_ERROR;
}

err_t drive_dio_input_init(drive_dio_input_init_t* input_init)
{
    if(input_init->input >= DRIVE_DIO_INPUTS_COUNT) return E_OUT_OF_RANGE;
    
    drive_dio_in_t* input = &dio.inputs[input_init->input];
    input->type = input_init->type;
    input->io.GPIO = input_init->GPIO;
    input->io.pin = input_init->pin;
    input->io.inversion = input_init->inversion;
    
    return E_NO_ERROR;
}

err_t drive_dio_output_init(drive_dio_output_init_t* output_init)
{
    if(output_init->output >= DRIVE_DIO_INPUTS_COUNT) return E_OUT_OF_RANGE;
    
    drive_dio_out_t* output = &dio.outputs[output_init->output];
    output->type = output_init->type;
    output->io.GPIO = output_init->GPIO;
    output->io.pin = output_init->pin;
    output->io.inversion = output_init->inversion;
    
    return E_NO_ERROR;
}

void drive_dio_set_on_input_changed_callback(drive_dio_on_input_changed_callback_t callback)
{
    dio.on_input_changed_callback = callback;
}

static drive_dio_state_t drive_dio_state(drive_dio_io_t* io)
{
    if(io->inversion == DRIVE_DIO_INVERSION_NONE){
        return (io->GPIO->IDR & io->pin) ? DRIVE_DIO_ON : DRIVE_DIO_OFF;
    }
    return (io->GPIO->IDR & io->pin) ? DRIVE_DIO_OFF : DRIVE_DIO_ON;
}

static void drive_dio_set_state(drive_dio_io_t* io, drive_dio_state_t state)
{
    if(io->inversion == DRIVE_DIO_INVERSION_NONE){
        if(state == DRIVE_DIO_ON) io->GPIO->BSRR |= io->pin;
        else io->GPIO->BRR |= io->pin;
    }else{
        if(state == DRIVE_DIO_ON) io->GPIO->BRR |= io->pin;
        else io->GPIO->BSRR |= io->pin;
    }
}

drive_dio_state_t drive_dio_input_state(drive_dio_input_t input)
{
    if(input >= DRIVE_DIO_INPUTS_COUNT) return DRIVE_DIO_OFF;
    
    return drive_dio_state(&dio.inputs[input].io);
}

drive_dio_state_t drive_dio_input_type_state(drive_dio_input_type_t type)
{
    size_t i;
    for(i = 0; i < DRIVE_DIO_INPUTS_COUNT; i ++){
        drive_dio_in_t* input = &dio.inputs[i];
        if(input->type == type && drive_dio_state(&input->io) == DRIVE_DIO_ON){
            return DRIVE_DIO_ON;
        }
    }
    return DRIVE_DIO_OFF;
}

drive_dio_state_t drive_dio_output_state(drive_dio_output_t output)
{
    if(output >= DRIVE_DIO_OUTPUTS_COUNT) return DRIVE_DIO_OFF;
    
    return drive_dio_state(&dio.outputs[output].io);
}

drive_dio_state_t drive_dio_output_type_state(drive_dio_output_type_t type)
{
    size_t i;
    for(i = 0; i < DRIVE_DIO_OUTPUTS_COUNT; i ++){
        drive_dio_out_t* output = &dio.outputs[i];
        if(output->type == type && drive_dio_state(&output->io) == DRIVE_DIO_ON){
            return DRIVE_DIO_ON;
        }
    }
    return DRIVE_DIO_OFF;
}

void drive_dio_set_output_state(drive_dio_output_t output, drive_dio_state_t state)
{
    drive_dio_set_state(&dio.outputs[output].io, state);
}

void drive_dio_set_output_type_state(drive_dio_output_type_t type, drive_dio_state_t state)
{
    size_t i;
    for(i = 0; i < DRIVE_DIO_OUTPUTS_COUNT; i ++){
        drive_dio_out_t* output = &dio.outputs[i];
        if(output->type == type) drive_dio_set_state(&output->io, state);
    }
}

void drive_dio_input_changed(drive_dio_input_t input)
{
    if(dio.on_input_changed_callback){
        drive_dio_in_t* in = &dio.inputs[input];
        dio.on_input_changed_callback(in->type, drive_dio_state(&in->io));
    }
}
