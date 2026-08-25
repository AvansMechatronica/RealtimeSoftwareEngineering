/*
 * PositionController.c
 *
 * Created: 14-9-2023 11:42:11
 *  Author: rasmsmee
 */ 


///////////////////////////////////////////////////////////////////////////////
// system includes

#include <asf.h>
#include <string.h>
#include <math.h>

///////////////////////////////////////////////////////////////////////////////
// application includes

#include "CommandConsole.h"
#include "vPrintString.h"
#include "TaskSleep.h"

///////////////////////////////////////////////////////////////////////////////
// HAL includes for RTSW board

#include "DeviceIOLib.h"
#include "ADCLib.h"
#include "DAC4921Lib.h"
#include "SPILib.h"
#include "LEDLib.h"
#include "SwitchLib.h"
#include "PortIOLib.h"
#include "QC7366Lib.h"
#include "InterruptLib.h"
#include "I2CLib.h"
#include "GyroFXASLib.h"
#include "StatusLED.h"


///////////////////////////////////////////////////////////////////////////////
// function prototypes

void CalculateConstants(void);
void PositionController(void);

///////////////////////////////////////////////////////////////////////////////
// void PositionController(void)

const double rhoAlu	=	2700;		// dichtheid aluminium
const double rhoSUS	=	7800;		// dichtheid staal
const double Gsus	=	79.3e9;
const double g		=	9.81;		//acceleration gravity [m/s^2]
const double pi		=	3.1428;

//last
const double mL1	=	0.5;		// lastmassa [kg], motorkant veer
const double mL2	=	0.5;		// lastmassa, lastkant veer
const double k		=	3.0e3;		// veerstijfheid tussen mL1 en mL2 [N/m]
const double DL		=	0;          // visceuze wrijving [Ns/m], bijv. 1

//tandriem
const double R1		=	0.03;       //radius poelie1
const double R2		=	0.03;       //radius poelie2
double i1			=	0.0;		// i1=1/R1, overbrengingsverhouding rotatie naar translatie [m-1]
double i2			=	0.0;		// i2=R2, overbrengingsverhouding translatie naar rotatie poelie 2 [m]
const double kspec	=	2.8e5;		//specifiek stijfheid [N] -AT5 tandriem 16mm breed met ijzerdraad
const double Ltr	=	0.4;		//afstand tussen poelie 1 en 2 [m]
double Lac			=	0.0;		//Lac=sqrt(Ltr^2+(R2-R1)^2), lengte tandriem
double ktr			=	0.0;		// double ktr=kspec/Lac,lin. stijfheid tandriem
double Kfietr		=	0.0;		// Kfietr=2*ktr*R1^2, torsiestijfheid voorgespannen tandriem

//poelies
const double d1		=	0.02;       //dikte poelie [m]
//double mp1=pi*R1^2*d1*rhoAlu;
double mp1=0.0;
//double Jp1=(1/2)*mp1*R1^2;  //massatraagheid poelie 1 [kgm2]
double Jp1=0.0;  //massatraagheid poelie 1 [kgm2]
const double d2=0.02;
//double mp2=pi*R2^2*d2*rhoAlu;
double mp2=0.0;
//double Jp2=(1/2)*mp2*R2^2;  //massatraagheid poelie 2
double Jp2=0.0;  //massatraagheid poelie 2

//reductoras
const double Ras=0.006;
const double Las=0.02;
//double mas=pi*Ras^2*Las*rhoSUS;
double mas=0.0;
//double Jas=(1/2)*mas*Ras^2; //massatraagheid reductoras [kgm2]
double Jas=0.0; //massatraagheid reductoras [kgm2]
//double Ipas=(pi/4)*Ras^4;
double Ipas=0.0;
//double Kas=Gsus*Ipas/Las;   //torsiestijfheid reductoras [Nm/rad]
double Kas=0.0;   //torsiestijfheid reductoras [Nm/rad]
// ******** double Karray[1,3]=[Kfietr, Kas, k/(i1^2)];
double KArray[4] = {0, 0, 0, 0};
// ******** double K12=min(Karray);     //Geef dominante stijfheid naar K12 [Nm/rad], reductorzijde
double K12 = 0.0;
const double D12=0;               //Dominante (materiaal)demping (reductorzijde)

