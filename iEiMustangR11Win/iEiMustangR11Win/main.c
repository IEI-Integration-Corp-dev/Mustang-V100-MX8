#include <stdio.h>
#include <wchar.h>
#if 1
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <sys/stat.h>
#include <Windows.h>
#ifndef _WIN32
#include <poll.h>
#endif
//#include <dirent.h>

#include "hidapi.h"


#define ieidbg              0

#define MAX_PATH_LENGTH 4096
#define MAX_HID_DEVICE_NUM (16)
#define MAX_HID_PARENT_NODE_NUM (16)
#define MAX_HID_USB_NODE_STRING_LEN (15)


#define F75114_MAX_RESET_PIN_NUM (8)
#define F75114_MAX_BOARDID_PIN_NUM (3)

#define F75114_VID (0x2c42)
#define F75114_PID (0x5114)

#define HID_CMD_SIGNATURE 0x43444948

// HID UART Command
#define HID_UART_INIT 0x70
#define USB_DATA_IN 0x71
#define USB_DATA_OUT 0x72
#define HID_GET_STATUS 0x80

#define HID_PACKET_SIZE 0x20

// device id pin
#define HID_DEVICE_ID_PIN1 0x0
#define HID_DEVICE_ID_PIN2 0x1
#define HID_DEVICE_ID_PIN3 0x7
#define HID_DEVICE_ID_PIN4 0x10
#define HID_DEVICE_ID_PIN5 0x11
#define HID_DEVICE_ID_PIN6 0x12
#define HID_DEVICE_ID_PIN7 0x13
#define HID_DEVICE_ID_PIN8 0x14
#define HID_DEVICE_ID_PIN_RESET 0x20

// board id pin
#define HID_BOARD_ID_PIN1 2
#define HID_BOARD_ID_PIN2 3
#define HID_BOARD_ID_PIN3 4

typedef enum {
  eGPIO_Direction_In,
  eGPIO_Direction_Out,
  eGPIO_Direction_Invalid,
} eGPIO_Direction;

typedef enum {
  eGPIO_Pull_Low,
  eGPIO_Pull_High,
  eGPIO_Pull_Disable,
  eGPIO_Pull_Invalid,
} eGPIO_Pull_Mode;


#ifdef _WIN32
#pragma pack(push) /* push current alignment to stack */
#pragma pack(1)    /* set alignment to 1 byte boundary */

typedef struct {
  unsigned char cmd;    // 1   command Code
  unsigned char len;    // 1   command Length
  unsigned short arg1;  // 2:
  unsigned short arg2;  // 2:
  unsigned short arg3;  // 2:  startAddress
  unsigned short arg4;  // 2:  r/w length

  unsigned int signature;  // 4:  HIDC
  unsigned short Reserve;  // 2
  unsigned char DevType;
  unsigned char FunIdx;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  unsigned char data4;
  unsigned char data5;
  unsigned char data6;
  unsigned int checksum;
} HID_CMD_T;

#pragma pack(pop) /* restore original alignment from stack */
#pragma comment(lib, "Shlwapi.lib")

errno_t bsl_strncpy(char* _Destination, size_t _SizeInBytes, char const* _Source, size_t _MaxCount) {
	return strncpy_s(_Destination, _SizeInBytes, _Source, _MaxCount);
}
#define sleep(seconds) Sleep((seconds))
#define usleep(us) Sleep((us) / 1000)

#else
typedef struct {
  unsigned char cmd;    // 1   command Code
  unsigned char len;    // 1   command Length
  unsigned short arg1;  // 2:
  unsigned short arg2;  // 2:
  unsigned short arg3;  // 2:  startAddress
  unsigned short arg4;  // 2:  r/w length

  unsigned int signature;  // 4:  HIDC
  unsigned short Reserve;  // 2
  unsigned char DevType;
  unsigned char FunIdx;
  unsigned char data1;
  unsigned char data2;
  unsigned char data3;
  unsigned char data4;
  unsigned char data5;
  unsigned char data6;
  unsigned int checksum;
} __attribute__((packed)) HID_CMD_T;

int bsl_strncpy(char* _Destination, size_t _SizeInBytes, char const* _Source, size_t _MaxCount) ;
int bsl_strncpy(char* _Destination, size_t _SizeInBytes, char const* _Source, size_t _MaxCount) 
{
    if (_Destination == NULL) 
    {
        return 1;
    }

    if (_Source == NULL) 
    {
        return 1;
    }

    size_t source_len = strnlen(_Source, _MaxCount);

    bool source_not_null_terminated = source_len >= _MaxCount;
    if (source_not_null_terminated) 
    {
        return 1;
    }

    bool destination_too_small = source_len >= _SizeInBytes;
    if (destination_too_small) 
    {
        return 1;
    }

  // At this point, source is ensured to be null terminated and can be fit into destination
    strncpy(_Destination, _Source, _SizeInBytes);
    _Destination[_SizeInBytes - 1] = '\0';  // Useless. Just to prevent the kw warning
    return 0;
}


