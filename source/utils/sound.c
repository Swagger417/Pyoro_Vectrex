// ***************************************************************************
// sprites
// ***************************************************************************

#include <vectrex.h>
#include "sound.h"
#include "utils.h"

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

const struct sound_music_t music_off = 
{
	.adsr_table = (struct sound_adsr_table_t*) &Vec_ADSR_FADE4, 
	.twang_table = (struct sound_twang_table_t*) &Vec_TWANG_VIBEHL,
	.music_data = {128, 128, 0, 128}
};

// ---------------------------------------------------------------------------
// Complex explosion sound-effect handler
// Explosion Parameter Table Description
// Byte 0 = Tone and noise channel enables
// Bit 0 = Tone channel #
// 1 = Tone channel #
// 2 = Tone channel #
// 3 = Noise source #
// 4 = Noise source #
// 5 = Noise source #
// Byte 1 = Noise source sweep
// = 0 � Sweep frequency UP
// > 0 � Sweep frequencey DOWN
// < 0 � Inhibit frequency sweep
// Byte 2 = Volume sweep
// = 0 � Sweep volume UP
// > 0 � Sweep volume DOWN
// < 0 � Inhibit volume sweep
// Byte 3 = Explosion duration
// $01 � Longest explosion duration
// $80 � Shortest explosion duration
// Entry Values
// U = Explosion parameter table pointer
// DP = $C8
// Return Values
// A = Destroyed
// B = Destroyed
// X = Destroyed
// XACON = $00 (when explosion is completed)
// ---------------------------------------------------------------------------

const struct sound_explosion_t explosion_off =
{
	0b00111111, SOUND_EXPL_RISE, SOUND_VOL_FALL, 128U
}; 

// ---------------------------------------------------------------------------
// global ram variables

const struct sound_music_t* current_music = &music_off;
const struct sound_explosion_t* current_explosion = &explosion_off;

// ---------------------------------------------------------------------------
// initialize global variables

void sound_init()
{
	current_music = &music_off;
	current_explosion = &explosion_off;
}

// ---------------------------------------------------------------------------
// stop current explosion

void stop_explosion()
{
	Vec_Expl_Timer = 0;
	Vec_Expl_Flag = 0;
	current_explosion = &explosion_off;
}

// ---------------------------------------------------------------------------
// stop current music
 
void stop_music()
{
	Vec_Music_Flag = 0;
	current_music = &music_off;
}

// ---------------------------------------------------------------------------
// start new music

void play_music(const struct sound_music_t* music)
{
	// Vec_Music_Flag == 0 indicates that no current sound is playing
	stop_explosion();
	current_music = music;
	Vec_Music_Flag = 1; // start new music
}

// ---------------------------------------------------------------------------
// start new explosion

void play_explosion(const struct sound_explosion_t* explosion)
{
	// Vec_Expl_Timer == 0 indicates that no current explosion is playing
	Stop_Sound();
	stop_music();
	current_explosion = explosion;
	Vec_Expl_Flag = 128; // start new explosion
}

// ---------------------------------------------------------------------------
// start tune (will keep playing)

void play_tune(unsigned int channel, long unsigned int frequency, unsigned int volume)
{
	unsigned int cha = channel << 1;
	unsigned int x = (unsigned int) (frequency & 255LU);
	Sound_Byte(cha++, x);
	unsigned int y = (unsigned int) (frequency >> 8);
	Sound_Byte(cha, y);
	unsigned int z = channel + 8U;
	Sound_Byte(z, volume);
	Sound_Byte(7, 0b00111000);
}

// ***************************************************************************
// end of file
// ***************************************************************************

