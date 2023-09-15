#include "stdlib.h"
#include "mm/mm.h"

char* itoa(int num){
	    char* num_a = (char*) malloc(15); // assuming max 15 digit number.

	    int digits = 0;
	    int temp = num;
	    while(temp > 0){
	        digits++;
	        temp/=10;
	    }

	    for(int i=0;i<digits;i++){
	        num_a[digits-i-1] = '0' + num%10;
	        num/=10;
	    }

	    return num_a;
	
}
