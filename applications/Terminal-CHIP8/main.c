#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define DB (0)

// #define DARK_PIXEL 176
// #define LIGHT_PIXEL 178

// #define DARK_PIXEL '.'
// #define LIGHT_PIXEL '$'

#define DARK_PIXEL 0x5f
#define LIGHT_PIXEL 0xdb

#define GAME_SPEED 1

#define SCREEN_SIZE_X (0x3F+1)
#define SCREEN_SIZE_Y (0x1F+1)
#define MEMORY_SIZE (0xFFF)
#define START_ADDR (0x200)

/*  ::::::::::::::::::::::::::: :::::::::     :::     :::::::: :::::::::: */
/* :+:    :+:   :+:   :+:    :+::+:    :+:  :+: :+:  :+:    :+::+:        */
/* +:+          +:+   +:+    +:++:+    +:+ +:+   +:+ +:+       +:+        */
/* +#++:++#++   +#+   +#+    +:++#++:++#: +#++:++#++::#:       +#++:++#   */
/*        +#+   +#+   +#+    +#++#+    +#++#+     +#++#+   +#+#+#+        */
/* #+#    #+#   #+#   #+#    #+##+#    #+##+#     #+##+#    #+##+#        */
/*  ########    ###    ######## ###    ######     ### ######## ########## */

int _kbhit(void) {
    return 1; // always ready
}

int _getch(void) {
    return getchar(); // will block until key pressed
}

struct timeb {
    time_t time;
    unsigned short millitm;
};

int ftime(struct timeb *tb) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // monotonic is better for timers
    tb->time = ts.tv_sec;
    tb->millitm = ts.tv_nsec / 1000000;
    return 0;
}

uint8_t screen[SCREEN_SIZE_X][SCREEN_SIZE_Y] = {0};

uint8_t memory[MEMORY_SIZE] = {0};
uint8_t registers[0xF+1] = {0};
uint8_t *V0 = &registers[0];
uint8_t *VF = &registers[0xF];
uint16_t PC = START_ADDR;
uint16_t I = 0;

uint16_t stack[12] = {0};
uint8_t delayTimer = 0;
uint8_t soundTimer = 0;

uint8_t currentKey = 0x80;

unsigned char chip8_fontset[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};


/* :::::::::     :::    ::::    ::::::::::::  :::::::: ::::    ::::  */
/* :+:    :+:  :+: :+:  :+:+:   :+::+:    :+::+:    :+:+:+:+: :+:+:+ */
/* +:+    +:+ +:+   +:+ :+:+:+  +:++:+    +:++:+    +:++:+ +:+:+ +:+ */
/* +#++:++#: +#++:++#++:+#+ +:+ +#++#+    +:++#+    +:++#+  +:+  +#+ */
/* +#+    +#++#+     +#++#+  +#+#+#+#+    +#++#+    +#++#+       +#+ */
/* #+#    #+##+#     #+##+#   #+#+##+#    #+##+#    #+##+#       #+# */
/* ###    ######     ######    #############  ######## ###       ### */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/* ::::    ::::::    :::::::    :::: ::::::::: :::::::::::::::::::   */
/* :+:+:   :+::+:    :+:+:+:+: :+:+:+:+:    :+::+:       :+:    :+:  */
/* :+:+:+  +:++:+    +:++:+ +:+:+ +:++:+    +:++:+       +:+    +:+  */
/* +#+ +:+ +#++#+    +:++#+  +:+  +#++#++:++#+ +#++:++#  +#++:++#:   */
/* +#+  +#+#+#+#+    +#++#+       +#++#+    +#++#+       +#+    +#+  */
/* #+#   #+#+##+#    #+##+#       #+##+#    #+##+#       #+#    #+#  */
/* ###    #### ######## ###       ############ #############    ###  */

unsigned char rndtable[256] = {
    0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66 ,
    74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36 ,
    95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188 ,
    52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224 ,
    149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242 ,
    145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0 ,
    175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235 ,
    25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113 ,
    94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75 ,
    136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196 ,
    135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113 ,
    80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241 ,
    24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224 ,
    145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95 ,
    28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226 ,
    71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36 ,
    17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106 ,
    197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136 ,
    120, 163, 236, 249
};

uint8_t chip8_random()
{
    static uint8_t random_index = 0;
    return rndtable[random_index++];
}

