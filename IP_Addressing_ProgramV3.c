/************************************************
Program name: Subnetting/CISR Addressing Program
Feature: To compute and display the IP table based on subnetting/CIDR principles
         given a network address with prefix and number of required hosts or subnets.
Author: Wayne Dayata
Date: March 18, 2021

NOTE: For VLSM Computations, please use the OTHER program, NOT this one!
************************************************/

#include<stdio.h>
#include<conio.h>
#include<math.h>

//variable declarations
char address[20]; //network address and prefix in string (192.168.1.0/24)
int ipdec[4]; //octets parsed to int {192,168,1,0}
int initip[4]; //initial network address
int ipbin[32]; //ip stored in binary form {11000000 10101000 00000001 00000000}
int prefix=0; //prefix (24)
int mode=0; //mode of segmentation (1 or 2)
int req; //required number of subnets/hosts (1-2^(32-prefix)-2)
int newprefix; //new prefix length after computation
int numbits=0; //bits to borrow as subnets
int maxsubnets; //max possible subnets
int range; //IP address range
int ctr, x, temp, ndx;
int isvalid=0; //check for input validity

char ch;
int isexit=0;

//function prototypes
void fixoverflow(int a[]);

void main(){
while(isexit==0){
	//A. Introduction and user input
	printf("Note: For VLSM mode of addressing, please use the other program to avoid errors.\n\n");
	printf("Program by: Wayne Matthew Dayata, version 1.5 (Mar 18, 2021)\n");
	printf("This program will compute and display the subnets of a certain network address.\n");
	printf("To proceed, enter below the given network address and prefix length (CIDR is supported here)\n");
	printf("Example: 192.168.32.0/20 or 192.168.32.0\n\n");
	printf("Network address: ");
	isvalid=0;
	while(isvalid==0){
		gets(address);
		//syntax checking
		ctr=0;
		for(ndx=0;address[ndx]!=0;ndx++){
			if(address[ndx]=='.') ctr++;
			if(address[ndx]=='/') ctr+=10;		
		}
		if(!(ctr==3||ctr==13)){
			printf("\nSyntax error. Please enter another network address.\n");
			printf("Network address: ");
		} else { 
			//logic checking 1: Parse address to int and assigning octet values
			x=0; prefix=0;
			ipdec[0]=0; initip[0]=0;//reset octet values
			ipdec[1]=0; initip[1]=0;
			ipdec[2]=0; initip[2]=0;
			ipdec[3]=0; initip[3]=0;
			for(ndx=0;address[ndx]!=0;ndx++){
				if(address[ndx]=='.') x++; //increase octet count when a dot is scanned
				if(address[ndx]=='/') x++; //change input transfer to prefix when a slash is scanned
				if(x<4){
					if(address[ndx]>='0' && address[ndx]<='9'){
						ipdec[x]=ipdec[x]*10+(address[ndx]-'0');
					}
				}
				if(x==4){
					if(address[ndx]>='0' && address[ndx]<='9'){
						prefix=prefix*10+(address[ndx]-'0');
					}
				}
			}
			//logic checking 2: Check if octet entries are valid (<=255), and if prefix is valid (<=30)
			if(ipdec[0]>223 || ipdec[0]==127 || ipdec[1]>255 || ipdec[2]>255 || ipdec[3]>255 || prefix>30 || (ipdec[0]==0 && ipdec[1]==0 && ipdec[2]==0 && ipdec[3]==0)){
				printf("\nInvalid network address format entered. Please enter another network address.\n");
				if(prefix>30) printf("Note: Addresses with prefix greater than /30 cannot yield usable hosts.\n");
				printf("Network address: ");
			} else {
				//logic checking 3: Checking if address entered is valid network address by looking if there are any 1s in host bits
				for(ndx=0;ndx<4;ndx++) initip[ndx]=ipdec[ndx];				
				if(ctr!=13){ //adjust prefix
					if(ipdec[0]<=126) prefix=8;
					else if(ipdec[0]<=191) prefix=16;
					else prefix=24;
				}
				//3.1 Parse decimal octets to bits
				for(ndx=0;ndx<32;ndx++) ipbin[ndx]=0; //initialize bits
				temp=ipdec[0]; x=128; //octet 1
				for(ndx=0;ndx<=7 && temp>0;ndx++){
					if(temp-x<0){
						ipbin[ndx]=0;
					} else {
						ipbin[ndx]=1;
						temp-=x;
					}
					x/=2;
				}
				temp=ipdec[1]; x=128; //octet 2
				for(ndx=8;ndx<=15 && temp>0;ndx++){
					if(temp-x<0){
						ipbin[ndx]=0;
					} else {
						ipbin[ndx]=1;
						temp-=x;
					}
					x/=2;
				}
				temp=ipdec[2]; x=128; //octet 3
				for(ndx=16;ndx<=23 && temp>0;ndx++){
					if(temp-x<0){
						ipbin[ndx]=0;
					} else {
						ipbin[ndx]=1;
						temp-=x;
					}
					x/=2;
				}
				temp=ipdec[3]; x=128; //octet 4
				for(ndx=24;ndx<=31 && temp>0;ndx++){
					if(temp-x<0){
						ipbin[ndx]=0;
					} else {
						ipbin[ndx]=1;
						temp-=x;
					}
					x/=2;
				}
				//3.2 check if there are 1 bits beyond prefix
				for(ndx=prefix;ndx<32&&ipbin[ndx]!=1;ndx++){}
				if(ndx<32){
					printf("\nAddress entered is an IP address. Please enter another network address.\n");
					printf("Network address: ");
				} else {
					isvalid=1; //proceed to next mode
				}
			}
		}
	}
	mode=0;
	isvalid=0;
	if(prefix>=29){
		mode=1;
		isvalid=1;
		numbits=0;
		newprefix=prefix;
		printf("\nNote: Addresses with prefix /29 and above cannot be further subnetted.\n");
	} else {
		printf("\nEnter mode of segmentation: 1 for subnet-based, 2 for host efficiency-based,");
		printf("\n                            3 for no segmentation (single-network addressing)\n");
		printf("Input mode: ");
	}
	while(mode==0){ //process here repeats until valid mode input is made
		scanf("%d",&mode);
		switch(mode){
			case 1: printf("\nEnter minimum subnets needed: "); break;
			case 2: printf("\nEnter maximum hosts per subnet needed: "); break;
			case 3: {mode=1; isvalid=1; numbits=0; newprefix=prefix; break;}
			default: { mode=0; printf("\nInvalid input. Please try again.\n"); printf("Input mode: "); break; }
		}
	}
	if(prefix==8||prefix==16||prefix==24) x=2; else x=1;
	while(isvalid==0){
		numbits=0; //initialize bits needed to borrow
		scanf("%d",&req);
		if(mode==1){
			if(req>=2) for(numbits=0;(int)pow(2,numbits)-x<req;numbits++) {}
			if(30-prefix>=numbits){
				newprefix=prefix+numbits;
				isvalid=1;
			}
		}
		if(mode==2){
			for(numbits=0;(int)pow(2,numbits)-2<req;numbits++) {}
			if(30-prefix>=numbits){
				newprefix=32-numbits;
				numbits=newprefix-prefix;
				isvalid=1;
			} else if (32-prefix>=numbits) {
				numbits=0;
				newprefix=prefix;
				isvalid=1;
			}
		}
		if(isvalid==0){
			printf("\nError: insufficient bits available to borrow.\n");
			if(mode==1) printf("With /%d network, you can only allocate up to %d subnets.\n",prefix,(int)pow(2,30-prefix)-x);
			if(mode==2) printf("With /%d network, you can only allocate up to %d hosts per subnet, or up to %d hosts for the entire network.\n",prefix,(int)pow(2,30-prefix)-2,(int)pow(2,32-prefix)-2);
			
			printf("Please enter another value: ");
		}
	}
	
	//B. Display basic details
	printf("\n=============================================================================================================");
	printf("\nNew subnet mask: ");
	switch(newprefix){
		case 8: printf("255.0.0.0"); break;
		case 9: printf("255.128.0.0"); break;
		case 10: printf("255.192.0.0"); break;
		case 11: printf("255.224.0.0"); break;
		case 12: printf("255.240.0.0"); break;
		case 13: printf("255.248.0.0"); break;
		case 14: printf("255.252.0.0"); break;
		case 15: printf("255.254.0.0"); break;
		case 16: printf("255.255.0.0"); break;
		case 17: printf("255.255.128.0"); break;
		case 18: printf("255.255.192.0"); break;
		case 19: printf("255.255.224.0"); break;
		case 20: printf("255.255.240.0"); break;
		case 21: printf("255.255.248.0"); break;
		case 22: printf("255.255.252.0"); break;
		case 23: printf("255.255.254.0"); break;
		case 24: printf("255.255.255.0"); break;
		case 25: printf("255.255.255.128"); break;
		case 26: printf("255.255.255.192"); break;
		case 27: printf("255.255.255.224"); break;
		case 28: printf("255.255.255.240"); break;
		case 29: printf("255.255.255.248"); break;
		case 30: printf("255.255.255.252"); break;
	}
	
	maxsubnets=(int)pow(2,numbits);
	range=(int)pow(2,32-prefix-numbits);
	
	if(numbits>0) {
		if(prefix==8 || prefix==16 || prefix==24){ //classful addressing, first and last subnets unusable
			printf("\nNumber of usable subnets: %d",maxsubnets-2);
		} else { //classless addressing, last subnet unusable
			printf("\nNumber of usable subnets: %d",maxsubnets-1);
		}	
		printf("\nNumber of usable hosts per subnet: %d",range-2);
	} else {
		printf("\nNumber of usable hosts: %d",range-2);
	}
	
	//C. Display addressing table
	printf("\n\n#       Subnets                Start IP            End IP              Broadcast Address    Usable(Y/N)");
	printf("\n=============================================================================================================\n");
	for(x=0;x<40 && x<maxsubnets;x++){
		printf("%-7d %3d.%3d.%3d.%3d/%d     ",x,ipdec[0],ipdec[1],ipdec[2],ipdec[3],newprefix);
		ipdec[3]++;
		printf("%3d.%3d.%3d.%3d     ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		ipdec[3]+=(range-3);
		fixoverflow(ipdec);
		printf("%3d.%3d.%3d.%3d     ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		ipdec[3]++;
		printf("%3d.%3d.%3d.%3d      ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		if(x==0 && maxsubnets>1 && (prefix==8 || prefix==16 || prefix==24)){
			printf("No\n");
		} else {
			if(x==maxsubnets-1 && numbits!=0){
				printf("No\n");
			} else{
				printf("Yes\n");
			}
		}
		ipdec[3]++;
		fixoverflow(ipdec);
	}
	if(x<maxsubnets){
		printf("...\n");
		initip[3]+=((maxsubnets-1)*range);
		fixoverflow(initip);
		printf("%-7d %3d.%3d.%3d.%3d/%d     ",maxsubnets-1,initip[0],initip[1],initip[2],initip[3],newprefix);
		initip[3]++;
		printf("%3d.%3d.%3d.%3d     ",initip[0],initip[1],initip[2],initip[3]);
		initip[3]+=(range-3);
		fixoverflow(initip);
		printf("%3d.%3d.%3d.%3d     ",initip[0],initip[1],initip[2],initip[3]);
		initip[3]++;
		printf("%3d.%3d.%3d.%3d      ",initip[0],initip[1],initip[2],initip[3]);
		printf("No\n");
	}
	
	//D. Restart or exit program
	printf("\n\nPress ENTER to input another network address");
	printf("\nPress any key to exit program");
	ch = getch();
	if(ch==13) {fflush(stdin); system("@cls||clear"); } else isexit=1;
}
}

void fixoverflow(int a[]){
	if(a[3]>255){
		a[2]+=(a[3]/256);
		a[3]%=256;
	}
	if(a[2]>255){
		a[1]+=(a[2]/256);
		a[2]%=256;
	}
	if(a[1]>255){
		a[0]+=(a[1]/256);
		a[1]%=256;
	}
}
