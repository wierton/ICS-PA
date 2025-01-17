#include "hal.h"
#include "device/video.h"
#include "device/palette.h"

#include <string.h>
#include <stdlib.h>

int get_fps();

typedef struct {
	uint32_t sw,dw,sp,dp,sx,sy,dx,dy,cw,ch;
} ACC;

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(dst && src);

	/* TODO: Performs a fast blit from the source surface to the 
	 * destination surface. Only the position is used in the
	 * ``dstrect'' (the width and height are ignored). If either
	 * ``srcrect'' or ``dstrect'' are NULL, the entire surface 
	 * (``src'' or ``dst'') is copied. The final blit rectangle 
	 * is saved in ``dstrect'' after all clipping is performed
	 * (``srcrect'' is not modified).
	 */
	int SrcX, SrcY, CopyWidth, CopyHeight, DstX, DstY;
	if(srcrect == NULL || dstrect == NULL)
	{
		SrcX = SrcY = DstX = DstY = 0;
		CopyWidth = src->w < dst->w ? src->w : dst->w;
		CopyHeight = src->h < dst->h ? src->h : dst->h;
	}
	else
	{
		int SrcCopyWidth = src->w - srcrect->x;
		int SrcCopyHeight = src->h - srcrect->y;
		int DstCopyWidth = dst->w - dstrect->x;
		int DstCopyHeight = dst->h - dstrect->y;
		int MinSDWidth = SrcCopyWidth < DstCopyWidth ? SrcCopyWidth : DstCopyWidth;
		int MinSDHeight = SrcCopyHeight < DstCopyHeight ? SrcCopyHeight : DstCopyHeight;
		SrcX = srcrect -> x;
		SrcY = srcrect -> y;
		DstX = dstrect -> x;
		DstY = dstrect -> y;
		CopyWidth = srcrect->w < MinSDWidth ? srcrect->w : MinSDWidth;
		CopyHeight = srcrect->h < MinSDHeight ? srcrect->h : MinSDHeight;
	}
#ifdef PA4
	int i,j;
	int volatile SrcPos = 0 + SrcX + src->w * (0 + SrcY);
	int volatile DstPos = 0 + DstX + dst->w * (0 + DstY);
	for(j = 0; j < CopyHeight; j++)
	{
		for(i = 0; i < CopyWidth; i++)
		{
			dst->pixels[DstPos]
				=src->pixels[SrcPos];
			SrcPos ++;
			DstPos ++;
		}
		SrcPos = SrcPos - CopyWidth + src->w;
		DstPos = DstPos - CopyWidth + dst->w;
	}
#else
	ACC volatile ac;
	ac.sx = SrcX; ac.sy = SrcY; ac.dx = DstX; ac.dy = DstY;
	ac.sp = (uint32_t)(src->pixels);
	ac.dp = (uint32_t)(dst->pixels);
	ac.cw = CopyWidth; ac.ch = CopyHeight;
	ac.sw = src->w; ac.dw = dst->w;
	asm volatile (".byte 0xd7" : : "a"(&ac));
#endif
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
	assert(dst);
	assert(color <= 0xff);

	/* TODO: Fill the rectangle area described by ``dstrect''
	 * in surface ``dst'' with color ``color''. If dstrect is
	 * NULL, fill the whole surface.
	 */

	int i, j;
	int FillWidth , FillHeight;
	FillWidth = dst->w - dstrect->x;
	FillHeight = dst->h - dstrect->y;
	FillWidth = dstrect -> w < FillWidth ? dstrect->w : FillWidth;
	FillHeight = dstrect -> h < FillHeight ? dstrect->h : FillHeight;
	for(i = 0; i < FillWidth; i++)
	{
		int DstPos = i + dstrect->x + dst->w *  dstrect->y;
		for(j = 0; j < FillHeight; j++)
		{
			dst->pixels[DstPos] = color;
			DstPos += dstrect->w;
		}
	}
}

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
	assert(screen);
	assert(screen->pitch == 320);
	if(screen->flags & SDL_HWSURFACE) {
		if(x == 0 && y == 0) {
			/* Draw FPS */
			vmem = VMEM_ADDR;
			char buf[80];
			sprintf(buf, "%dFPS", get_fps());
			draw_string(buf, 0, 0, 10);
		}
		return;
	}

	/* TODO: Copy the pixels in the rectangle area to the screen. */
#ifdef PA4
	int i,j;
	int SrcPos = 0 + x + screen->w * (0 + y);
	int DstPos = 0 + x + screen->w * (0 + y);
	for(j = 0; j < h; j++)
	{
		for(i = 0; i < w; i++)
		{
			vmem[DstPos] = screen->pixels[SrcPos];
			SrcPos ++;
			DstPos ++;
		}
		SrcPos = SrcPos - w + screen->w;
		DstPos = DstPos - w + screen->w;
	}
#else
	ACC volatile ac;
	ac.sx = x; ac.sy = y; ac.dx = x; ac.dy = y;
	ac.sp = (uint32_t)(screen->pixels);
	ac.dp = (uint32_t)(VMEM_ADDR);
	ac.cw = w; ac.ch = h;
	ac.sw = screen->w; ac.dw = screen->w;
	asm volatile (".byte 0xd7" : : "a"(&ac));
#endif
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
		int firstcolor, int ncolors) {
	assert(s);
	assert(s->format);
	assert(s->format->palette);
	assert(firstcolor == 0);

	if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
		if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
			/* If the size of the new palette is different 
			 * from the old one, free the old one.
			 */
			free(s->format->palette->colors);
		}

		/* Get new memory space to store the new palette. */
		s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
		assert(s->format->palette->colors);
	}

	/* Set the new palette. */
	s->format->palette->ncolors = ncolors;
	memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

	if(s->flags & SDL_HWSURFACE) {
		/* TODO: Set the VGA palette by calling write_palette(). */
		write_palette(s->format->palette->colors, s->format->palette->ncolors);
	}
}

/* ======== The following functions are already implemented. ======== */

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *scrrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(src && dst);
	int x = (scrrect == NULL ? 0 : scrrect->x);
	int y = (scrrect == NULL ? 0 : scrrect->y);
	int w = (scrrect == NULL ? src->w : scrrect->w);
	int h = (scrrect == NULL ? src->h : scrrect->h);

	assert(dstrect);
	if(w == dstrect->w && h == dstrect->h) {
		/* The source rectangle and the destination rectangle
		 * are of the same size. If that is the case, there
		 * is no need to stretch, just copy. */
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_BlitSurface(src, &rect, dst, dstrect);
	}
	else {
		/* No other case occurs in NEMU-PAL. */
		assert(0);
	}
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
		uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
	SDL_Surface *s = malloc(sizeof(SDL_Surface));
	assert(s);
	s->format = malloc(sizeof(SDL_PixelFormat));
	assert(s);
	s->format->palette = malloc(sizeof(SDL_Palette));
	assert(s->format->palette);
	s->format->palette->colors = NULL;

	s->format->BitsPerPixel = depth;

	s->flags = flags;
	s->w = width;
	s->h = height;
	s->pitch = (width * depth) >> 3;
	s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
	assert(s->pixels);

	return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
	return SDL_CreateRGBSurface(flags,  width, height, bpp,
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
	if(s != NULL) {
		if(s->format != NULL) {
			if(s->format->palette != NULL) {
				if(s->format->palette->colors != NULL) {
					free(s->format->palette->colors);
				}
				free(s->format->palette);
			}

			free(s->format);
		}
		
		if(s->pixels != NULL && !(s->flags & SDL_HWSURFACE)) {
			free(s->pixels);
		}

		free(s);
	}
}

