/*
 * @Author: Peter Vican
 * @Login: xvican02
 * @University: Brno University of Technology
 * @Faculty: Faculty of Information Technology
 * @Name: Project to PRL 2019/2020 - Odd Even Transposition sort
 * @CreatedAt: 10.3.2020
 * @VersionCode: 20
 * @Notes: Some part of code is use from last year project to PRL and from example codes
 */

#include "ots.h"

int main(int argc, char **argv) {
    printLogs(LEVEL_DEBUG, "main()");

    // Control arguments
    vector<string> arguments(argv, argv + argc);

    if (!parseArgument(argc, COUNT_ARGC, arguments)) {
        cerr << getErrorMessage(ERROR_COUNT) << endl;
        return ERROR_COUNT;
    }

    printLogs(LEVEL_DEBUG, "Success parse arguments and exist files");
    printLogs(LEVEL_DEBUG, "Filename: " + arguments.at(1));

    // MPI status with code, errors etc.
    MPI_Status mpiStatus;

    // first item is nodeId and second item is number of processors
    int mpiInfo[2];

    // initialize MPI and set mpiInfo
    initializeMPI(argc, argv, mpiInfo);

    int neighbourValue; // neighbour value
    int actualValue; //my value

    double start = startMPITime();

    if (mpiInfo[0] == ROOT_NODE) {
        loadNumbersFromFile(argv[1]);
    }

    MPI_Recv(&actualValue, 1, MPI_INT, 0, TAG_MPI, MPI_COMM_WORLD, &mpiStatus);
    printLogs(LEVEL_DEBUG, to_string(mpiInfo[0]) + "value is: " + to_string(actualValue));

    int oddLimit = 2 * (mpiInfo[1] / 2) - 1;
    int evenLimit = 2 * ((mpiInfo[1] - 1) / 2);
    int halfCycles = mpiInfo[1] / 2;

    for (int j = 1; j <= halfCycles; j++) {
        // even processors
        if ((!(mpiInfo[0] % 2) || mpiInfo[0] == 0) && (mpiInfo[0] < oddLimit)) {
            evenProcessorsSendReceive(mpiInfo, mpiStatus, actualValue);
        } else if (mpiInfo[0] <= oddLimit) {
            oddProcessorsSwap(mpiInfo, mpiStatus, neighbourValue, actualValue);
        }

        // odd processors
        if ((mpiInfo[0] % 2) && (mpiInfo[0] < evenLimit)) {
            oddProcessorsSendReceive(mpiInfo, mpiStatus, actualValue);
        } else if (mpiInfo[0] <= evenLimit && mpiInfo[0] != 0) {
            evenProcessorsSwap(mpiInfo, mpiStatus, neighbourValue, actualValue);
        }
    }

    lastOrderedAndShowRootNode(mpiStatus, mpiInfo, neighbourValue, actualValue, start);

    MPI_Finalize();
    return SUCCESS;
}

// function for run mpi time
double startMPITime() {
#if SHOW_TIME
    return MPI_Wtime();
#else
    return 0;
#endif
}

// function for show consumption time for algorithm
void showConsumptionTime(double start) {
#if SHOW_TIME
    double end = MPI_Wtime();
    cerr << "Consumption time: " << (end - start) << endl;
#endif
}

void evenProcessorsSwap(const int *mpiInfo, MPI_Status &mpiStatus, int &neighbourValue, int &actualValue) {
    MPI_Recv(&neighbourValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD,
             &mpiStatus); //jsem sudy a prijimam

    if (neighbourValue >
        actualValue) {                                             //pokud je leveho sous cislo vetsi
        MPI_Send(&actualValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD);       //poslu svoje
        actualValue = neighbourValue;                                              //a vemu si jeho
    } else {
        MPI_Send(&neighbourValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD);
    }
}

