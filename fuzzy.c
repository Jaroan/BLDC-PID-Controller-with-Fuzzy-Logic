#include<stdio.h>
#include<stdlib.h>
#include<math.h>
volatile int Fuzzy_Matrix[3][3];
volatile int NE,NDE,PE,PDE,ZE,ZDE;
volatile float Kp,Ki,out,err_sum,error,d_err,prev_err;
volatile int Kp_small,Kp_medium,Kp_large;
volatile int Ki_small,Ki_medium,Ki_large;
const float Kp_s=0.2,Kp_m=0.5,Kp_l=0.8;
const float Ki_s=0.003,Ki_m=0.0013,Ki_l=0.0023;
int min(int x,int y){
	return x<y?x:y;
}
int max(int x,int y){
	return x>y?x:y;
}
void init(){
  int i,j;
  NE=0; PDE=0; NDE=0; PE=0; ZE=0; ZDE=0;
  Kp=0; Ki=0;
  error=0;
  prev_err=0;
  err_sum=0;
  for(i=0;i<3;i++){
    for(j=0;j<3;j++) Fuzzy_Matrix[i][j]=0;
  }
}

//fuzzification
void Fuzzification(){
  NE=0; PDE=0; NDE=0; PE=0;
  if(error<=-20) NE=20;
  else if(error>=20) PE=20;
  else{
    if(error>=0){
      PE=error;
      NDE=20-error;
    }
    else{
      NE=-error;
      ZDE=20+error;
    }
  }
  if(d_err<=-20) NDE=20;
  else if(d_err>=20) PDE=20;
  else{
    if(d_err>=0){
      PDE=d_err;
      ZDE=20-d_err;
    }
    else{
      NDE=-d_err;
      ZDE=20+d_err;
    }
  }
}

//creation of the fuzzy matrix
void Create_Fuzzy_Matrix(){
  Fuzzy_Matrix[0][0]=min(NE,NDE);
  Fuzzy_Matrix[0][1]=min(NE,ZDE);
  Fuzzy_Matrix[0][2]=min(NE,PDE);
  Fuzzy_Matrix[1][0]=min(ZE,NDE);
  Fuzzy_Matrix[1][1]=min(ZE,ZDE);
  Fuzzy_Matrix[1][2]=min(ZE,PDE);
  Fuzzy_Matrix[2][0]=min(PE,NDE);
  Fuzzy_Matrix[2][1]=min(PE,ZDE);
  Fuzzy_Matrix[2][2]=min(PE,PDE);
}

//This part h=gives us the physical values for the coefficients
void Defuzzification(){
  Kp_large=max(Fuzzy_Matrix[0][0],Fuzzy_Matrix[0][1]);
  Kp_small=max(Fuzzy_Matrix[2][1],Fuzzy_Matrix[2][2]);
  Kp_medium=max(Fuzzy_Matrix[0][1],max(Fuzzy_Matrix[1][0],max(Fuzzy_Matrix[1][1],max(Fuzzy_Matrix[1][2],Fuzzy_Matrix[2][1]))));
  
  Ki_large=max(Fuzzy_Matrix[0][0],Fuzzy_Matrix[0][1]);
  Ki_small=max(Fuzzy_Matrix[2][1],Fuzzy_Matrix[2][2]);
  Ki_medium=max(Fuzzy_Matrix[0][1],max(Fuzzy_Matrix[1][0],max(Fuzzy_Matrix[1][1],max(Fuzzy_Matrix[1][2],Fuzzy_Matrix[2][1]))));

  if(!(Kp_small==0 && Kp_medium==0 && Kp_large==0))  Kp=(Kp_s*Kp_small+Kp_m*Kp_medium+Kp_l*Kp_large)/(Kp_small+Kp_medium+Kp_large);
  if(!(Ki_small==0 && Ki_medium==0 && Ki_large==0)) Ki=(Ki_s*Ki_small+Ki_m*Ki_medium+Ki_l*Ki_large)/(Ki_small+Ki_medium+Ki_large);
  
}

float Compute_PWM(){
  err_sum+=error;
  out=Kp*error + Ki*err_sum;
  prev_err=error;
  if(out>255) out=255;
  else if(out<0) out=0;
  return out;
}

int main(){
	int e,i;
	init();
	for(e=1200;e>=-100;e-=10){
		error=e;
		d_err=error-prev_err;
		Fuzzification();
		Create_Fuzzy_Matrix();
		Defuzzification();
		out=Compute_PWM();
		printf("error=%f, Kp=%f, Ki=%f, PWM=%f\n",error,Kp,Ki,out);
	}
	for(i=1;i<=50;i++){
		error=-100*exp(-i)*cos(i);
		d_err=error-prev_err;
		Fuzzification();
		Create_Fuzzy_Matrix();
		Defuzzification();
		out=Compute_PWM();
		printf("error=%f, Kp=%f, Ki=%f, PWM=%f\n",error,Kp,Ki,out);
	}
	return 0;
}
