/*
 * inicio_letras.c
 * 
 * Copyright 2019 Alberto <alberto@alberto-VirtualBox>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "inicio_letras.h"

#define FILEPATH "/dev/fb1"
#define NUM_WORDS 64
#define FILESIZE (NUM_WORDS * sizeof(uint16_t))
#define RGB565_YELLOW 0xFFE0

void inicio_letras(void){
	#include <linux/fb.h>
	int fbfd = 0;
	struct fb_fix_screeninfo fix_info;
	uint16_t *map;
    uint16_t *p;
    int i,j = 0;
    
	fbfd = open(FILEPATH, O_RDWR);
	if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
		perror("Error (llamada del ioctl");
		close(fbfd);
		exit(EXIT_FAILURE);
	}

	//*	Comprueba que se ha encontrado el dispositivo correcto *//
	if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
		printf("%s\n", "Error: RPI-Sense FB no se encuentra");
		close(fbfd);
		exit(EXIT_FAILURE);
	}

	/* map the led frame buffer device into memory */
    map = mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (map == MAP_FAILED) {
	close(fbfd);
	perror("Error mmapping the file");
	exit(EXIT_FAILURE);
    }

    /* set a pointer to the start of the memory area */
    p = map;

    /* clear the led matrix */
    memset(map, 0, FILESIZE);

    /* light it up! */
	for (i = 0; i < NUM_WORDS; i++) {
	*(p + i) = RGB565_YELLOW;
	usleep(25000);
    }
	memset(map, 0, FILESIZE);
	 for (i = 0; i < 3; i++) {
	usleep(25000);
	memset(map, 0xFF, FILESIZE);
	usleep(25000);
	memset(map, 0, FILESIZE);
    }
    usleep(25000);
	memset(map, 0, FILESIZE);
	    if (munmap(map, FILESIZE) == -1) {
	perror("Error un-mmapping the file");
    }
    close(fbfd);
    return;
}
