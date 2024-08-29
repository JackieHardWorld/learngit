#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
int bmp0487()
{
    // 1.打开LCD设备
    int fd = open("/dev/fb0", O_RDWR);
    if (fd == -1)
    {
        perror("Failed to open LCD device");
        return -1;
    }
    else
    {
        printf("LCD device opened successfully\n");
    }

    // 2.映射LCD设备到内存 ,并获取二维指针
    int(*p)[800] = mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED)
    {
        perror("Failed to mmap LCD device");
        return -1;
    }
    else
    {
        printf("LCD device mmaped successfully\n");
    }

    // 3.打开图片文件
    int fd2 = open("0487.bmp", O_RDONLY);
    if (fd2 == -1)
    {
        perror("Failed to open bmp file");
        return -1;
    }
    else
    {
        printf("bmp file opened successfully\n");
    }

    // 4.读取图片的头数据
    char head[54];
    read(fd2, head, 54);

    // 5.解析图片的宽度和高度
    int width = *(int *)&head[18];
    int height = *(int *)&head[22];

    printf("width:%d, height:%d\n", width, height);

    // 6.读取图片的像素数据
    char rgb[800 * 480 * 3];
    read(fd2, rgb, 800 * 480 * 3);

    // 7.把24位的像素数据转换为32位
    char rgb32[800 * 480 * 4];
    for (int i = 0; i < 800 * 480; i++)
    {
        rgb32[0 + i * 4] = rgb[0 + i * 3];
        rgb32[1 + i * 4] = rgb[1 + i * 3];
        rgb32[2 + i * 4] = rgb[2 + i * 3];
        rgb32[3 + i * 4] = 0;
    }

    // 8.把像素数据转换为与映射地址匹配的类型
    int(*color)[800] = (void *)rgb32;

    // 9.把颜色放入对应的映射地址
    for (int y = 0; y < 480; y++)
    {
        for (int x = 0; x < 800; x++)
        {
            p[y][x] = color[480-y][x]; // 把对应的坐标颜色点，放入对应的位置
        }
    }
}