//motorzijde
const double Dmm	=	0.0;           //visceuze demping motorzijde
const double DLstar	=	0.0;        //visceuze demping last op reductoras getransformeerd
const double Jm		=	63e-7;        //massatraagheid motoras Faulhaber 3268024bx4 [kgm2]
const double Rm		=	1.47;         //weerstand motor [Ohm]
const double Lind	=	110e-6;     //inductie motor [Henry]
const double Kt		=	0.0435;       //motorconstante [Nm/A]
const double i0fh	=	6.6;        //overbrengingsverhouding Faulhaber-motor

//controller+versterker
const double Ka		=	0.2;          //spanning naar stroomversterker [A/V]
const double C1		=	1.0;            //Kpm*C1 is de regelaar, met C1=1 een P-regelaar
//double C2=Ka*Kt;        //regelaar: Kpm*C2 is servostijfheid
double C2			=	0.0;        //regelaar: Kpm*C2 is servostijfheid

//transformeren, inertia match
//double Jstar=Jas+Jp1+mL1/(i1^2)+mL2/(i1^2)+Jp2/(i2^2*i1^2); //op reductoras getranformeerde lastparameters
double Jstar=0.0; //op reductoras getranformeerde lastparameters
//double i0=sqrt(Jstar/Jm); //overbrengingsverhouding inertia match
double i0=0.; //overbrengingsverhouding inertia match
//Veronderstel k/(i1^2) is dominante stijfheid
//double Je1=Jm*i0fh^2+Jas+Jp1+mL1/(i1^2);   //massatrgh. op reductoras motorkant veer [kgm2]
double Je1=0.0;   //massatrgh. op reductoras motorkant veer [kgm2]
//double Je2=mL2/(i1^2)+Jp2/(i1^2*i2^2);     //massatrgh. op reductoras lastkant veer [kgm2]
double Je2=0.0;     //massatrgh. op reductoras lastkant veer [kgm2]
//double Je1m=Je1/(i0fh^2);
double Je1m=0.0;
//double Je2m=Je2/(i0fh^2);
double Je2m=0.0;
//double Je1L=Je1*i1^2;
double Je1L=0.0;
//double Je2L=Je2*i1^2;
double Je2L=0.0;
//double K12m=K12/(i0fh^2);
double K12m=0.0;
//double K12L=K12*i1^2;
double K12L=0.0;
//double D12m=D12/(i0fh^2);
double D12m=0.0;
//double D12L=D12*i1^2;
double D12L=0.0;
//double Dmred=Dmm*i0fh^2;
double Dmred=0.0;
//double DmL=Dmred*i1^2;
double DmL=0.0;
//double DLm=DL/(i0fh^2*i1^2);
double DLm=0.0;
//double DLred=DL/(i1^2);
double DLred=0.0;

//war en wr (op welke as gereduceerd dan ook)
//double war=sqrt(K12/Je2);       //antiresonantie freq. [rad/s]
double war=0.0;       //antiresonantie freq. [rad/s]
//double mu=(Je1*Je2)/(Je1+Je2);
double mu=0.0;
//double wr=sqrt(K12/mu);         //resonantie freq. [rad/s]
double wr=0.0;         //resonantie freq. [rad/s]

//
const double betaopt	=	0.1;    //derdegraads bewegingsprofiel
//double wbm=0.5*wr;     //bandwidth [rad/s]
double wbm	=	0.0;     //bandwidth [rad/s]

//regeling op motoras getransformeerd, sensor lastkant stijfheid
//double wbl=0.1*wr;
double wbl=0.0;


