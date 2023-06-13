#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

void dev_writer(char mode, int seven, short* led);
short* if_people();

/*Create to save data of area number*/ 
//Baseball Stadium, Big City, Zoo
//Child, Adult, Elder, total(C,A,E)
unsigned dist_data[3][4] = {0};
//mode:{Normal, Search, Report, Exit}
unsigned TOTAL = 0, MODE = 0;
char* dist_name[3] = {"Baseball Stadium", "Big City", "Zoo"};

int main(int argc, char** argv){

    char MODE;
    short* led;
    int seven;

    while(1){
        MODE = ' ';
        printf("1. Search\n2. Report\n3. Exit\n");
        scanf(" %c", &MODE); //need to space to clean buffer

        if(MODE == '1'){
            //Search mode
            //DISTRICT same as AREA
            char DISTRICT;

            while(1){
                //write the total number to seven segment
                //LED light district which have people
                seven = dist_data[0][3] + dist_data[1][3] + dist_data[2][3];
                led = if_people();
                dev_writer('N', seven, led);

                //get total number of each DISTRICT
                //read function
                //write function(into search mode)
                printf("1. Baseball Stadium : %d\n", dist_data[0][3]);
                printf("2. Big City : %d\n", dist_data[1][3]);
                printf("3. Zoo : %d\n", dist_data[2][3]);
                scanf(" %c", &DISTRICT);
                
                if((DISTRICT == '1') || (DISTRICT == '2') || (DISTRICT == '3')){
               
                    //write the total number of each district to seven segment
                    //flash LED district
                    seven = dist_data[DISTRICT - '1'][3];
                    for(int i = 0; i < 3; i++){
                        if(i != (DISTRICT - '1'))
                            led[i] = -1;
                        else
                            led[i] = 1;
                    }
                    dev_writer('L', seven, led);

                    printf("Child : %d\n", dist_data[DISTRICT - '1'][0]);
                    printf("Adult : %d\n", dist_data[DISTRICT - '1'][1]);
                    printf("Elder : %d\n", dist_data[DISTRICT - '1'][2]);

                    getchar();
                    getchar();
                    // system("read -p '' var");
                }
                else if(DISTRICT == 'q'){
                    led = if_people();
                    seven = -1;
                    dev_writer('N', seven, led);
                    break;
                }
            }
        }
        else if(MODE == '2'){
            //Report mode
            char AddSub, GROUP, EoC;
            unsigned num = 0, AREA;

            while(1){
                printf("Area (1~3) : ");
                scanf(" %d", &AREA);
                printf("Add or Reduce ('a' or 'r') : ");
                scanf(" %c", &AddSub);
                printf("Age group ('c', 'a', 'e') : ");
                scanf(" %c", &GROUP);
                printf("The number of passenger : ");
                scanf(" %d", &num);

                //write (change number to driver) 
                if(AddSub == 'a'){
                    switch(GROUP){
                        case 'c':
                            dist_data[AREA - 1][0] += num;
                            break; 
                        case 'a':
                            dist_data[AREA - 1][1] += num;
                            break;
                        case 'e':
                            dist_data[AREA - 1][2] += num;
                            break;
                    }
                    dist_data[AREA - 1][3] += num;
                }
                else if(AddSub == 'r'){
                    switch(GROUP){
                        case 'c':
                            dist_data[AREA - 1][0] -= num;
                            break; 
                        case 'a':
                            dist_data[AREA - 1][1] -= num;
                            break;
                        case 'e':
                            dist_data[AREA - 1][2] -= num;
                            break;
                    }
                    dist_data[AREA - 1][3] -= num;
                }

                seven = dist_data[0][3] + dist_data[1][3] + dist_data[2][3];
                led = if_people();
                dev_writer('N', seven, led);

                printf("(Exit or Continue)\n");
                while(1){
                    scanf(" %c", &EoC);
                    // EoC = getchar(); 
                    if((EoC == 'e') || (EoC == 'c'))
                        break;
                }
                led = if_people();
                seven = -1;
                dev_writer('N', seven, led);
                if(EoC == 'e')
                    break;
            }
        }
        else if(MODE == '3'){
            dev_writer('C', seven, led);
            break;
        }

    }

    return 0;
}