#endif

static unsigned char m_hid_count = 0;

static char m_hid_path_list[MAX_HID_DEVICE_NUM][MAX_PATH_LENGTH];
//static int m_hid_boardid_list[MAX_HID_DEVICE_NUM];
typedef struct {
  int pin_id;
  int device_id;
} F75114_PIN_MAP;
static const F75114_PIN_MAP m_hid_reset_pin_map[F75114_MAX_RESET_PIN_NUM] = {
    {HID_DEVICE_ID_PIN1, 0}, {HID_DEVICE_ID_PIN2, 1}, {HID_DEVICE_ID_PIN3, 2}, {HID_DEVICE_ID_PIN4, 3},
    {HID_DEVICE_ID_PIN5, 4}, {HID_DEVICE_ID_PIN6, 5}, {HID_DEVICE_ID_PIN7, 6}, {HID_DEVICE_ID_PIN8, 7}};

int iei_hid_scan();
unsigned int _cal_checksum(unsigned char* buf, unsigned int size) ;

//USB command
int _send_buffer_to_dev(hid_device* handle, unsigned char* read_write_buff) ;
int _send_init_cmd(hid_device* handle) ;
int _send_output_cmd_with_buf(hid_device* handle, int pin_id, int func_id, unsigned char* buf) ;
int _send_output_cmd(hid_device* handle, int pin_id, int eMode, int func_id) ;
int _send_input_cmd_with_buf(hid_device* handle, int pin_id, int func_id, unsigned char* buf) ;
int _send_output_enable_cmd(hid_device* handle, int pin_id, eGPIO_Direction eMode) ;
int _send_pull_mode_cmd(hid_device* handle, int pin_id, eGPIO_Pull_Mode eMode) ;
int _send_output_data_cmd(hid_device* handle, int pin_id, int value) ;


unsigned int _cal_checksum(unsigned char* buf, unsigned int size) 
{
    unsigned int sum;
    int i;

    i = 0;
    sum = 0;
    while (size--) 
    {
        sum += buf[i++];
    }

    return sum;
}

// if output_buf is null, means caller do not care the output data
int _send_buffer_to_dev(hid_device* handle, unsigned char* read_write_buff) 
{
    int ret = hid_write(handle, read_write_buff, HID_PACKET_SIZE);
    if (ret <= 0) 
    {
        printf("hidapi_write failed\n");
        return -1;
    }
#if ieidbg    
    printf("write ret=%d\n", ret);
#endif


#ifdef _WIN32
    usleep(2000);
#else
    struct pollfd fds;

    fds.fd = hid_get_fd(handle);
    fds.events = POLLIN;
    fds.revents = 0;
    ret = poll(&fds, 1, 50);  //-1 to milliseconds

    if (ret == -1 || ret == 0)
    // if (ret == -1 || (fds.revents & ~POLLOUT) != 0)
    {
        printf("failed to poll data from f75114\n");
        return -1;
    }     
    else 
    {
        /* Check for errors on the file descriptor. This will
        indicate a device disconnection. */
        if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) 
        {
            printf("device maybe disconnect\n");
            return -1;
        }
    }

#endif

    int length = hid_read(handle, read_write_buff, HID_PACKET_SIZE);
#if ieidbg
    printf("read length: %d\n", length);
#endif
    return length > 0 ? 0 : -1;
}


int _send_init_cmd(hid_device* handle) 
{
    HID_CMD_T cmd;
    int ret;
    unsigned char buf[HID_PACKET_SIZE] = {0x0};
    memset(&cmd, 0, sizeof(cmd));

    cmd.cmd = HID_UART_INIT;
    // cmd.len = sizeof(cmd) - 4;  /* Not include checksum */
    cmd.arg1 = 0;
    cmd.arg3 = 0;
    cmd.arg4 = 0x20;
    cmd.signature = HID_CMD_SIGNATURE;
    cmd.len = cmd.DevType = 0;
    cmd.FunIdx = 0;
    cmd.checksum = _cal_checksum((unsigned char*)&cmd, cmd.len);
    memcpy(buf, &cmd, sizeof(cmd));

    ret = _send_buffer_to_dev(handle, buf);

    if (ret != 0 || buf[0] != 0xFF) 
    {
        printf("read result failed;ret=%d;buf[0]=%x\n", ret, buf[0]);
        return -1;
    }
    return 0;
}