/*
//------------------------------------------------------------------------------------
//servostijfheid motor- en lastregeling beide zonder servodemping (dus helling Hol -2)
// Kpm*C2 is de servostijfheid zonder servodemping op motorkant
double Ksm[1,2]=[wbm^2*(Je1m+Je2m), wbl^2*(Je1m+Je2m)];
double Kpm[1,2]=[Ksm[1,1]/C2, Ksm[1,2]/C2];
//------------------------------------------------------------------------------------
//servostijfheid motor- en lastregeling beide met servodemping (dus helling Hol -1)
// Kpsd*C2 is de servostijfheid bij servodemping op motorkant
// Ktam*C2 is de servodemping motorkant
double Ksdm[1,2]=[wbm^2*(Je1m+Je2m), wbl^2*(Je1m+Je2m)];
double KsdL[1,2]=[wbm^2*(Je1L+Je2L), wbl^2*(Je1L+Je2L)];
double KpsdL[1,2]=[KsdL[1,1]/C2, KsdL[1,2]/C2];
double KpsdM[1,2]=[Ksdm[1,1]/C2, Ksdm[1,2]/C2];
double KvmL[1,2]=[2*betaopt*sqrt(KsdL[1,1]*(Je1L+Je2L))-D12L, 2*betaopt*sqrt(KsdL[1,2]*(Je1L+Je2L))-D12L];
double KvmM[1,2]=[2*betaopt*sqrt(Ksdm[1,1]*(Je1m+Je2m))-D12m, 2*betaopt*sqrt(Ksdm[1,2]*(Je1m+Je2m))-D12m];
double KtaL[1,2]=[KvmL[1,1]/C2, KvmL[1,2]/C2];
double KtaM[1,2]=[KvmM[1,1]/C2, KvmM[1,2]/C2];
////------------------------------------------------------------------------------------
double taud[1,2]=[3/wbm, 3/wbl];
double tauf[1,2]=[1/(3*wbm), 1/(3*wbl)];
double taui[1,2]=[10/wbm, 10/wbl];
*/

//------------------------------------------------------------------------------------
//servostijfheid motor- en lastregeling beide zonder servodemping (dus helling Hol -2)
// Kpm*C2 is de servostijfheid zonder servodemping op motorkant
double Ksm[3]	=	{0, 0, 0};
double Kpm[3]	=	{0, 0, 0};
	
double Ksdm[3]	=	{0, 0, 0};
double KsdL[3]	=	{0, 0, 0};
double KpsdL[3]	=	{0, 0, 0};
double KpsdM[3]	=	{0, 0, 0};
double KvmL[3]	=	{0, 0, 0};
double KvmM[3]	=	{0, 0, 0};
double KtaL[3]	=	{0, 0, 0};
double KtaM[3]	=	{0, 0, 0};

////------------------------------------------------------------------------------------
double taud[3]	=	{0, 0, 0};
double tauf[3]	=	{0, 0, 0};
double taui[3]	=	{0, 0, 0};

///////////////////////////////////////////////////////////////////////////////
// Sample and Hold


double Ts = 0.005;
double sample_time = 0.0;
double tstart = 0.1;    //start bewegingsprofiel


//H1red en H2red (reductoras), Kpm=1,
//State-space modellering en regeling op motoras, dus w1 van LF asymp. met i0 vermenigvuldigen
double wbm=0.0;
double taudm= 0 ;
double tauim = 0;
double taufm = 0;
// z-transformatie met Tustin rule: U(z)/E(z)=Kp*[p0+p1*z^(-1)+p2*z^(-2)]/[q0+q1*z^(-1)+q2*z^(-2)]
double p0=0;
double p1=0;
double p2=0;
double q0=0;
double q1=0;
double q2=0;

//bewegingsprofiel (derdegraads)
double itot=i0fh*i1;
double xmax=0.15;       //max verplaatsing [m]
double tmax=5*2*pi/(0.3*wr);        //max tijd [s] voor motorasregeling
double fiemax=xmax*itot;                      //max hoekverplaatsing motor [rad]
double jmax=fiemax*32/(tmax^3);               //max jerk
double t3[1,4]=[0, tmax/4, 3*tmax/4, tmax];    //array voor tijdswaarden verandering hoekversnelling
double alfa3[1,4]=[0, jmax*tmax/4, -jmax*tmax/4, 0];
double alfamax=jmax*tmax/4;
//
double t1=tmax/4;
double t2=3*tmax/4;

