/* $Id$ */

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "shiroi.h"

#define ON_COLOR ((Color){0xc0, 0, 0, 0xff})
#define OFF_COLOR ((Color){0x20, 0x20, 0x20, 0xff})

shiroi_t shiroi;

void thread_start(void);
void thread_end(void);

bool get_bit(int n, int f) { return (n & (1 << f)) ? true : false; }

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
		fprintf(fconf, "slot4=text_mark_1\n");
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

			if(strlen(line) != 0 && line[0] != '#') {
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
							} else if(strcmp(line + j + 1, "video_mark_2") == 0) {
								dev = SHIROI_VIDEO_MARK_II;
								n = "Video Mark II";
							} else if(strcmp(line + j + 1, "sound_mark_1") == 0) {
								dev = SHIROI_SOUND_MARK_I;
								n = "Sound Mark I";
							} else if(strcmp(line + j + 1, "math_mark_1") == 0) {
								dev = SHIROI_MATH_MARK_I;
								n = "Math Mark I";
							} else if(strcmp(line + j + 1, "text_mark_1") == 0) {
								dev = SHIROI_TEXT_MARK_I;
								n = "Text Mark I";
							} else if(strcmp(line + j + 1, "debug") == 0) {
								dev = SHIROI_DEBUG;
								n = "Debug";
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

	shiroi_card_t* textcard = shiroi_get_text_card(&shiroi);
	shiroi_text_t* text = NULL;
	if(textcard != NULL) {
		text = textcard->textptr;
	}

	shiroi_card_t* debugcard = shiroi_get_debug_card(&shiroi);
	shiroi_debug_t* debug = NULL;
	if(debugcard != NULL) {
		debug = debugcard->debugptr;
	}

	SetTraceLogLevel(LOG_NONE);
	if(video != NULL) {
		InitWindow(video->width * scx + ((debug == NULL && text == NULL) ? 0 : 200), video->height * scy, "Shiroi Emulator");
	} else {
		InitWindow(640 + ((debug == NULL && text == NULL) ? 0 : 200), 480, "Shiroi Emulator");
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
	if(video != NULL) {
		r = LoadRenderTexture(video->width, video->height);
		BeginTextureMode(r);
		ClearBackground(BLACK);
		EndTextureMode();
	}
	PlayAudioStream(as);
	thread_start();
	while(!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BLACK);

		if(text != NULL) {
			/*
			 * / 1 2 3 4 5 6 7 8 9 10 11 12 13
			 * 1 1 2 3 4 5 6 7 8 9 0  -  =  bs
			 * 2 q w e r t y u i o p  [  ]  rt
			 * 3 a s d f g h j k l ;  '  \  cl
			 * 4 z x c v b n m , . /  sp bk
			 */
			int c = GetKeyPressed();
			if(KEY_ONE <= c && c <= KEY_NINE) {
				text->key = (1 << 4) | (c - KEY_ONE + 1);
			} else if(c == KEY_ZERO) {
				text->key = (1 << 4) | 10;
			} else if(c == KEY_MINUS) {
				text->key = (1 << 4) | 11;
			} else if(c == KEY_EQUAL) {
				text->key = (1 << 4) | 12;
			} else if(c == KEY_BACKSPACE) {
				text->key = (1 << 4) | 13;
			} else if(c == KEY_LEFT_BRACKET) {
				text->key = (2 << 4) | 11;
			} else if(c == KEY_RIGHT_BRACKET) {
				text->key = (2 << 4) | 12;
			} else if(c == KEY_ENTER) {
				text->key = (2 << 4) | 13;
			} else if(c == KEY_SEMICOLON) {
				text->key = (3 << 4) | 10;
			} else if(c == KEY_APOSTROPHE) {
				text->key = (3 << 4) | 11;
			} else if(c == KEY_BACKSLASH) {
				text->key = (3 << 4) | 12;
			} else if(c == KEY_LEFT_SHIFT || c == KEY_RIGHT_SHIFT) {
				text->key = (3 << 4) | 13;
				text->caps = !text->caps;
			} else if(c == KEY_F1) {
				if(shiroi.stop) {
					shiroi.stop = false;
					shiroi.reset = true;
					thread_start();
				} else {
					shiroi.reset = true;
					shiroi.stop = true;
					thread_end();
				}
			} else if(c == KEY_F2) {
				shiroi.reset = true;
			} else if(c == KEY_COMMA) {
				text->key = (4 << 4) | 8;
			} else if(c == KEY_PERIOD) {
				text->key = (4 << 4) | 9;
			} else if(c == KEY_SLASH) {
				text->key = (4 << 4) | 10;
			} else if(c == KEY_SPACE) {
				text->key = (4 << 4) | 11;
			} else if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && c == KEY_PAUSE) {
				text->key = (4 << 4) | 12;
			} else if(c == KEY_Q) {
				text->key = (2 << 4) | 1;
			} else if(c == KEY_W) {
				text->key = (2 << 4) | 2;
			} else if(c == KEY_E) {
				text->key = (2 << 4) | 3;
			} else if(c == KEY_R) {
				text->key = (2 << 4) | 4;
			} else if(c == KEY_T) {
				text->key = (2 << 4) | 5;
			} else if(c == KEY_Y) {
				text->key = (2 << 4) | 6;
			} else if(c == KEY_U) {
				text->key = (2 << 4) | 7;
			} else if(c == KEY_I) {
				text->key = (2 << 4) | 8;
			} else if(c == KEY_O) {
				text->key = (2 << 4) | 9;
			} else if(c == KEY_P) {
				text->key = (2 << 4) | 10;
			} else if(c == KEY_A) {
				text->key = (3 << 4) | 1;
			} else if(c == KEY_S) {
				text->key = (3 << 4) | 2;
			} else if(c == KEY_D) {
				text->key = (3 << 4) | 3;
			} else if(c == KEY_F) {
				text->key = (3 << 4) | 4;
			} else if(c == KEY_G) {
				text->key = (3 << 4) | 5;
			} else if(c == KEY_H) {
				text->key = (3 << 4) | 6;
			} else if(c == KEY_J) {
				text->key = (3 << 4) | 7;
			} else if(c == KEY_K) {
				text->key = (3 << 4) | 8;
			} else if(c == KEY_L) {
				text->key = (3 << 4) | 9;
			} else if(c == KEY_Z) {
				text->key = (4 << 4) | 1;
			} else if(c == KEY_X) {
				text->key = (4 << 4) | 2;
			} else if(c == KEY_C) {
				text->key = (4 << 4) | 3;
			} else if(c == KEY_V) {
				text->key = (4 << 4) | 4;
			} else if(c == KEY_B) {
				text->key = (4 << 4) | 5;
			} else if(c == KEY_N) {
				text->key = (4 << 4) | 6;
			} else if(c == KEY_M) {
				text->key = (4 << 4) | 7;
			}
		}

		if(video != NULL) {
			BeginTextureMode(r);

			int y, x;
			for(y = 0; y < video->height; y++) {
				for(x = 0; x < video->width; x++) {
					if(video->fb[y * video->width + x] != fb[y * video->width + x]) {
						uint32_t c = video->fb[y * video->width + x];
						DrawPixel(x, y, (Color){(c >> 24) & 0xff, (c >> 16) & 0xff, (c >> 8) & 0xff, c & 0xff});
						fb[y * video->width + x] = c;
					}
				}
			}

			EndTextureMode();

			DrawTexturePro(r.texture, (Rectangle){0, 0, video->width, -video->height}, (Rectangle){(text == NULL && debug == NULL) ? 0 : 200, 0, GetScreenWidth() - ((text == NULL && debug == NULL) ? 0 : 200), GetScreenHeight()}, (Vector2){0, 0}, 0, WHITE);
		} else {
			DrawText("No Video", 200 + 5, 5, 20, WHITE);
		}

		if(text != NULL) {
			DrawText("Caps Lock", 5, 5, 10, WHITE);

			DrawCircle(200 - 10, 10, 5, text->caps ? ON_COLOR : OFF_COLOR);
		}

		if(debug != NULL) {
			DrawText("Debug", 5, 5 + 10 + 5, 10, WHITE);
			for(i = 0; i < 4; i++) {
				int shx = i * (5 + 20 + 5 + 5 + 10);
				DrawRectangle(shx + 10, 5 + 5 + 10 + 5 + 10 + 5, 20, 5, get_bit(debug->latch[i], 0) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 5, 5 + 5 + 10 + 5 + 10 + 10, 5, 20, get_bit(debug->latch[i], 1) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 5 + 20 + 5, 5 + 5 + 10 + 5 + 10 + 10, 5, 20, get_bit(debug->latch[i], 2) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 10, 5 + 5 + 10 + 5 + 10 + 5 + 20 + 5, 20, 5, get_bit(debug->latch[i], 3) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 5, 5 + 5 + 10 + 5 + 10 + 10 + 5 + 20, 5, 20, get_bit(debug->latch[i], 4) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 5 + 20 + 5, 5 + 5 + 10 + 5 + 10 + 10 + 5 + 20, 5, 20, get_bit(debug->latch[i], 5) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 10, 5 + 5 + 10 + 5 + 10 + 5 + 20 + 5 + 20 + 5, 20, 5, get_bit(debug->latch[i], 6) ? ON_COLOR : OFF_COLOR);
				DrawRectangle(shx + 5 + 5 + 20 + 5 + 5, 5 + 5 + 10 + 5 + 10 + 5 + 20 + 5 + 20 + 5, 5, 5, get_bit(debug->latch[i], 7) ? ON_COLOR : OFF_COLOR);
			}
		}

		EndDrawing();
	}
	CloseWindow();
	UnloadAudioStream(as);
	if(video != NULL) UnloadRenderTexture(r);
	shiroi.stop = true;
	thread_end();
}
