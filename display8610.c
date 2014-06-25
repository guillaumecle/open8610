/*  open8610 - history8610.c
 *
 *  Version 0.01
 *
 *  Control WS8610 weather station
 *
 *  Copyright 2003-2006, Kenneth Lavrsen, Grzegorz Wisniewski, Sander Eerkes, Philip Rayner
 *  Portions by Laurent Chauvin
 *
 *  This program is published under the GNU General Public license
 */

#include "rw8610.h"

/********************************************************************
 * print_usage prints a short user guide
 *
 * Input:   none
 *
 * Output:  prints to stdout
 *
 * Returns: exits program
 *
 ********************************************************************/
void print_usage(void)
{
    printf("\n");
    printf("history8610 - Dump all history data from WS-8610 to file.\n");
    printf("(C)2003 Kenneth Lavrsen, Grzegorz Wisniewski, Sander Eerkes.\n");
    printf("(C)2006-7 Phil Rayner.\n");
    printf("This program is released under the GNU General Public License (GPL)\n\n");
    printf("Usage:\n");
    printf("history8610 filename start_record end_record\n");
    printf("Record number in dec, range 0 - 3200\n");
    exit(0);
}


/********** MAIN PROGRAM ************************************************
 *
 * This program reads the history records from a WS8610
 * weather station at a given record range
 * and prints the data to stdout and to a file.
 * Just run the program without parameters for usage.
 *
 * It uses the config file for device name.
 * Config file locations - see open8610.conf
 *
 ***********************************************************************/
int main(int argc, char *argv[])
{
    WEATHERSTATION ws;
    unsigned char data[32768];
    int o_count;
    int i;
    int start_rec, end_rec, start_adr, end_adr;
    struct config_type config;

    // Get in-data and select mode.

    // Get serial port from connfig file.
    // Note: There is no command line config file path feature!
    // history8610 will only search the default locations for the config file

    get_configuration(&config, "");

    start_rec = 0;
    end_rec = 500;
    int k;
    for (k=0; k<=6; k++)
    {


    // Setup serial port
    ws = open_weatherstation(config.serial_device_name);

    if ((o_count = outdoor_count(ws)) == -1)
    {
        printf("Cannot get count of outdoor sensors\n");
        close_weatherstation(ws);
        exit(0);
    }

    for (i=k*500+start_rec; i<=k*500+end_rec; i++)
    {
        printf("Record %04i ",i);
        //printf("\n");

        struct history_record hr;
	read_history_record(ws, i, &hr, o_count);
        unsigned char logline[1024] = "";
        char str[50];
        char datestring[100];        //used to hold the date stamp for the log file
        time_t basictime;

        /* READ TEMPERATURE INDOOR */
        sprintf(logline,"%sTi: %s | ", logline, temp2str(hr.Temp[0], "%.1f", str));

        /* READ RELATIVE HUMIDITY INDOOR */
        sprintf(logline,"%sHi: %s | ", logline, RH2str(hr.RH[0], "%d", str));

        /* READ TEMPERATURE OUTDOOR */
        sprintf(logline,"%sTo1: %s | ", logline, temp2str(hr.Temp[1], "%.1f", str));

        /* READ RELATIVE HUMIDITY OUTDOOR */
        sprintf(logline,"%sHo1: %s | ", logline, RH2str(hr.RH[1], "%d", str));

        /* READ SECOND TEMPERATURE OUTDOOR */
        sprintf(logline,"%sTo2: %s | ", logline, temp2str(hr.Temp[2], "%.1f", str));

        /* READ SECOND RELATIVE HUMIDITY OUTDOOR */
        sprintf(logline,"%sHo2: %s | ", logline, RH2str(hr.RH[2], "%d", str));

        /* READ THIRD TEMPERATURE OUTDOOR */
        sprintf(logline,"%sTo3: %s | ", logline, temp2str(hr.Temp[3], "%.1f", str));

        /* READ THIRD RELATIVE HUMIDITY OUTDOOR */
        sprintf(logline,"%sHo3: %s | ", logline, RH2str(hr.RH[3], "%d", str));

        /* GET TIME STAMP OF LAST HISTORY RECORD */
        sprintf(logline,"%s%s", logline, ctime(&hr.time_stamp));

        /* GET DATE AND TIME FOR LOG FILE, PLACE BEFORE ALL DATA IN LOG LINE */
        time(&basictime);
        strftime(datestring,sizeof(datestring),"%d/%m %H:%M:%S |",
                         localtime(&basictime));

        // Print out and leave
        printf("%s %s",datestring, logline);

    }

    // Goodbye and Goodnight
    close_weatherstation(ws);

    }

    return(0);
}
