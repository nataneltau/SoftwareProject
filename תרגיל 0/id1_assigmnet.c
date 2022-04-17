#include<stdio.h>
#include <math.h>

int sum =0;
int i =0;

int isLegal(char, int);

char numVal(int);

void stringRev(char[], int);

int getNumFromUser(){
    int i, num;
    char inNumBase[2];
    char inputNum;

    i = 0;


    while((inputNum = getchar()) != '\n'){
        if(i>=2){
            return -1;
        }
        inNumBase[i] = inputNum;
        i++;
    }//end of while

    if(i==0){
        return -1;
    }//end of if

    if(i==1){
        num = inNumBase[0] - '0';
        if(2<=num<=9){
            return num;
        }
        return -1;
    }

    num = inNumBase[0] - '0';
    if(num!=1){
        return -1;
    }
    num = num*10; //num = 10;
    num+= (inNumBase[1] - '0');

    if(num>=10 && num<=16){
        return num;
    }
    return -1;


}

void convToDecimalNumberFromBaseA(int);
void convToBaseBFromDecimal(char[], int, int);

int main(){

    int numBase, desiredBase;
    int number;
    char strNumber[256];

    printf("Please enter the numbers base: \n");

   numBase = getNumFromUser();

    if(numBase == -1){
        printf("Invalid input base\n");
        return 0;
    }//end of if

    printf("Please enter the desired base: \n");
    desiredBase = getNumFromUser();

    if(desiredBase == -1){
        printf("Invalid desired base\n");
        return 0;
    }//end of if


    printf("Please enter a number in base %d:\n", numBase);

    convToDecimalNumberFromBaseA(numBase);

    number = sum;

    if(number == -1){
        printf("Invalid number!\n");
        return 0;
    }//end of if

    convToBaseBFromDecimal(strNumber, number, desiredBase);

    printf("The result is: %s\n", strNumber);

    return 0;

}//end of function main



void convToDecimalNumberFromBaseA(int numBase){
    
    char num;

    while((num = getchar()) != '\n'){
        convToDecimalNumberFromBaseA(numBase);
        if(isLegal(num, numBase) && (sum !=-1)){
            if(!(num >='0' && num<='9')){
                num = num - 'a' +10 +'0';
            }
            
            sum+= (num -'0')*pow(numBase,i);
            i++;
        }//end of if
        else{
            sum = -1;
            return;
        }//end of else

    }//end of while

    return;

}//end of function getNumberInBaseA

void convToBaseBFromDecimal(char res[], int num, int desBase){
    int index=0;

    for(int i=0; num>0; index++){
        res[index] = numVal(num%desBase);
        num = num/desBase;
    }
    res[index] = '\0';

    stringRev(res, index);
    return;
}

void stringRev(char str[], int index){

    char temp;

    for(int i=0; i<index/2; i++){
        temp = str[i];
        str[i] = str[index-i-1];
        str[index-i-1] = temp;
    }
    return;
}


char numVal(int num){

    if (num >= 0 && num <= 9)
        return (char)(num + '0');
    else
        return (char)(num - 10 + 'a');
}


int isLegal(char numC, int base){

    int num;

    if(numC >='0' && numC<='9'){

        num = numC-'0';
    }
    else{
        num = numC - 'a' +10;
    }

    if(num >= base){
        return 0;
    }

    return 1;
}
