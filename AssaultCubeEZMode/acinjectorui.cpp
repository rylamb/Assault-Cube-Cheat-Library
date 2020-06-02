#include "acinjectorui.h"
#include "ui_acinjectorui.h"

//Constructor
ACInjectorUI::ACInjectorUI(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ACInjectorUI)
{
    ui->setupUi(this);
    this->processFoundUpdate();
    this->injectableFoundUpdate();
}


//Destructor
ACInjectorUI::~ACInjectorUI()
{
    delete ui;
}

//Logic Functions

/**************************************************
 * checkInjectReady()
 * Checks if the requirements for injecting a .dll
 * file have been met.
 * Requires that Assault Cube process be found and
 * running, and that a valid .dll has been set.
 * @param: None
 * @return: None
 * ***********************************************/
void ACInjectorUI::checkInjectReady()
{
    if(this->processFound and this->dllSet) {
        ui->InjectEjectButton->setEnabled(true);
    } else {
        ui->InjectEjectButton->setEnabled(false);
    }
}

/**************************************************
 * processFoundUpdate()
 * Updates UI based on result of isRunning().
 * @param: None
 * @return: None
 * ***********************************************/
void ACInjectorUI::processFoundUpdate()
{
    QString successImg = "../Images/checkmark-512.png";
    QString failImg = "../Images/x-mark-512.png";
    QString successMsg = "Assault Cube Process Detected";
    QString failMsg = "Assault Cube Process Not Found";

    if(this->isRunning()) {
        ui->ProcessFoundIcon->setPixmap(successImg);
        ui->ProcessFoundText->setText(successMsg);
        this->processFound = true;
    } else {
        ui->ProcessFoundIcon->setPixmap(failImg);
        ui->ProcessFoundText->setText(failMsg);
        this->processFound = false;
    }

    this->checkInjectReady();
}

/**************************************************
 * injectableFoundUpdate()
 * Updates UI based on the contents of the "Location"
 * text field.
 * @param: None
 * @return: None
 * ***********************************************/
void ACInjectorUI::injectableFoundUpdate()
{
    QString successImg = "../Images/checkmark-512.png";
    QString failImg = "../Images/x-mark-512.png";
    QString successMsg = "Injectable DLL Set";
    QString failMsg = "Injectable DLL Not Set";
    QString error = "Error: Injectable set does not have '.dll' file extension.";
    if (this->injectableFilePath != "") {
        QRegularExpression re("[A-Za-z]*\.dll");
        QRegularExpressionMatch match = re.match(this->injectableFilePath);

        //Set success status and return if match, show error otherwise
        if(match.hasMatch()) {
            ui->InjectableFoundIcon->setPixmap(successImg);
            ui->InjectableFoundText->setText(successMsg);
            this->dllSet = true;
            this->checkInjectReady();
            return;
        } else {
            this->showError(error);
        }
    }
    //If execution reaches here then injectableFilePath is blank
    //or injectableFilePath is set to non-dll file -> set fail status
    ui->InjectableFoundIcon->setPixmap(failImg);
    ui->InjectableFoundText->setText(failMsg);
    this->injectableFilePath = "";
    this->dllSet = false;
    this->checkInjectReady();
}

/**************************************************
 * isRunning()
 * Determines if Assault Cube is currently running.
 * Used to update the UI and allow injection attempts.
 * @param: None
 * @return: Bool
 * ***********************************************/
