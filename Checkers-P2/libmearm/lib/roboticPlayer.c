/**
 * Copyright (C) 2019 
 * Área Académica de Ingeniería en Computadoras, ITCR, Costa Rica
 *
 * Interpreter
 * Implementation file
 *
 * @Author: Jeremy Rodriguez
 * @Date  : 22.05.2019
 */

#include <roboticPlayer.h>

/**
 * This function read configuration from config file
 */
int readConfFile (void){
	FILE *conf_file = NULL;

	if (confFileName == NULL) {
        printf("Conf path not found");
		return -1;
	}

	conf_file = fopen(confFileName, "r");

	if (conf_file == NULL) {
		printf("Can not open config file: %s", confFileName);
		return -1;
	}

    //////////// Read lines from config file /////////////////////////////////////////////////////////

    char *line, buff[256];

    // Read line by line
    while ((line = fgets(buff, sizeof buff, conf_file)) != NULL) {

        // Ignore comments with "#" and blankspaces
	    if (buff[0] == '\n' || buff[0] == '#'){
            continue;
	    }

        // Call function to parse the line
        int res = paser(line);

        if ( (res == 1) || (res == 0) ) {
            continue;
        }
        else if (res == -1) {
            return -1;
        }
    }

    // Close conf file
    fclose(conf_file);

    return 0;
}

/**
 * Parse a line as a command
 * Receive a string line type char * to parse
 * Return 0  -> succesfully
 *        1  -> Empty line
 *        -1 -> Error
 */
int paser (char * line) {

    char * lineTmp;
    char * buff = line;

    // Tokenizer line with parameters and command
    char command[MAXLEN], valueX[MAXLEN], valueY[MAXLEN];

    // If theres no line, continue to the next one
    if (line==NULL) {
        return 1;
    }
    // Else, copy first token as a command
    else {
        lineTmp = strtok(buff, "(");

        // Copy first token as a command
        strncpy(command, lineTmp, MAXLEN);
        trimValue(command);

        // Check if the command is an allowed value
        if ( (strcmp(command, "move")!=0) && (strcmp(command, "pick")!=0) && 
             (strcmp(command, "drop")!=0) && (strcmp(command, "moveandpick")!=0) && 
             (strcmp(command, "moveanddrop")!=0) ) {
                printf("Command not allowed: %s \n", command);
                return -1;
        }

        if ( (strcmp(command, "pick")!=0) && (strcmp(command, "drop")!=0) ) {
            // Next Token (Parameters) (if there is anyone)
            lineTmp = strtok(NULL, "(");
        }
        // If theres no parameters and not certain commands
        if ( (lineTmp==NULL && ( (strcmp(command, "move")==0) || 
                                (strcmp(command, "moveandpick")==0) || 
                                (strcmp(command, "moveanddrop")==0) ) ) || 
            ((strcmp(lineTmp, ")")==0) && ( (strcmp(command, "move")==0) || 
                                (strcmp(command, "moveandpick")==0) || 
                                (strcmp(command, "moveanddrop")==0) ) ) ) {
            printf("Command sintax incorrect. Parameters not found: %s \n", command);
            return -1;
        }
        // If command is pick or drop, ignore the rest of the line and continue
        else if (( (strcmp(command, "pick")==0) || 
                 (strcmp(command, "drop")==0) ) ) {}
        // Else, copy values as parameters
        else {
            char parameters[MAXLEN];
            lineTmp = strtok(lineTmp, ")");
            strncpy(parameters, lineTmp, MAXLEN);

            // First parameter
            lineTmp = strtok(parameters, ",");
            strncpy (valueX, lineTmp, MAXLEN);
            trimValue (valueX);
            // Check if the value is a number
            if (!isNumber(valueX)) {
                printf("Parameter X: %s is not a decimal value \n", valueX);
                return -1;
            }

            // Second parameter
            lineTmp = strtok(NULL, ",");
            // If theres no a second parameter
            if (lineTmp == NULL) {
                printf("Command sintax incorrect. Second parameter not found: %s \n", command);
                return -1;
            }
            strncpy (valueY, lineTmp, MAXLEN);
            trimValue (valueY);
            // Check if the value is a number
            if (!isNumber(valueY)) {
                printf("Parameter Y: %s is not a decimal value \n", valueY);
                return -1;
            }
        }
    }
    cmmd = command;

    if ( (valueX != NULL) && (valueY != NULL) ) {
        argX = atoi(valueX);
        argY = atoi(valueY);
    }

    // Call respective function
    if (strcmp(cmmd,"move")==0) {
        move(argX,argY);
    }
    else if (strcmp(cmmd,"pick")==0) {
        pick();
    }
    else if (strcmp(cmmd,"drop")==0) {
        drop();
    }
    else if (strcmp(cmmd,"moveandpick")==0) {
        moveandpick(argX,argY);
    }
    else if (strcmp(cmmd,"moveanddrop")==0) {
        moveanddrop(argX,argY);
    }

    return 0;
}

