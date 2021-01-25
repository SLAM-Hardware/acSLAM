#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;
using namespace std;

int memfd;
void *mapped_dma_in_base;
void *mapped_dma_out_base;
void *mapped_ddr_base;

#undef readl
#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })
#undef writel
#define writel(addr, b) (void)((*(volatile unsigned int *)(addr)) = (b))
#define DMA_IN_BASE_ADDRESS 0xA0000000
#define DMA_OUT_BASE_ADDRESS 0xA0001000
#define DDR_BASE_ADDRESS 0x40000000
void *memory_map(unsigned int map_size, off_t base_addr) //map_size = n MByte
{
    void *mapped_base;
    mapped_base = mmap(0, map_size * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, base_addr);
    if (mapped_base == (void *)-1)
    {
        printf("Can't map memory to user space.\n");
        exit(0);
    }
#ifdef DEBUG
    printf("Memory mapped at address %p.\n", mapped_base);
#endif
    return mapped_base;
}

void memory_unmap(unsigned int map_size, void *mapped_base)
{
    if (munmap(mapped_base, map_size * 1024 * 1024) == -1)
    {
        printf("Can't unmap memory from user space.\n");
        exit(0);
    }
}

int load_bin(char *path, void *offset)
{
    int pb_in = open(path, O_RDONLY);
    long size;
    int numread = 0;

    if (pb_in == -1)
    {
        printf("Err: open %s: %s\n", __func__, path);
        return -1;
    }

    // obtain file size:
    struct stat filestate;
    stat(path, &filestate);
    size = filestate.st_size;

    if ((numread = read(pb_in, offset, size)) != size)
    {
        printf("Err: fread error, actually read: %d\n", numread);
    }
    close(pb_in);
    return 0;
}

static void timespec_sub(struct timespec *t1, const struct timespec *t2)
{
    assert(t1->tv_nsec >= 0);
    assert(t1->tv_nsec < 1000000000);
    assert(t2->tv_nsec >= 0);
    assert(t2->tv_nsec < 1000000000);
    t1->tv_sec -= t2->tv_sec;
    t1->tv_nsec -= t2->tv_nsec;
    if (t1->tv_nsec >= 1000000000)
    {
        t1->tv_sec++;
        t1->tv_nsec -= 1000000000;
    }
    else if (t1->tv_nsec < 0)
    {
        t1->tv_sec--;
        t1->tv_nsec += 1000000000;
    }
}

int dump(char *path, void *from) // legacy
{
    FILE *pb_out, *pb_golden;
    long size;

    pb_out = fopen(path, "wb");

    //obtain golden file size
    struct stat filestate;
    size = filestate.st_size;

    if (pb_out == NULL)
    {
        printf("dump_ddr:open file error\n");
        return 1;
    }

    fwrite(from, 1, size, pb_out);

    fclose(pb_out);
    return 0;
}

int init_fpga()
{

    printf("start loading bin file\n");
    printf("finish loading bin file\n");

#ifdef DEBUG
    unsigned int instr_data;
    int i = 0;
    for (i = 0; i < 10; i++)
    {
        instr_data = readl(mapped_ddr_base + 4 * i);
        printf("%08x\n", instr_data);
    }
//     for(i=0;i<10;i++){
//         instr_data = readl(mapped_weight_base+4*i);
//         printf("%08x\n", instr_data);
//     }
//     for(i=0;i<10;i++){
//         instr_data = readl(mapped_inst_base+4*i);
//         printf("%08x\n", instr_data);
//     }
#endif
    ////// RUN !!!!/////
    printf("start configuring dpu registers\n");

    writel(mapped_dma_in_base + 0x18, DDR_BASE_ADDRESS);
    writel(mapped_dma_in_base + 0x28, 466616);
    
    writel(mapped_dma_out_base + 0x48, DDR_BASE_ADDRESS + 0x02000000);
    writel(mapped_dma_out_base + 0x58, 524288);
    printf("finish configuring dpu registers\n");
    ////// WAIT //////
    return 0;
}

int main()
{
    struct timespec ts_start, ts_end;
    int rc;
    rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);
    off_t dma_in_base = DMA_IN_BASE_ADDRESS;
    off_t dma_out_base = DMA_OUT_BASE_ADDRESS;
    off_t ddr_base = DDR_BASE_ADDRESS;
    printf("start mapping memory to user space\n");
    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    mapped_dma_in_base = memory_map(1, dma_in_base);
    mapped_dma_out_base = memory_map(1, dma_out_base);
    mapped_ddr_base = memory_map(128, ddr_base);
    volatile uint8_t *addrptr_a = (uint8_t *)memory_map(100, (off_t)(0x40000000));
    volatile uint32_t *addrptr_b = (uint32_t *)memory_map(100, (off_t)(0x42000000));
    Mat image;
    image = imread("data/000000.png", IMREAD_GRAYSCALE);
    for (int i = 0; i < 376; ++i)
        for (int j = 0; j < 1241; ++j)
            addrptr_a[i * 1241 + j] = (uint8_t)image.ptr<uchar>(i)[j];
    
//     for (int i =466606; i < 466616; ++i)
//         printf("%d \n", addrptr_a[i]);
    // ////// RUN !!!!/////
    printf("writing \n");
    writel(mapped_dma_in_base, 1);
    writel(mapped_dma_out_base+0x30, 1);
    printf("write finish \n");
    printf("finish mapping memory!\n\n");
    printf("start initing \n");
    init_fpga();
    printf("finish initing \n\n");

    // // while (!(readl(mapped_reg_base + 0x14) & 0x2)){
    // //      usleep(100);
    // // }
    printf("status%d\n", readl(mapped_dma_in_base));
    if ((!((readl(mapped_dma_in_base+0x04) & 0x1))) and (!((readl(mapped_dma_out_base+0x34) & 0x1))))
        printf("HLS peripheral is ready.  Starting... \n\r");
    else
    {
        printf("!!! HLS peripheral is not ready! Exiting...\n\r");
        exit(-1);
    }

    for (int i = 0; i < 16; i++)
    {
        uint32_t tmprst = addrptr_b[i];
        printf("result is %d \n", tmprst);
    }

    // ////// WAIT //////s
    do
    {
        usleep(10000);
        
    } while (!((readl(mapped_dma_out_base+0x34) >> 1) & 0x1));

    printf("status%d\n", readl(mapped_dma_out_base+0x34));

    printf("start copying result\n");
    printf("finish copying result\n\n");
    printf("start unmapping memory\n");

    for (int i = 32640; i < 32688; i++)
    {
        uint32_t tmprst = addrptr_b[i];
        printf("result is %d \n", tmprst);
    }
    memory_unmap(1, mapped_dma_in_base);
    memory_unmap(1, mapped_dma_out_base);
    memory_unmap(128, mapped_ddr_base);
    printf("finish unmapping memory\n");

    rc = clock_gettime(CLOCK_MONOTONIC, &ts_end);

    timespec_sub(&ts_end, &ts_start);
    printf("CLOCK_MONOTONIC reports %ld.%09ld seconds\n\n",
           ts_end.tv_sec, ts_end.tv_nsec);
    return 0;
}
