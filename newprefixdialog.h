#ifndef NEWPREFIX_H
#define NEWPREFIX_H

#include <QDialog>
#include <QMessageBox>
#include "BitcoinCrypto.h"


namespace Ui {
    class NewPrefixDialog;
}

class NewPrefixDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewPrefixDialog(QWidget *parent = 0);
    QStringList getPrefPrivPubValues();
    ~NewPrefixDialog();


private:
    BitcoinCrypto bc;
    Ui::NewPrefixDialog *ui;
    bool addOk;

private slots:
    void on_btnAdd_released();
    void on_txtPrivKey_textChanged();
    void on_txtPubKey_textChanged();
    void on_btnReGen_released();
};

#endif // NEWPREFIX_H
