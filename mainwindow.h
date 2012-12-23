#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QClipboard>
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QDesktopServices>
#include <QStandardItemModel>
#include <QCryptographicHash>
#include <QInputDialog>
#include <QTimer>
#include <QMenu>
#include <QDesktopWidget>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include "BitcoinCrypto.h"
#include "newprefixdialog.h"
#include "solvedialog.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    BitcoinCrypto bc;
    NewPrefixDialog* npDialog;
    SolveDialog* slvDialog;
    QStandardItemModel prefixTable;
    QMenu *contextMenu;
    QAction *copyBtcAdAction;
    QAction *copyPkBase58Action;
    QAction *showSolveDialogAction;
    QAction *copyPrefixAction;
    QTimer timer;

    void copyDataFromTable(int column);
    void addRowToTable(QString prefix, QString privatekey, bool solved = false);
    QByteArray AESCrypt(const QByteArray input, QString password, bool decrypt = false);

private slots:
    void on_btnLoadFromFile_released();
    void on_btnSaveToFile_released();
    void on_btnNewPrefix_released();
    void on_txtPubKey_textChanged();
    void on_txtPrivKey_textChanged();
    void on_txtSolution_textChanged();
    void on_tableView_customContextMenuRequested(QPoint pos);
    void on_btnBitcoinAddress_released();
    void on_btnReGen_released();
    void resetTxtStyleSheetsSlot();
    void solveActionSlot();
    void copyBtcAdActionSlot();
    void copyPrefixActionSlot();
    void copyPubkActionSlot();
    void copyPkActionSlot();
    void copyPkBase58ActionSlot();
    void sendToFactoryActionSlot();
    void removeActionSlot();
};

#endif // MAINWINDOW_H