bool ACInjectorUI::isRunning()
{
    //The commented-out section works, but only when the app is compiled for 64-bit,
    //which will cause injection into a 32-bit application (i.e. Assault Cube) to fail.
    //Decided to keep it just in case we need to do anyting with 64-bit down the road.

    /*
    DWORD aProcesses[1024];         //Pointer to array that receives list of process IDs
    DWORD returnedSize;             //Number of bytes returned in the process IDs array
    DWORD numProcesses;             //Calculated number of process IDs
    TCHAR processName[_MAX_PATH];   //String for holding the name of a given process
    TCHAR acProcessName[_MAX_PATH] = TEXT("ac_client.exe");   //Name of target process
    HANDLE hProcess = NULL;         //Handle for working with selected process
    HMODULE hMod = NULL;            //For working with a specific process module

    QString error = "Cannot Enumerate Running Processes\nError Code: ";
    QString errorCode;                //Variable for holding value from GetLastError()

    //Reset stored processID for AssaultCube -> may not be valid anymore
    this->acProcessID = 0;

    //Use Windows API to get list of process identifiers
    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &returnedSize)) {
        errorCode = QString::number(GetLastError());
        this->showError(error + errorCode);
        return false;
    }

    //Calculate how many process IDs were returned
    numProcesses = returnedSize/sizeof(DWORD);

    //Determine if the Assault Cube process ID is in the returned process ID list
    for (int i = 0; i < numProcesses; i++) {
        ZeroMemory(processName, sizeof(processName));   //Clear any previous contents
        if (aProcesses[i] != 0) {
            //Get a handle to the process
            hProcess = OpenProcess(
                PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                FALSE, aProcesses[i]);

            //Get the process name
            if (hProcess != NULL) {
                if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &returnedSize)) {
                    GetModuleBaseName(hProcess, hMod, processName,
                        sizeof(processName)/sizeof(TCHAR));
                } else {
                    errorCode = QString::number(GetLastError());
                    this->showError(error + errorCode);
                    CloseHandle(hProcess);
                    return false;
                }
            }

            //Check if process name matches known Assault Cube process name
            //ProcessName will be blank if hProcess == NULL
            if(_tcscmp(processName, acProcessName) == 0) {
                this->acProcessID = aProcesses[i];
                CloseHandle(hProcess);
                return true;
            }
        }

        //Close hProcess handle if not done so already
        if(hProcess != NULL) {
            CloseHandle(hProcess);
        }
    }
    //If execution makes it here, everything worked but Assault Cube wasn't found
    return false;
    */

    //Get new processID -> Assault Cube is running if getProcId returns non-zero value
    this->acProcessID = this->getProcId(L"ac_client.exe");
    if(this->acProcessID == 0) {
        this->showError("Error: Assault Cube Process Not Found."
                        "\nEnsure Assault Cube is running and try again.");
        return false;
    }
    return true;
} 

/**************************************************
 * showError()
 * Standard function used to show an error in a
 * warning box to the user.
 * @param: None
 * @return: None
 * ***********************************************/
void ACInjectorUI::showError(QString message)
{
    QMessageBox::warning(this, "Warning", message);
}

/**************************************************
 * getProcId()
 * Fetches the process id of a process whose name
 * matches the name given in the input.
 * Returns 0 if no matching process found.
 * @param: const wchar_t -> name of target process
 * @return: DWORD -> process ID of target process,
 *      or 0 if target process not found
 * ***********************************************/
