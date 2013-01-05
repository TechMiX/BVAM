#include "solvedialog.h"
#include "ui_solvedialog.h"

SolveDialog::SolveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SolveDialog)
{
    ui->setupUi(this);
}

SolveDialog::~SolveDialog()
{
    delete ui;
}

void SolveDialog::setPrefPrivPub(QString prefixValue, QString privatekeyValue, QString publickeyValue) {
    prefix = prefixValue;
    ui->lblSolution->setText("Solution for '" + prefix + "' :");
    privatekey = privatekeyValue;
    publickey = publickeyValue;
    solveOk = false;
}

void SolveDialog::on_btnAdd_released()
{
    QString finalAddress = "";
    QString multipliedAddress = "";
    QString addedAddress = "";
    bool solutionAdded = true;
    solveOk = false;

    if (!privatekey.isEmpty()) {
        bc.setPrivateKey(privatekey);
        bc.mergeWithPrivateKey(ui->txtSolution->toPlainText());
        addedAddress = bc.getBitcoinAddress();
    } else if (!publickey.isEmpty()) {
        bc.setPublicKey(publickey);
        bc.mergeWithPublicKey(ui->txtSolution->toPlainText());
        addedAddress = bc.getBitcoinAddress();
    }

    if (!addedAddress.startsWith(prefix)) {
        solutionAdded = false;
        if (!privatekey.isEmpty()) {
            bc.setPrivateKey(privatekey);
            bc.mergeWithPrivateKey(ui->txtSolution->toPlainText(), true);
            multipliedAddress = bc.getBitcoinAddress();
            if (multipliedAddress.startsWith(prefix))
                finalAddress = multipliedAddress;
        } else if (!publickey.isEmpty()) {
            bc.setPublicKey(publickey);
            bc.mergeWithPublicKey(ui->txtSolution->toPlainText(), true);
            multipliedAddress = bc.getBitcoinAddress();
            if (multipliedAddress.startsWith(prefix))
                finalAddress = multipliedAddress;
        }
    } else
        finalAddress = addedAddress;

    if (finalAddress.isEmpty())
        QMessageBox::critical(this, tr("Error"), tr("Solution is wrong!"));
    else {
        QString finalPrivateKey = bc.getPrivateKey();
        QString finalPublicKey = bc.getPublicKey();

        QMessageBox::information(this,
                                 tr("Success"),
                                 tr("Solution was correct!") +
                                 tr("\n\n- Solution Type: ") +
                                 (solutionAdded?tr("Additive"):tr("Multiplicative")) +
                                 tr("\n- Bitcoin Address: ") +
                                 finalAddress +
                                 tr("\n- Final Private key: ") +
                                 (finalPrivateKey.isEmpty()?tr("Unknown"):finalPrivateKey) +
                                 tr("\n- Final Public key: ") +
                                 finalPublicKey);
        solveOk = true;
        privatekey = finalPrivateKey;
        publickey = finalPublicKey;
        this->hide();
    }

}

QString SolveDialog::getPrivateKey() {
    if (solveOk)
        return privatekey;
    else
        return "";
}

QString SolveDialog::getPublicKey() {
    if (solveOk)
        return publickey;
    else
        return "";
}

void SolveDialog::on_txtSolution_textChanged()
{
    if (!bc.isHex(ui->txtSolution->toPlainText()))
        ui->txtSolution->setPlainText("");
}
