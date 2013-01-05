#include "newprefixdialog.h"
#include "ui_newprefixdialog.h"


NewPrefixDialog::NewPrefixDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPrefixDialog)
{
    ui->setupUi(this);

    QRegExp rx("1(([a-k]|[m-z]|[A-H]|[J-N]|[P-Z])*([1-9])*)*");

    ui->txDesPref->setValidator(new QRegExpValidator(rx, 0));

    addOk = false;
}

NewPrefixDialog::~NewPrefixDialog()
{
    delete ui;
}

QStringList NewPrefixDialog::getPrefPrivPubValues() {
    QStringList sl;
    if (addOk) {
        sl.append(ui->txDesPref->text());
        sl.append(ui->txtPrivKey->toPlainText());
        sl.append(ui->txtPubKey->toPlainText());
        addOk = false;
    }
    return sl;
}

void NewPrefixDialog::on_btnReGen_released()
{
    bc.generateKeyPairs();
    ui->txtPrivKey->setPlainText(bc.getPrivateKey());
    ui->txtPubKey->setPlainText(bc.getPublicKey());
}

void NewPrefixDialog::on_txtPubKey_textChanged()
{
    if (!bc.isHex(ui->txtPubKey->toPlainText()))
        ui->txtPubKey->setPlainText("");
}

void NewPrefixDialog::on_txtPrivKey_textChanged()
{
    if (!bc.isHex(ui->txtPrivKey->toPlainText()))
        ui->txtPrivKey->setPlainText("");
}

void NewPrefixDialog::on_btnAdd_released()
{
    bool prefixCheck = false, privCheck = false, pubCheck = false;

    if (ui->txDesPref->text().length() > 1)
        prefixCheck = true;

    if (ui->txtPrivKey->toPlainText().isEmpty()) {
        if (ui->txtPubKey->toPlainText().length() > 129)
            pubCheck = true;
    } else {
        if (ui->txtPrivKey->toPlainText().length() > 63)
            privCheck = true;
    }

    if (prefixCheck && (pubCheck || privCheck)) {
        addOk = true;
        this->hide();
    }
    else
        QMessageBox::warning( this, "Error", "No key entered or input is too short!" );
}
