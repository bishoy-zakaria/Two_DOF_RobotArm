/* 2dof planer robotic arm shooting targets project 
 *  Mechatronics Department
 *  faculty of Engineering Assuit University
 */

 /* ****************** Standard types ******************* */
typedef unsigned char  uint8;
typedef signed char    sint8;
typedef unsigned short uint16;
typedef signed short   sint16;
typedef unsigned long  uint32;
typedef signed long    sint32;
typedef float          float32;
typedef double         float64;
/* ****************** Bit Math ***************************** */
#define SET_BIT(REG,BIT)  (REG |= (1<<BIT))
#define CLR_BIT(REG,BIT)  (REG &=~ (1<<BIT))
#define Toggle_BIT(REG,BIT) (REG ^= (1<<BIT))
#define GET_BIT(REG,BIT)  ((REG >> BIT) & (0X01))
/* ***************** Timer1 regesters ********************* */
#define control_reg        (*(volatile uint8*)(0x80))
#define Clock_regs         (*(volatile uint8*)(0x81))
#define force_output_regs  (*(volatile uint8*)(0x82)) 
#define Over_Flow_regs     (*(volatile uint16*)(0x84)) // 16 bit rigster over flow regester
#define PWM_regs_1         (*(volatile uint16*)(0x88)) // 16 bit channel one combare match regester
#define PWM_regs_2         (*(volatile uint16*)(0x8A)) // 16 bit channel two  combare match regester
#define TOP_regs           (*(volatile uint16*)(0x86)) // 16 bit TOP regester
/* *************** Timer Prescallers ************************** */
#define Pres_CLK                   1
#define Pres_CLK_8                 8
#define Pres_CLK_64                64
#define Pres_CLK_256               256
#define Pres_CLK_1024              1024
/* ******************** PWM channels *************************** */
#define PWM_Channel_1              1
#define PWM_Channel_2              2
/* ***************** Timer Configurations ***************** */
#define Freq                 16    // MEGA HERTEZ 
#define Timer_CLK            Pres_CLK
#define Dead_Band_Width      8     // Micro second
/* ************************** API ************************** */
void PWM_init(void);
void timer1_start(void);
void Servo_FS90MG_Move(float64 angel , uint8 channel);
/* ************************ Application ********************* */
uint8 Servo_Motor_1 = 9;
uint8 Servo_Motor_2 = 10;
uint8 Laser_Pointer = 8;
uint32 Baud_Rate    = 9600 ;
/* ********** Global variables ********** */
#define Object_Num 3
uint8 Data[Object_Num][2]={
                           {85,102},
                           {108,114},
                           {71,77}
                       };
uint8 Data_Buffer=0;
void setup() {
  PWM_init();
  pinMode(Servo_Motor_1, OUTPUT);
  pinMode(Servo_Motor_2, OUTPUT);
  pinMode(Laser_Pointer, OUTPUT);
  timer1_start(); 
}
//Servo_FS90MG_Move(0 , PWM_Channel_1);
    //Servo_FS90MG_Move(0 , PWM_Channel_2);
void loop() 
{ 
   for(int i=0;i<Object_Num;i++)
   {
    Servo_FS90MG_Move(Data[i][0] , PWM_Channel_1);
    delay(1000);
    Servo_FS90MG_Move(Data[i][1] , PWM_Channel_2);
    for(int a=0;a<3;a++)
    {
      digitalWrite(Laser_Pointer,HIGH);
      delay(500);
      digitalWrite(Laser_Pointer,LOW);
    }
    digitalWrite(Laser_Pointer,HIGH);
    delay(5000);
    digitalWrite(Laser_Pointer,LOW);
    delay(1000);
   }
   Servo_FS90MG_Move(0 , PWM_Channel_1);
    Servo_FS90MG_Move(0 , PWM_Channel_2);
   delay(5000);
}

/* *********************** Implimentation ******************** */
void PWM_init(void)
{
  SET_BIT(control_reg,4); // Inverting Fast PWM for Channel 2
  SET_BIT(control_reg,5);
  SET_BIT(control_reg,6); // Inverting Fast PWM for Channel 1
  SET_BIT(control_reg,7);
  CLR_BIT(control_reg,0); // Phase correct PWM ICR1 TOP (Timer1 mode)
  SET_BIT(control_reg,1);
  CLR_BIT(Clock_regs,3);
  SET_BIT(Clock_regs,4);
  CLR_BIT(force_output_regs,6); // DISable force output combare
  CLR_BIT(force_output_regs,7);
}

void timer1_start(void)
{
  #if  Timer_CLK     ==       Pres_CLK   
  {
    SET_BIT(Clock_regs,0);
    CLR_BIT(Clock_regs,1);
    CLR_BIT(Clock_regs,2);
  } 
  #elif  Timer_CLK     ==       Pres_CLK_8 
  {
    CLR_BIT(Clock_regs,0);
    SET_BIT(Clock_regs,1);
    CLR_BIT(Clock_regs,2);
  }
  #elif  Timer_CLK     ==       Pres_CLK_64
  {
    SET_BIT(Clock_regs,0);
    SET_BIT(Clock_regs,1);
    CLR_BIT(Clock_regs,2);
  }
 #elif  Timer_CLK     ==       Pres_CLK_256
 {
   CLR_BIT(Clock_regs,0);
   CLR_BIT(Clock_regs,1);
   SET_BIT(Clock_regs,2);
 } 
 #elif  Timer_CLK     ==      Pres_CLK_1024
 {
   SET_BIT(Clock_regs,0);
   CLR_BIT(Clock_regs,1);
   SET_BIT(Clock_regs,2);
 }
 #endif

}

void Servo_FS90MG_Move(float64 angel , uint8 channel)
{
  float32 TOP=0;
  float32 x=400;
  float64 Duty=0;
  TOP =(16000000) / (Timer_CLK * (x*2)/* Frequency*/ );
  TOP_regs = TOP;
  if(angel<=90)
   Duty = ((angel*73/180)+22);
  else
   Duty = (((angel-5)*25/84)+(1059/28));
   if(channel == PWM_Channel_1 )
    PWM_regs_1 = TOP-(Duty* TOP /100);
   else
   {
    Duty=(4*angel/9)+22;
    PWM_regs_2 = TOP-(Duty* TOP /100);
}}