/* :::    ::::::::::::::::   :::    :::    :::    :::    ::::    :::::::::::: :::       :::::::::::::::::::  */
/* :+:   :+: :+:       :+:   :+:    :+:    :+:  :+: :+:  :+:+:   :+::+:    :+::+:       :+:       :+:    :+: */
/* +:+  +:+  +:+        +:+ +:+     +:+    +:+ +:+   +:+ :+:+:+  +:++:+    +:++:+       +:+       +:+    +:+ */
/* +#++:++   +#++:++#    +#++:      +#++:++#+++#++:++#++:+#+ +:+ +#++#+    +:++#+       +#++:++#  +#++:++#:  */
/* +#+  +#+  +#+          +#+       +#+    +#++#+     +#++#+  +#+#+#+#+    +#++#+       +#+       +#+    +#+ */
/* #+#   #+# #+#          #+#       #+#    #+##+#     #+##+#   #+#+##+#    #+##+#       #+#       #+#    #+# */
/* ###    #############   ###       ###    ######     ######    ############# #######################    ### */

uint8_t decodech(char ret)
{
    switch(ret)
        {
            case 0x20:
                PC = MEMORY_SIZE;
                break;
            case '1':
                ret = 0x1;
                break;
            case '2':
                ret = 0x2;
                break;
            case '3':
                ret = 0x3;
                break;
            case '4':
                ret = 0xC;
                break;
            case 'q':
                ret = 0x4;
                break;
            case 'w':
                ret = 0x5;
                break;
            case 'e':
                ret = 0x6;
                break;
            case 'r':
                ret = 0xD;
                break;          
            case 'a':
                ret = 0x7;
                break;
            case 's':
                ret = 0x8;
                break;
            case 'd':
                ret = 0x9;
                break;
            case 'f':
                ret = 0xE;
                break;
            case 'z':
                ret = 0xA;
                break;
            case 'x':
                ret = 0x0;
                break;
            case 'c':
                ret = 0xB;
                break;
            case 'v':
                ret = 0xF;
                break;
            default:
                break;
        }
    return ret;
}

uint8_t getKeypad(char cmp)
{
    uint8_t ret = (currentKey == cmp);
    if(ret)
        currentKey = 0x80;
    return ret;
}

void checkKeyboard()
{
    if(_kbhit())
        currentKey = decodech(_getch());       
}


/*  :::::::: ::::::::: :::    ::: */
/* :+:    :+::+:    :+::+:    :+: */
/* +:+       +:+    +:++:+    +:+ */
/* +#+       +#++:++#+ +#+    +:+ */
/* +#+       +#+       +#+    +#+ */
/* #+#    #+##+#       #+#    #+# */
/*  ######## ###        ########  */

