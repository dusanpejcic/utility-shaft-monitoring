#define SPI_Ethernet_HALFDUPLEX 0
#define SPI_Ethernet_FULLDUPLEX 1

#define DR PORTA.F5

#define TasterDisplay PORTB.F0
#define TasterPage    PORTB.F1

typedef struct {
   unsigned canCloseTCP: 1;
   unsigned isBroadcast: 1;
} TEthPktFlags;

const unsigned char httpHeader[] = "HTTP/1.1 200 OK\nContent-type: ";
const unsigned char httpMimeTypeHTML[] = "text/html\n\n";

unsigned char httpMethod[] = "GET /";

sfr sbit SPI_Ethernet_Rst at RA1_bit;
sfr sbit SPI_Ethernet_CS at RA0_bit;
sfr sbit SPI_Ethernet_Rst_Direction at TRISA1_bit;
sfr sbit SPI_Ethernet_CS_Direction at TRISA0_bit;

unsigned char myMacAddr[6] = {0x00, 0x14, 0xA5, 0x76, 0x19, 0x3F};
unsigned char myIpAddr[4] = {10, 99, 12, 1};

unsigned char getRequest[15];

unsigned char i;
unsigned char brojac;
unsigned char Sahta;
unsigned char Flag1;
unsigned char FlagLCD;
unsigned char ch;
unsigned char OBB;

unsigned char niz[200];
unsigned char br_ch;

unsigned char counterDisplay;
unsigned char counterPage;
unsigned char Counter1;

unsigned char Comm[32];
unsigned char Poklopac[32];
unsigned char Voda[32];
unsigned char EnablePoklopac[32];
unsigned char EnableVoda[32];
unsigned char AlarmPoklopac[32];
unsigned char AlarmVoda[32];
unsigned char Alarm[32];

// LCD pinout
sbit LCD_RS at RC0_bit;
sbit LCD_EN at RC2_bit;
sbit LCD_D7 at RB7_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D4 at RB4_bit;

sbit LCD_RS_Direction at TRISC0_bit;
sbit LCD_RW_Direction at TRISC1_bit;
sbit LCD_EN_Direction at TRISC2_bit;
sbit LCD_D7_Direction at TRISB7_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D4_Direction at TRISB4_bit;

void init_variables();
void init();
void UpdateLCD();
void transmit(unsigned char DATA8b);
void dodajUNiz(char *p_ch);
void formirajNiz();

void init_variables()
{
   br_ch = 0x00;
   OBB = 0x00;
   Flag1 = 0x00;
   FlagLCD = 0x00;
   brojac = 0x00;
   Sahta = 0x1F;
   ch = 0x00;

   counterDisplay = 0x00;
   counterPage = 0x00;
   Counter1 = 0x00;

   for (i = 0; i < 200; i++)
      niz[i] = 0x00;

   for (i = 0; i < 32; i++)
   {
      Comm[i] = 0x00;
      Poklopac[i] = 0x00;
      Voda[i] = 0x00;
      EnablePoklopac[i] = 0x00;
      EnableVoda[i] = 0x00;
      AlarmPoklopac[i] = 0x00;
      AlarmVoda[i] = 0x00;
      Alarm[i] = 0x00;
   }
}

void init()
{
   PIR1 = 0b00000000;
   PIE1 = 0b00100001;

   T1CON = 0b10110000;
   T1CON.TMR1ON = 1;

   TMR1L = 0xB5;
   TMR1H = 0xB3;

   INTCON = 0b01000000;
   INTCON.GIE = 1;

   //TRISA = 0x0C;
   TRISA = 0x00;
   TRISB = 0x0F;
   TRISC = 0xD0;

   PORTA = 0x00;
   PORTB = 0x00;
   PORTC = 0x00;

   ADCON0 = 0x00;
   ADCON1 = 0x0F;

   UART1_Init(19200);
   TXSTA.TXEN = 1;
   RCSTA.SPEN = 1;
   RCSTA.CREN = 1;

   Lcd_Init();
   Lcd_Cmd(_LCD_CURSOR_OFF);

   SPI1_Init_Advanced(_SPI_MASTER_OSC_DIV64,
                      _SPI_DATA_SAMPLE_MIDDLE,
                      _SPI_CLK_IDLE_LOW,
                      _SPI_LOW_2_HIGH);

   SPI_Ethernet_Init(myMacAddr, myIpAddr, SPI_Ethernet_FULLDUPLEX);
}

