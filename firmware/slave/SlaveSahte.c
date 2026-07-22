// Input pins
#define PinPoklopac PORTD.F0  // Manhole cover sensor
#define PinVoda     PORTD.F1  // Water sensor
#define Taster      PORTB.F0  // Display page button

// Output pins
#define LED_enP     PORTA.F2  // Manhole cover monitoring enabled
#define LED_enV     PORTA.F3  // Water monitoring enabled
#define LED_Alarm   PORTA.F4  // Alarm indicator

// RS-485 driver direction control
#define DR PORTC.F5

// Slave node address
unsigned char SAHTA_ID = 0x00;

// Sensor states
bit Poklopac;
bit Voda;

// Individual alarm states
bit AlarmPoklopac;
bit AlarmVoda;

// General alarm state
bit Alarm;

// Monitoring enable flags received from the master
bit enV;
bit enP;

// UART communication variables
unsigned char BytesToReceive = 0x00;
unsigned char ch = 0x00;
unsigned char Command = 0x00;
unsigned char CommandModified = 0x00;

// Control flags
bit UpdateLCDFlag;
bit CallFlag;

// Timer and button counters
unsigned char Counter = 0x00;
unsigned char Counter2 = 0x00;
unsigned char Counter3 = 0x00;


// Function declarations
void init_variables();
void init();
void transmit(unsigned char DATA8b);
void ProcessInputs();
void UpdateLCD();


// LCD pin configuration
sbit LCD_RS at RC0_bit;
sbit LCD_EN at RC2_bit;
sbit LCD_D7 at RD7_bit;
sbit LCD_D6 at RD6_bit;
sbit LCD_D5 at RD5_bit;
sbit LCD_D4 at RD4_bit;

// LCD pin directions
sbit LCD_RS_Direction at TRISC0_bit;
sbit LCD_EN_Direction at TRISC2_bit;
sbit LCD_D7_Direction at TRISD7_bit;
sbit LCD_D6_Direction at TRISD6_bit;
sbit LCD_D5_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD4_bit;


/**
 * Initializes all global variables.
 */
void init_variables()
{
    SAHTA_ID = 0x00;

    Poklopac = 0;
    Voda = 0;

    AlarmPoklopac = 0;
    AlarmVoda = 0;
    Alarm = 0;

    enV = 0;
    enP = 0;

    BytesToReceive = 0x00;
    ch = 0x00;
    Command = 0x00;
    CommandModified = 0x00;

    Counter = 0x00;
    Counter2 = 0x00;
    Counter3 = 0x00;

    UpdateLCDFlag = 0;
    CallFlag = 0;
}


/**
 * Initializes GPIO, Timer1, UART and interrupts.
 */
void init()
{
    // RA0 and RA1 are board potentiometer inputs.
    TRISA = 0x03;

    // RB0-RB5 are configured as digital inputs.
    TRISB = 0x3F;

    // RC6 and RC7 are used for serial communication.
    TRISC = 0xC0;

    // RD0-RD3 are digital inputs.
    TRISD = 0x0F;

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;

    // Disable ADC and configure pins as digital.
    ADCON0 = 0x00;
    ADCON1 = 0b00000110;

    INTCON = 0b11000000;
    PIE1 = 0b00000000;

    /*
     * Timer1 configuration:
     * Fosc = 20 MHz
     * Timer clock = Fosc / 4
     * Prescaler = 1:8
     * Timer period = approximately 100 ms
     */
    T1CON = 0b00110000;
    TMR1H = 0x0B;
    TMR1L = 0xDC;
    T1CON.TMR1ON = 1;

    PIR1.TMR1IF = 0;
    PIE1.TMR1IE = 1;

    // Initialize UART at 19200 baud.
    UART1_Init(19200);

    TXSTA.TXEN = 1;
    RCSTA.SPEN = 1;
    RCSTA.CREN = 1;
    PIE1.RCIE = 1;

    // Enable global interrupts.
    INTCON.GIE = 1;
}


/**
 * Transmits one byte through UART.
 */
void transmit(unsigned char DATA8b)
{
    TXREG = DATA8b;

    while (!TXSTA.TRMT)
        ;
}


/**
 * Updates the LCD with current sensor, enable and alarm states.
 */
