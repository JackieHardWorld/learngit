#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>

unsigned int *init_framebuffer_device(int *fd_arg)
{
	int fb_fd;
	unsigned int *fb_addr;

	fb_fd = open("/dev/fb0", O_RDWR);//读写的方式打开显存文件
	if(fb_fd == -1)
	{
		perror("打开显存文件失败");
		return NULL;
	}
	
	fb_addr = mmap(NULL, 800*480*4, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0);//获取显存映射到虚拟内存的地址
	if(fb_addr == MAP_FAILED)
	{
		perror("映射显存内存失败\n");
		close(fb_fd);
		return NULL;
	}

	*fd_arg = fb_fd;

	return fb_addr;
}

int destroy_framebuffer_device(int fb_fd, unsigned int *fb_addr)
{
	munmap(fb_addr, 800*480*4);
	
	return close(fb_fd);

}


static inline void display_point_to_framebuffer(int x, int y, unsigned int color, unsigned int *fb_addr)
{
	*(fb_addr+800*y+x) = color;
}

void display_frame_to_framebuffer(unsigned int *frame_addr, unsigned int *fb_addr)
{
	memcpy(fb_addr, frame_addr, 800*480*4);
}


// int main(void)
// {
// 	int fb_fd;
// 	int x, y;
// 	unsigned int *fb_addr;
// 	unsigned int *my_buffer;

// 	fb_addr = init_framebuffer_device(&fb_fd);//初始化显存设备

// 	my_buffer = malloc(800*480*4*3);//申请了3个显存大小的空间




// 	for(y=0; y<480; y++)
// 	{
// 		for(x=0; x<800; x++)
// 		{
				
// 			display_point_to_framebuffer(x, y, 0x00ff0000, my_buffer);
				
// 		}
// 	}

// 	for(y=480; y<960; y++)
// 	{
// 		for(x=0; x<800; x++)
// 		{
// 			display_point_to_framebuffer(x, y, 0x0000ff00, my_buffer);		
// 		}
// 	}

// 	for(y=960; y<1440; y++)
// 	{
// 		for(x=0; x<800; x++)
// 		{
				
// 			display_point_to_framebuffer(x, y, 0x000000ff, my_buffer);
				
// 		}
// 	}
	
// 	unsigned int *line_addr;
// 	int line=100;

// 	while(1)
// 	{

// 		for(line=0; line<960; line++)
// 		{
// 			line_addr = line*800+my_buffer;

// 			display_frame_to_framebuffer(line_addr, fb_addr);

// 			usleep(100000);
// 		}

// 	}

// 	destroy_framebuffer_device(fb_fd, fb_addr);//销毁显存设备


// 	return 0;
// }