int _send_output_cmd_with_buf(hid_device* handle, int pin_id, int func_id, unsigned char* buf) 
{
    HID_CMD_T cmd;

    int ret;

    cmd.cmd = USB_DATA_OUT;
    // cmd.len = sizeof(cmd) - 4;  /* Not include checksum */
    cmd.arg1 = 0;
    cmd.arg2 = 0;
    cmd.arg3 = 0;
    cmd.arg4 = 0x20;
    cmd.signature = HID_CMD_SIGNATURE;
    cmd.Reserve = 0;
    cmd.DevType = 0;
    cmd.len = (unsigned char)func_id;
    cmd.FunIdx = (unsigned char)func_id;
    cmd.data1 = buf[2];
    cmd.data2 = buf[3];
    cmd.data3 = buf[4];
    cmd.data4 = buf[5];
    cmd.data5 = buf[6];
    cmd.data6 = buf[7];
    cmd.checksum = _cal_checksum((unsigned char*)&cmd, cmd.len);
    memcpy(buf, &cmd, sizeof(cmd));

    ret = _send_buffer_to_dev(handle, buf);

    if (ret != 0 || buf[0] != 0x80) 
    {
        printf("read result failed;ret=%d;buf[0]=%x\n", ret, buf[0]);
        return -1;
    }

  return 0;
}


int _send_output_cmd(hid_device* handle, int pin_id, int eMode, int func_id) 
{
    HID_CMD_T cmd;
    int index = 0;
    int set = 0;
    int count = 0;

    unsigned char buf[HID_PACKET_SIZE] = {0x0};

    memset(&cmd, 0, sizeof(cmd));

    set = (pin_id >> 4) & 0x0f;
    count = (pin_id >> 0) & 0x0f;
    buf[0] = 0x00;                    // GPIO
    buf[1] = (unsigned char)func_id;  // GPIO_DIR

    index = 2 * set + !eMode;
    int buf_index = index + 2;
    if (buf_index < 0 || buf_index >= HID_PACKET_SIZE - 1) 
    {
        return -1;
    }
    buf[buf_index] = (unsigned char)(1 << count);

    return _send_output_cmd_with_buf(handle, pin_id, func_id, buf);
}

int _send_input_cmd_with_buf(hid_device* handle, int pin_id, int func_id, unsigned char* buf) 
{
    HID_CMD_T cmd;

    memset(&cmd, 0, sizeof(cmd));

    cmd.cmd = USB_DATA_IN;
    // cmd.len = sizeof(cmd) - 4;  /* Not include checksum */
    cmd.arg1 = 0;
    cmd.arg3 = 0;
    cmd.arg4 = 0x20;
    cmd.signature = HID_CMD_SIGNATURE;
    cmd.DevType = 0;
    cmd.len = func_id;
    cmd.FunIdx = func_id;
    cmd.data1 = buf[2];
    cmd.data2 = buf[3];
    cmd.data3 = buf[4];
    cmd.data4 = buf[5];
    cmd.data5 = buf[6];
    cmd.data6 = buf[7];
    cmd.checksum = _cal_checksum((unsigned char*)&cmd, cmd.len);
    memcpy(buf, &cmd, sizeof(cmd));

    return _send_buffer_to_dev(handle, buf);
}

int _send_output_enable_cmd(hid_device* handle, int pin_id, eGPIO_Direction eMode) 
{
    return _send_output_cmd(handle, pin_id, (int)eMode, 0x04);
}



//_SetGpioPullMode
//__SetGpioPullMode_F75114__
int _send_pull_mode_cmd(hid_device* handle, int pin_id, eGPIO_Pull_Mode eMode) 
{
    if (eMode < eGPIO_Pull_Disable) 
    {
        return _send_output_cmd(handle, pin_id, (int)eMode, 0x01);
    } 
    else 
    {
        int set = 0;
        int count = 0;
        unsigned char addr = 0;
        unsigned char buff[HID_PACKET_SIZE] = {0x00};
        int ret = 0;
        if (set == 0 && count < 4)
            addr = 0x60;
        else if (set == 0 && count > 3)
            addr = 0x61;
        else if (set == 1 && count < 4)
            addr = 0x62;
        else if (set == 1 && count > 3)
            addr = 0x63;
        else if (set == 2)
            addr = 0x64;
        buff[0] = 0xF0;  // For all registry
        buff[1] = 0x00;  // Read
        buff[2] = addr;
        ret = _send_input_cmd_with_buf(handle, pin_id, 0x00, buff);
        if (ret) 
        {
            printf("send input cmd failed\n");
            return ret;
        }
        buff[3] = buff[0];
#if ieidbg        
        printf("pre-process buff[3]: %x\n", buff[0]);
#endif        
        if ((pin_id & 0x0f) == 0 || (pin_id & 0x0f) == 4)
            buff[3] &= 0xfc;
        else if ((pin_id & 0x0f) == 1 || (pin_id & 0x0f) == 5)
            buff[3] &= 0xf3;
        else if ((pin_id & 0x0f) == 2 || (pin_id & 0x0f) == 6)
            buff[3] &= 0xcf;
        else if ((pin_id & 0x0f) == 3 || (pin_id & 0x0f) == 7)
            buff[3] &= 0x3f;
#if ieidbg            
        printf("post-process buff[3]: %x\n", buff[0]);
#endif        
        buff[0] = 0xF0;  // For all registry
        buff[1] = 0x01;  // write
        buff[2] = addr;

        ret = _send_output_cmd_with_buf(handle, pin_id, 0x01, buff);
        if (ret) 
        {
            printf("send output cmd failed\n");
            return ret;
        }
        return 0;
    }
}