///////////////////////////////////////////////////////////////////////////////
// void CalculateConstants(void)

void CalculateSAH(void)
{
	sample_time = Ts;
	wbm=20*i0fh;
	
	taudm=1/(wbm/3);
	tauim=1/(wbm/10);
	taufm=1/(3*wbm);
	
	// z-transformatie met Tustin rule: U(z)/E(z)=Kp*[p0+p1*z^(-1)+p2*z^(-2)]/[q0+q1*z^(-1)+q2*z^(-2)]
	p0 = 1 + (2/Ts) * (taudm+tauim) + 4*taudm*tauim/pow(Ts, 2);
	
	p1 = 2*(1-4*taudm*tauim/pow(Ts,2));
	p2 = 1-(2/Ts)*(taudm+tauim)+4*taudm*tauim/pow(Ts, 2);
	q0 = (2*tauim/Ts)*(1+2*taufm/Ts);
	q1 = -8*taufm*tauim/pow(Ts, 2);
	q2 = (2*tauim/Ts)*(-1+2*taufm/Ts);
}

//variables
/*
discrete Real theta1;
discrete Real theta1_window[3];
discrete Real uth1_window[3];
discrete Real error_th1_window[3];
//gew. hoekposities voor motoras voor bewegingsprofiel 1
discrete Real thetag, omegag, alfag;  //gewenste hoekpositie, -snelheid en -versnelling
discrete Real ffdM;   //feedforward
*/

///////////////////////////////////////////////////////////////////////////////
// void CalculateConstants(void)

