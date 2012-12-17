#ifndef SOLVE_H
#define SOLVE_H

#include <QDialog>
#include <QMessageBox>
#include "BitcoinCrypto.h"

namespace Ui {
    class SolveDialog;
}

class SolveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SolveDialog(QWidget *parent = 0);
    void setPrefPriv(QString prefixValue, QString privatekeyValue);
    QString getPrivateKey();
    ~SolveDialog();
    bool solveOk;

private:
    Ui::SolveDialog *ui;
    BitcoinCrypto bc;
    QString prefix;
    QString privatekey;


private slots:
    void on_txtSolution_textChanged();
    void on_btnAdd_released();
};

#endif // SOLVE_H