/*

Thema: Preliminary Report: Vectrex Sound
Von: XXXXX (Christopher L. Tumber)
Datum: 29 Jun 1998 19:41:08 GMT


This is a preliminary introduction into producing sound on the Vectrex.
Eventually, I hope to mold this document into a more comprehensive
examination of the Vectrex's sound capabilities, however, I think what I've
done so far will be of use to other developers so rather than wait, here it
is...


The Basics
==========

The Vectrex uses an AY-3-8912 Sound Chip. For the purposes of generating
sound, it's not really necessary to understand of the chip interfaces with
the Vectrex CPU because you'll be working mostly with one BIOS routine to do
all the work. That routine is byte_2_sound_chip ($F256).

The AY-3-8912 has 3 tone generators and 1 noise generator, these four sound
generators are accessible from 3 Voices (or channels). The three tone
generators map directly to the corresponding Voice while the noise generator
may be used by any of the three voices. So, tone generator 1 is always played
through Voice 1 and tone generator 2 always plays through Voice 2 but the
noise generator may play through Voice 1, Voice 2 or/and Voice 3. Each Voice
may play a sound generated by it's corresponding tone generator, the noise
generator or both. In this way you can mix tone and noise sounds to create
more complex sounds.

However, there is a serious limitation to the AY-3-8912. While you may play
three different tones at once, you only have access to the single noise
generator. This means that if you want to play noise on Voice 1 and Voice 3,
the sound is going to come from the same source (the noise generator) and
will be of the same frequency. In addition, you need to keep in mind that when
the noise generator is already being used for one sound, if you want to start
up a new one you will cut off the previous. (For example, if an enemy ship
blows up just before the play's ship does, you're going to need to turn off
the enemy ship's explosion early to start the explosion for the player's
ship. You also need to make sure that the remenants of the enemy's ship's
explosion doesn't interfere with the new sound, ie: If you would normally
faded out the enemy ship explosion sound.)

Registers
=========

The Vectrex uses 15 registers to communicate with the AY-3-8192. By storing
values to these registers, you program the AY-3-8192 to make sounds. The best
way to write to these registers is with the BIOS routine byte_2_sound_chip.
You can also use copy_bytes_2_sound_chip ($F27D) to write to several
registers. However, the problem with this routine is that it overwrites
registers you may want to just modify (Some registers are shared between
voices and if you simply write to the register, you will affect the other
voices. All you really want to do is AND and OR specific bits on that
register). Also the added overhead from looping probably isn't really a
good cost/benefit when you are usually only writing to a few registers at
a time.

To use byte_2_sound_chip is very simple. You simply specify which register
you want to write to in A and what value you want written in B and then call
byte_2_sound_chip.

For example: lda #$00 ;Modify Register 0
ldb #$ff ;Register will = #$ff
jsr byte_2_sound_chip ;Set Register

This code puts a value of #$ff into AY-3-8192 register 0. To make sounds, you
simply write the appropriate values to the appropriate registers.

byte_2_sound_chip also makes a copy of the values written to the AY-3-8192
into $c800-$c80d (ie: Anything written to register 5 also goes in $c805).
This is important so that you can check the state of the AY-3-8192 at any
time. To see if a Voice is in use already, for example.

The Long and Short of It
========================

Register Function
0 - Lower 8 Bits of Frequency for Tone generator 1
1 - Upper 4 Bits of Frequency for Tone generator 1
2 - Lower 8 Bits of Frequency for Tone generator 2
3 - Upper 4 Bits of Frequency for Tone generator 2
4 - Lower 8 Bits of Frequency for Tone generator 3
5 - Upper 4 Bits of Frequency for Tone generator 3
6 - 5 Bit frequency for Noise generator
7 - Tone/Noise On/Off switches for Voices 1,2 and 3
8 - Volume for Voice 1 (0-15)
9 - Volume for Voice 2 (0-15)
10 - Volume for Voice 3 (0-15)

(Higher registers seem to allow for waveform and ADSR envelope type
manipulation. Haven't gotten that far yet...)

To generate a sound, you need to set the Frequency for the sound generator(s)
you intend to use, set the Volume, and throw the appropriate on/off switches.

Tone Frequency is a 12 byte value from #$000 to #$fff. The lower 8 bits
go into register 0, 2 or 4 depending upon which tone generator and voice you
want to use. The upper 4 bits go into the lower four bits of register 1, 3 or
5.

So, to give Tone Generator 1 a Frequency value of #$123, you would execute
the following:

lda #$01 ;Modify Register 1
ldb #$01 ;Tone Frequency = #$1XX
jsr byte_2_sound_chip ;Set register
lda #$00 ;Modify Register 0
ldb #$23 ;Tone Frequency = #$X23
jsr byte_2_sound_chip ;Set register

Noise Frequency is a 5 bit value from #$00-#$1f. To set the Noise Generator's
Frequency at #$12:

lda #$06 ;Modify Register 6
ldb #$12 ;Noise Frequency=#$12
jsr byte_2_sound_chip ;Set register

Slightly counter-intuitively, the _lower_ the Frequency value, the higher the
frequency of the sound.

I don't, as yet, have a Real World to AY-3-8192 Frequency conversion yet...
You'll just have to play it by ear.


Volume and On/Off
=================

We've learned how to set the Frequency on the sound generators. However,
that's not enough to start any sound.

Next, we have to set the volume. The volume is controlled by registers 8, 9
and 10 with each register corresponding to a Voice. Volume can range from 0
to 15 with 15 being maximum volume and 0 being silence.

To set Voice 1 to a Volume of #10:

lda #$08 ;Modify Register 8
ldb #10 ;Volume=10
jsr byte_2_sound_chip ;Set register


In addition, we must determine which sound generators the Voice will use.
This is register 7's job. Register 7 is a little different from the others in
that it is shared among all three voices. Each bit in register 7 acts as an
on/off switch for one of the three Voices as follows:

Bit Effect
0 - Voice 1 use Tone Generator 1 On/Off
1 - Voice 2 use Tone Generator 2 On/Off
2 - Voice 3 use Tone Generator 3 On/Off
3 - Voice 1 use Noise Generator On/Off
4 - Voice 2 use Noise Generator On/Off
5 - Voice 3 use Noise Generator On/Off
(Bits 6 and 7 are unused)

For the purposes of register 7, a bit value of 0 is ON and a bit value of 1
is OFF. So, if bit 3 is 0, then Voice 1 will play Noise. If bit 3 is 1, then
Voice 1 will not play Noise.

As mentioned before, Voices may be set to play a Tone or Noise or Both. So,
if bit 0 and bit 3 are 0, the Voice 1 will play BOTH Tone and Noise.

To turn on a Tone for Voice 1:

lda #$07 ;Modify Register 7
ldb #$fe ;Turn Tone on Voice 1 on
jsr byte_2_sound_chip ;Set register

However, this code ALSO has the effect of turning off Voices 2 and 3 if they
happened to be on. This was the problem previously mentioned with using
copy_bytes_2_sound_chip. If Voice 2 or 3 happened to be playing some other
sound, you've just turned them off prematurely.

Instead, you want to just modify and not completely overwrite register 7. Do
this by referring to $c807 and using logical operands AND and OR to set the
appropriate bits.

So instead try:

lda #$07 ;Modify Register 7
ldb $c807 ;Get previous value for register 7
andb #$fe ;Turn on Tone on Voice 1
orb #$08 ;Turn off Noise on Voice 1
jsr byte_2_sound_chip ;Set register


Playing a Sound
===============

To play a tone on Voice 1:


lda #$01 ;Modify Register 1
ldb #$00 ;Tone Frequency = #$0XX
jsr byte_2_sound_chip ;Set register
lda #$00 ;Modify Register 0
ldb #$ff ;Tone Frequency = #$Xff
jsr byte_2_sound_chip ;Set register
lda #$08 ;Modify Register 8
ldb #15 ;Volume=15
jsr byte_2_sound_chip ;Set register
lda #$07 ;Modify Register 7
ldb $c807 ;Get previous value for register 7
andb #$fe ;Turn on Tone on Voice 1
orb #$08 ;Turn off Noise on Voice 1
jsr byte_2_sound_chip ;Set register

This turns on a tone on Voice 1. This tone will play indefinately. You have
to turn it off manually, it won't just go away. To do this, just flip the
appropriate switch on register 7:

lda #$07 ;Modify Register 7
ldb $c807 ;Get previous value for register 7
orb #$01 ;Turn off Tone on Voice 1
orb #$08 ;Turn off Noise on Voice 1
jsr byte_2_sound_chip ;Set register


Putting it all Together
=======================

Adding sound effects to a game is not difficult. As the Vectrex depends upon
a main_loop excuted many times per second to redraw the screen fast enough to
build a stable image we can simply integrate our a sound handling routine
into this loop.

Something like this:

main_loop:
jsr draw_screen ;Draw the screen
jsr update_everything ;Update movement, scores, collisions etc
jsr snd_check ;Sound management
bra main_loop:


What we need are flags to tell the sound routine which sound fx to stop/start
and away we go. To give you an example we'll have a flag called
shot_snd_flag. When the player shoots, the variable is given a value of #15.

When the snd_check routine is executed, if shot_snd_flag is non-zero then
shot_snd_flag is decremented by 1 and the sound of the player shooting is
made. If decrementing shot_snd_flag resulted in shot_snd_flag being zero,
then the shooting sound is turned off. This results in the tone being played
for 15 iterations of the main loop. Once shot_snd_flag is 0 no sound will be
made until the player fires again.


snd_check:
lda shot_snd_flag ;Get shot Flag
beq no_shot ;If 0, don't make shot sound

dec shot_snd_flag ;Reduce shot_snd_flag counter by 1
bne do_shot_snd ;If 0, turn off sound
ldb $c807 ;Get previous value of Register 7
orb #$01 ;Turn on bit 1 - No pulse on Voice 1
lda #$07 ;Select Register 7 (On/Off Switches)
jsr byte2sndchip ;Modify Register
rts ;End

do_shot_snd:
ldb #$ff ;Frequency = #$0ff
lda #$00 ;Select Register 0 (Voice 1 Frequency)
jsr byte2sndchip ;Modify Register
ldb #$00 ;Frequency = #$0ff
lda #$01 ;Select Register 1 (Voice 1 Frequency)
jsr byte2sndchip ;Modify Register
ldb #$0f ;Volume=#$0f
lda #$08 ;Register 8 (Voice 1 Volume)
jsr byte2sndchip ;Modify Register
ldb $c807 ;Get previous value of Register 7
andb #$FE ;Turn off bit 0 - Pulse on Voice 1
orb #$08 ;Turn on bit 4 - No noise on Voice 1
lda #$07 ;Select Register 7 (On/Off Switches)
jsr byte2sndchip ;Modify Register
no_shot:
rts ;End

This routine is open for improvement (but you should see the basic idea)...
There's really no need to keep turning the sound on for every shot_snd_flag=1
to 14, just when shot_snd_flag if 15 (but this is supposed to be a simple
example...).

Also, we can create much more complex sounds. For example, instead of just
using shot_snd_flag as a counter, we can also use it as an index. An index to
create an ADSR envelope or to modify the frequency dynamically.

If we did this instead:

lda shot_snd_flag ;shot_snd_flag is an index
ldx #shot_ADSR ;shot_ADSR is an ADSR structure
ldb a,x ;Get relative Volume value from ADSR structure
lda #$08 ;Register 8 (Volume)
jsr byte2sndchip ;Modify Register

shot_ADSR: fcb 0,3,5,7,9,11,13,15,13,11,9,7,5,3,1

Modifying the previous routine as above, will result in a tone which rises in
volume and then fades, rather than a flat waveform.

I believe the higher AY-3-8912 registers will do this sort of thing for us,
but, this may be a better alternative for dynamically preogramming the chip
since our main_loop is executed so many times per second anyway (50 times per
second, actually).

Similarily, we could modify the frequency dynamically to create even more
complex sounds... Like wawa pedals, or tones which rise or fall in frequency
(falling bombs...)



That's it for now. I'm still planning an improved song playing routine which
I hope will see the light of day shortly. I doubt it'll just be a hacked
init_sound/do_sound as all I really need from there is the
note->frequency->AY-3-8192 frequency algorithym. I'd also like a better
handle on what the "Envelope Generator" and "D/A Converters" are for. More
later when I've experimented more...


Comments welcome! (Particularly if anything isn't clear...)


Thema: Music Note Frequency Table
Von: aa993@FreeNet.Carleton.CA (Christopher L. Tumber)
Datum: 30 Jun 1998 17:47:12 GMT


Ah-ha!

As suspected, the ROM BIOS contains a table of Vectrex Note Frequencies. This
table starts at $fc8d. Each Frequency is 2 bytes, with the first byte being
the upper 4-bits of Frequency data and the second byte being the lower 8-bits
of Frequency Data.

The first 2 bytes are for note G2 and progress sequentially as per Appendix F
in Chris' (The other one!) tutorial.

So, to have Voice 1 play a G2 note:

lda #$01 ;Modify Register 1
ldb $fc8d ;Tone Frequency High 4-bits
jsr byte_2_sound_chip ;Set register
lda #$00 ;Modify Register 0
ldb #fc8e ;Tone Frequency Low 8-bits
jsr byte_2_sound_chip ;Set register

(And of course set the other registers: volume, on\off, etc)

Or, to have Voice 1 play an E4 note:

lda #$01 ;Modify Register 1
ldb $fcb7 ;Tone Frequency High 4-bits
jsr byte_2_sound_chip ;Set register
lda #$00 ;Modify Register 0
ldb #fcb8 ;Tone Frequency Low 8-bits
jsr byte_2_sound_chip ;Set register


Just do an indexed reference to the table (as init_sound does) and there you
go!

music_routine_data:
FC8D 03 .byte 0x03;
FC8E BD .byte 0xBD;
FC8F 03 .byte 0x03;
FC90 87 .byte 0x87;
FC91 03 .byte 0x03;
FC92 54 .byte 0x54;
FC93 03 .byte 0x03;
FC94 24 .byte 0x24;
FC95 02 .byte 0x02;
FC96 F7 .byte 0xF7;
FC97 02 .byte 0x02;
FC98 CD .byte 0xCD;
FC99 02 .byte 0x02;
FC9A A4 .byte 0xA4;
FC9B 02 .byte 0x02;
FC9C 7E .byte 0x7E;
FC9D 02 .byte 0x02;
FC9E 5B .byte 0x5B;
FC9F 02 .byte 0x02;
FCA0 39 .byte 0x39;
FCA1 02 .byte 0x02;
FCA2 19 .byte 0x19;
FCA3 01 .byte 0x01;
FCA4 FB .byte 0xFB;
FCA5 01 .byte 0x01;
FCA6 DE .byte 0xDE;
FCA7 01 .byte 0x01;
FCA8 C3 .byte 0xC3;
FCA9 01 .byte 0x01;
FCAA AA .byte 0xAA;
FCAB 01 .byte 0x01;
FCAC 92 .byte 0x92;
FCAD 01 .byte 0x01;
FCAE 7C .byte 0x7C;
FCAF 01 .byte 0x01;
FCB0 66 .byte 0x66;
FCB1 01 .byte 0x01;
FCB2 52 .byte 0x52;
FCB3 01 .byte 0x01;
FCB4 3F .byte 0x3F;
FCB5 01 .byte 0x01;
FCB6 2D .byte 0x2D;
FCB7 01 .byte 0x01;
FCB8 1C .byte 0x1C;
FCB9 01 .byte 0x01;
FCBA 0C .byte 0x0C;
FCBB 00 .byte 0x00;
FCBC FD .byte 0xFD;
FCBD 00 .byte 0x00;
FCBE EF .byte 0xEF;
FCBF 00 .byte 0x00;
FCC0 E2 .byte 0xE2;
FCC1 00 .byte 0x00;
FCC2 D5 .byte 0xD5;
FCC3 00 .byte 0x00;
FCC4 C9 .byte 0xC9;
FCC5 00 .byte 0x00;
FCC6 BE .byte 0xBE;
FCC7 00 .byte 0x00;
FCC8 B3 .byte 0xB3;
FCC9 00 .byte 0x00;
FCCA A9 .byte 0xA9;
FCCB 00 .byte 0x00;
FCCC A0 .byte 0xA0;
FCCD 00 .byte 0x00;
FCCE 97 .byte 0x97;
FCCF 00 .byte 0x00;
FCD0 8E .byte 0x8E;
FCD1 00 .byte 0x00;
FCD2 86 .byte 0x86;
FCD3 00 .byte 0x00;
FCD4 7F .byte 0x7F;
FCD5 00 .byte 0x00;
FCD6 78 .byte 0x78;
FCD7 00 .byte 0x00;
FCD8 71 .byte 0x71;
FCD9 00 .byte 0x00;
FCDA 6B .byte 0x6B;
FCDB 00 .byte 0x00;
FCDC 65 .byte 0x65;
FCDD 00 .byte 0x00;
FCDE 5F .byte 0x5F;
FCDF 00 .byte 0x00;
FCE0 5A .byte 0x5A;
FCE1 00 .byte 0x00;
FCE2 55 .byte 0x55;
FCE3 00 .byte 0x00;
FCE4 50 .byte 0x50;
FCE5 00 .byte 0x00;
FCE6 4B .byte 0x4B;
FCE7 00 .byte 0x00;
FCE8 47 .byte 0x47;
FCE9 00 .byte 0x00;
FCEA 43 .byte 0x43;
FCEB 00 .byte 0x00;
FCEC 3F .byte 0x3F;
FCED 00 .byte 0x00;
FCEE 3C .byte 0x3C;
FCEF 00 .byte 0x00;
FCF0 38 .byte 0x38;
FCF1 00 .byte 0x00;
FCF2 35 .byte 0x35;
FCF3 00 .byte 0x00;
FCF4 32 .byte 0x32;
FCF5 00 .byte 0x00;
FCF6 2F .byte 0x2F;
FCF7 00 .byte 0x00;
FCF8 2D .byte 0x2D;
FCF9 00 .byte 0x00;
FCFA 2A .byte 0x2A;
FCFB 00 .byte 0x00;
FCFC 28 .byte 0x28;
FCFD 00 .byte 0x00;
FCFE 26 .byte 0x26;
FCFF 00 .byte 0x00;
FD00 24 .byte 0x24;
FD01 00 .byte 0x00;
FD02 22 .byte 0x22;
FD03 00 .byte 0x00;
FD04 20 .byte 0x20;
FD05 00 .byte 0x00;
FD06 1E .byte 0x1E;
FD07 00 .byte 0x00;
FD08 1C .byte 0x1C;
FD09 00 .byte 0x00;
FD0A 1B .byte 0x1B;
FD0B 00 .byte 0x00;
FD0C 00 .byte 0x00;


Read more: http://vectorgaming.proboards.com/thread/312/vectrex-sound-programming#ixzz48qXFGwrT

*/