void UpdateLCD()
{
   unsigned char pocetak;

   if (counterPage == 0)
      pocetak = 0;
   else
      pocetak = 16;

   if (counterDisplay == 0)
      Lcd_Out(1, 1, "Voda            ");
   else if (counterDisplay == 1)
      Lcd_Out(1, 1, "Poklopac        ");
   else if (counterDisplay == 2)
      Lcd_Out(1, 1, "Alarm Voda      ");
   else if (counterDisplay == 3)
      Lcd_Out(1, 1, "Alarm Poklopac  ");
   else if (counterDisplay == 4)
      Lcd_Out(1, 1, "Alarm           ");
   else if (counterDisplay == 5)
      Lcd_Out(1, 1, "Enable Voda     ");
   else if (counterDisplay == 6)
      Lcd_Out(1, 1, "Enable Poklopac ");

   for (i = 0; i < 16; i++)
   {
      if (counterDisplay == 0)
         Lcd_Chr(2, i + 1, Voda[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 1)
         Lcd_Chr(2, i + 1, Poklopac[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 2)
         Lcd_Chr(2, i + 1, AlarmVoda[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 3)
         Lcd_Chr(2, i + 1, AlarmPoklopac[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 4)
         Lcd_Chr(2, i + 1, Alarm[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 5)
         Lcd_Chr(2, i + 1, EnableVoda[pocetak + (15 - i)] + '0');
      else if (counterDisplay == 6)
         Lcd_Chr(2, i + 1, EnablePoklopac[pocetak + (15 - i)] + '0');
   }
}

void transmit(unsigned char DATA8b)
{
   TXREG = DATA8b;
   while (!TXSTA.TRMT);
}

void interrupt()
{
   if ((PIE1.TMR1IE == 1) && (PIR1.TMR1IF == 1))
   {
      PIR1.TMR1IF = 0;

      if (brojac == 0x04)
      {
         brojac = 0x00;
         Flag1 = 0x01;
      }
      else
      {
         brojac++;
      }

      if (Counter1 > 0)
         Counter1--;

      if (Counter1 == 0)
      {
         if (TasterDisplay == 1)
         {
            Counter1 = 10;

            if (counterDisplay == 6)
               counterDisplay = 0;
            else
               counterDisplay++;

            FlagLCD = 0x01;
         }
         else if (TasterPage == 1)
         {
            Counter1 = 10;

            if (counterPage == 0)
               counterPage = 1;
            else
               counterPage = 0;

            FlagLCD = 0x01;
         }
      }

      TMR1L = 0xB5;
      TMR1H = 0xB3;
   }

   if ((PIE1.RCIE == 1) && (PIR1.RCIF == 1))
   {
      ch = RCREG;

      if (OBB != 0x00)
      {
         if (OBB == 0x02)
         {
            if (((ch & 0x1F) == Sahta) && ((ch & 0x80) == 0x80))
            {
               Comm[Sahta] = 1;
               OBB = 0x01;
            }
         }
         else if (OBB == 0x01)
         {
            Poklopac[Sahta] = (ch & 0x01) ? 1 : 0;
            Voda[Sahta] = (ch & 0x02) ? 1 : 0;
            AlarmPoklopac[Sahta] = (ch & 0x04) ? 1 : 0;
            AlarmVoda[Sahta] = (ch & 0x08) ? 1 : 0;
            Alarm[Sahta] = (ch & 0x10) ? 1 : 0;

            OBB = 0x00;
            FlagLCD = 0x01;
         }
      }
   }
}

void dodajUNiz(char *p_ch)
{
   while ((*p_ch) != 0x00)
   {
      niz[br_ch] = *p_ch;
      br_ch++;
      p_ch++;
   }
}

void formirajNiz()
{
   char txt[4];

   br_ch = 0;

   for (i = 0; i < 32; i++)
   {
      if (Comm[i] == 1)
      {
         dodajUNiz("Sahta: ");
         ByteToStr(i, txt);
         dodajUNiz(txt);
         dodajUNiz("; ");

         if (EnablePoklopac[i] == 1)
            dodajUNiz("Enable Poklopac; ");

         if (EnableVoda[i] == 1)
            dodajUNiz("Enable Voda; ");

         if (Voda[i] == 1)
            dodajUNiz("Voda; ");

         if (Poklopac[i] == 1)
            dodajUNiz("Poklopac; ");

         if (AlarmVoda[i] == 1)
            dodajUNiz("Alarm Voda; ");

         if (AlarmPoklopac[i] == 1)
            dodajUNiz("Alarm Poklopac; ");

         if (Alarm[i] == 1)
            dodajUNiz("Alarm; ");

         dodajUNiz("\n\n");
      }
   }

   niz[br_ch] = 0x00;
}

unsigned int putConstString(const char *s)
{
   unsigned int ctr = 0;

   while (*s)
   {
      SPI_Ethernet_putByte(*s++);
      ctr++;
   }

   return ctr;
}

unsigned int putString(char *s)
{
   unsigned int ctr = 0;

   while (*s)
   {
      SPI_Ethernet_putByte(*s++);
      ctr++;
   }

   return ctr;
}

unsigned int SPI_Ethernet_UserTCP(unsigned char *remoteHost,
                                  unsigned int remotePort,
                                  unsigned int localPort,
                                  unsigned int reqLength,
                                  char *canClose)
{
   unsigned int len = 0;

   if (localPort != 80)
      return 0;

   PORTA.F4 = 1;

   for (i = 0; i < 14; i++)
      getRequest[i] = SPI_Ethernet_getByte();

   getRequest[i] = 0;

   if (memcmp(getRequest, httpMethod, 5))
      return 0;

   if (getRequest[5] == 'v')
   {
      for (i = 0; i < 32; i++)
         EnableVoda[i] = 0x00;

      if ((getRequest[6] & 0x08) == 0x08) EnableVoda[31] = 1;
      if ((getRequest[6] & 0x04) == 0x04) EnableVoda[30] = 1;
      if ((getRequest[6] & 0x02) == 0x02) EnableVoda[29] = 1;
      if ((getRequest[6] & 0x01) == 0x01) EnableVoda[28] = 1;

      if ((getRequest[7] & 0x08) == 0x08) EnableVoda[27] = 1;
      if ((getRequest[7] & 0x04) == 0x04) EnableVoda[26] = 1;
      if ((getRequest[7] & 0x02) == 0x02) EnableVoda[25] = 1;
      if ((getRequest[7] & 0x01) == 0x01) EnableVoda[24] = 1;

      if ((getRequest[8] & 0x08) == 0x08) EnableVoda[23] = 1;
      if ((getRequest[8] & 0x04) == 0x04) EnableVoda[22] = 1;
      if ((getRequest[8] & 0x02) == 0x02) EnableVoda[21] = 1;
      if ((getRequest[8] & 0x01) == 0x01) EnableVoda[20] = 1;

      if ((getRequest[9] & 0x08) == 0x08) EnableVoda[19] = 1;
      if ((getRequest[9] & 0x04) == 0x04) EnableVoda[18] = 1;
      if ((getRequest[9] & 0x02) == 0x02) EnableVoda[17] = 1;
      if ((getRequest[9] & 0x01) == 0x01) EnableVoda[16] = 1;

      if ((getRequest[10] & 0x08) == 0x08) EnableVoda[15] = 1;
      if ((getRequest[10] & 0x04) == 0x04) EnableVoda[14] = 1;
      if ((getRequest[10] & 0x02) == 0x02) EnableVoda[13] = 1;
      if ((getRequest[10] & 0x01) == 0x01) EnableVoda[12] = 1;

      if ((getRequest[11] & 0x08) == 0x08) EnableVoda[11] = 1;
      if ((getRequest[11] & 0x04) == 0x04) EnableVoda[10] = 1;
      if ((getRequest[11] & 0x02) == 0x02) EnableVoda[9] = 1;
      if ((getRequest[11] & 0x01) == 0x01) EnableVoda[8] = 1;

      if ((getRequest[12] & 0x08) == 0x08) EnableVoda[7] = 1;
      if ((getRequest[12] & 0x04) == 0x04) EnableVoda[6] = 1;
      if ((getRequest[12] & 0x02) == 0x02) EnableVoda[5] = 1;
      if ((getRequest[12] & 0x01) == 0x01) EnableVoda[4] = 1;

      if ((getRequest[13] & 0x08) == 0x08) EnableVoda[3] = 1;
      if ((getRequest[13] & 0x04) == 0x04) EnableVoda[2] = 1;
      if ((getRequest[13] & 0x02) == 0x02) EnableVoda[1] = 1;
      if ((getRequest[13] & 0x01) == 0x01) EnableVoda[0] = 1;
   }

   if (getRequest[5] == 'p')
   {
      for (i = 0; i < 32; i++)
         EnablePoklopac[i] = 0x00;

      if ((getRequest[6] & 0x08) == 0x08) EnablePoklopac[31] = 1;
      if ((getRequest[6] & 0x04) == 0x04) EnablePoklopac[30] = 1;
      if ((getRequest[6] & 0x02) == 0x02) EnablePoklopac[29] = 1;
      if ((getRequest[6] & 0x01) == 0x01) EnablePoklopac[28] = 1;

      if ((getRequest[7] & 0x08) == 0x08) EnablePoklopac[27] = 1;
      if ((getRequest[7] & 0x04) == 0x04) EnablePoklopac[26] = 1;
      if ((getRequest[7] & 0x02) == 0x02) EnablePoklopac[25] = 1;
      if ((getRequest[7] & 0x01) == 0x01) EnablePoklopac[24] = 1;

      if ((getRequest[8] & 0x08) == 0x08) EnablePoklopac[23] = 1;
      if ((getRequest[8] & 0x04) == 0x04) EnablePoklopac[22] = 1;
      if ((getRequest[8] & 0x02) == 0x02) EnablePoklopac[21] = 1;
      if ((getRequest[8] & 0x01) == 0x01) EnablePoklopac[20] = 1;

      if ((getRequest[9] & 0x08) == 0x08) EnablePoklopac[19] = 1;
      if ((getRequest[9] & 0x04) == 0x04) EnablePoklopac[18] = 1;
      if ((getRequest[9] & 0x02) == 0x02) EnablePoklopac[17] = 1;
      if ((getRequest[9] & 0x01) == 0x01) EnablePoklopac[16] = 1;

      if ((getRequest[10] & 0x08) == 0x08) EnablePoklopac[15] = 1;
      if ((getRequest[10] & 0x04) == 0x04) EnablePoklopac[14] = 1;
      if ((getRequest[10] & 0x02) == 0x02) EnablePoklopac[13] = 1;
      if ((getRequest[10] & 0x01) == 0x01) EnablePoklopac[12] = 1;

      if ((getRequest[11] & 0x08) == 0x08) EnablePoklopac[11] = 1;
      if ((getRequest[11] & 0x04) == 0x04) EnablePoklopac[10] = 1;
      if ((getRequest[11] & 0x02) == 0x02) EnablePoklopac[9] = 1;
      if ((getRequest[11] & 0x01) == 0x01) EnablePoklopac[8] = 1;

      if ((getRequest[12] & 0x08) == 0x08) EnablePoklopac[7] = 1;
      if ((getRequest[12] & 0x04) == 0x04) EnablePoklopac[6] = 1;
      if ((getRequest[12] & 0x02) == 0x02) EnablePoklopac[5] = 1;
      if ((getRequest[12] & 0x01) == 0x01) EnablePoklopac[4] = 1;

      if ((getRequest[13] & 0x08) == 0x08) EnablePoklopac[3] = 1;
      if ((getRequest[13] & 0x04) == 0x04) EnablePoklopac[2] = 1;
      if ((getRequest[13] & 0x02) == 0x02) EnablePoklopac[1] = 1;
      if ((getRequest[13] & 0x01) == 0x01) EnablePoklopac[0] = 1;
   }

   formirajNiz();

   len = putConstString(httpHeader);
   len += putConstString(httpMimeTypeHTML);
   len += putString(niz);

   return len;
}

unsigned int SPI_Ethernet_UserUDP(unsigned char *remoteHost,
                                  unsigned int remotePort,
                                  unsigned int destPort,
                                  unsigned int reqLength,
                                  TEthPktFlags *flags)
{
   return 0;
}

void main()
{
   unsigned char ByteX = 0x00;

   init_variables();
   init();
   UpdateLCD();

   while (1)
   {
      SPI_Ethernet_doPacket();

      if (FlagLCD == 0x01)
      {
         FlagLCD = 0x00;
         UpdateLCD();
      }

      if (Flag1 == 0x01)
      {
         Flag1 = 0x00;

         Sahta++;

         if (Sahta == 0x20)
         {
            Sahta = 0x00;
            FlagLCD = 0x01;
         }

         ByteX = 0x80;

         if (EnableVoda[Sahta] == 1)
            ByteX = ByteX | 0x40;

         if (EnablePoklopac[Sahta] == 1)
            ByteX = ByteX | 0x20;

         ByteX = ByteX | Sahta;

         DR = 1;
         transmit(ByteX);
         DR = 0;

         OBB = 0x02;
      }
   }
}
