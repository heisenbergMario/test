/*
https://blog.csdn.net/psy6653/article/details/56678276?utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-5.searchformbaiduhighlight&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7EBlogCommendFromBaidu%7Edefault-5.searchformbaiduhighlight
https://blog.csdn.net/u014647208/article/details/80006305
http://www.360doc.com/content/19/0806/08/7551_853249151.shtml
https://blog.csdn.net/fxbjye/article/details/78292589
https://blog.csdn.net/wince_lover/article/details/70337809
https://stackoverflow.com/questions/56642221/is-device-is-kernel-driver-active-not-working-in-libusb-1-0-22-7z
*/

#include<iostream>
#include <stdio.h>
#include<libusb.h>
using namespace std;

#define USB_VID 0x8564
#define USB_PID 0x1000
#define CTRL_IN			(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_IN)
#define CTRL_OUT		(LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_ENDPOINT_OUT)
#define USB_RQ			0x04

static libusb_device_handle* dev_handle = NULL;

static void print_devs(libusb_device** devs)
{
	int r = 0;

	struct libusb_device_descriptor desc;

	libusb_device* dev;
	int i = 0, j = 0;
	uint8_t path[8];
	int interfaceNum = 0;

	libusb_config_descriptor* config_descriptor_in = NULL, * config_descriptor_out = NULL;
	int config = 0;

	int endpoint = 130;
	unsigned char data[4096] = "aaaaaffff11111";
	data[4095] = '1';
	unsigned char data_rec[4096] = "\0";
	int lenth = 0;
	int timeout = 2000;
	while ((dev = devs[i++]) != NULL) {
		
		
		r = libusb_get_device_descriptor(dev, &desc);
		printf("03).libusb_get_device_descriptor():%d\n", r);
		if (r < 0) {
			printf(":%s(%d)\n", libusb_error_name(r), r);
		}				
		if (desc.idVendor == USB_VID && desc.idProduct == USB_PID)
		{
			printf("\n\n%04x:%04x  (bus %2d, device %2d  port:%2d)", desc.idVendor, desc.idProduct, libusb_get_bus_number(dev), libusb_get_device_address(dev), libusb_get_port_number(dev));
			r = libusb_get_port_numbers(dev, path, sizeof(path));
			if (r > 0) {
				printf("  path:%d", path[0]);
				for (j = 1; j < r; j++)
					printf(".%d", path[j]);
			}			
			
			r = libusb_get_config_descriptor(dev, 0, &config_descriptor_in);		
			printf("\nbNumConfigurations:%d", desc.bNumConfigurations);
			printf("\nconfig DescriptorType:%d", config_descriptor_in->bDescriptorType);
			printf("\n  config-0.bNumInterfaces:%d", config_descriptor_in->bNumInterfaces);
			printf(("\n  bInterfaceNumber:%d"), config_descriptor_in->interface->altsetting->bInterfaceNumber);						
			printf("\n  interface-0 DescriptorType:%d", config_descriptor_in->interface->altsetting->bDescriptorType);
			printf("\n    bNumEndpoints:%d", config_descriptor_in->interface->altsetting->bNumEndpoints);
			printf(("\n    bEndpoint-0 Address:0x%02X(DT:%d attribute:%d)"), config_descriptor_in->interface->altsetting->endpoint->bEndpointAddress
				, config_descriptor_in->interface->altsetting->endpoint->bDescriptorType, config_descriptor_in->interface->altsetting->endpoint->bmAttributes);
			printf(("\n    bEndpoint-1-Address:0x%2X(DT:%d attribute:%d)"), (config_descriptor_in->interface->altsetting->endpoint)[1].bEndpointAddress
				, (config_descriptor_in->interface->altsetting->endpoint)[1].bDescriptorType, (config_descriptor_in->interface->altsetting->endpoint)[1].bmAttributes);
			printf("\n\n");

			r = libusb_open(dev, &dev_handle);
			printf("04).libusb_open():%d\n", r);
			if (r < 0) {
				printf(":%s(%d)\n", libusb_error_name(r), r);
			}			
			else {

				r = libusb_kernel_driver_active(dev_handle, interfaceNum);
				printf("05).libusb_kernel_driver_active(,interfaceNum-%d):%d\n", interfaceNum, r);
				if (r < 0) {
					printf(":%s(%d)\n", libusb_error_name(r), r);
				}
				if (r == 1) {
					printf("interface-%d Kernel Driver Active\n", interfaceNum);

					r = libusb_detach_kernel_driver(dev_handle, interfaceNum);
					printf("06).libusb_detach_kernel_driver(,interfaceNum-%d):%d\n", interfaceNum, r); 
					if (r < 0) {
						printf(":%s(%d)\n", libusb_error_name(r), r);
					}
				}				
				/*r = libusb_set_configuration(dev_handle, 3);
				if (r < 0) {
					printf("Cannot set_configuration\n:%s(%d)", libusb_error_name(r), r);
				}
				else {
					perror("set_configuration\n");
				}*/
				r = libusb_claim_interface(dev_handle, interfaceNum);
				printf("07).libusb_claim_interface(,interfaceNum-%d):%d\n", interfaceNum, r);
				if (r < 0) {
					printf(":%s(%d)\n", libusb_error_name(r), r);
				}
				
				//r = libusb_get_configuration(dev_handle, &config);
				//printf("08).libusb_get_configuration(,config-%d):%d\n", config, r);
				//if (r < 0) {
				//	printf(":%s(%d)\n", libusb_error_name(r), r);
				//}
				//r = libusb_get_config_descriptor(dev, 0, &config_descriptor_in);
				//printf("09).libusb_get_config_descriptor(,0,config_descriptor_in->interfaceNum%d):%d\n", config_descriptor_in->bNumInterfaces, r);
				//if (r < 0) {
				//	printf(":%s(%d)\n", libusb_error_name(r), r);
				//}
				//	
				//r = libusb_get_config_descriptor(dev, 1, &config_descriptor_out);			
				//printf("10).libusb_get_config_descriptor(,1,config_descriptor_out->interfaceNum):%d\n", r);
				////printf("10).libusb_get_config_descriptor(,0,config_descriptor_out->interfaceNum%d):%d\n", r, config_descriptor_out->bNumInterfaces);
				//if (r < 0) {
				//	printf(":%s(%d)\n", libusb_error_name(r), r);
				//}					
					
				r = libusb_bulk_transfer(dev_handle, endpoint, data, (int)sizeof(data), &lenth, timeout);//host--------------->device
				printf("11).libusb_bulk_transfer(,endpoint-%d,data-'%s',lenth-%d,actualLenth-%d,timeout-%d):%d\n", endpoint, data, (int)sizeof(data), lenth, timeout, r);
				if (r < 0){
					printf(":%s(%d)\n\n", libusb_error_name(r), r);
				}
				else{
					perror("host->device finish");
					printf("%d bulk transfer success:%c ......%c\n", (int)sizeof(data), data[0], data[sizeof(data) - 1]);
					printf("%s\n\n", data);
				}
				//else
				//{
				//	for (int i = 0; i < 100000; i++)
				//	{
				//		memset(data_rec, 0, 4096);
				//		r = libusb_bulk_transfer(dev_handle, 129, data_rec, 4096, &length, 0);//device--------->host
				//		printf("i = %d,receive data：%s", i, data_rec);
				//	}
				//}
				/*}
				else
					printf("get config_descriptor error\n");*/
							
			}
		}		
	}
}