void oddProcessorsSendReceive(const int *mpiInfo, MPI_Status &mpiStatus, int &actualValue) {
    MPI_Send(&actualValue, 1, MPI_INT, mpiInfo[0] + 1, TAG_MPI, MPI_COMM_WORLD);           //poslu sousedovi svoje cislo
    MPI_Recv(&actualValue, 1, MPI_INT, mpiInfo[0] + 1, TAG_MPI, MPI_COMM_WORLD, &mpiStatus);    //a cekam na nizsi

}

// function for odd processors swap
void oddProcessorsSwap(const int *mpiInfo, MPI_Status &mpiStatus, int &neighbourValue, int &actualValue) {
    MPI_Recv(&neighbourValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD, &mpiStatus);

    if (neighbourValue > actualValue) {
        MPI_Send(&actualValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD);
        actualValue = neighbourValue;
    } else {
        MPI_Send(&neighbourValue, 1, MPI_INT, mpiInfo[0] - 1, TAG_MPI, MPI_COMM_WORLD);
    }
}

// function for even send and receive
void evenProcessorsSendReceive(const int *mpiInfo, MPI_Status &mpiStatus, int &actualValue) {
    MPI_Send(&actualValue, 1, MPI_INT, mpiInfo[0] + 1, TAG_MPI, MPI_COMM_WORLD);          //poslu sousedovi svoje cislo
    MPI_Recv(&actualValue, 1, MPI_INT, mpiInfo[0] + 1, TAG_MPI, MPI_COMM_WORLD, &mpiStatus);   //a cekam na nizsi
}

// function for load data from file
void loadNumbersFromFile(char *fileName) {
    int processorDestination = 0;
    fstream inputFile;
    int readValue;

    inputFile.open(fileName, ios::in);

    readValue = inputFile.get();

    while (readValue != EOF) {
        cout << readValue;

        printLogs(LEVEL_DEBUG, to_string(processorDestination) + ": " + to_string(readValue));
        MPI_Send(&readValue, 1, MPI_INT, processorDestination, TAG_MPI, MPI_COMM_WORLD);
        processorDestination++;
        readValue = inputFile.get();
        if (readValue != EOF) {
            cout << " ";
        }
    }

    cout << endl;
    inputFile.close();
}

void printLogs(const char *logsLevel, const string &message) {
#if DEBUG
    cerr << logsLevel << message << endl;
#endif
}

// parse argument if is okay return true otherwise false
bool parseArgument(int actualArgc, int expectedArgc, vector<string> argument) {
    if (actualArgc != expectedArgc)
        return false;

    return isFileExist(argument.at(1));
}

bool isFileExist(string &fileName) {
    ifstream inputFile(fileName);
    return inputFile.good();
}

// get Error message
string getErrorMessage(int errorCode) {
    if (errorCode == ERROR_COUNT) {
        return "`Wrong arguments or file does not exists";
    } else {
        return "Unkown error";
    }
}

// initialize MPI
void initializeMPI(int argc, char **argv, int *infos) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &infos[0]);
    MPI_Comm_size(MPI_COMM_WORLD, &infos[1]);
}

void lastOrderedAndShowRootNode(MPI_Status &status, const int *mpiInfo, int &nValue, int &value, double start) {
    int orderedValues[mpiInfo[1]];

    for (int i = 1; i < mpiInfo[1]; i++) {
        if (mpiInfo[0] == i) MPI_Send(&value, 1, MPI_INT, ROOT_NODE, TAG_MPI, MPI_COMM_WORLD);
        if (mpiInfo[0] == ROOT_NODE) {
            MPI_Recv(&nValue, 1, MPI_INT, i, TAG_MPI, MPI_COMM_WORLD, &status); //jsem 0 a prijimam
            orderedValues[i] = nValue;
        }
    }

    if (mpiInfo[0] == ROOT_NODE) {
        orderedValues[0] = value;

        showConsumptionTime(start);

        for (int i = 0; i < mpiInfo[1]; i++) {
            cout << orderedValues[i] << endl;
        }
    }
}
