/************************************************
Program name: VLSM Addressing Program
Feature: This program is a continuation of the previous subnetting/CIDR program
         where host-based subnets will be more accurately allocated.
Author: Wayne Dayata
Date: April 18, 2021

NOTE: For NON-VLSM Computations, please use the OTHER program, NOT this one!
************************************************/

#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<math.h>

//variable declarations
char address[20]; //network address and prefix in string (192.168.1.0/24)
int ipdec[4]; //octets parsed to int {192,168,1,0}
int initip[4]; //initial network address
int ipbin[32]; //ip stored in binary form {11000000 10101000 00000001 00000000}
int prefix=0; //prefix (24)
int req; //required number of subnets/hosts (1-2^(32-prefix)-2)
int newprefix; //new prefix length after computation
int range; //IP address range
int ctr, x, temp, ndx;
int isvalid=0; //check for input validity

int* hosts; //VLSM: array of hosts in all the subnet (dynamic allocation)
int rangectr; //VLSM: track the amount of space allocated so far by using the range (ex. +128 for /25, +4 for /30)
int maxrange; //VLSM: the maximum possible space to allocate (ex. 256 for /24)
int status=0; //VLSM: checks if all subnets are all allocated within the given network space or not
              //1 - successful, 0 - failed

char ch;
int isexit=0;

//function prototypes
void fixoverflow(int a[]);

void main(){
	
while(isexit==0){
	//A. Introduction and network input
	printf("Note: For VLSM ONLY! For subnetting/CIDR, please use the other program to avoid errors.\n\n");
	printf("Program by: Wayne Matthew Dayata, version 1.4 (Apr 18, 2021)\n");
	printf("This program will compute and display the addressing table for a given network via VLSM.\n");
	printf("To proceed, enter below the given network address and prefix length\n");
	printf("Example: 192.168.64.0/20 or 192.168.64.0\n\n");
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
			ipdec[0]=0;
			ipdec[1]=0;
			ipdec[2]=0;
			ipdec[3]=0;
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
			if(ipdec[0]>223 || ipdec[0]==127 || ipdec[1]>255 || ipdec[2]>255 || ipdec[3]>255 || prefix>=30 || (ipdec[0]==0 && ipdec[1]==0 && ipdec[2]==0 && ipdec[3]==0)){
				printf("\nInvalid network address format entered. Please enter another network address.\n");
				printf("Network address: ");
			} else {
				//logic checking 3: Checking if address entered is valid network address by looking if there are any 1s in host bits
		
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
	
	isvalid=0;
	
	//B. Inputting and checking subnet requirement if below the maximum possible
	
	printf("\nEnter number of subnets needed to create: ");

	while(isvalid==0){
		scanf("%d",&req);
		if (req>(int)pow(2,30-prefix)){
			printf("\nError: insufficient bits available to borrow.");
			printf("\nWith /%d network, you can only allocate up to %d subnets.",prefix,(int)pow(2,30-prefix));
			printf("\nPlease enter another value: ");
		} else {
			isvalid=1; //proceed to next mode
		}
	}
	
	isvalid=0;
	
	//C. Inputting host count for every subnet and checking if successfully allocated or not (via total range)
	
	hosts = (int*)calloc(req,sizeof(int));
	maxrange = (int)pow(2,32-prefix);
	
	printf("\nEnter host counts for every subnet; separate inputs with a space: \n");
	
	while(isvalid==0){
		rangectr=0;
		for(ndx=0;ndx<req;ndx++){
			scanf("%d",&hosts[ndx]);
			//determine the range/block needed
			for(temp=2;(int)pow(2,temp)-2<hosts[ndx];temp++) {}
			rangectr+=(int)pow(2,temp);
		}
		if(rangectr>maxrange){
			printf("\nError: Insufficient space available to allocate indicated hosts.");
			printf("\nYou have consumed a total block size of %d which is higher than the maximum allowed (%d for a /%d network).",rangectr,maxrange,prefix);
			printf("\nEnter the host counts again: \n");
		} else {
			isvalid=1;
		}
	}
	
	isvalid=0;
	//D. Sorting the array for efficient subnet allocation
	for(ndx=0;ndx<req-1;ndx++){
		for(x=0;x<req-ndx-1;x++){
			if (hosts[x] < hosts[x+1]){
				temp = hosts[x];
				hosts[x] = hosts[x+1];
				hosts[x+1] = temp;
			}
		}
	}

	//E. Display basic details
	printf("\n==================================================================================================================");
	printf("\nSuccessfully created %d subnets. You have allocated %d out of %d available space. See results below:", req, rangectr, maxrange);
	
	//F. Display addressing table
	printf("\n\n#       Subnets                 Start IP            End IP              Broadcast Address    Used/Allocated");
	printf("\n==================================================================================================================\n");
	for(x=0;x<req;x++){
		//determine range for the current subnet
		for(temp=2;(int)pow(2,temp)-2<hosts[x];temp++) {}
		range = (int)pow(2,temp);
		newprefix = 32-temp;
		
		printf("%-7d %3d.%3d.%3d.%3d /%d     ",x,ipdec[0],ipdec[1],ipdec[2],ipdec[3],newprefix);
		ipdec[3]++;
		printf("%3d.%3d.%3d.%3d     ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		ipdec[3]+=(range-3);
		fixoverflow(ipdec);
		printf("%3d.%3d.%3d.%3d     ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		ipdec[3]++;
		printf("%3d.%3d.%3d.%3d      ",ipdec[0],ipdec[1],ipdec[2],ipdec[3]);
		ipdec[3]++;
		fixoverflow(ipdec);
		printf("%6d/%6d\n", hosts[x],range-2);
	}
	
	//D. Restart or exit program
	free(hosts);
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