void CalculateConstants(void)
{
	i1=1/R1;
	i2=R2;
	
	Lac = sqrt( pow(Ltr, 2) + pow(R2-R1, 2) );
	ktr = kspec/Lac;
	Kfietr = 2 * ktr * pow(R1,2);
	
	mp1 = pow(R1, 2) * pi * d1 * rhoAlu;
	Jp1 = (1/2) * mp1 * pow(R1, 2);			//massatraagheid poelie 1 [kgm2]
	mp2 = pi * pow(R2, 2) * d2 * rhoAlu;
	Jp2 = (1/2) * mp2 * pow(R2, 2);			//massatraagheid poelie 2
	
	mas  = pi * pow(Ras, 2) * Las *rhoSUS;
	Ipas = (pi/4) * pow(Ras, 4);
	Kas  = (Gsus * Ipas) / Las;
	KArray[1] = Kfietr;
	KArray[2] = Kas;
	KArray[3] = k/pow(i1, 2);
	
	Jstar	= Jas + Jp1 + mL1/pow(i1, 2) + mL2/pow(i1, 2) + Jp2/(pow(i2, 2) + pow(i1, 2)); //op reductoras getranformeerde lastparameters
	i0		= sqrt(Jstar/Jm); //overbrengingsverhouding inertia match
	Je1		= Jm*pow(i0fh, 2) + Jas + Jp1 + mL1/pow(i1, 2);   //massatrgh. op reductoras motorkant veer [kgm2]
	Je2		= mL2/pow(i1, 2) + Jp2/(pow(i1, 2) * pow(i2, 2)) ;     //massatrgh. op reductoras lastkant veer [kgm2]
	Je1m	= Je1/pow(i0fh, 2);
	Je2m	= Je2/pow(i0fh, 2);
	Je1L	= Je1*pow(i1, 2);
	Je2L	= Je2*pow(i1, 2);
	//  K12=min(Karray);
	K12m	= K12/pow(i0fh, 2);
	K12L	= K12*pow(i1, 2);
	D12m	= D12/pow(i0fh, 2);
	Dmred	= Dmm*pow(i0fh, 2);
	DmL		= Dmred*pow(i1, 2);
	DLred	= DL/pow(i1, 2);
	
	//war en wr (op welke as gereduceerd dan ook)
	war		= sqrt(K12/Je2);       //antiresonantie freq. [rad/s]
	mu		= (Je1*Je2)/(Je1+Je2);
	wr		= sqrt(K12/mu);         //resonantie freq. [rad/s]
	
	//regeling op motoras getransformeerd, sensor lastkant stijfheid
	wbl = 0.1*wr;
	
	C2	=	Ka*Kt;        //regelaar: Kpm*C2 is servostijfheid
	
	Ksm[1] = pow(wbm, 2) * (Je1m+Je2m);
	Ksm[2] = pow(wbl, 2) * (Je1m+Je2m);

	Kpm[1] = Ksm[1]/C2;
	Kpm[2] = Ksm[2]/C2;
	
	Kpm[1] = Ksm[1]/C2;
	Kpm[2] = Ksm[2]/C2;

	//------------------------------------------------------------------------------------
	//servostijfheid motor- en lastregeling beide met servodemping (dus helling Hol -1)
	// Kpsd*C2 is de servostijfheid bij servodemping op motorkant
	// Ktam*C2 is de servodemping motorkant
	//Ksdm[1,2]=[wbm^2*(Je1m+Je2m), wbl^2*(Je1m+Je2m)];
	Ksdm[1]	=	pow(wbm, 2) * (Je1m+Je2m);
	Ksdm[2]	=	pow(wbl, 2) * (Je1m+Je2m);
	
	//KsdL[1,2]=[wbm^2*(Je1L+Je2L), wbl^2*(Je1L+Je2L)];
	KsdL[1]	=	pow(wbm, 2) * (Je1L+Je2L);
	KsdL[2]	=	pow(wbl, 2) * (Je1L+Je2L);

	//KpsdL[1,2]=[KsdL[1,1]/C2, KsdL[1,2]/C2];
	KpsdL[1] = KsdL[1]/C2;
	KpsdL[2] = KsdL[2]/C2;
	
	//KpsdM[1,2]=[Ksdm[1,1]/C2, Ksdm[1,2]/C2];
	KpsdM[2] = Ksdm[1]/C2;
	KpsdM[2] = Ksdm[2]/C2;
	
	//KvmL[1,2]=[2*betaopt*sqrt(KsdL[1,1]*(Je1L+Je2L))-D12L, 2*betaopt*sqrt(KsdL[1,2]*(Je1L+Je2L))-D12L];
	KvmL[1]	= 2 * betaopt * sqrt(KsdL[1]*(Je1L+Je2L)) - D12L;
	KvmL[2]	= 2 * betaopt * sqrt(KsdL[2]*(Je1L+Je2L)) - D12L;

	//KvmM[1,2]=[2*betaopt*sqrt(Ksdm[1,1]*(Je1m+Je2m))-D12m, 2*betaopt*sqrt(Ksdm[1,2]*(Je1m+Je2m))-D12m];
	KvmM[1]	= 2 * betaopt * sqrt(Ksdm[1]*(Je1m+Je2m)) - D12m;
	KvmM[2]	= 2 * betaopt * sqrt(Ksdm[2]*(Je1m+Je2m)) - D12m;

	//KtaL[1,2]=[KvmL[1,1]/C2, KvmL[1,2]/C2];
	KtaL[1]	= KvmL[1]/C2;
	KtaL[2] = KvmL[2]/C2;

	//KtaM[1,2]=[KvmM[1,1]/C2, KvmM[1,2]/C2];
	KtaM[1]	= KvmM[1]/C2;
	KtaM[2]	= KvmM[2]/C2;
	
	//taud[1,2]=[3/wbm, 3/wbl];
	taud[1]	= 3/wbm;
	taud[2] = 3/wbl;
	
	//tauf[1,2]=[1/(3*wbm), 1/(3*wbl)];
	tauf[1]	= 1/(3*wbm);
	tauf[2]	= 1/(3*wbl);
	
	//taui[1,2]=[10/wbm, 10/wbl];
	taui[1]	= 10/wbm;
	taui[2]	= 10/wbl;
}


void PositionController(void)
{
	CalculateConstants();
	
}