DWORD ACInjectorUI::getProcId(const wchar_t *procName)
{
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Create a snapshot

    if (hSnap != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) // Get the first process
        {
            do
            {
                //_bstr_t b(procEntry.szExeFile);
                //char *procEntry_szExeFile = b;
                if (!_wcsicmp(procEntry.szExeFile, procName)) // Process name matches what we're looking for
                {
                    procId = procEntry.th32ProcessID; // Get the process ID
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

//Functions for handling UI interactions

void ACInjectorUI::on_CheckProcessButton_clicked()
{
    this->processFoundUpdate();
}

void ACInjectorUI::on_LocationBrowseButton_clicked()
{
    //Open file browser for user -> they should choose injectable dll
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Select Injectable DLL",
        QDir::currentPath(),
        "All files (*.*)");
    injectableFilePath = fileName;
    this->injectableFoundUpdate();
    ui->LocationTextField->setText(injectableFilePath);
}

void ACInjectorUI::on_InjectEjectButton_clicked()
{
    //Commented-out section is my injection attempt/solution for 64-bit
    //Saving here in case we want to do something with 64-bit process

    /*
    QString acNotFound = "Error: Assault Cube Process Not Found";
    QString invalidHandle = "Error: OpenProcess Failure";
    QString writeProcMemFail = "Error: WriteProcessMemory Failure\nError Code: ";
    QString createRemThreadFail = "Error: CreateRemoteThread Failure\nError Code: ";
    QString errorCode;
    HANDLE hProc = NULL;   //Handle used for accessing Assault Cube process
    */
    /*Convert dll file path from QString to const char*
    * Need to explicitly convert to QByteArray first and then to const char*
    * Doing the following:
    * const char *filePath = this->injectableFilePath.toLocal8Bit().data();
    * will make the app crash as the QByteArray has not been stored and no longer exists*/

    //Also need to convert all "/" to "\\" to make file path work
    /*
    QString convertedFilePath = this->injectableFilePath.replace(QLatin1String("/"), QLatin1String("\\\\"));
    QByteArray iFPByteArray = convertedFilePath.toLocal8Bit();
    const char *filePath = iFPByteArray.data();
    */
    /*
    std::string filePathString = convertedFilePath.toStdString();
    char *filePath;
    memset(filePath, 0, sizeof(filePath));
    strcpy(filePath, filePathString);
    filePath = filePathString.c_str();
    char *filePath = new char[1024];
    memset(filePath, 0, sizeof(char) * 1024);
    strcpy(filePath, iFPByteArray.data());
    */
    /*
    if(this->isRunning()) {
        //Attempt injection
        hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, this->acProcessID);
        if (hProc and hProc != INVALID_HANDLE_VALUE) {
            void* loc = VirtualAllocEx(hProc, 0, MAX_PATH,
                MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

            if(loc != NULL and !WriteProcessMemory(hProc, loc, filePath, strlen(filePath) + 1, 0)) {
                //Handle error -> WriteProcessMemory call was unsuccessful
                errorCode = QString::number(GetLastError());
                showError(writeProcMemFail + errorCode);
                CloseHandle(hProc);
                return;
            }
            HANDLE hThread = CreateRemoteThread(hProc, 0, 0,
                (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

            if (hThread) {
                CloseHandle(hThread);
            } else {
                //Handle error -> CreateRemoteThread call was unsuccessful
                errorCode = QString::number(GetLastError());
                showError(createRemThreadFail + errorCode);
                CloseHandle(hProc);
                return;
            }
        } else {
            //Handle error -> Error during OpenProcess/invalid Handle
            showError(invalidHandle);
            this->processFoundUpdate();
            CloseHandle(hProc);
            return;
        }

    } else {
        //Handle error -> Assault Cube isn't currently running
        showError(acNotFound);
        this->processFoundUpdate();
        CloseHandle(hProc);
        return;
    }

    //Close handle if still open
    if(hProc != NULL) {
        CloseHandle(hProc);
    }
    */

    //Working 32-bit version of injector function
    QString errorCode;
    QString convertedFilePath = this->injectableFilePath.replace(
        QLatin1String("/"), QLatin1String("\\\\"));
    QByteArray iFPByteArray = convertedFilePath.toLocal8Bit();
    const char *dllPath = iFPByteArray.data();

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, this->acProcessID);

    if (hProc && hProc != INVALID_HANDLE_VALUE)
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if(!WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0)) {
            //errorCode = QString::number(GetLastError());
            //showError("Error: WriteProcessMemory Failure\nError Code: " + errorCode);
            processFoundUpdate();   //The process wasn't found - update the UI
        }

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

        if (hThread)
        {
            CloseHandle(hThread);
        } else {
            errorCode = QString::number(GetLastError());
            showError("Error: CreateRemoteThread Failure\nError Code: " + errorCode);
        }
    } else {
        //errorCode = QString::number(GetLastError());
        //showError("Error: OpenProcess Failure\nError Code: " + errorCode);
        processFoundUpdate();   //The process wasn't found - update the UI
    }

    if (hProc)
    {
        CloseHandle(hProc);
    }
}
