#include "storage.h"
#include <string.h>
#include "utils/utils.h"
#include "utils/delay.h"


//! Время между запросами флага WIP.
#define EEPROM_WAIT_ITER_TIME_US 1


//! Тип структуры хранилища данных.
typedef struct _Storage {
    m95x_t* eeprom; //!< Память EEPROM для хранения данных.
} storage_t;

//! Хранилище данных.
static storage_t storage;


/**
 * Ждёт завершения записи данных.
 * @param storage Хранилище данных.
 * @return Код ошибки.
 */
static err_t storage_wait_eeprom_wip(void)
{
    m95x_status_t status;
    
    do {
        delay_us(EEPROM_WAIT_ITER_TIME_US);
        RETURN_ERR_IF_FAIL(m95x_read_status(storage.eeprom, &status));
    } while(status.write_in_progress);
    
    return E_NO_ERROR;
}


err_t storage_init(m95x_t* eeprom)
{
    if(eeprom == NULL) return E_NULL_POINTER;
    
    memset(&storage, 0x0, sizeof(storage_t));
    
    storage.eeprom = eeprom;
    
    return E_NO_ERROR;
}

err_t storage_read(storage_address_t address, void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    
    RETURN_ERR_IF_FAIL(m95x_read(storage.eeprom, address, data, size));
    
    return m95x_wait(storage.eeprom);
}

err_t storage_write(storage_address_t address, const void* data, size_t size)
{
    if(data == NULL) return E_NULL_POINTER;
    if(size == 0) return E_INVALID_VALUE;
    
    size_t avail_size = 0;
    
    do {
        avail_size = m95x_avail_data_size(storage.eeprom, address);
        avail_size = MIN(avail_size, size);
        
        RETURN_ERR_IF_FAIL(m95x_write(storage.eeprom, address, data, avail_size));
        RETURN_ERR_IF_FAIL(storage_wait_eeprom_wip());
        
        address += avail_size;
        data += avail_size;
        size -= avail_size;
    } while(size != 0);
    
    return E_NO_ERROR;
}
