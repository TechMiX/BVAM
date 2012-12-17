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

void SolveDialog::setPrefPriv(QString prefixValue, QString privatekeyValue) {
    prefix = prefixValue;
    ui->lblSolution->setText("Solution for '" + prefix + "' :");
    privatekey = privatekeyValue;
    solveOk = false;
}

void SolveDialog::on_btnAdd_released()
{
    QString finalAddress = "";
    QString multipliedAddress = "";
    QString addedAddress = "";
    bool solutionAdded = true;
    solveOk = false;

    bc.setPrivateKey(privatekey);
    bc.mergeWithPrivateKey(ui->txtSolution->toPlainText());
    addedAddress = bc.getBitcoinAddress();

    if (!addedAddress.startsWith(prefix)) {
        solutionAdded = false;
        bc.setPrivateKey(privatekey);
        bc.mergeWithPrivateKey(ui->txtSolution->toPlainText(), true);
        multipliedAddress = bc.getBitcoinAddress();
        if (multipliedAddress.startsWith(prefix))
            finalAddress = multipliedAddress;
    } else
        finalAddress = addedAddress;

    if (finalAddress.isEmpty())
        QMessageBox::critical(this, tr("Error"), tr("Solution is wrong!"));
    else {
        QMessageBox::information(this,
                                 tr("Success"),
                                 tr("Solution was correct!") +
                                 tr("\n\n- Solution Type: ") +
                                 (solutionAdded?tr("Additive"):tr("Multiplicative")) +
                                 tr("\n- Bitcoin Address: ") +
                                 finalAddress +
                                 tr("\n- Final Private key: ") +
                                 bc.getPrivateKey());
        solveOk = true;
        privatekey = bc.getPrivateKey();
        this->hide();
    }

}

QString SolveDialog::getPrivateKey() {
    if (solveOk)
        return privatekey;
    else
        return "";
}

void SolveDialog::on_txtSolution_textChanged()
{
    if (!bc.isHex(ui->txtSolution->toPlainText()))
        ui->txtSolution->setPlainText("");
}