int main()
{
	cout << "helloWorld" << endl << endl;
	libusb_device** devs;
	int r;
	ssize_t cnt;

	r = libusb_init(NULL);
	printf("01).libusb_init():%d\n", r);
	if (r < 0) {
		printf(":%s(%d)\n", libusb_error_name(r), r);
	}
	libusb_set_option(NULL, LIBUSB_OPTION_LOG_LEVEL);

	cnt = libusb_get_device_list(NULL, &devs);
	printf("02).libusb_get_device_list():%d\n", (int)cnt);
	if ((int)cnt < 0) {
		printf(":%s(%d)\n", libusb_error_name((int)cnt), (int)cnt);
	}	
	print_devs(devs);

	/*dev_handle = libusb_open_device_with_vid_pid(NULL, USB_VID, USB_PID);

	if (dev_handle == NULL) {
		perror("Cannot open device\n");
	}
	else {
		perror("Device Opened\n");
	}*/

	libusb_free_device_list(devs, 1);

	/*if (libusb_kernel_driver_active(dev_handle, 0) == 1) {
		printf("Kernel Driver Active\n");
		if (libusb_detach_kernel_driver(dev_handle, 0) == 0) {
			printf("Kernal Driver Detached!\n");
		}
	}

	int ret = libusb_claim_interface(dev_handle, 0);
	if (ret < 0) {
		perror("Cannot Claim Interface\n");
		return 1;
	}

	unsigned char data[0x10];*/
	/*
	dev_handle 这就是之前libopen函数获得的句柄
	bmRequestType 请求字段的类型，比如我使用的是AOA相关的(LIBUSB_REQUEST_TYPE_VENDOR)
	bRequest 请求字段的内容，例如我请求字段的类型是LIBUSB_REQUEST_TYPE_STANDARD时，bRequest的值可能是如下：
	wValue 这个简单理解为请求字段的id，可以随意设置。
	wIndex 简单点可以理解为字段内容的位置，假设你选择准备发送string类型的话，可以会有多个string，分开发送，那肯定就会出现index从0开始到N结束。
	data 字段内容
	wLength 字段内容长度，记得+1
	timeout 超时设置，以毫秒为单位
	*/
	/*ret = libusb_control_transfer(dev_handle, CTRL_OUT, USB_RQ, 0x07, 0, data, 1, 0);
	if (ret < 0) {
		fprintf(stderr, "F0 error:%d(%s)\n", ret,libusb_error_name(ret));

		return ret;
	}	
		*/
	libusb_exit(NULL);
	return 0;
}