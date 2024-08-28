/* $Id$ */

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "shiroi.h"

shiroi_t shiroi;

void thread_start(void);
void thread_end(void);

int main(int argc, char** argv) {
	FILE* fconf = fopen("shiroi.ini", "r");
	if(fconf == NULL) {
		printf("Creating config.\n");
		fconf = fopen("shiroi.ini", "w");
		if(fconf == NULL) {
			fprintf(stderr, "Failed to create config\n");
			return 1;
		}
		fprintf(fconf, "slot1=video_mark_1\n");
		fprintf(fconf, "slot2=sound_mark_1\n");
		fprintf(fconf, "slot3=math_mark_1\n");
		fprintf(fconf, "rom=shiroi.rom\n");
		fclose(fconf);
	}

	shiroi_init_cards(&shiroi);

	fconf = fopen("shiroi.ini", "r");

	struct stat s;
	stat("shiroi.ini", &s);

	char* ini = malloc(s.st_size + 1);
	fread(ini, s.st_size, 1, fconf);

	ini[s.st_size] = 0;

	fclose(fconf);

	int incr = 0;
	int i;
	for(i = 0;; i++) {
		if(ini[i] == 0 || ini[i] == '\n') {
			char oldc = ini[i];
			ini[i] = 0;

			char* line = ini + incr;

			if(strlen(line) != 0) {
				int j;
				for(j = 0; line[j] != 0; j++) {
					if(line[j] == '=') {
						line[j] = 0;
						if(strcmp(line, "rom") == 0) {
							printf("ROM: %s\n", line + j + 1);
							if(stat(line + j + 1, &s) != 0) {
								fprintf(stderr, "ROM not found\n");
								free(ini);
								return 1;
							}
							printf("ROM size: %d\n", s.st_size);
							FILE* f = fopen(line + j + 1, "rb");
							fread(shiroi.ram, s.st_size, 1, f);
							fclose(f);
						} else if(line[0] == 's' && line[1] == 'l' && line[2] == 'o' && line[3] == 't') {
							int slot = atoi(line + 4);
							const char* n = "";
							int dev = -1;
							if(strcmp(line + j + 1, "video_mark_1") == 0) {
								dev = SHIROI_VIDEO_MARK_I;
								n = "Video Mark I";
							} else if(strcmp(line + j + 1, "sound_mark_1") == 0) {
								dev = SHIROI_SOUND_MARK_I;
								n = "Sound Mark I";
							} else if(strcmp(line + j + 1, "math_mark_1") == 0) {
								dev = SHIROI_MATH_MARK_I;
								n = "Math Mark I";
							}
							if(dev == -1) {
								fprintf(stderr, "No such device called `%s' ; ignoring\n", line + j + 1);
							} else {
								shiroi_install(&shiroi, slot, dev);
								printf("Installed `%s' into slot %d\n", n, slot);
							}
						}
						break;
					}
				}
			}

			incr = i + 1;
			if(oldc == 0) break;
		}
	}

	free(ini);

	shiroi_init(&shiroi);

	double scx = 2;
	double scy = 2;

	shiroi_card_t* videocard = shiroi_get_video_card(&shiroi);
	shiroi_video_t* video = NULL;
	if(videocard != NULL) {
		video = videocard->videoptr;
	}

	SetTraceLogLevel(LOG_NONE);
	if(video != NULL) {
		InitWindow(video->width * scx, video->height * scy, "Shiroi Emulator");
	} else {
		InitWindow(640, 480, "Shiroi Emulator");
	}
	InitAudioDevice();
	SetAudioStreamBufferSizeDefault(512);
	AudioStream as = LoadAudioStream(48000, 16, 1);
	SetAudioStreamCallback(as, shiroi.play_audio);
	SetTargetFPS(60);
	uint32_t* fb = NULL;

	if(video != NULL) {
		fb = malloc(sizeof(*fb) * video->width * video->height);

		int x, y;
		for(y = 0; y < video->height; y++) {
			for(x = 0; x < video->width; x++) {
				fb[y * video->width + x] = 0xffffff;
			}
		}
	}

	RenderTexture r;
	if(video != NULL) r = LoadRenderTexture(video->width, video->height);
	PlayAudioStream(as);
	thread_start();
	while(!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);

		if(video != NULL) {
			BeginTextureMode(r);

			int y, x;
			for(y = 0; y < video->height; y++) {
				for(x = 0; x < video->width; x++) {
					if(video->fb[y * video->width + x] != fb[y * TMS9918_PIXELS_X + x]) {
						uint32_t c = video->fb[y * video->width + x];
						// printf("%X\n", c);
						DrawPixel(x, y, (Color){(c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, 0xff});
						fb[y * video->width + x] = c;
					}
				}
			}

			EndTextureMode();

			DrawTexturePro(r.texture, (Rectangle){0, 0, video->width, -video->height}, (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()}, (Vector2){0, 0}, 0, WHITE);
		} else {
			DrawText("No Video", 0, 0, 20, WHITE);
		}

		EndDrawing();
	}
	CloseWindow();
	UnloadAudioStream(as);
	if(video != NULL) UnloadRenderTexture(r);
	shiroi.stop = true;
	thread_end();
}