//_SetGpioOutputDataIdx
//__SetGpioOutputDataIdx_F75114__
int _send_output_data_cmd(hid_device* handle, int pin_id, int value) 
{
    if (value != 0)
        value = 1;
    else
        value = 0;
    return _send_output_cmd(handle, pin_id, value, 0x02);
}



void log_hidapi_device_info(const struct hid_device_info* cur_dev) 
{
    printf("Device Found:\n");
    printf("VID:PID:        %04hx:%04hx\n", cur_dev->vendor_id, cur_dev->product_id);
    printf("Path:           %s\n", cur_dev->path);
    printf("Serial Number:  %ls\n", cur_dev->serial_number);
    printf("Path:           %s\n", cur_dev->path);
#if ieidbg  
  printf("Manufacturer:   %ls\n", cur_dev->manufacturer_string);
  printf("Product:        %ls\n", cur_dev->product_string);
  printf("Release:        %hx\n", cur_dev->release_number);
  printf("Interface:      %d\n", cur_dev->interface_number);
#endif  

}



int iei_hid_scan()
{
      m_hid_count = 0;
    printf("scan F75114 device...\n");

  struct hid_device_info* device_list = hid_enumerate(F75114_VID, F75114_PID);

  for (struct hid_device_info* cur_dev = device_list; cur_dev != NULL; cur_dev = cur_dev->next) 
  {
        log_hidapi_device_info(cur_dev);
        bsl_strncpy(m_hid_path_list[m_hid_count], sizeof(m_hid_path_list[m_hid_count]), cur_dev->path,
                    sizeof(m_hid_path_list[m_hid_count]));
        m_hid_count++;
  }

  hid_free_enumeration(device_list);


  printf("found %d F75114 device\n", m_hid_count);
  return m_hid_count > 0 ? 0 : -1;
}




#endif

int main(int argc, char* argv[])
{
#if 1
    if(iei_hid_scan()!=0)
    {
        printf("no reset device found\n");
        return 0;
    }

    for (int i = 0; i < m_hid_count; i++) 
    {
        char* hid_path = m_hid_path_list[i];
        assert(hid_path);

        hid_device* handle = hid_open_path(hid_path);

        if (handle == NULL) 
        {
            return -1;
        }

        hid_set_nonblocking(handle, 1);
        _send_init_cmd(handle);
        int j = 0;
        for (; j < F75114_MAX_RESET_PIN_NUM; j++) 
        {
            int pin_id = m_hid_reset_pin_map[j].pin_id;
            _send_output_enable_cmd(handle, pin_id, eGPIO_Direction_Out);
            _send_pull_mode_cmd(handle, pin_id, eGPIO_Pull_Disable);
            _send_output_data_cmd(handle, pin_id, 1);
        }

        _send_output_enable_cmd(handle, HID_DEVICE_ID_PIN_RESET, eGPIO_Direction_Out);
        _send_pull_mode_cmd(handle, HID_DEVICE_ID_PIN_RESET, eGPIO_Pull_Disable);
        _send_output_data_cmd(handle, HID_DEVICE_ID_PIN_RESET, 0);
    //_send_output_enable_cmd(handle, HID_BOARD_ID_PIN1, eGPIO_Direction_In);
    //_send_output_enable_cmd(handle, HID_BOARD_ID_PIN2, eGPIO_Direction_In);
    //_send_output_enable_cmd(handle, HID_BOARD_ID_PIN3, eGPIO_Direction_In);

    // need read board id,and put it into m_hid_boardid_list
    // m_hid_boardid_list
        hid_close(handle);
        printf("Device %s pin Pull up\n",hid_path);
    }


#endif
    return 0;
}