void execute(uint16_t instruction)
{
    uint8_t code = instruction>>12;
    uint8_t *VX = &registers[(instruction>>8)&0xF];
    uint8_t *VY = &registers[(instruction>>4)&0xF];
    uint8_t subcode4 = instruction&0xF;
    uint8_t subcode8 = instruction&0xFF;
    static uint8_t stack_ptr = 0;

    #if DB
    printf("instruction=%04x, I=%04x\nVX=V%1x=%02x,VY=V%1x=%02x\n",
    instruction,
    I,
    (instruction>>8)&0xF,
    *VX,
    (instruction>>4)&0xF,
    *VY
    );
    printf("registers= |");
    for(int i =0; i<=0xF; i++)
        printf("%02x|",registers[i]);
    printf("\n");
    printf("stack_ptr %02x\n", stack_ptr);
    printf("stack at ptr %02x\n",stack[stack_ptr]);
    getchar();
    #endif

    switch(code)
    {
        case 0x0:
            switch(subcode8)
            {
                case 0xE0:
                    for(int j=0; j< SCREEN_SIZE_Y; j++)
                    {
                        for(int i=0; i< SCREEN_SIZE_X; i++)
                        {
                            screen[i][j] = 0;
                        }
                    }
                    break;
                case 0xEE:
                    PC = stack[--stack_ptr];
                    break;
                default:
                    break;
            }
            break;
        case 0x1:
            PC = (instruction&0xFFF)-2;
            break;
        case 0x2:
            stack[stack_ptr++] = PC;
            PC = (instruction&0xFFF)-2;
            break;
        case 0x3:
            if(*VX == (instruction&0xFF))
                PC+=2;
            break;
        case 0x4:
            if(*VX != (instruction&0xFF))
                PC+=2;
            break;
        case 0x5:
            if(*VX==*VY)
                PC+=2;
            break;
        case 0x6:
            *VX = instruction&0xFF;
            break;
        case 0x7:
            *VX += instruction&0xFF;
            break;
        case 0x8:
            switch(subcode4)
            {
                case 0x0:
                    *VX=*VY;
                    break;
                case 0x1:
                    *VX|=*VY;
                    *VF = 0;
                    break;
                case 0x2:
                    *VX&=*VY;
                    *VF = 0;
                    break;
                case 0x3:
                    *VX^=*VY;
                    *VF = 0;
                    break;
                case 0x4:
                    {
                        uint16_t sum = *VX+*VY;
                        *VX = sum&0xFF;
                        *VF = sum!=*VX?1:0;
                    }
                    break;
                case 0x5:
                    {
                        uint8_t flag = *VX<*VY?0:1;
                        *VX -=*VY;
                        *VF = flag;
                    }
                    break;
                case 0x6:
                    {
                        uint8_t flag = *VY&0x01;
                        *VX = *VY>>1;
                        *VF = flag;
                    }
                    break;
                case 0x7:
                    {
                        uint8_t flag=*VY>=*VX?1:0;
                        *VX=*VY-*VX;
                        *VF = flag;
                    }
                    break;
                case 0xE:
                    {
                        uint8_t flag = *VY>>7;
                        *VX = *VY<<1;
                        *VF = flag;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 0x9:
            if(*VX!=*VY)
                PC+=2;
            break;
        case 0xA:
            I = instruction&0xFFF;
            break;
        case 0xB:
            PC = *V0+(instruction&0xFFF) - 2;
            break;
        case 0xC:
            *VX = chip8_random()&(instruction&0xFF);
            break;
        case 0xD:
            *VF = 0;
            for(int j = 0; j<subcode4; j++)
            {
                uint8_t mem8 = memory[I+j];
                for(int i =0; i<8; i++)
                {
                    uint8_t y_coord = (*VY+j)%32;
                    uint8_t x_coord = (*VX+i)%64;
                    if((screen[x_coord][y_coord]) && ((mem8>>(7-i))&0x1))
                        *VF = 1;
                    screen[x_coord][y_coord] ^= (mem8>>(7-i))&0x1;
                
                }
            }
            break;
        case 0xE:
            switch(subcode8)
            {
                case 0x9E:
                    if(getKeypad(*VX))
                    {
                        PC+=2;
                    }
                    break;
                case 0xA1:
                    if(!getKeypad(*VX))
                    {
                        PC+=2;
                    }
                    break;
                default:
                break;
            }
        case 0xF:
            switch(subcode8)
            {
                case 0x07:
                    *VX = delayTimer;
                    break;
                case 0x0A:
                    *VX = decodech(getchar());
                    break;
                case 0x15:
                    delayTimer = *VX;
                    break;
                case 0x18:
                    soundTimer = *VX;
                    // printf("\a");
                    break;
                case 0x1E:
                    I += *VX;
                    break;
                case 0x29:
                    I = *VX*0x5;
                    break;
                case 0x33:
                    {
                        char BCD[3];
                        sprintf(BCD,"%03d",*VX);
                        memory[I] = BCD[0] - 0x30;
                        memory[I+1] = BCD[1] - 0x30;
                        memory[I+2] = BCD[2] - 0x30;
                    }
                    break;
                case 0x55:
                    {
                        int x = (instruction>>8)&0xF;
                        for(int i = 0; i<=x; i++)
                        {
                            memory[I+i] = registers[i];
                        }
                        I += x + 1;
                    }
                    break;
                case 0x65:
                    {
                        int x = (instruction>>8)&0xF;
                        for(int i = 0; i<=x; i++)
                        {
                            registers[i] = memory[I+i];
                        }
                        I += x + 1;
                    }
                    break;
            }
        default:
        break;
    }
}


/*  :::::::: ::::::::: :::    ::: */
/* :+:    :+::+:    :+::+:    :+: */
/* +:+       +:+    +:++:+    +:+ */
/* :#:       +#++:++#+ +#+    +:+ */
/* +#+   +#+#+#+       +#+    +#+ */
/* #+#    #+##+#       #+#    #+# */
/*  ######## ###        ########  */

void printScreen()
{
    static char screenPrintBuf[SCREEN_SIZE_Y*SCREEN_SIZE_X+100] = {0};
    uint32_t n = 0;
    for(int j = 0; j<SCREEN_SIZE_Y; j++)
    {
        for(int i =0; i<SCREEN_SIZE_X; i++)
        {
            char pixel[2] = {DARK_PIXEL,0};
            if(screen[i][j] != 0)
            {
                pixel[0] = LIGHT_PIXEL;
            }
            sprintf(&screenPrintBuf[n++],pixel);
        }
        sprintf(&screenPrintBuf[n++],"\n");
    }
    sprintf(&screenPrintBuf[n++],"PC = %04x\n",PC);
    fwrite(screenPrintBuf, sizeof(char), n+9, stdout);
}

/* ::::::::::::::    ::::::::::::::::::::::: :::   :::   :::::::: ::::::::::::::::::::::::    ::::::::::::  */
/* :+:       :+:+:   :+:    :+:    :+:    :+::+:   :+:  :+:    :+::+:           :+:    :+:    :+::+:    :+: */
/* +:+       :+:+:+  +:+    +:+    +:+    +:+ +:+ +:+   +:+       +:+           +:+    +:+    +:++:+    +:+ */
/* +#++:++#  +#+ +:+ +#+    +#+    +#++:++#:   +#++:    +#++:++#+++#++:++#      +#+    +#+    +:++#++:++#+  */
/* +#+       +#+  +#+#+#    +#+    +#+    +#+   +#+            +#++#+           +#+    +#+    +#++#+        */
/* #+#       #+#   #+#+#    #+#    #+#    #+#   #+#     #+#    #+##+#           #+#    #+#    #+##+#        */
/* #############    ####    ###    ###    ###   ###      ######## ##########    ###     ######## ###        */

int main(void)
{
    struct timeb t_clock,t_60,end;
    ftime(&t_clock);
    ftime(&t_60);
    for(int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];
    
    
    /* :::::::::  :::::::: ::::    ::::     :::    :::    :::    ::::    :::::::::::: :::       :::::::::::::::::::  */
    /* :+:    :+::+:    :+:+:+:+: :+:+:+    :+:    :+:  :+: :+:  :+:+:   :+::+:    :+::+:       :+:       :+:    :+: */
    /* +:+    +:++:+    +:++:+ +:+:+ +:+    +:+    +:+ +:+   +:+ :+:+:+  +:++:+    +:++:+       +:+       +:+    +:+ */
    /* +#++:++#: +#+    +:++#+  +:+  +#+    +#++:++#+++#++:++#++:+#+ +:+ +#++#+    +:++#+       +#++:++#  +#++:++#:  */
    /* +#+    +#++#+    +#++#+       +#+    +#+    +#++#+     +#++#+  +#+#+#+#+    +#++#+       +#+       +#+    +#+ */
    /* #+#    #+##+#    #+##+#       #+#    #+#    #+##+#     #+##+#   #+#+##+#    #+##+#       #+#       #+#    #+# */
    /* ###    ### ######## ###       ###    ###    ######     ######    ############# #######################    ### */
    char* rompath = malloc(1024);
    scanf("%s",rompath);
    char c;
    while((c = getchar()) != '\n' && c != EOF);
    FILE *file = fopen(rompath, "rb");
    
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }    
    uint8_t ch;
    uint16_t i = START_ADDR;
    while (((ch = fgetc(file)) != EOF) && (i<MEMORY_SIZE)) {
        memory[i++] = (ch&0xFF);
        memory[i++] = fgetc(file)&0xFF;
    }
    fclose(file);
    free(rompath);
    /* :::::::::::::    ::::::::::::::::::::: :::    ::::::::::::::::::::::::    :::       ::::::::  :::::::: :::::::::  */
    /* :+:       :+:    :+::+:      :+:    :+::+:    :+:    :+:    :+:           :+:      :+:    :+::+:    :+::+:    :+: */
    /* +:+        +:+  +:+ +:+      +:+       +:+    +:+    +:+    +:+           +:+      +:+    +:++:+    +:++:+    +:+ */
    /* +#++:++#    +#++:+  +#++:++# +#+       +#+    +:+    +#+    +#++:++#      +#+      +#+    +:++#+    +:++#++:++#+  */
    /* +#+        +#+  +#+ +#+      +#+       +#+    +#+    +#+    +#+           +#+      +#+    +#++#+    +#++#+        */
    /* #+#       #+#    #+##+#      #+#    #+##+#    #+#    #+#    #+#           #+#      #+#    #+##+#    #+##+#        */
    /* #############    #####################  ########     ###    ##########    ##################  ######## ###        */
    static uint8_t gameTic = 2;
    #if GAME_SPEED == 1
        gameTic = 2;
    #elif GAME_SPEED == 2
        gameTic = 1;
    #endif

    while(PC < MEMORY_SIZE)
    {
        ftime(&end);
        if((1000.0 * (end.time - t_clock.time)+(end.millitm - t_clock.millitm)) > gameTic)
        {
            ftime(&t_clock);
            execute((memory[PC]<<8)+memory[PC+1]);
            PC+=2;
        }
        if((1000.0 * (end.time - t_60.time)+(end.millitm - t_60.millitm)) > 17)
        {
            ftime(&t_60);
            if(delayTimer != 0) //60Hz
                delayTimer --;
            if(soundTimer != 0) //60Hz
            {
                soundTimer --;
            }
        }
        checkKeyboard();
        printScreen();
    }
    return 0;
}