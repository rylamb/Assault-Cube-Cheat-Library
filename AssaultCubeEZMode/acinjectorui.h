#ifndef ACINJECTORUI_H
#define ACINJECTORUI_H

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <Psapi.h>
#include <iostream>
#include <TlHelp32.h>
#include <wchar.h>

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class ACInjectorUI; }
QT_END_NAMESPACE

class ACInjectorUI : public QMainWindow
{
    Q_OBJECT

public:
    ACInjectorUI(QWidget *parent = nullptr);
    ~ACInjectorUI();

private slots:
    void checkInjectReady();
    void processFoundUpdate();
    void injectableFoundUpdate();
    bool isRunning();
    void showError(QString message);
    DWORD getProcId(const wchar_t *procName);

    void on_CheckProcessButton_clicked();

    void on_LocationBrowseButton_clicked();

    void on_InjectEjectButton_clicked();

private:
    Ui::ACInjectorUI *ui;
    DWORD acProcessID = 0;
    QString injectableFilePath = "";
    bool processFound = false;
    bool dllSet = false;
};
#endif // ACINJECTORUI_H
