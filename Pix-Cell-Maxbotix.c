//*************************************************************************
// This code is used for UControl Pix-Cell GSM Controller to power 
// a Maxbotic ultrasonic range sensor, takes a sample, and send out the 
// data via SMS meassages. 
// 
// By Yuttanant Suwansiri in 2011 
//*************************************************************************

void CLRWDT(void);
void Delay_ml(unsigned int time_limit);
void InitBrd(void);
void GsmSerialInit(const int BaudRate);
unsigned char GsmSerialRd(void);
void GsmSerialWr(unsigned char Data);
void UserSerialInit(const int BaudRate);
unsigned char UserSerialRd(void);
void UserSerialWr(unsigned char Data);
int BitRdInput(int InputNum);
unsigned char AllRdInput(void);
unsigned int AnaRdInput(int InputNum);
int TempRd(void);
unsigned int RtcVoltRd(void);
void BitWrOut(int value);
void PulseWrOut(unsigned int Time);
void PulseWrOutNot(unsigned int Time);
unsigned char MemRd(unsigned int address);
void MemWrt(unsigned int address,unsigned char data_m);
void GsmOff(void);
void GsmOn(void);
void RstGsm(void);
void GreenLED(int Status);
void SendAtCommand(unsigned char p[]);
unsigned char *GetGsmData(void);
int SmsInit(void);
int SendSms(unsigned char phone_num[],unsigned char msg[]);
unsigned char *ReadSms (int index);
int DeleteSms(int index);
int CheckModem(void);
int SetRtc(unsigned char TD_Buffer[]);
unsigned char *GetRtc(void);
void PrintAsciiDataToUser(unsigned char p[]);

// Rev: A0
// Date: 20.12.07
//*************************************************************************
//   This example waits for a logical pulse signal in Input number 1,
//   after it receives the pulse the green LED is turn on and an SMS message
//   is send to a specific phone number.
//*************************************************************************

void main()
{
     unsigned char phone_num[]="316XXXXXXX",msg[]="XXX";
     double current;
     InitBrd();               //Initialize the GSM Board functions.
     while(!CheckModem());    //Checks if the modem is connected.
     while(!SmsInit());       //Iniatilize the GSM modem for SMS.
     while(1)
     {
        GreenLED(0);             //Turn OFF the green LED.
        Delay_ml(60000);        //Delay of 15mS for stability of the input.
        GreenLED(1);             //Turn ONthe green LED.
        BitWrOut(1);             //On for 5 secs
        Delay_ml(5000);
        current = (AnaRdInput(1)); //waits until a logical 1 is received in input 1.
        BitWrOut(0);
        GreenLED(0);
        //while(!BitRdInput(1));//waits until the input signal return to logical 0.
        Delay_ml(2000);
        if (current > 1) {
        GreenLED(1);          //Turn the green LED ON
        Delay_ml(2000);
        SendSms("316XXXXXXX","XXX");
        //GreenLED(0); //Send a SMS.
        }
        }
        //you can also use this form: SendSms(phone_num,msg);
     }

