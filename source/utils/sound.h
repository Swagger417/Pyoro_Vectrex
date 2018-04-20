// ***************************************************************************
// sound
// ***************************************************************************

#pragma once

// ---------------------------------------------------------------------------
// Music data format:
// header word -> $C84F 32 nibble ADSR table
// header word -> $C851 8-byte "twang" table
// data bytes
//
// The ADSR table is simply 32 nibbles (16 bytes) of amplitude values.
// The twang table is 8 signed bytes to modify the base frequency of
// each note being played. Each channel has a different limit to its
// twang table index (6-8) to keep them out of phase to each other.
// 
// Music data bytes:
// Bits 0-5 = frequency
// Bit 6 clear = tone
// Bit 6 set = noise
// Bit 7 set = next music data byte is for next channel
// Bit 7 clear, play note with duration in next music data byte:
// bits 0-5 = duration
// bit 6 = unused
// bit 7 set = end of music
// ---------------------------------------------------------------------------

struct sound_adsr_table_t
{
	int amplitude[16];	// 32 nibbles amplitude offsets
};

// ---------------------------------------------------------------------------

struct sound_twang_table_t
{
	int frequency[8];	// base frequency offsets
};

// ---------------------------------------------------------------------------

struct sound_music_t
{
	const struct sound_adsr_table_t* adsr_table;	// pointer to 32 nibble ADSR table
	const struct sound_twang_table_t* twang_table; 	// pointer to 8 byte twang table
	unsigned int music_data[];						// music data bytes
};

// ---------------------------------------------------------------------------

extern const struct sound_music_t music_off;
extern const struct sound_music_t* current_music;

// ---------------------------------------------------------------------------

struct sound_explosion_t
{
	int enable;				// noise (bits 5-3) and tone channel (bits 2-0) enable (0b00nnnccc), 
	int noise;				// noise source sweep, =0 up, >0 down, <0 inhibit  
	int volume;				// volume sweep, =0 up, >0 down, <0 inhibit
	unsigned int duration;	// explosion duration, 0x01 longest to 0x80 shortest 
};

// ---------------------------------------------------------------------------

#define SOUND_EXPL_RISE 0
#define SOUND_EXPL_FALL 1
#define SOUND_EXPL_EVEN -1

#define SOUND_VOL_RISE 0
#define SOUND_VOL_FALL 1
#define SOUND_VOL_EVEN -1

extern const struct sound_explosion_t explosion_off;
extern const struct sound_explosion_t* current_explosion;

// ---------------------------------------------------------------------------

void sound_init();

void play_music(const struct sound_music_t* music);
void stop_music();

void play_explosion(const struct sound_explosion_t* explosion);
void stop_explosion();

void play_tune(unsigned int channel, long unsigned int frequency, unsigned int volume);

// ---------------------------------------------------------------------------

#define __N_G2	0x00U
#define __N_GS2	0x01U
#define __N_A2	0x02U
#define __N_AS2	0x03U
#define __N_B2	0x04U
#define __N_C3	0x05U
#define __N_CS3	0x06U
#define __N_D3	0x07U
#define __N_DS3	0x08U
#define __N_E3	0x09U
#define __N_F3	0x0AU
#define __N_FS3	0x0BU
#define __N_G3	0x0CU
#define __N_GS3	0x0DU
#define __N_A3	0x0EU
#define __N_AS3	0x0FU
#define __N_B3	0x10U
#define __N_C4	0x11U
#define __N_CS4	0x12U
#define __N_D4	0x13U
#define __N_DS4	0x14U
#define __N_E4	0x15U
#define __N_F4	0x16U
#define __N_FS4	0x17U
#define __N_G4	0x18U
#define __N_GS4	0x19U
#define __N_A4	0x1AU
#define __N_AS4	0x1BU
#define __N_B4	0x1CU
#define __N_C5	0x1DU
#define __N_CS5	0x1EU
#define __N_D5	0x1FU
#define __N_DS5	0x20U
#define __N_E5	0x21U
#define __N_F5	0x22U
#define __N_FS5	0x23U
#define __N_G5	0x24U
#define __N_GS5	0x25U
#define __N_A5	0x26U
#define __N_AS5	0x27U
#define __N_B5	0x28U
#define __N_C6	0x29U
#define __N_CS6	0x2AU
#define __N_D6	0x2BU
#define __N_DS6	0x2CU
#define __N_E6	0x2DU
#define __N_F6	0x2EU
#define __N_FS6	0x2FU
#define __N_G6	0x30U
#define __N_GS6	0x31U
#define __N_A6	0x32U
#define __N_AS6	0x33U
#define __N_B6	0x34U
#define __N_C7	0x35U
#define __N_CS7	0x36U
#define __N_D7	0x37U
#define __N_DS7	0x38U
#define __N_E7	0x39U
#define __N_F7	0x3AU
#define __N_FS7	0x3BU
#define __N_G7	0x3CU
#define __N_GS7	0x3DU
#define __N_A7	0x3EU
#define __N_AS7	0x3FU

// ***************************************************************************
// end of file
// ***************************************************************************