void UpdateLCD()
{
    if (Counter3 == 0)
    {
        Lcd_Out(1, 1, "Voda Pok EnV EnP");

        if (Voda == 1)
            Lcd_Chr(2, 1, '1');
        else
            Lcd_Chr(2, 1, '0');

        if (Poklopac == 1)
            Lcd_Chr(2, 6, '1');
        else
            Lcd_Chr(2, 6, '0');

        if (enV == 1)
            Lcd_Chr(2, 10, '1');
        else
            Lcd_Chr(2, 10, '0');

        if (enP == 1)
            Lcd_Chr(2, 14, '1');
        else
            Lcd_Chr(2, 14, '0');
    }
    else
    {
        Lcd_Out(1, 1, "Alarm AVod APok");

        if (Alarm == 1)
            Lcd_Chr(2, 1, '1');
        else
            Lcd_Chr(2, 1, '0');

        if (AlarmVoda == 1)
            Lcd_Chr(2, 7, '1');
        else
            Lcd_Chr(2, 7, '0');

        if (AlarmPoklopac == 1)
            Lcd_Chr(2, 13, '1');
        else
            Lcd_Chr(2, 13, '0');
    }
}


/**
 * Reads sensor inputs and updates alarm and LED states.
 */
void ProcessInputs()
{
    // Read manhole cover sensor.
    if (PinPoklopac == 1)
    {
        Poklopac = 1;

        if (enP == 1)
            AlarmPoklopac = 1;
    }
    else
    {
        Poklopac = 0;
    }

    // Read water sensor.
    if (PinVoda == 1)
    {
        Voda = 1;

        if (enV == 1)
            AlarmVoda = 1;
    }
    else
    {
        Voda = 0;
    }

    // Update manhole cover monitoring indicator.
    if (enP == 0)
    {
        AlarmPoklopac = 0;
        LED_enP = 0;
    }
    else
    {
        LED_enP = 1;
    }

    // Update water monitoring indicator.
    if (enV == 0)
    {
        AlarmVoda = 0;
        LED_enV = 0;
    }
    else
    {
        LED_enV = 1;
    }

    // Activate the general alarm if either individual alarm is active.
    if ((AlarmPoklopac == 1) || (AlarmVoda == 1))
    {
        Alarm = 1;
        LED_Alarm = 1;
    }
    else
    {
        Alarm = 0;
        LED_Alarm = 0;
    }
}


/**
 * Main program.
 */
void main()
{
    init_variables();
    init();

    Lcd_Init();
    Lcd_Cmd(_LCD_CURSOR_OFF);

    UpdateLCD();

    while (1)
    {
        // Periodically update inputs and LCD content.
        if (UpdateLCDFlag == 1)
        {
            UpdateLCDFlag = 0;

            ProcessInputs();
            UpdateLCD();
        }

        // Send the current slave status after a request from the master.
        if (CallFlag == 1)
        {
            CallFlag = 0;
            CommandModified = 0x00;

            if (Poklopac == 1)
                CommandModified |= 0x01;

            if (Voda == 1)
                CommandModified |= 0x02;

            if (AlarmPoklopac == 1)
                CommandModified |= 0x04;

            if (AlarmVoda == 1)
                CommandModified |= 0x08;

            if (Alarm == 1)
                CommandModified |= 0x10;

            // Enable the RS-485 transmitter.
            DR = 1;

            transmit(Command);
            transmit(CommandModified);

            // Return to receive mode.
            DR = 0;
        }
    }
}


/**
 * Timer1 and UART receive interrupt service routine.
 */
void interrupt()
{
    // Timer1 interrupt every approximately 100 ms.
    if ((PIE1.TMR1IE == 1) && (PIR1.TMR1IF == 1))
    {
        PIR1.TMR1IF = 0;

        // Button debounce delay.
        if (Counter2 > 0)
            Counter2--;

        // Change the LCD page when the button is pressed.
        if (Counter2 == 0)
        {
            if (Taster == 1)
            {
                Counter2 = 10;

                if (Counter3 == 1)
                    Counter3 = 0;
                else
                    Counter3 = 1;

                UpdateLCDFlag = 1;
            }
        }

        // Request an LCD and sensor update every second.
        if (Counter == 9)
        {
            Counter = 0;
            UpdateLCDFlag = 1;
        }
        else
        {
            Counter++;
        }

        ProcessInputs();

        // Reload Timer1.
        TMR1H = 0x0B;
        TMR1L = 0xDC;
    }

    // UART receive interrupt.
    if ((PIE1.RCIE == 1) && (PIR1.RCIF == 1))
    {
        ch = RCREG;

        if (BytesToReceive == 0x00)
        {
            /*
             * The received command is valid when:
             * - bit 7 is set;
             * - the lower five bits match this slave address.
             */
            if (((ch & 0x1F) == SAHTA_ID) &&
                ((ch & 0x80) == 0x80))
            {
                Command = ch;
                BytesToReceive = 0x00;

                // Bit 6 enables water monitoring.
                if ((ch & 0x40) == 0x40)
                    enV = 1;
                else
                    enV = 0;

                // Bit 5 enables manhole cover monitoring.
                if ((ch & 0x20) == 0x20)
                    enP = 1;
                else
                    enP = 0;

                CallFlag = 1;
            }
        }
    }
}
