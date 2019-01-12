#include<stdio.h>
#include<stdlib.h>
volatile int Fuzzy_Matrix[3][3];
volatile int NE,NDE,PE,PDE,ZE,ZDE,err_sum,error,d_err,Kp,Ki,Kd,prev_err;
//volatile float
volatile int PWM_Fuzzy,ticks;
//volatile int target=150;
volatile int Kp_small,Kp_medium,Kp_large;
volatile int Ki_small,Ki_medium,Ki_large;
volatile int Kd_small,Kd_medium,Kd_large;
volatile int out;
const int Kp_s=2,Kp_m=4,Kp_l=6;
const int Ki_s=1,Ki_m=2,Ki_l=3;
const int Kd_s=1,Kd_m=2,Kd_l=3;

void init(){
	int i,j;
	NE=0; PDE=0; NDE=0; PE=0; ZE=0; ZDE=0;
	Kp=0; Ki=0; Kd=0;
	error=0;
	prev_err=0;
	PWM_Fuzzy=0;
	err_sum=0;
	for(i=0;i<3;i++){
		for(j=0;j<3;j++) Fuzzy_Matrix[i][j]=0;
	}
}
int max(int x,int y){
	return x>y?x:y;
}
int min(int x,int y){
	return x>y?y:x;
}
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
void Defuzzification(){
	Kp_small=max(Fuzzy_Matrix[0][0],Fuzzy_Matrix[0][1]);
	Kp_large=max(Fuzzy_Matrix[2][1],Fuzzy_Matrix[2][2]);
	Kp_medium=max(Fuzzy_Matrix[0][1],max(Fuzzy_Matrix[1][0],max(Fuzzy_Matrix[1][1],max(Fuzzy_Matrix[1][2],Fuzzy_Matrix[2][1]))));

	Ki_small=max(Fuzzy_Matrix[0][0],Fuzzy_Matrix[0][1]);
	Ki_large=max(Fuzzy_Matrix[2][1],Fuzzy_Matrix[2][2]);
	Ki_medium=max(Fuzzy_Matrix[0][1],max(Fuzzy_Matrix[1][0],max(Fuzzy_Matrix[1][1],max(Fuzzy_Matrix[1][2],Fuzzy_Matrix[2][1]))));

	Kd_large=max(Fuzzy_Matrix[0][0],Fuzzy_Matrix[0][1]);
	Kd_small=max(Fuzzy_Matrix[2][1],Fuzzy_Matrix[2][2]);
	Kd_medium=max(Fuzzy_Matrix[0][1],max(Fuzzy_Matrix[1][0],max(Fuzzy_Matrix[1][1],max(Fuzzy_Matrix[1][2],Fuzzy_Matrix[2][1]))));


	if(Kp_small!=0 || Kp_medium!=0 || Kp_large!=0) Kp=(Kp_s*Kp_small+Kp_m*Kp_medium+Kp_l*Kp_large)/(Kp_small+Kp_medium+Kp_large);
	if(Ki_small!=0 || Ki_medium!=0 || Ki_large!=0) Ki=(Ki_s*Ki_small+Ki_m*Ki_medium+Ki_l*Ki_large)/(Ki_small+Ki_medium+Ki_large);
	if(Kd_small!=0 || Kd_medium!=0 || Kd_large!=0) Kd=(Kd_s*Kd_small+Kd_m*Kd_medium+Kd_l*Kd_large)/(Kd_small+Kd_medium+Kd_large);
}
int Compute_PWM(){
	err_sum+=error;
	out=Kp*error + Ki*err_sum + Kd*d_err;
	if(out>127) out=127;
	else if(out<0) out=0;
	prev_err=error;
	return out;
}
int main(){
    init();
    int i,e;
    for(i=200;i>=-10;i-=10){
    error=i;
    d_err=error-prev_err;
    Fuzzification();
    Create_Fuzzy_Matrix();
    Defuzzification();
    out=Compute_PWM();
    printf("error=%d,Kp=%d,Ki=%d,Kd=%d,PWM=%d\n",i,Kp,Ki,Kd,out);
    }
    return 0;
    }