/**
 * This function execute the operation move
 * Receive two int values as a position
 */
void move (int x, int y) {
    printf("move function: %d, %d \n", argX, argY);

    // Intruction buffer
    char instruction[MAXLEN];

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z50\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move X

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"X%d\n",x);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move Y

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"Y%d\n",y);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);

    sleep(2);
}

/**
 * This function execute the operation pick
 */
void pick (void) {
    printf("pick function \n");

    // Intruction buffer
    char instruction[MAXLEN];

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z-25\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    // Pick
    meArmWrite(robot,"P");

    sleep(2);
}

/**
 * This function execute the operation drop
 */
void drop (void) {
    printf("drop function \n");

    // Intruction buffer
    char instruction[MAXLEN];

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z-25\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //Drop

    meArmWrite(robot,"D");

    sleep(2);
}

/**
 * This function execute the operation moveandpick
 * Receive two int values as a position
 */
void moveandpick (int x, int y) {
    printf("moveandpick function: %d, %d \n", argX, argY);

    // Intruction buffer
    char instruction[MAXLEN];

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z50\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move X

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"X%d\n",x);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move Y

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"Y%d\n",y);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);

    sleep(2);

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z-25\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    // Pick

    snprintf(instruction,MAXLEN,"P\n");

    printf("Sending instruction: %s\n",instruction);
    meArmWrite(robot,instruction);

    sleep(2);
}

/**
 * This function execute the operation moveanddrop
 * Receive two int values as a position
 */
void moveanddrop (int x, int y) {
    printf("moveanddrop function: %d, %d \n", argX, argY);

    // Intruction buffer
    char instruction[MAXLEN];

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z50\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move X

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"X%d\n",x);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    //////// Move Y

    // Cast the integer argument X to string and concatenate command
    snprintf(instruction,MAXLEN,"Y%d\n",y);

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);

    sleep(2);

    //////// Move Z

    snprintf(instruction,MAXLEN,"Z-25\n");

    printf("Sending instruction: %s\n",instruction);

    // Write isntruction to the device
    meArmWrite(robot,instruction);
    sleep(2);

    // Drop

    snprintf(instruction,MAXLEN,"D\n");

    printf("Sending instruction: %s\n",instruction);
    meArmWrite(robot,instruction);

    sleep(2);
}

/** 
  * This function trims the value found in readConfFile
  * Receive a string line type char * to trim
  * Return the trimed line
  */
char * trimValue (char * line){
  //Initialize start, end pointers 
  char *start = line, *end = &line[strlen (line) - 1];

  //Trim right side 
  while ( (isspace (*end)) && (end >= start) )
    end--;
  *(end+1) = '\0';

  //Trim left side 
  while ( (isspace (*start)) && (start < end) )
    start++;

  //Copy finished string 
  strcpy (line, start);
  return line;
}

/**
 * This function check if a strig is a number value
 * Recive a string type char * to verify if is a number
 * Return 1 -> is Number
 *        0 -> is not a number
 */
int isNumber (const char * str) {

    const char * strTmp = str;

    if (str == NULL || *str == '\0') {
        return 0;
    }

    int dotCount = 0;
    int it = 1;

    while (*str) {
        char c = *str;
        switch (c) {
            case '.':
                if ( (++dotCount > 1) || (it==1) || (it==((int)(strlen(strTmp)))) )
                    return 0;
                break;
            
            case '+':
                if (it > 1) {
                    return 0;
                }
                break;

            case '-':
                if (it > 1) {
                    return 0;
                }
                break;
            default:
                if (c == '\0') {
                    return 1;
                }
                if (c < '0' || c > '9') {
                    return 0;
                }
        }

        it++;
        str ++;
    }

    return 1;
}
/**
 * call init Function from the device library
 * Receive the device file path and the baud speed to communicate
 */
void initDevice (const char* path, int baud) {
    if ((robot = meArmInit(path,baud))<0) {
        printf("Init device error \n");
    }
}

/**
 * Call the Close function from the device library
 */
void closeDevice () {
    meArmClose(robot);
}