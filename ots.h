/*
 * Header file OTS
 * @Author: Peter Vican
 * @Login: xvican02
 * @University: Brno University of Technology
 * @Faculty: Faculty of Information Technology
 * @Name: Project to PRL 2019/2020 - Odd Even Transposition sort
 * @CreatedAt: 10.3.2020
 * @VersionCode: 20
 * @Notes: Some part of code is use from last year project to PRL.
 */

#ifndef ODD_EVEN_OTS_H
#define ODD_EVEN_OTS_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

using namespace std;

#define DEBUG false
#define SHOW_TIME false

const int COUNT_ARGC = 2;
const int ERROR_COUNT = -5;
const int SUCCESS = 0;
const int ROOT_NODE = 0;
const int TAG_MPI = 0;
const char* LEVEL_DEBUG = "DEBUG: ";

bool parseArgument(int actualArgc, int expectedArgc, vector<string> argument);

bool isFileExist(string &fileName);

string getErrorMessage(int errorCode);

void initializeMPI(int argc, char **argv, int *infos);

void loadNumbersFromFile(char *fileName);

void printLogs(const char* logsLevel, const string& message);

void showConsumptionTime(double start);

double startMPITime();

void processorsSendReceive(const int *mpiInfo, MPI_Status &mpiStatus, int &actualValue);

void processorsSwap(const int *mpiInfo, MPI_Status &mpiStatus, int &neighbourValue, int &actualValue);

void lastOrderAndShowRootNode(MPI_Status &status, const int *mpiInfo, int &nValue, int &value, double start);

#endif //ODD_EVEN_OTS_H