short* if_people(){
    short* a = (short*)malloc(3 * sizeof(short));
    for(int i = 0; i < 3; i++){
        if(dist_data[i][3] != 0)
            a[i] = 1;
        else
            a[i] = -1;
    }
    
    return a;
}

void dev_writer(char mode, int seven, short* led){
    //for led 1 light, -1 dark, 0 not change
    //inital all light is not change
    char output[4] = {'n', 'n', 'n', 'n'}, zero[4] = {'n', 'n', 'n', 'n'}, fd;
    fd = open("/dev/etx_device", O_WRONLY);

    //led part copy to output
    for(int i = 0; i < 3; i++){
        if(led[i] == 1)
            output[i] = '1';
        else if(led[i] == -1)
            output[i] = 'd';
    }

    /////////////// flash LED mode /////////////////
    if(mode == 'L'){
        //when use this mode, evey led is 0 unless it need to flash
        short key, count = 0;
        //enter district whose led need flash, but other don't
        for(int i = 0; i < 3; i++){
            if(led[i] == 1){
                zero[i] = 'd';
                output[i] = '1';
                key = i;
            }     
            else
                zero[i] = 'd';
        }

        //also flash seven segment
        if(seven >= 10){
            int sev;
            short out_sev[4], sev_key;

            // create a array for seven segment output
            sev = seven;
            for(int i = 0; i < 4; i++){
                out_sev[i] = (sev % 10) + '0';
                sev /= 10;
                if(sev == 0){
                    sev_key = i;
                    break;
                }     
            }

            // start write output to device
            zero[3] = 'd';
            while((sev_key >= 0) || (count < 3)){
                //if all number are show ,seven segment dark
                if(sev_key >= 0){
                    output[3] = out_sev[sev_key];
                    sev_key--;
                }
                else
                    output[3] = 'd';

                //output number
                write(fd, output, 4);
                usleep(1000*500);
                //output null
                write(fd, zero, 4);
                usleep(1000*500);

                //adjust led back to origin state
                count++;
                if(count >= 3){
                    if(led[key] == 1){
                        zero[key] = '1'; 
                        output[key] = '1';
                    }
                    else{
                        zero[key] = 'd'; 
                        output[key] = 'd';
                    }    
                }
            }
            output[3] = 'd';
        }
        //only flash led
        else{
            if(seven == -1)
                output[3] = 'd';
            // else if(seven == 'n')
            //     output[3] = 'd';
            else
                output[3] = seven + '0';
            zero[3] = seven + '0';
            while(count < 3){
                //output number
                write(fd, output, 4);
                usleep(1000*500);
                //output null
                write(fd, zero, 4);
                usleep(1000*500);

                count++;
                if(count >= 3){
                    if(led[key] == 1){
                        zero[key] = '1'; 
                        output[key] = '1';
                    }
                    else{
                        zero[key] = 'd'; 
                        output[key] = 'd';
                    }    
                }
            }
        }
        //led & seven back to final state
        write(fd, output, 4);
    }
    /////////////// normal state /////////////////
    else if(mode == 'N'){
        //flash seven segment when it big then 9
        if(seven >= 10){
            int sev;
            short out_sev[4], sev_key;

            // create a array for seven segment output
            sev = seven;
            for(int i = 0; i < 4; i++){
                out_sev[i] = (sev % 10) + '0';
                sev /= 10;
                if(sev == 0){
                    sev_key = i;
                    break;
                }     
            }
            // start write output to device
            zero[3] = 'd';
            for(int i = sev_key; i >= 0; i--){
                output[3] = out_sev[i];
                //output number
                write(fd, output, 4);
                usleep(1000*500);
                //output null
                write(fd, zero, 4);
                usleep(1000*500);
            }   
            output[3] = 'd';
            write(fd, output, 4);
        }
        else{
            if(seven == -1)
                output[3] = 'd';
            // else if(seven == 'n')
            //     output[3] = 'd';
            else
                output[3] = seven + '0';
            write(fd, output, 4);
        }
    }
    /////////////// every thing is dark mode /////////////////
    else if(mode == 'C'){
        for(int i = 0; i < 4; i++)
            zero[i] = 'd';
        write(fd, zero, 4);
    }

    close(fd);
}