//************************************************************************
//************************************************************************
void CLRWDT(void)
{
  asm{
  CLRWDT
  }
}// CLRWDT
//***********************************************************
void Delay_ml(unsigned int time_limit)
{
  unsigned int i;
  for(i=0;i<time_limit;i++)
  {
     Delay_ms(1);
     CLRWDT();
  }
}//Delay_ml
//***********************************************************
void InitBrd(void)
{
     CLRWDT();
     OSCCON=0x70;        // $70 - Primary Clock 8MHz.
     OSCTUNE=0x40;        // 4xPLL enable, 8MHz x 4xPLL=32MHz
                          // $0 - Enabled the External Oscillator.
     TRISA=0xFF;         // PortA Register = All PortA are Inputs.
     TRISB=0xFD;         // PortB Register = RB1=Output(SW_OFF) RB0=Input(LED).
     PortB=0;            // Set the voltage to the Cellular Module.
     TRISC=0xBF;         // PortC Register = RC6-Input, Other-Outputs.
     TRISD=0x00;         // PortD Register = All PortD are Outputs.
     PortD=0;            // Set PortD output = Zero.
     TRISE=0x00;         // PortE Register = All PortE are Outputs.
     PortE=0;            // Set PortE output = Zero.
     TRISF=0x06;         // PortF Register = RF1,RF2-Input, Other-Outputs.
     PortF=0;            // Set PortF output = Zero.
     TRISG=0xFD;         // PortG Register = RG1-Output, Other-Inputs.
     ADCON0=0x00;        // A/D Control Register = A/D Off.
     ADCON1=0x0f;        // Analog Select Register = All Ports are Digital.
     I2C1_Init(100000);   // initialize clock=100kHz.
     Uart1_init(9600);  // initialize USART1 module - GSM Serial Com.
     Uart2_init(9600);  // initialize USART2 module - User Serial Com.
     PortD=0x80;         // Power-up the Cellular Module.
     Delay_ml(3000);
     PortD=0x0;
     //Soft_Uart_Init(PORTA, 0, 2, 9600, 0);
     Delay_ml(9000);
}//InitBrd
//***********************************************************
void GsmSerialInit(const int BaudRate)
{
     switch (BaudRate)
     {
     //the compiler demands that the value be constant in the function call...
            case 2400:Uart1_Init(2400);break;
            case 4800:Uart1_Init(4800);break;
            case 9600:Uart1_Init(9600);break;
            case 19200:Uart1_Init(19200);break;
            case 38400:Uart1_Init(38400);break;
            case 57600:Uart1_Init(57600);break;
     }
}//GsmSerialInit
//***********************************************************
unsigned char GsmSerialRd(void)
{
     while(!Uart1_Data_Ready());
     return Uart1_Read();
     CLRWDT();
}//GsmSerialRd
//***********************************************************
void GsmSerialWr(unsigned char Data)
{
    Uart1_Write(Data);
    Delay_ml(3);
}//GsmSerialWr
//***********************************************************
void UserSerialInit(const int BaudRate)
{
     switch (BaudRate)
     {
     //the compiler demands that the value be constant in the function call...
            case 2400:Uart2_Init(2400);break;
            case 4800:Uart2_Init(4800);break;
            case 9600:Uart2_Init(9600);break;
            case 19200:Uart2_Init(19200);break;
            case 38400:Uart2_Init(38400);break;
            case 57600:Uart2_Init(57600);break;
     }
}//GsmSerialInit
//***********************************************************
unsigned char UserSerialRd(void)
{
     while(!Uart2_Data_Ready());
     return Uart2_Read();
}//UserSerialRd
//***********************************************************
void UserSerialWr(unsigned char Data)
{
    Uart2_Write(Data);
}//UserSerialWr
//***********************************************************
int BitRdInput(int InputNum)
{
 int Result;
 TRISA=0xFF;
 TRISF=0x06;
 ADCON0=0x0;
 ADCON1=0x0f;
 CLRWDT();
 switch (InputNum)
     {
            case 1: Result=PortA&0x1;              break;
            case 2: Result=((PortA&0x2)>>1);       break;
            case 3: Result=((PortF&0x2)>>1);       break;
     }
 return Result;
}//BitRdInput
//***********************************************************
unsigned char AllRdInput(void)
{
     TRISA=0xFF;
     TRISF=0x02;
     ADCON0=0x0;
     ADCON1=0x0f;
     return (((PortF & 0x2)<<1)|(PortA & 0x03));
}//AllRdInput
//***********************************************************
unsigned int AnaRdInput(int InputNum)
{
     unsigned int i,Analog_Value;
     InputNum=InputNum-1;
     if (InputNum==2)InputNum=6;
     ADCON1=0x17;
     ADCON2=0x90;
     Delay_ml(5);
     for(i=0;i<5;i++)
     {
       Analog_Value=ADC_read(InputNum);
       Delay_ml(7);
     }
     ADCON0=0x0;
     ADCON1=0x0F;
     CLRWDT();
     return Analog_Value;
}//AnaRdInput
//***********************************************************
int TempRd(void)
{
     unsigned int i, Temp_Value;
     ADCON1=0x17;
     ADCON2=0x90;
     CMCON=0x07;
     Delay_ml(5);
     for(i=0;i<5;i++)
     {
       Temp_Value=ADC_read(7);
       Delay_ml(7);
     }
     ADCON0=0x0;
     ADCON1=0x0F;
     return (((Temp_Value-41)/4)-50);
}//TempRd
//***********************************************************
unsigned int RtcVoltRd(void)
{
 unsigned int i, Voltage_Value;
 ADCON1=0x17;
 ADCON2=0x90;
 Delay_ml(5);
 for(i=0;i<5;i++)
 {
   Voltage_Value=ADC_read(4);
   Delay_ml(7);
 }
 ADCON0=0x0;
 ADCON1=0x0F;
 return (Voltage_Value/40);
}//RtcVoltRd
//***********************************************************
void BitWrOut(int value)
{
     TRISE=0x0;
     switch (value)
     {
            case 0  : PortE=0x0;     break;
            case 1  : PortE=0x4;     break;
           default  : PortE=0x0;     break;

     }
}//BitWrOut
//***********************************************************
void PulseWrOut(unsigned int Time)
{
     unsigned int  i;
     TRISE=0;
     PortE=0;
     Delay_ml(3);
     PortE=0x4;
     for(i=0;i<Time;i++) Delay_ml(1);
     PortE=0;
}//PulseWrOut
//***********************************************************
void PulseWrOutNot(unsigned int Time)
{
     unsigned int  i;
     TRISE=0;
     PortE=0x4;
     Delay_ml(3);
     PortE=0;
     for(i=0;i<Time;i++) Delay_ml(1);
     PortE=0x4;
}//PulseWrOutNot
//***********************************************************
unsigned char MemRd(unsigned int address)
 {
  unsigned char data_m,Add_H,Add_L;
  Add_H=((address&0xff00)>>8);
  Add_L=(address&0x00FF);
  I2C1_Init(100000);
  I2C1_Start();
  I2C1_Wr(0xA0);
  I2C1_Wr(Add_H);
  I2C1_Wr(Add_L);
  I2C1_Repeated_Start();
  I2C1_Wr(0xA1);
  data_m = I2C1_Rd(0u);
  I2C1_Stop();
  return data_m;
 }//MemRd
