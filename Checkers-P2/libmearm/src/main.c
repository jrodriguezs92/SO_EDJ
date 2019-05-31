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

void printHelp(void);

int main(int argc, char *argv[]) {
	static struct option long_options[] = {
		{"conf_file", required_argument, 0, 'c'},
		{"help", no_argument, 0, 'h'},
		{NULL, 0, 0, 0}
	};

	// Variables for process command line arguments
	int value, optionIndex = 0;
	appName = argv[0];

	// Process all command line arguments
	while ((value = getopt_long(argc, argv, "c:h", long_options, &optionIndex)) != -1) {
		switch (value) {
			case 'c':
				confFileName = strdup(optarg);
				break;
			case 'h':
				printHelp();
				return EXIT_SUCCESS;
			case '?':
				printHelp();
				return EXIT_FAILURE;
			default:
				break;
		}
	}

	// Init device
	initDevice("/dev/ttyUSB0",115200);

	if (robot != -1) {
		// Reads configuration from config file
		readConfFile();

		// Close device
		closeDevice();
	}

	return 0;
	
}

/**
 * Prints help for this application
 */
void printHelp (void){
	printf("\n Usage: %s [OPTIONS]\n\n", appName);
	printf("  Options:\n");
	printf("   -h --help                 Print this help\n");
	printf("   -c --conf_file filename   Read configuration from the file\n");
	printf("\n");
}