//***********************************************************
void MemWrt(unsigned int address,unsigned char data_m)
 {
   unsigned char Add_H,Add_L;
   Add_H=((address&0xFF00)>>8);
   Add_L=(address&0x00FF);
   I2C1_Init(100000);
   I2C1_Start();
   I2C1_Wr(0xA0);
   I2C1_Wr(Add_H);
   I2C1_Wr(Add_L);
   I2C1_Wr(data_m);
   I2C1_Stop();
   Delay_ml(20);
 }//MemWrt
//***********************************************************
void GsmOff(void)
{
     PortB=0x2;
     PortD=0x0;
     Delay_ml(500);
}//GsmOff
//**********************************************************
void GsmOn(void)
{
     PortB=0x0;
     PortD=0x0;
     Delay_ml(500);
}//GsmOn
//**********************************************************
void RstGsm(void)
{
       PortD=0x80;
       Delay_ml(2000);
       PortD=0;
}//RstGsmHw
//**********************************************************
void GreenLED(int Status)
{
     TRISF=0x6;
     switch (Status)
     {
            case 0  : PortF=0x80;     break;
            case 1  : PortF=0x0;      break;
           default  : PortF=0x80;     break;

     }
}//GreenLED
//***********************************************************
void SendAtCommand(unsigned char p[])
{
      int i;
      for(i=0;i<strlen(p);i++)
      {
          GsmSerialWr(p[i]);
      }
}//SendAtCommand
//***********************************************************
unsigned char *GetGsmData(void)
{
     unsigned char Buffer[201]={0x0},index=0;
     unsigned char *GsmBuffer=Buffer;
     int time_counter=0,time_limit=1000;
     while((!Uart1_Data_Ready())&& (time_counter < 5000))
     {
           Delay_ml(1);
           time_counter++;
     }
     if (time_counter>=5000) return Buffer;
     (*GsmBuffer++)=Uart1_Read();
     while(1)
     {
             time_counter=0;
             while((!Uart1_Data_Ready()) && (time_counter < time_limit))
             {
                 Delay_ml(1);
                 time_counter++;
             }
             index++;
             if ((time_counter<time_limit)&&(index < 200))
                (*GsmBuffer++)=Uart1_Read();
             else  return Buffer;

     }
}//GetGsmData
//***********************************************************
int SmsInit(void)
{
   unsigned char *GsmBuffer;

   SendAtCommand("ATE0\r");
   GsmBuffer=GetGsmData();
   if (strstr(GsmBuffer,"OK"))
   {
    SendAtCommand("AT+CSDH=1\r");
    GsmBuffer=GetGsmData();
    if (strstr(GsmBuffer,"OK"))
    {
       SendAtCommand("AT+CSMP=17,167\r");
       GsmBuffer=GetGsmData();
       if (strstr(GsmBuffer,"OK"))
       {
          SendAtCommand("AT+CMGF=1\r");
          GsmBuffer=GetGsmData();
          if (strstr(GsmBuffer,"OK"))
          {
             SendAtCommand("AT+CPMS=\"SM\",\"SM\",\"SM\"\r");
             GsmBuffer=GetGsmData();
             if (strstr(GsmBuffer,"OK"))
             {
                SendAtCommand("AT+CNMI=2,0\r");
                GsmBuffer=GetGsmData();
                if (strstr(GsmBuffer,"OK")) return 1;
                else return 0;
             }
             else return 0;
          }
          else return 0;
       }
       else return 0;
    }
    else return 0;
  }
  else return 0;
}//SmsInit
//***********************************************************
int SendSms(unsigned char phone_num[],unsigned char msg[])
{
   unsigned char AtBuffer[30]={0x0},*GsmBuffer;
   strcpy(AtBuffer,"AT+CMGS=\"");
   strcat(AtBuffer,phone_num);
   strcat(AtBuffer,"\"\r");
   SendAtCommand(AtBuffer);
   msg[strlen(msg)]=0x1a;
   Delay_ml(5);
   SendAtCommand(msg);
   GetGsmData();
   GsmBuffer=GetGsmData();
   if (strstr(GsmBuffer,"OK")) return 1;
   else return 0;
}//SendSms
//***********************************************************
unsigned char *ReadSms (int index)
{
   unsigned char AtBuffer[20]={0x0},*GsmBuffer;
   SendAtCommand("AT+CSDH=1\r");
   sprintf(AtBuffer,"AT+CMGR=%d\r",index);
   SendAtCommand(AtBuffer);
   GsmBuffer=GetGsmData();
   if (strstr(GsmBuffer,"OK"))  return GsmBuffer;
   else return 0;
}//ReadSms
//***********************************************************//
int DeleteSms(int index)
{
     unsigned char AtBuffer[20]={0x0},*GsmBuffer;
     sprintf(AtBuffer,"AT+CMGD=%d\r",index);
     SendAtCommand(AtBuffer);
     GsmBuffer=GetGsmData();
     if (strstr(GsmBuffer,"321"))  return 2;
     if (strstr(GsmBuffer,"OK"))  return 1;
     else return 0;
}//DeleteSms
//***********************************************************//
int CheckModem(void)
{
     unsigned char AtBuffer[50]={0x0},*GsmBuffer;
     SendAtCommand("AT\r");
     GsmBuffer=GetGsmData();
     if (strstr(GsmBuffer,"OK"))
     {
        SendAtCommand("ATE0\r");
        GetGsmData();
        return 1;
     }
     else return 0;
}//CheckModem
//***********************************************************//
int SetRtc(unsigned char TD_Buffer[])
{
     unsigned char AtBuffer[32]="AT+CCLK=\"01/01/01,00:00:00+00\"\r",*GsmBuffer;
     SendAtCommand("ATE0\r");
     GetGsmData();
     AtBuffer[9]=TD_Buffer[0];AtBuffer[10]=TD_Buffer[1];AtBuffer[12]=TD_Buffer[2];
     AtBuffer[13]=TD_Buffer[3];AtBuffer[15]=TD_Buffer[4];AtBuffer[16]=TD_Buffer[5];
     AtBuffer[18]=TD_Buffer[6];AtBuffer[19]=TD_Buffer[7];AtBuffer[21]=TD_Buffer[8];
     AtBuffer[22]=TD_Buffer[9];AtBuffer[24]=TD_Buffer[10];AtBuffer[25]=TD_Buffer[11];
     AtBuffer[27]=TD_Buffer[12];AtBuffer[28]=TD_Buffer[13];
     SendAtCommand(AtBuffer);
     GsmBuffer=GetGsmData();
     if (strstr(GsmBuffer,"OK"))  return 1;
     else return 0;
}//SetRtc
//***********************************************************
unsigned char *GetRtc(void)
{
   unsigned char i,k=0,TD_Buffer[13]={0x0},*GsmBuffer;
   SendAtCommand("AT+CCLK?\r");
   GsmBuffer=GetGsmData();
   if (strstr(GsmBuffer,"OK"))
   {
      for (i=10;i<=26;i+=3)
      {
        TD_Buffer[k]=GsmBuffer[i];
        TD_Buffer[k+1]=GsmBuffer[i+1];
        k+=2;
      }
       return &TD_Buffer;
   }
   else return 0;
}//GetRtc
void PrintAsciiDataToUser(unsigned char p[])
 {
      int i;
      for(i=0;i<strlen(p);i++)
          UserSerialWr(p[i]);

 }//PrintAsciiDataToUser
//***********************************************************
//***********************************************************